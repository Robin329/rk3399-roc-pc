#!/bin/bash


make dtbs -j32 O=build
sudo cp build/arch/arm64/boot/dts/rockchip/rk3399-roc-pc.dtb /home/rock/tftpboot/
