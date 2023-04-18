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
#include <robin_vkms_drv.h>

#define pr_fmt(fmt) KBUILD_MODNAME ":[%s:%d] " fmt, __func__, __LINE__

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

static const struct drm_mode_config_funcs robin_vkms_mode_funcs = {
	.fb_create = drm_gem_fb_create,
	.atomic_check = drm_atomic_helper_check,
	.atomic_commit = drm_atomic_helper_commit,
};

static const struct drm_mode_config_helper_funcs
	robin_vkms_mode_config_helpers = {
		.atomic_commit_tail = robin_vkms_atomic_commit_tail,
	};

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
