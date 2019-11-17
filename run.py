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

def build_kernel(command_one):
    """build kernel-5.3.6

    """
    if "build_kernel" == command_one:
        print("===============" + '\033[1;33m' + "Start Build Image" + '\033[0m' + "===============")
        os.system("make Image")
        print("===============" + '\033[1;33m' + "End Build Image" + '\033[0m' + "===============")
        time.sleep(3)
        print("===============" + '\033[1;33m' + "Start Build dtbs" + '\033[0m' + "===============")
        os.system("make dtbs")
        print("===============" + '\033[1;33m' + "END Build dtbs" + '\033[0m' + "===============")
    elif "dtbs" == command_one:
        os.system("make dtbs")
    elif "Image" == command_one:
        os.system("make Image")
    else:
        logger.error("Unknown command  name %s", command_one)
        sys.exit(1)


def main(argv):
    if len(argv) != 1:
        print(__doc__)
        sys.exit(1)
    build_kernel(argv[0])


if __name__ == '__main__':
        main(sys.argv[1:])

