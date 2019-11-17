#!/usr/bin/env python
#
# This scritps to compile rk3399-roc-pc kernel-5.3.6
#
# Author:  Robin.J
# Date:    2019-11-18


import os
import os.path
import sys
import time
import logging

logger = logging.getLogger(__name__)

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
        os.system("make rk3399-roc-pc_defconfig")
        print("===============" + '\033[1;33m' + "Start Build Image" + '\033[0m' + "===============")
        ret = os.system("make Image 2>&1 | tee build_Image.log")
        if ret == 0:
            print("Compile Finished !!!")
        print("===============" + '\033[1;33m' + "End Build Image" + '\033[0m' + "===============")
        #time.sleep(3)
        print("===============" + '\033[1;33m' + "Start Build dtbs" + '\033[0m' + "===============")
        os.system("make dtbs 2>&1 | tee build_dtbs.log")
        print("===============" + '\033[1;33m' + "END Build dtbs" + '\033[0m' + "===============")
    elif "dtbs" == command_one:
        os.system("make dtbsb 2>&1 | tee build_dtbs.log")
    elif "Image" == command_one:
        os.system("make Image")
    elif "clean" == command_one:
        finish = os.system("make clean")
        if finish == 0:
            print("Clean Finish !!!")
    else:
        logger.error("Unknown command  name %s", command_one)
        sys.exit(1)


def main(argv):
    if len(argv) != 1:
        print(__doc__)
        sys.exit(1)
    set_env()
    build_kernel(argv[0])


if __name__ == '__main__':
        main(sys.argv[1:])
