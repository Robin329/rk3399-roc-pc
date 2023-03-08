// SPDX - License - Identifier : GPL - 2.0

#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <drm/drm_device.h>
#include <drm/drm_drv.h>
#include <drm/drm_print.h>
#include <drm/drm_debugfs.h>
#include <drm/drm_crtc.h>
#include <drm/drm_encoder.h>
#include <drm/drm_connector.h>
#include <drm/drm_gem.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_gem_atomic_helper.h>
#include <drm/drm_gem_shmem_helper.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_mode_config.h>
#include <drm/drm_atomic.h>
#include <drm/drm_plane.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_simple_kms_helper.h>
#include <drm/drm_vblank.h>
#include <drm/drm_fb_helper.h>
#include <linux/crc32.h>
#include <drm/drm_writeback.h>

#define pr_fmt(fmt) KBUILD_MODNAME ":[%s:%d] " fmt, __func__, __LINE__

static const u32 robin_vkms_formats[] = {
	DRM_FORMAT_XRGB8888,
};

static const u32 robin_vkms_plane_formats[] = { DRM_FORMAT_ARGB8888,
						DRM_FORMAT_XRGB8888 };

#define XRES_MIN 20
#define YRES_MIN 20

#define XRES_DEF 640
#define YRES_DEF 480

#define XRES_MAX 4096
#define YRES_MAX 4096

#define DRIVER_NAME "robin-vkms"
#define DRIVER_DESC "Virtual Kernel Mode Setting"
#define DRIVER_DATE "20230308"
#define DRIVER_MAJOR 1
#define DRIVER_MINOR 1

DEFINE_DRM_GEM_FOPS(robin_vkms_driver_fops);

static struct drm_driver robin_vkms_driver = {
	.driver_features = DRIVER_MODESET | DRIVER_ATOMIC | DRIVER_GEM,
	.fops = &robin_vkms_driver_fops,
	DRM_GEM_SHMEM_DRIVER_OPS,
	.name = DRIVER_NAME,
	.desc = DRIVER_DESC,
	.date = DRIVER_DATE,
	.major = DRIVER_MAJOR,
	.minor = DRIVER_MINOR,
};

struct robin_vkms_writeback_job {
	struct dma_buf_map map[DRM_FORMAT_MAX_PLANES];
	struct dma_buf_map data[DRM_FORMAT_MAX_PLANES];
};

struct robin_vkms_composer {
	struct drm_framebuffer fb;
	struct drm_rect src, dst;
	struct dma_buf_map map[4];
	unsigned int offset;
	unsigned int pitch;
	unsigned int cpp;
};

/**
 * vkms_plane_state - Driver specific plane state
 * @base: base plane state
 * @composer: data required for composing computation
 */
struct robin_vkms_plane_state {
	struct drm_shadow_plane_state base;
	struct robin_vkms_composer *composer;
};

struct robin_vkms_crtc_state {
	struct drm_crtc_state base;
	struct work_struct composer_work;
	int num_active_planes;
	/* stack of active planes for crc computation, should be in z order */
	struct robin_vkms_plane_state **active_planes;
	struct robin_vkms_writeback_job *active_writeback;
	/* below four are protected by vkms_output.composer_lock */
	bool crc_pending;
	bool wb_pending;
	u64 frame_start;
	u64 frame_end;
};

struct robin_vkms_plane {
	struct drm_plane base;
};

struct robin_vkms_output {
	struct drm_crtc crtc;
	struct drm_encoder encoder;
	struct drm_connector connector;
	struct drm_writeback_connector wb_connector;
	struct hrtimer vblank_hrtimer;
	ktime_t period_ns;
	struct drm_pending_vblank_event *event;
	/* ordered wq for composer_work */
	struct workqueue_struct *composer_workq;
	/* protects concurrent access to composer */
	spinlock_t lock;
	/* protected by @lock */
	bool composer_enabled;
	struct robin_vkms_crtc_state *composer_state;
	spinlock_t composer_lock;
};

struct robin_vkms_device {
	struct drm_device drm;
	struct platform_device *platform;
	struct robin_vkms_output output;
};

#define to_vkms_crtc_state(target)                                             \
	container_of(target, struct robin_vkms_crtc_state, base)

#define to_vkms_plane_state(target)                                            \
	container_of(target, struct robin_vkms_plane_state, base.base)

#define drm_crtc_to_vkms_output(target)                                        \
	container_of(target, struct robin_vkms_output, crtc)

#define drm_device_to_vkms_device(target)                                      \
	container_of(target, struct robin_vkms_device, drm)

static struct robin_vkms_device *vkms_device;

