#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x2c83f465, "module_layout" },
	{ 0x1fdc7df2, "_mcount" },
	{ 0x51e77c97, "pfn_valid" },
	{ 0xc5850110, "printk" },
	{ 0x9688de8b, "memstart_addr" },
	{ 0x2b157753, "contig_page_data" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

