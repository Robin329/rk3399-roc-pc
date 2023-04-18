// SPDX-License-Identifier: GPL-2.0+

#include "robin_vkms_drv.h"
#include <drm/drm_atomic_helper.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_simple_kms_helper.h>

static void robin_vkms_connector_destroy(struct drm_connector *connector)
{
	drm_connector_cleanup(connector);
}

static const struct drm_connector_funcs robin_vkms_connector_funcs = {
	.fill_modes = drm_helper_probe_single_connector_modes,
	.destroy = robin_vkms_connector_destroy,
	.reset = drm_atomic_helper_connector_reset,
	.atomic_duplicate_state = drm_atomic_helper_connector_duplicate_state,
	.atomic_destroy_state = drm_atomic_helper_connector_destroy_state,
};

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

int robin_vkms_output_init(struct robin_vkms_device *vkmsdev, int index)
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
