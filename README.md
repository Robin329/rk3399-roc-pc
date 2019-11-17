# rk3399-roc-pc-kernel-5.3.6
Porting rk3399-roc-pc4.4 kernel version to linux5.3.6

### 1.Compile Steps

`export ARCH=arm64`

`export CROSS_COMPILE=aarch64-linux-gnu-`

`make rk3399-roc-pc_defconfig`

`make Image`

`make dtbs`
