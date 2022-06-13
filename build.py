#!/usr/bin/env python
#
# This scritps to compile rk3399-roc-pc kernel-5.17
#
# Author:  Robin.J
# Date:    2019-11-18


import os
import os.path
import sys
import time
import logging

logger = logging.getLogger(__name__)

def help():
    print("Please input ./build.py [param]:")
    print("[param]:")
    print("         [build_kernel]: full compile kernel.")
    print("         [dtbs]: compile rk3399-roc-pc.dtb.")
    print("         [Image]: compile Image.")
    print("         [clean]: make clean.")
    print("         [modules]: make modules.")
    print("         [build_virt]: make QEMU virt Image.")

def set_env():
    """Set Build enviroment.

    """
    arch = os.popen("echo $ARCH")
    arch_value = arch.read()
    print("Current ARCH = " + '\033[1;32m' + arch_value + '\033[0m')
    arch.close()
    cross_comp = os.popen("echo $CROSS_COMPILE")
    cross_comp_value = cross_comp.read()
    print("Current CROSS_COMPILE = " + '\033[1;32m' +cross_comp_value + '\033[0m')
    cross_comp.close()
    if arch_value == "" and cross_comp_value == "":
        os.system("chmod a+x setenv.sh")
        os.system("source setenv.sh")
    elif arch_value == "arm64" or cross_comp_value == "aarch64-linux-gnu-":
        sys.exit(1)

def build_kernel(command_one):
    """build kernel-5.3.6

       rk3399-roc-pc
    """
    if "build_kernel" == command_one:
        os.system("make O=out roc-rk3399-pc_defconfig")
        print("===============" + '\033[1;33m' + "Start Build Image" + '\033[0m' + "===============")
        ret = os.system("make Image -j32 O=out  2>&1 | tee build_Image.log")
        if ret == 0:
            print("Compile Finished !!!")
        print("===============" + '\033[1;33m' + "End Build Image" + '\033[0m' + "===============")
        #time.sleep(3)
        print("===============" + '\033[1;33m' + "Start Build dtbs" + '\033[0m' + "===============")
        os.system("make dtbs O=out  2>&1 | tee build_dtbs.log")
        print("===============" + '\033[1;33m' + "END Build dtbs" + '\033[0m' + "===============")
        if os.path.isdir("/home/robin/tftpboot"):
            print("tftpboot is exist!")
            os.system("cp out/arch/arm64/boot/Image ~/tftpboot/")
            os.system("cp out/arch/arm64/boot/dts/rockchip/rk3399-roc-pc.dtb /home/robin/tftpboot/")
            print("copy rk3399-roc-pc.dtb finish!")
            print("copy Image finish!")
        else:
            print("tftpboot not exist!")
    elif "dtbs" == command_one:
        os.system("make dtbs -j32 O=out  2>&1 | tee build_dtbs.log")
        if os.path.isdir("/home/robin/tftpboot"):
            print("tftpboot is exist!")
            os.system("cp out/arch/arm64/boot/dts/rockchip/rk3399-roc-pc.dtb /home/robin/tftpboot/")
            print("copy rk3399-roc-pc.dtb finish!")
    elif "Image" == command_one:
        os.system("make Image -j32 O=out  2>&1 | tee build_Image.log")
        if os.path.isdir("/home/robin/tftpboot"):
            print("tftpboot is exist!")
            os.system("cp out/arch/arm64/boot/Image /home/robin/tftpboot/")
            print("copy Image to tftpboot dir finish!")
    elif "clean" == command_one:
        finish_1 = os.system("make clean O=out")
        finish_2 = os.system("make mrproper O=out")
        if finish_1 == 0 and finish_2 == 0:
            print("Clean Finish !!!")
    elif "modules" == command_one:
        print("===============" + '\033[1;33m' + "Start Build MODULES" + '\033[0m' + "===============")
        #os.system("make ARCH=arm64 mrproper")
        ret_modules = os.system("make  ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules")
        print("===============" + '\033[1;33m' + "END Build modules" + '\033[0m' + "===============")
        if ret_modules == 0:
            print("===============" + '\033[1;33m' + "Start INSTALL MODULES" + '\033[0m' + "===============")
            os.system("sudo make ARCH=arm64 INSTALL_MOD_PATH=~/nfs_rootfs/rootfs modules_install")
            print("===============" + '\033[1;33m' + "END INSTALL MODULES" + '\033[0m' + "===============")
        else:
            print("COMPILE MODULES FAILED !!!")
    elif "build_virt" == command_one:
        os.system("make O=out virt_defconfig")
        print("===============" + '\033[1;33m' + "Start Build Image" + '\033[0m' + "===============")
        ret = os.system("make Image -j32 O=out  2>&1 | tee build_Image.log")
        if ret == 0:
            print("Compile Finished !!!")
        print("===============" + '\033[1;33m' + "End Build Image" + '\033[0m' + "===============")
    else:
        logger.error("Unknown command  name %s", command_one)
        sys.exit(1)


def main(argv):
    if len(argv) != 1:
        help()
        sys.exit(1)
    set_env()
    build_kernel(argv[0])


if __name__ == '__main__':
        main(sys.argv[1:])
