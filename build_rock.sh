#!/bin/bash


make O=build roc-rk3399-pc_defconfig
make dtbs -j32 O=build
sudo cp build/arch/arm64/boot/dts/rockchip/rk3399-roc-pc.dtb /home/rock/tftpboot/
make -j32 O=build
make O=build -j32 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules
sudo cp build/arch/arm64/boot/Image /home/rock/tftpboot/
ls -alh  /home/rock/tftpboot/*
python ./scripts/clang-tools/gen_compile_commands.py -d build
sudo make O=build -j32 ARCH=arm64 INSTALL_MOD_PATH=/data/nfs_rootfs modules_install
