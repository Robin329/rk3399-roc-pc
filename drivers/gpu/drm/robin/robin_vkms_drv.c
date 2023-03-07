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

#define DRIVER_NAME "robin-vkms"
#define DRIVER_DESC "Virtual Kernel Mode Setting"
#define DRIVER_DATE "20230308"
#define DRIVER_MAJOR 1
#define DRIVER_MINOR 1

static struct drm_driver robin_vkms_driver = {
	.name = DRIVER_NAME,
	.desc = DRIVER_DESC,
	.date = DRIVER_DATE,
	.major = DRIVER_MAJOR,
	.minor = DRIVER_MINOR,
};

struct robin_vkms_output {
	struct drm_crtc crtc;
	struct drm_encoder encoder;
	struct drm_connector connector;
	/* protects concurrent access to composer */
	spinlock_t lock;
};

struct robin_vkms_device {
	struct drm_device drm;
	struct platform_device *platform;
	struct robin_vkms_output output;
};

static struct robin_vkms_device *vkms_device;
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
	ret = drm_dev_register(&vkms_device->drm, 0);
	if (ret)
		goto out_devres;
out_devres:
	devres_release_group(&pdev->dev, NULL);
out_unregister:
	platform_device_unregister(pdev);
	return 0;
}

static void robin_vkms_destroy(struct robin_vkms_device *rv_dev)
{
	drm_dev_unregister(&rv_dev->drm);
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