static void robin_vkms_atomic_commit_tail(struct drm_atomic_state *old_state)
{
	struct drm_device *dev = old_state->dev;
	struct drm_crtc *crtc;
	struct drm_crtc_state *old_crtc_state;
	int i;

	drm_atomic_helper_commit_modeset_disables(dev, old_state);

	drm_atomic_helper_commit_planes(dev, old_state, 0);

	drm_atomic_helper_commit_modeset_enables(dev, old_state);

	drm_atomic_helper_fake_vblank(old_state);

	drm_atomic_helper_commit_hw_done(old_state);

	drm_atomic_helper_wait_for_flip_done(dev, old_state);

	for_each_old_crtc_in_state (old_state, crtc, old_crtc_state, i) {
		struct robin_vkms_crtc_state *vkms_state =
			to_vkms_crtc_state(old_crtc_state);

		flush_work(&vkms_state->composer_work);
	}

	drm_atomic_helper_cleanup_planes(dev, old_state);
}

static void robin_vkms_plane_reset(struct drm_plane *plane)
{
}
static struct drm_plane_state *
robin_vkms_plane_duplicate_state(struct drm_plane *plane)
{
	return NULL;
}

static void robin_vkms_plane_destroy_state(struct drm_plane *plane,
					   struct drm_plane_state *state)
{
}

static const struct drm_plane_funcs robin_vkms_plane_funcs = {
	.update_plane = drm_atomic_helper_update_plane,
	.disable_plane = drm_atomic_helper_disable_plane,
	.reset = robin_vkms_plane_reset,
	.atomic_duplicate_state = robin_vkms_plane_duplicate_state,
	.atomic_destroy_state = robin_vkms_plane_destroy_state,
};

static const struct drm_mode_config_funcs robin_vkms_mode_funcs = {
	.fb_create = drm_gem_fb_create,
	.atomic_check = drm_atomic_helper_check,
	.atomic_commit = drm_atomic_helper_commit,
};

static const struct drm_mode_config_helper_funcs
	robin_vkms_mode_config_helpers = {
		.atomic_commit_tail = robin_vkms_atomic_commit_tail,
	};

static void robin_vkms_plane_atomic_update(struct drm_plane *plane,
					   struct drm_atomic_state *state)
{
}

static int robin_vkms_plane_atomic_check(struct drm_plane *plane,
					 struct drm_atomic_state *state)
{
	return 0;
}

static const struct drm_plane_helper_funcs robin_vkms_primary_helper_funcs = {
	.atomic_update = robin_vkms_plane_atomic_update,
	.atomic_check = robin_vkms_plane_atomic_check,
	DRM_GEM_SHADOW_PLANE_HELPER_FUNCS,
};
struct robin_vkms_plane *
robin_vkms_plane_init(struct robin_vkms_device *vkmsdev,
		      enum drm_plane_type type, int index)
{
	struct drm_device *dev = &vkmsdev->drm;
	const struct drm_plane_helper_funcs *funcs;
	struct robin_vkms_plane *plane;
	const u32 *formats;
	int nformats;

	switch (type) {
	case DRM_PLANE_TYPE_CURSOR:
	case DRM_PLANE_TYPE_OVERLAY:
		formats = robin_vkms_plane_formats;
		nformats = ARRAY_SIZE(robin_vkms_plane_formats);
		funcs = &robin_vkms_primary_helper_funcs;
		break;
	case DRM_PLANE_TYPE_PRIMARY:
	default:
		formats = robin_vkms_formats;
		nformats = ARRAY_SIZE(robin_vkms_formats);
		funcs = &robin_vkms_primary_helper_funcs;
		break;
	}
	plane = drmm_universal_plane_alloc(dev, struct robin_vkms_plane, base,
					   1 << index, &robin_vkms_plane_funcs,
					   formats, nformats, NULL, type, NULL);
	if (IS_ERR(plane))
		return plane;

	drm_plane_helper_add(&plane->base, funcs);

	return plane;
}
static u32 get_pixel_from_buffer(int x, int y, const u8 *buffer,
				 const struct robin_vkms_composer *composer)
{
	u32 pixel;
	int src_offset =
		composer->offset + (y * composer->pitch) + (x * composer->cpp);

	pixel = *(u32 *)&buffer[src_offset];

	return pixel;
}
/**
 * compute_crc - Compute CRC value on output frame
 *
 * @vaddr: address to final framebuffer
 * @composer: framebuffer's metadata
 *
 * returns CRC value computed using crc32 on the visible portion of
 * the final framebuffer at vaddr_out
 */
