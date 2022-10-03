#!/bin/bash
#
# This scripts for set Build rk3399-roc-pc
# kernel 5.17 envrioment.
#   Author: Robin.J
#   Date  : 2019-11-20

PATH=`pwd`
echo "Current Path: $PATH"


SET_ARM=$ARCH
SET_CROSS_COMPILE=$CROSS_COMPILE

if [ "$SET_ARM" == "arm64" -a \
        "$SET_CROSS_COMPILE" == "aarch64-linux-gnu-" ];then
        echo "ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- ENVIROMENT EXISTED !!!"
else
    echo "set ARCH and CROSS COMPILE"
    export ARCH=arm64
    export CROSS_COMPILE=aarch64-linux-gnu-
    echo "SET COMPILE ENV END !!!"
fi
BISCUITOS_DIR=./driver/biscuitos

if [ ! -d $BISCUITOS_DIR ];then
	sudo ln -s $PATH/workspace/linux_stable/driver/biscuitos $PATH/drivers/biscuitos
fi	
