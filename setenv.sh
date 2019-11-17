#!/bin/bash
#
# This scripts for set Build rk3399-roc-pc 
# kernel 5.3.6 envrioment.
#   Author: Robin.J
#   Date  : 2019-11-20

PATH="${`pwd`}"
echo "Current Path: $(PATH)"


SET_ARM=`echo $ARCH`
SET_CROSS_COMPILE=`echo $CROSS_COMPILE`

if [ $SET_ARM == "arm64" -a \
        $SET_CROSS_COMPILE == "aarch64-linux-gnu-" ];then
        echo "ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- ENVIROMENT EXISTED !!!"
else
    `export ARCH=arm64`
    `export CROSS_COMPILE=aarch64-linux-gnu-`
    echo "SET COMPILE ENV END !!!"
fi