static uint32_t compute_crc(const u8 *vaddr,
			    const struct robin_vkms_composer *composer)
{
	int x, y;
	u32 crc = 0, pixel = 0;
	int x_src = composer->src.x1 >> 16;
	int y_src = composer->src.y1 >> 16;
	int h_src = drm_rect_height(&composer->src) >> 16;
	int w_src = drm_rect_width(&composer->src) >> 16;

	for (y = y_src; y < y_src + h_src; ++y) {
		for (x = x_src; x < x_src + w_src; ++x) {
			pixel = get_pixel_from_buffer(x, y, vaddr, composer);
			crc = crc32_le(crc, (void *)&pixel, sizeof(u32));
		}
	}

	return crc;
}

static u8 blend_channel(u8 src, u8 dst, u8 alpha)
{
	u32 pre_blend;
	u8 new_color;

	pre_blend = (src * 255 + dst * (255 - alpha));

	/* Faster div by 255 */
	new_color = ((pre_blend + ((pre_blend + 257) >> 8)) >> 8);

	return new_color;
}
/**
 * blend - blend value at vaddr_src with value at vaddr_dst
 * @vaddr_dst: destination address
 * @vaddr_src: source address
 * @dst_composer: destination framebuffer's metadata
 * @src_composer: source framebuffer's metadata
 * @pixel_blend: blending equation based on plane format
 *
 * Blend the vaddr_src value with the vaddr_dst value using a pixel blend
 * equation according to the supported plane formats DRM_FORMAT_(A/XRGB8888)
 * and clearing alpha channel to an completely opaque background. This function
 * uses buffer's metadata to locate the new composite values at vaddr_dst.
 *
 * TODO: completely clear the primary plane (a = 0xff) before starting to blend
 * pixel color values
 */
static void blend(void *vaddr_dst, void *vaddr_src,
		  struct robin_vkms_composer *dst_composer,
		  struct robin_vkms_composer *src_composer,
		  void (*pixel_blend)(const u8 *, u8 *))
{
	int i, j, j_dst, i_dst;
	int offset_src, offset_dst;
	u8 *pixel_dst, *pixel_src;

	int x_src = src_composer->src.x1 >> 16;
	int y_src = src_composer->src.y1 >> 16;

	int x_dst = src_composer->dst.x1;
	int y_dst = src_composer->dst.y1;
	int h_dst = drm_rect_height(&src_composer->dst);
	int w_dst = drm_rect_width(&src_composer->dst);

	int y_limit = y_src + h_dst;
	int x_limit = x_src + w_dst;

	for (i = y_src, i_dst = y_dst; i < y_limit; ++i) {
		for (j = x_src, j_dst = x_dst; j < x_limit; ++j) {
			offset_dst = dst_composer->offset +
				     (i_dst * dst_composer->pitch) +
				     (j_dst++ * dst_composer->cpp);
			offset_src = src_composer->offset +
				     (i * src_composer->pitch) +
				     (j * src_composer->cpp);

			pixel_src = (u8 *)(vaddr_src + offset_src);
			pixel_dst = (u8 *)(vaddr_dst + offset_dst);
			pixel_blend(pixel_src, pixel_dst);
			/* clearing alpha channel (0xff)*/
			pixel_dst[3] = 0xff;
		}
		i_dst++;
	}
}
/**
 * alpha_blend - alpha blending equation
 * @argb_src: src pixel on premultiplied alpha mode
 * @argb_dst: dst pixel completely opaque
 *
 * blend pixels using premultiplied blend formula. The current DRM assumption
 * is that pixel color values have been already pre-multiplied with the alpha
 * channel values. See more drm_plane_create_blend_mode_property(). Also, this
 * formula assumes a completely opaque background.
 */
static void alpha_blend(const u8 *argb_src, u8 *argb_dst)
{
	u8 alpha;

	alpha = argb_src[3];
	argb_dst[0] = blend_channel(argb_src[0], argb_dst[0], alpha);
	argb_dst[1] = blend_channel(argb_src[1], argb_dst[1], alpha);
	argb_dst[2] = blend_channel(argb_src[2], argb_dst[2], alpha);
}

/**
 * x_blend - blending equation that ignores the pixel alpha
 *
 * overwrites RGB color value from src pixel to dst pixel.
 */
static void x_blend(const u8 *xrgb_src, u8 *xrgb_dst)
{
	memcpy(xrgb_dst, xrgb_src, sizeof(u8) * 3);
}

