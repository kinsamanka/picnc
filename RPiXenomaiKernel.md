Building Xenomai 2.6.2.1 for a v3.8.y RPi kernel on a Debian based x86 system.

## Install cross compiler ##
Download and untar the crosscompiler from https://github.com/raspberrypi/tools/archive/master.tar.gz
```
cd <working dir>
wget https://github.com/raspberrypi/tools/archive/master.tar.gz
tar xzf master.tar.gz
```
## Download source files and patches ##
Download kernel
```
git clone -b rpi-3.8.y --depth 1 git://github.com/raspberrypi/linux.git linux-rpi-3.8.y
```
Download Xenomai
```
git clone git://git.xenomai.org/xenomai-head.git xenomai-head
```
Download minimal config
```
wget https://www.dropbox.com/s/dcju74md5sz45at/rpi_xenomai_config
```
## Apply patches ##
Apply ipipe core pre-patch
```
(cd linux-rpi-3.8.y; patch -Np1 < ../xenomai-head/ksrc/arch/arm/patches/raspberry/ipipe-core-3.8.13-raspberry-pre-2.patch)
```
Apply Xenomai ipipe core patch
```
xenomai-head/scripts/prepare-kernel.sh --arch=arm --linux=linux-rpi-3.8.y \
 --adeos=xenomai-head/ksrc/arch/arm/patches/ipipe-core-3.8.13-arm-3.patch
```
Apply ipipe core post-patch
```
(cd linux-rpi-3.8.y; patch -Np1 < ../xenomai-head/ksrc/arch/arm/patches/raspberry/ipipe-core-3.8.13-raspberry-post-2.patch)
```
## Compile kernel ##
Create build directory
```
mkdir linux-rpi-3.8.y/build
```
Configure kernel
```
cd linux-rpi-3.8.y
make mrproper
make ARCH=arm O=build menuconfig
```
Or use the minimal configuration file
```
cp rpi_xenomai_config linux-rpi-3.8.y/build/.config
cd linux-rpi-3.8.y
make mrproper
make ARCH=arm O=build oldconfig
```
Compile
```
make ARCH=arm O=build CROSS_COMPILE=../../tools-master/arm-bcm2708/arm-\
bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-
```
Install modules
```
make ARCH=arm O=build INSTALL_MOD_PATH=dist modules_install
```
Install headers
```
make ARCH=arm O=build INSTALL_HDR_PATH=dist headers_install
find build/dist/include \( -name .install -o -name ..install.cmd \) -delete
```
The linux-rpi-3.8.y/build/dist directory contains the kernel headers and modules while the kernel image is located at linux-rpi-3.8.y/build/arch/arm/boot/ .


---

## USB FIQ fix ##
_(taken from http://www.xenomai.org/pipermail/xenomai/2014-March/030412.html)_

Download patch
```
cd <working dir>
wget https://gist.githubusercontent.com/kinsamanka/10256843/raw/4d5d3e02a443e4d17d9b82a1fe027ef17fb14470/usb_fiq.patch
```

Apply patch
```
cd linux-rpi-3.8.y
patch -p1 < ../usb_fiq.patch
```

Re-compile kernel
```
make ARCH=arm O=build CROSS_COMPILE=../../tools-master/arm-bcm2708/arm-\
bcm2708hardfp-linux-gnueabi/bin/arm-bcm2708hardfp-linux-gnueabi-
```


---

## UPDATE ##
Packages for the kernel and header files are available here: http://0ptr.link/raspbian/pool/main/l/linux/
