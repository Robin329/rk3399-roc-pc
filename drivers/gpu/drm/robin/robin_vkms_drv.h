#ifndef __ROBIN_VKMS_DRV_H__
#define __ROBIN_VKMS_DRV_H__
#include <linux/hrtimer.h>

#include <drm/drm.h>
#include <drm/drm_gem.h>
#include <drm/drm_gem_atomic_helper.h>
#include <drm/drm_encoder.h>
#include <drm/drm_writeback.h>

#define XRES_MIN 20
#define YRES_MIN 20

#define XRES_DEF 640
#define YRES_DEF 480

#define XRES_MAX 4096
#define YRES_MAX 4096


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
	/* below four are protected by robin_vkms_output.composer_lock */
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

/* CRTC */
int robin_vkms_crtc_init(struct drm_device *dev, struct drm_crtc *crtc,
		   struct drm_plane *primary, struct drm_plane *cursor);

int robin_vkms_output_init(struct robin_vkms_device *vkmsdev, int index);

struct robin_vkms_plane *robin_vkms_plane_init(struct robin_vkms_device *vkmsdev,
				   enum drm_plane_type type, int index);

/* CRC Support */
const char *const *robin_vkms_get_crc_sources(struct drm_crtc *crtc, size_t *count);
int robin_vkms_set_crc_source(struct drm_crtc *crtc, const char *src_name);
int robin_vkms_verify_crc_source(struct drm_crtc *crtc, const char *source_name,
			   size_t *values_cnt);

/* Composer Support */
void robin_vkms_composer_worker(struct work_struct *work);
void robin_vkms_set_composer(struct robin_vkms_output *out, bool enabled);

/* Writeback */
int robin_vkms_enable_writeback_connector(struct robin_vkms_device *vkmsdev);

#endif /* __ROBIN_VKMS_DRV_H__ */