static void compose_plane(struct robin_vkms_composer *primary_composer,
			  struct robin_vkms_composer *plane_composer,
			  void *vaddr_out)
{
	struct drm_framebuffer *fb = &plane_composer->fb;
	void *vaddr;
	void (*pixel_blend)(const u8 *p_src, u8 *p_dst);

	if (WARN_ON(dma_buf_map_is_null(&primary_composer->map[0])))
		return;

	vaddr = plane_composer->map[0].vaddr;

	if (fb->format->format == DRM_FORMAT_ARGB8888)
		pixel_blend = &alpha_blend;
	else
		pixel_blend = &x_blend;

	blend(vaddr_out, vaddr, primary_composer, plane_composer, pixel_blend);
}

static int compose_active_planes(void **vaddr_out,
				 struct robin_vkms_composer *primary_composer,
				 struct robin_vkms_crtc_state *crtc_state)
{
	struct drm_framebuffer *fb = &primary_composer->fb;
	struct drm_gem_object *gem_obj = drm_gem_fb_get_obj(fb, 0);
	const void *vaddr;
	int i;

	if (!*vaddr_out) {
		*vaddr_out = kzalloc(gem_obj->size, GFP_KERNEL);
		if (!*vaddr_out) {
			DRM_ERROR("Cannot allocate memory for output frame.");
			return -ENOMEM;
		}
	}

	if (WARN_ON(dma_buf_map_is_null(&primary_composer->map[0])))
		return -EINVAL;

	vaddr = primary_composer->map[0].vaddr;

	memcpy(*vaddr_out, vaddr, gem_obj->size);

	/* If there are other planes besides primary, we consider the active
	 * planes should be in z-order and compose them associatively:
	 * ((primary <- overlay) <- cursor)
	 */
	for (i = 1; i < crtc_state->num_active_planes; i++)
		compose_plane(primary_composer,
			      crtc_state->active_planes[i]->composer,
			      *vaddr_out);

	return 0;
}
/**
 * vkms_composer_worker - ordered work_struct to compute CRC
 *
 * @work: work_struct
 *
 * Work handler for composing and computing CRCs. work_struct scheduled in
 * an ordered workqueue that's periodically scheduled to run by
 * _vblank_handle() and flushed at vkms_atomic_crtc_destroy_state().
 */
void robin_vkms_composer_worker(struct work_struct *work)
{
	struct robin_vkms_crtc_state *crtc_state =
		container_of(work, struct robin_vkms_crtc_state, composer_work);
	struct drm_crtc *crtc = crtc_state->base.crtc;
	struct robin_vkms_output *out = drm_crtc_to_vkms_output(crtc);
	struct robin_vkms_composer *primary_composer = NULL;
	struct robin_vkms_plane_state *act_plane = NULL;
	bool crc_pending, wb_pending;
	void *vaddr_out = NULL;
	u32 crc32 = 0;
	u64 frame_start, frame_end;
	int ret;

	spin_lock_irq(&out->composer_lock);
	frame_start = crtc_state->frame_start;
	frame_end = crtc_state->frame_end;
	crc_pending = crtc_state->crc_pending;
	wb_pending = crtc_state->wb_pending;
	crtc_state->frame_start = 0;
	crtc_state->frame_end = 0;
	crtc_state->crc_pending = false;
	spin_unlock_irq(&out->composer_lock);

	/*
	 * We raced with the vblank hrtimer and previous work already computed
	 * the crc, nothing to do.
	 */
	if (!crc_pending)
		return;

	if (crtc_state->num_active_planes >= 1) {
		act_plane = crtc_state->active_planes[0];
		if (act_plane->base.base.plane->type == DRM_PLANE_TYPE_PRIMARY)
			primary_composer = act_plane->composer;
	}

	if (!primary_composer)
		return;

	if (wb_pending)
		vaddr_out = crtc_state->active_writeback->data[0].vaddr;

	ret = compose_active_planes(&vaddr_out, primary_composer, crtc_state);
	if (ret) {
		if (ret == -EINVAL && !wb_pending)
			kfree(vaddr_out);
		return;
	}

	crc32 = compute_crc(vaddr_out, primary_composer);

	if (wb_pending) {
		drm_writeback_signal_completion(&out->wb_connector, 0);
		spin_lock_irq(&out->composer_lock);
		crtc_state->wb_pending = false;
		spin_unlock_irq(&out->composer_lock);
	} else {
		kfree(vaddr_out);
	}

	/*
	 * The worker can fall behind the vblank hrtimer, make sure we catch up.
	 */
	while (frame_start <= frame_end)
		drm_crtc_add_crc_entry(crtc, true, frame_start++, &crc32);
}
static void robin_vkms_atomic_crtc_destroy_state(struct drm_crtc *crtc,
						 struct drm_crtc_state *state)
{
	struct robin_vkms_crtc_state *vkms_state = to_vkms_crtc_state(state);

	__drm_atomic_helper_crtc_destroy_state(state);

