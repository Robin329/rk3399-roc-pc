rk3399-roc-pc-kernel
Porting rk3399-roc-pc4.4 kernel version to newest linux

1.Compile Steps
export ARCH=arm64

export CROSS_COMPILE=aarch64-linux-gnu-

make rk3399-roc-pc_defconfig

make Image

make dtbs
