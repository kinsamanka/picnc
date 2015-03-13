Building packages by cross compiling is sometimes not possible and doing natively is a bit slow. This document describes how to build packages using QEMU on a chrooted environment. This procedure can also be extended to other architectures by selecting the proper Debian mirror when doing the debootstrap step.

# Prerequisites #
This build is based on the assumption that the host is running on a Debian based system or its derivatives. This procedure was created and tested on a Debian Wheezy host.
# Create qemu chroot environment #
## Install required packages ##
```
sudo apt-get install qemu qemu-user qemu-user-static binfmt-support debootstrap
```
## Create minimal chroot environment using debootstrap ##
> Select an appropriate Raspbian(Debian) mirror at http://www.raspbian.org/RaspbianMirrors
> _(change **arch** and **mirror** for other architectures/board)_.
```
cd <working dir>
sudo debootstrap --foreign --no-check-gpg --include=ca-certificates --arch=armhf wheezy rootfs <mirror>
```
> Edit _/etc/qemu-binfmt.conf_, add the following line:
```
EXTRA_OPTS="-L/usr/lib/arm-linux-gnueabihf"
```
> Copy qemu executable and then chroot
```
sudo cp $(which qemu-arm-static) rootfs/usr/bin
sudo chroot rootfs/ /debootstrap/debootstrap --second-stage --verbose
```
> add the mirror used to /etc/apt/sources.list
```
sudo sh -c 'echo deb <mirror> wheezy main > rootfs/etc/apt/sources.list'
sudo sh -c 'echo deb-src <mirror> wheezy main >> rootfs/etc/apt/sources.list'
```
> Create _/etc/resolv.conf_
```
sudo cp /etc/resolv.conf rootfs/etc
```
> Chroot
```
sudo chroot rootfs /bin/bash
```
> Set locale to ignore warnings
```
LC_ALL=C
LANGUAGE=C
LANG=C
```
> Install required packages
```
apt-get update
apt-get install --no-install-recommends git-core kernel-package fakeroot build-essential devscripts lsb-release
```
# Examples #
## Compiling Linux kernel ##
### Download source files and patches ###
> Change to working directory
```
cd /usr/src
```
> Download kernel
```
git clone -b rpi-3.8.y --depth 1 git://github.com/raspberrypi/linux.git linux-rpi-3.8.y
```
> Download Xenomai
```
git clone git://git.xenomai.org/xenomai-head.git xenomai-head
```
> Download minimal config
```
wget https://www.dropbox.com/s/dcju74md5sz45at/rpi_xenomai_config
```
> Download USB fiq fix
```
wget https://gist.githubusercontent.com/kinsamanka/10256843/raw/4d5d3e02a443e4d17d9b82a1fe027ef17fb14470/usb_fiq.patch
```
### Apply patches ###
> Apply ipipe core pre-patch
```
(cd linux-rpi-3.8.y; patch -Np1 < ../xenomai-head/ksrc/arch/arm/patches/raspberry/ipipe-core-3.8.13-raspberry-pre-2.patch)
```
> Apply Xenomai ipipe core patch
```
xenomai-head/scripts/prepare-kernel.sh --arch=arm --linux=linux-rpi-3.8.y \
 --adeos=xenomai-head/ksrc/arch/arm/patches/ipipe-core-3.8.13-arm-3.patch
```
> Apply ipipe core post-patch
```
(cd linux-rpi-3.8.y; patch -Np1 < ../xenomai-head/ksrc/arch/arm/patches/raspberry/ipipe-core-3.8.13-raspberry-post-2.patch)
```
> Apply usb fiq patch
```
(cd linux-rpi-3.8.y; patch -Np1 < ../usb_fiq.patch)
```
### Compile kernel ###
> Edit kernel package maintainer info
```
vi /etc/kernel-pkg.conf
```
> Install prerequisites
```
apt-get install --no-install-recommends ncurses-dev bc
```
> Configure kernel
```
cd linux-rpi-3.8.y
make menuconfig
```
> Or use the minimal configuration file
```
cd linux-rpi-3.8.y
cp ../rpi_xenomai_config .config
make oldconfig
```
> Compile
```
make-kpkg clean
make-kpkg --append-to-version=-xenomai --revision=1.0 kernel_image kernel_headers
```
## Compiling Xenomai Userspace ##
> Change directory
```
cd /usr/src/xenomai-head
```
> Install required dependencies
```
apt-get build-dep xenomai
```
> Compile
```
debuild -i -us -uc -b
```
## Compiling Machinekit ##
> Change directory
```
cd /usr/src
```
> Download source
```
git clone -b master -o github-machinekit --depth 1 git://github.com/machinekit/machinekit.git
```
> Install kernel and xenomai
```
dpkg -i linux-image-3.8.13-xenomai+_1.0_armhf.deb
dpkg -i libxenomai1_2.6.3_armhf.deb
dpkg -i libxenomai-dev_2.6.3_armhf.deb
```
> Compile Linuxcnc
```
cd /usr/src/machinekit
debian/configure --disable-docs
dpkg-checkbuilddeps
apt-get install <missing deps>
debuild -i -us -uc -b
```
> > These were the dependencies that were needed at the time of writing
```
apt-get install --no-install-recommends bwidget libboost-python-dev libgl1-mesa-dev \
	libglib2.0-dev libglu1-mesa-dev libgtk2.0-dev libmodbus-dev libreadline-dev \
	libusb-1.0-0-dev libxmu-dev libxmu-headers python-dev python-support pkg-config \
	psmisc python-tk tcl-dev tk-dev

```