	WARN_ON(work_pending(&vkms_state->composer_work));
	kfree(vkms_state->active_planes);
	kfree(vkms_state);
}

static void robin_vkms_atomic_crtc_reset(struct drm_crtc *crtc)
{
	struct robin_vkms_crtc_state *vkms_state =
		kzalloc(sizeof(*vkms_state), GFP_KERNEL);

	if (crtc->state)
		robin_vkms_atomic_crtc_destroy_state(crtc, crtc->state);

	__drm_atomic_helper_crtc_reset(crtc, &vkms_state->base);
	if (vkms_state)
		INIT_WORK(&vkms_state->composer_work,
			  robin_vkms_composer_worker);
}

static enum hrtimer_restart robin_vkms_vblank_simulate(struct hrtimer *timer)
{
	struct robin_vkms_output *output =
		container_of(timer, struct robin_vkms_output, vblank_hrtimer);
	struct drm_crtc *crtc = &output->crtc;
	struct robin_vkms_crtc_state *state;
	u64 ret_overrun;
	bool ret, fence_cookie;

	fence_cookie = dma_fence_begin_signalling();

	ret_overrun =
		hrtimer_forward_now(&output->vblank_hrtimer, output->period_ns);
	if (ret_overrun != 1)
		pr_warn("%s: vblank timer overrun\n", __func__);

	spin_lock(&output->lock);
	ret = drm_crtc_handle_vblank(crtc);
	if (!ret)
		DRM_ERROR("vkms failure on handling vblank");

	state = output->composer_state;
	spin_unlock(&output->lock);

	if (state && output->composer_enabled) {
		u64 frame = drm_crtc_accurate_vblank_count(crtc);

		/* update frame_start only if a queued vkms_composer_worker()
		 * has read the data
		 */
		spin_lock(&output->composer_lock);
		if (!state->crc_pending)
			state->frame_start = frame;
		else
			DRM_DEBUG_DRIVER(
				"crc worker falling behind, frame_start: %llu, frame_end: %llu\n",
				state->frame_start, frame);
		state->frame_end = frame;
		state->crc_pending = true;
		spin_unlock(&output->composer_lock);

		ret = queue_work(output->composer_workq, &state->composer_work);
		if (!ret)
			DRM_DEBUG_DRIVER("Composer worker already queued\n");
	}

	dma_fence_end_signalling(fence_cookie);

	return HRTIMER_RESTART;
}

static struct drm_crtc_state *
robin_vkms_atomic_crtc_duplicate_state(struct drm_crtc *crtc)
{
	struct robin_vkms_crtc_state *vkms_state;

	if (WARN_ON(!crtc->state))
		return NULL;

	vkms_state = kzalloc(sizeof(*vkms_state), GFP_KERNEL);
	if (!vkms_state)
		return NULL;

	__drm_atomic_helper_crtc_duplicate_state(crtc, &vkms_state->base);

	INIT_WORK(&vkms_state->composer_work, robin_vkms_composer_worker);

	return &vkms_state->base;
}

static int robin_vkms_enable_vblank(struct drm_crtc *crtc)
{
	struct drm_device *dev = crtc->dev;
	unsigned int pipe = drm_crtc_index(crtc);
	struct drm_vblank_crtc *vblank = &dev->vblank[pipe];
	struct robin_vkms_output *out = drm_crtc_to_vkms_output(crtc);

	drm_calc_timestamping_constants(crtc, &crtc->mode);

	hrtimer_init(&out->vblank_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	out->vblank_hrtimer.function = &robin_vkms_vblank_simulate;
	out->period_ns = ktime_set(0, vblank->framedur_ns);
	hrtimer_start(&out->vblank_hrtimer, out->period_ns, HRTIMER_MODE_REL);

	return 0;
}
static void robin_vkms_disable_vblank(struct drm_crtc *crtc)
{
	struct robin_vkms_output *out = drm_crtc_to_vkms_output(crtc);

	hrtimer_cancel(&out->vblank_hrtimer);
}

static int robin_vkms_crc_parse_source(const char *src_name, bool *enabled)
{
	int ret = 0;

	if (!src_name) {
		*enabled = false;
	} else if (strcmp(src_name, "auto") == 0) {
		*enabled = true;
	} else {
		*enabled = false;
		ret = -EINVAL;
	}

	return ret;
}

void robin_vkms_set_composer(struct robin_vkms_output *out, bool enabled)
{
	bool old_enabled;

	if (enabled)
		drm_crtc_vblank_get(&out->crtc);

	spin_lock_irq(&out->lock);
	old_enabled = out->composer_enabled;
	out->composer_enabled = enabled;
	spin_unlock_irq(&out->lock);

	if (old_enabled)
		drm_crtc_vblank_put(&out->crtc);
}

static bool robin_vkms_get_vblank_timestamp(struct drm_crtc *crtc,
					    int *max_error,
					    ktime_t *vblank_time,
					    bool in_vblank_irq)
{
	struct drm_device *dev = crtc->dev;
	unsigned int pipe = crtc->index;
	struct robin_vkms_device *vkmsdev = drm_device_to_vkms_device(dev);
	struct robin_vkms_output *output = &vkmsdev->output;
	struct drm_vblank_crtc *vblank = &dev->vblank[pipe];

	if (!READ_ONCE(vblank->enabled)) {
		*vblank_time = ktime_get();
		return true;
	}

	*vblank_time = READ_ONCE(output->vblank_hrtimer.node.expires);

	if (WARN_ON(*vblank_time == vblank->time))
		return true;

	/*
	 * To prevent races we roll the hrtimer forward before we do any
	 * interrupt processing - this is how real hw works (the interrupt is
	 * only generated after all the vblank registers are updated) and what
	 * the vblank core expects. Therefore we need to always correct the
	 * timestampe by one frame.
	 */
	*vblank_time -= output->period_ns;

	return true;
}

static const char *const pipe_crc_sources[] = { "auto" };

const char *const *robin_vkms_get_crc_sources(struct drm_crtc *crtc,
					      size_t *count)
{
	*count = ARRAY_SIZE(pipe_crc_sources);
	return pipe_crc_sources;
}
int robin_vkms_set_crc_source(struct drm_crtc *crtc, const char *src_name)
{
	struct robin_vkms_output *out = drm_crtc_to_vkms_output(crtc);
	bool enabled = false;
	int ret = 0;

	ret = robin_vkms_crc_parse_source(src_name, &enabled);

	robin_vkms_set_composer(out, enabled);

	return ret;
}
int robin_vkms_verify_crc_source(struct drm_crtc *crtc, const char *src_name,
				 size_t *values_cnt)
{
	bool enabled;

	if (robin_vkms_crc_parse_source(src_name, &enabled) < 0) {
		DRM_DEBUG_DRIVER("unknown source %s\n", src_name);
		return -EINVAL;
	}

	*values_cnt = 1;

	return 0;
}
static const struct drm_crtc_funcs robin_vkms_crtc_funcs = {
	.set_config = drm_atomic_helper_set_config,
	.destroy = drm_crtc_cleanup,
	.page_flip = drm_atomic_helper_page_flip,
	.reset = robin_vkms_atomic_crtc_reset,
	.atomic_duplicate_state = robin_vkms_atomic_crtc_duplicate_state,
	.atomic_destroy_state = robin_vkms_atomic_crtc_destroy_state,
	.enable_vblank = robin_vkms_enable_vblank,
	.disable_vblank = robin_vkms_disable_vblank,
	.get_vblank_timestamp = robin_vkms_get_vblank_timestamp,
	.get_crc_sources = robin_vkms_get_crc_sources,
	.set_crc_source = robin_vkms_set_crc_source,
	.verify_crc_source = robin_vkms_verify_crc_source,
};

static int robin_vkms_crtc_atomic_check(struct drm_crtc *crtc,
					struct drm_atomic_state *state)
{
	struct drm_crtc_state *crtc_state =
		drm_atomic_get_new_crtc_state(state, crtc);
	struct robin_vkms_crtc_state *vkms_state =
		to_vkms_crtc_state(crtc_state);
	struct drm_plane *plane;
	struct drm_plane_state *plane_state;
	int i = 0, ret;

	if (vkms_state->active_planes)
		return 0;

	ret = drm_atomic_add_affected_planes(crtc_state->state, crtc);
	if (ret < 0)
		return ret;

	drm_for_each_plane_mask (plane, crtc->dev, crtc_state->plane_mask) {
		plane_state = drm_atomic_get_existing_plane_state(
			crtc_state->state, plane);
		WARN_ON(!plane_state);

		if (!plane_state->visible)
			continue;

		i++;
	}

	vkms_state->active_planes = kcalloc(i, sizeof(plane), GFP_KERNEL);
	if (!vkms_state->active_planes)
		return -ENOMEM;
	vkms_state->num_active_planes = i;

	i = 0;
	drm_for_each_plane_mask (plane, crtc->dev, crtc_state->plane_mask) {
		plane_state = drm_atomic_get_existing_plane_state(
			crtc_state->state, plane);

		if (!plane_state->visible)
			continue;

		vkms_state->active_planes[i++] =
			to_vkms_plane_state(plane_state);
	}
	return 0;
}
static void robin_vkms_crtc_atomic_begin(struct drm_crtc *crtc,
					 struct drm_atomic_state *state)
{
	drm_crtc_vblank_on(crtc);
}
static void robin_vkms_crtc_atomic_flush(struct drm_crtc *crtc,
					 struct drm_atomic_state *state)
{
	drm_crtc_vblank_off(crtc);
}
static void robin_vkms_crtc_atomic_enable(struct drm_crtc *crtc,
					  struct drm_atomic_state *state)
{
	struct robin_vkms_output *vkms_output = drm_crtc_to_vkms_output(crtc);

	/* This lock is held across the atomic commit to block vblank timer
	 * from scheduling vkms_composer_worker until the composer is updated
	 */
	spin_lock_irq(&vkms_output->lock);
}
static void robin_vkms_crtc_atomic_disable(struct drm_crtc *crtc,
					   struct drm_atomic_state *state)
{
	struct robin_vkms_output *vkms_output = drm_crtc_to_vkms_output(crtc);

	if (crtc->state->event) {
		spin_lock(&crtc->dev->event_lock);

		if (drm_crtc_vblank_get(crtc) != 0)
			drm_crtc_send_vblank_event(crtc, crtc->state->event);
		else
			drm_crtc_arm_vblank_event(crtc, crtc->state->event);

		spin_unlock(&crtc->dev->event_lock);

		crtc->state->event = NULL;
	}

	vkms_output->composer_state = to_vkms_crtc_state(crtc->state);

	spin_unlock_irq(&vkms_output->lock);
}
static const struct drm_crtc_helper_funcs robin_vkms_crtc_helper_funcs = {
	.atomic_check = robin_vkms_crtc_atomic_check,
	.atomic_begin = robin_vkms_crtc_atomic_begin,
	.atomic_flush = robin_vkms_crtc_atomic_flush,
	.atomic_enable = robin_vkms_crtc_atomic_enable,
	.atomic_disable = robin_vkms_crtc_atomic_disable,
};

int robin_vkms_crtc_init(struct drm_device *dev, struct drm_crtc *crtc,
			 struct drm_plane *primary, struct drm_plane *cursor)
{
	struct robin_vkms_output *vkms_out = drm_crtc_to_vkms_output(crtc);
	int ret;

	ret = drm_crtc_init_with_planes(dev, crtc, primary, cursor,
					&robin_vkms_crtc_funcs, NULL);
	if (ret) {
		pr_err("Failed to init CRTC\n");
		return ret;
	}

	drm_crtc_helper_add(crtc, &robin_vkms_crtc_helper_funcs);

	spin_lock_init(&vkms_out->lock);
	spin_lock_init(&vkms_out->composer_lock);

	vkms_out->composer_workq =
		alloc_ordered_workqueue("robin_vkms_composer", 0);
	if (!vkms_out->composer_workq)
		return -ENOMEM;

	return ret;
}

static int robin_vkms_conn_get_modes(struct drm_connector *connector)
{
	int count;

	count = drm_add_modes_noedid(connector, XRES_MAX, YRES_MAX);
	drm_set_preferred_mode(connector, XRES_DEF, YRES_DEF);

	return count;
}

static const struct drm_connector_helper_funcs robin_vkms_conn_helper_funcs = {
	.get_modes = robin_vkms_conn_get_modes,
};

static void robin_vkms_connector_destroy(struct drm_connector *connector)
{
}
static const struct drm_connector_funcs robin_vkms_connector_funcs = {
	.fill_modes = drm_helper_probe_single_connector_modes,
	.destroy = robin_vkms_connector_destroy,
	.reset = drm_atomic_helper_connector_reset,
	.atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
};

static int robin_vkms_output_init(struct robin_vkms_device *vkmsdev, int index)
{
	struct robin_vkms_output *output = &vkmsdev->output;
	struct drm_device *dev = &vkmsdev->drm;
	struct drm_connector *connector = &output->connector;
	struct drm_encoder *encoder = &output->encoder;
	struct drm_crtc *crtc = &output->crtc;
	struct robin_vkms_plane *primary, *cursor = NULL, *overlay = NULL;
	int ret;

	primary = robin_vkms_plane_init(vkmsdev, DRM_PLANE_TYPE_PRIMARY, index);
	if (IS_ERR(primary)) {
		pr_info("\n");
		return PTR_ERR(primary);
	}
	overlay = robin_vkms_plane_init(vkmsdev, DRM_PLANE_TYPE_OVERLAY, index);
	if (IS_ERR(overlay)) {
		pr_info("\n");
		return PTR_ERR(overlay);
	}

	if (!overlay->base.possible_crtcs)
		overlay->base.possible_crtcs = drm_crtc_mask(crtc);

	cursor = robin_vkms_plane_init(vkmsdev, DRM_PLANE_TYPE_CURSOR, index);
	if (IS_ERR(cursor)) {
		pr_info("\n");
		return PTR_ERR(cursor);
	}

	ret = robin_vkms_crtc_init(dev, crtc, &primary->base, &cursor->base);
	pr_info("ret:%d\n", ret);
	if (ret)
		return ret;

	ret = drm_connector_init(dev, connector, &robin_vkms_connector_funcs,
				 DRM_MODE_CONNECTOR_VIRTUAL);
	if (ret) {
		pr_err("Failed to init connector\n");
		goto err_connector;
	}

	drm_connector_helper_add(connector, &robin_vkms_conn_helper_funcs);

	ret = drm_simple_encoder_init(dev, encoder, DRM_MODE_ENCODER_VIRTUAL);
	if (ret) {
		pr_err("Failed to init encoder\n");
		goto err_encoder;
	}

	ret = drm_connector_attach_encoder(connector, encoder);
	if (ret) {
		pr_err("Failed to attach connector to encoder\n");
		goto err_attach;
	}

	drm_mode_config_reset(dev);

	return 0;

err_attach:
	drm_encoder_cleanup(encoder);

err_encoder:
	drm_connector_cleanup(connector);

err_connector:
	drm_crtc_cleanup(crtc);

	return ret;
}

static int robin_vkms_modeset_init(struct robin_vkms_device *vkmsdev)
{
	struct drm_device *dev = &vkmsdev->drm;

	drm_mode_config_init(dev);
	dev->mode_config.funcs = &robin_vkms_mode_funcs;
	dev->mode_config.min_width = YRES_MIN;
	dev->mode_config.min_height = YRES_MIN;
	dev->mode_config.max_width = XRES_MAX;
	dev->mode_config.max_height = YRES_MAX;
	dev->mode_config.cursor_width = 32;
	dev->mode_config.cursor_height = 32;
	dev->mode_config.preferred_depth = 0;
	dev->mode_config.helper_private = &robin_vkms_mode_config_helpers;

	return robin_vkms_output_init(vkmsdev, 0);
}

static int __init robin_vkms_init(void)
{
	int ret;
	struct platform_device *pdev;

	pdev = platform_device_register_simple(DRIVER_NAME, -1, NULL, 0);
	if (IS_ERR(pdev))
		return PTR_ERR(pdev);

	if (!devres_open_group(&pdev->dev, NULL, GFP_KERNEL)) {
		ret = -ENOMEM;
		goto out_unregister;
	}
	vkms_device = devm_drm_dev_alloc(&pdev->dev, &robin_vkms_driver,
					 struct robin_vkms_device, drm);
	if (IS_ERR(vkms_device)) {
		ret = PTR_ERR(vkms_device);
		goto out_devres;
	}
	vkms_device->platform = pdev;

	ret = dma_coerce_mask_and_coherent(vkms_device->drm.dev,
					   DMA_BIT_MASK(64));

	if (ret) {
		DRM_ERROR("Could not initialize DMA support\n");
		goto out_devres;
	}

	ret = drm_vblank_init(&vkms_device->drm, 1);
	if (ret) {
		DRM_ERROR("Failed to vblank\n");
		goto out_devres;
	}

	ret = robin_vkms_modeset_init(vkms_device);
	pr_info("ret:%d\n", ret);
	if (ret)
		goto out_devres;

	ret = drm_dev_register(&vkms_device->drm, 0);
	pr_info("ret:%d\n", ret);
	if (ret)
		goto out_devres;

	drm_fbdev_generic_setup(&vkms_device->drm, 0);

	return 0;

out_devres:
	devres_release_group(&pdev->dev, NULL);
out_unregister:
	platform_device_unregister(pdev);
	return 0;
}

static void robin_vkms_destroy(struct robin_vkms_device *rv_dev)
{
	drm_dev_unregister(&rv_dev->drm);
	drm_atomic_helper_shutdown(&rv_dev->drm);
	devres_release_group(&rv_dev->platform->dev, NULL);
	platform_device_unregister(rv_dev->platform);
}

static void __exit robin_vkms_exit(void)
{
	struct platform_device *pdev;
	pdev = vkms_device->platform;
	if (!pdev) {
		pr_info("vkms device is NULL!\n");
		return;
	}
	if (vkms_device)
		robin_vkms_destroy(vkms_device);
}
module_init(robin_vkms_init);
module_exit(robin_vkms_exit);

MODULE_AUTHOR("Robin Jiang <jiangrenbin329@gmail.com>");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");
