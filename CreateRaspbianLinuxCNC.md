
# Prerequisites #
This build is based on the assumption that the host is running on a Debian based system or its derivatives. This procedure was created and tested on a Debian Wheezy host.
# Create minimal Raspbian image #
The Raspbian image is created using debootstrap and QEMU. These steps can also be performed on a Raspberry Pi.
## Install required packages ##
### On host PC ###
```
sudo apt-get install qemu qemu-user qemu-user-static binfmt-support debootstrap
```
### Or on RPi ###
```
sudo apt-get install debootstrap
```
## Build image using debootstrap ##
Select an appropriate Raspbian mirror at http://www.raspbian.org/RaspbianMirrors
### On host PC ###
```
cd <working dir>
sudo debootstrap --foreign --no-check-gpg --include=ca-certificates --arch=armhf wheezy rootfs <Raspbian mirror>
```
Edit _/etc/qemu-binfmt.conf_, add the following line:
```
EXTRA_OPTS="-L/usr/lib/arm-linux-gnueabihf"
```

Copy qemu executable and then chroot
```
sudo cp $(which qemu-arm-static) rootfs/usr/bin
sudo chroot rootfs/ /debootstrap/debootstrap --second-stage --verbose
```
add the raspbian mirror used to /etc/apt/sources.list
```
sudo sh -c 'echo deb <Raspbian mirror> wheezy main > rootfs/etc/apt/sources.list'
```

### Or on RPI ###
```
cd <working dir>
sudo debootstrap --no-check-gpg --include=ca-certificates wheezy rootfs <Raspbian mirror>
```
## Configure Raspbian ##
Edit _/etc/hostname_
```
sudo sh -c 'echo rpi-linuxcnc >rootfs/etc/hostname'
```
Edit _/etc/hosts_
```
sudo sh -c 'echo 127.0.0.1\\trpi-linuxcnc >> rootfs/etc/hosts'
```
Edit network interface
```
sudo sh -c 'cat> rootfs/etc/network/interfaces << EOF
auto lo
iface lo inet loopback

auto eth0
iface eth0 inet dhcp
EOF
'
```
Edit _/etc/fstab_
```
sudo sh -c 'cat> rootfs/etc/fstab << EOF
proc /proc proc defaults 0 0
/dev/mmcblk0p1 /boot vfat defaults 0 0
EOF
'
```
Create _/etc/resolv.conf_
```
sudo cp /etc/resolv.conf rootfs/etc
```
Set locale to ignore warnings
```
sudo sh -c 'cat >> rootfs/root/.bashrc << EOF
LC_ALL=C
LANGUAGE=C
LANG=C
EOF
'
```
Chroot
```
sudo chroot rootfs /bin/bash
```
Update
```
apt-get update
```
### Install Xenomai Kernel ###
Download xenomai kernel
```
wget https://dl.dropboxusercontent.com/u/17024524/linuxcnc/Raspbian/linux-image-3.8.13-xenomai%2B_1.0.gpo_armhf.deb
```
Install kernel
```
dpkg -i linux-image-3.8.13-xenomai+_1.0.gpo_armhf.deb
mv /boot/vmlinuz-3.8.13-xenomai+ /boot/kernel.img
```

### Install Xenomai Userspace ###
Download Xenomai
```
wget https://dl.dropboxusercontent.com/u/17024524/linuxcnc/Raspbian/xenomai-runtime_2.6.3_armhf.deb
wget https://dl.dropboxusercontent.com/u/17024524/linuxcnc/Raspbian/libxenomai-dev_2.6.3_armhf.deb
wget https://dl.dropboxusercontent.com/u/17024524/linuxcnc/Raspbian/libxenomai1_2.6.3_armhf.deb
```
Install Xenomai
```
dpkg -i libxenomai1_2.6.3_armhf.deb
dpkg -i libxenomai-dev_2.6.3_armhf.deb
dpkg -i xenomai-runtime_2.6.3_armhf.deb
```

### Install Linuxcnc ###
Install Linuxcnc dependencies
```
apt-get install --no-install-recommends g++ python yapps2-runtime \
	libatk1.0-0 libboost-python1.49.0 libcairo2 libfontconfig1 \
	libfreetype6 libgdk-pixbuf2.0-0 libgl1-mesa-swx11 libglib2.0-0 \
	libglu1-mesa libgtk2.0-0 libmodbus5  libpango1.0-0 libpython2.7 \
	libusb-1.0-0 libxft2 libxinerama1 libxmu6 libxss1 tk8.5 bwidget \
	libtk-img python-tk python-gnome2 python-glade2 python-numpy \
	python-imaging python-imaging-tk python-gtksourceview2 \
	python-vte python-xlib python-gtkglext1 python-configobj \
	tclreadline bc 
```
Download Linuxcnc
```
wget https://dl.dropboxusercontent.com/u/17024524/linuxcnc/Raspbian/linuxcnc-dev_2.5.3_armhf.deb
wget https://dl.dropboxusercontent.com/u/17024524/linuxcnc/Raspbian/linuxcnc-posix_2.5.3_armhf.deb
wget https://dl.dropboxusercontent.com/u/17024524/linuxcnc/Raspbian/linuxcnc-xenomai_2.5.3_armhf.deb
wget https://dl.dropboxusercontent.com/u/17024524/linuxcnc/Raspbian/linuxcnc_2.5.3_armhf.deb
```
Install Linuxcnc
```
dpkg -i linuxcnc*deb
```

### Install Additional Programs ###
Extra packages
```
apt-get install --no-install-recommends locales sudo openssh-server ntp usbmount \
	make git patch less rsync
```
Minimal X
```
apt-get install --no-install-recommends xinit xserver-xorg-core xserver-xorg \
	xserver-xorg-input-all xserver-xorg-input-evdev xserver-xorg-video-fbdev
```
Minimal window manager
```
apt-get install --no-install-recommends lxde lxde-icon-theme iceweasel
```
Enable remote desktop access
```
apt-get install xrdp
```

Cleanup
```
rm *deb
apt-get clean
```
Create new user
```
adduser pi
usermod -a -G xenomai,sudo,staff,kmem,plugdev pi
```
Configure usbmount
```
sed -i -e 's/""/"-fstype=vfat,flush,gid=plugdev,dmask=0007,fmask=0117"/g' /etc/usbmount/usbmount.conf
```
Configure udev rules
```
cat >/etc/udev/rules.d/xenomai.rules<<EOF
# allow RW access to /dev/mem
KERNEL=="mem", MODE="0660", GROUP="kmem" 
# real-time heap device (Xenomai:rtheap)
KERNEL=="rtheap", MODE="0660", GROUP="xenomai"
# real-time pipe devices (Xenomai:rtpipe)
KERNEL=="rtp[0-9]*", MODE="0660", GROUP="xenomai"
EOF
```
Exit chroot
```
exit
```
Cleanup QEMU
```
sudo rm rootfs/usr/bin/qemu-arm-static
```
# Create SD card image #
This section creates a 2GB SD image file
Create sparse file
```
cd <working dir>
dd if=/dev/zero of=rpi-lcnc.img count=0 bs=1 seek=2021654528
```
Create partitions
```
sudo sh -c 'cat <<EOF | sfdisk --force rpi-lcnc.img
unit: sectors

1 : start=     2048, size=   204800, Id= c
2 : start=   206848, size=  3741696, Id=83
EOF
'
```
Format partitions
```
sudo losetup /dev/loop0 rpi-lcnc.img -o $((2048*512))
sudo mkfs.vfat -F 32 -n BOOT /dev/loop0
sudo losetup -d /dev/loop0
sudo losetup /dev/loop0 rpi-lcnc.img -o $((206848*512))
sudo mkfs.ext4 -L ROOT /dev/loop0
sudo losetup -d /dev/loop0
```
Mount partitions
```
mkdir -p mnt/{boot,root}
sudo mount -o loop,offset=$((2048*512)) rpi-lcnc.img mnt/boot
sudo mount -o loop,offset=$((206848*512)) rpi-lcnc.img mnt/root
```
Download and unzip firmware
```
wget https://github.com/raspberrypi/firmware/archive/master.zip
unzip master.zip
```
Populate ROOT
```
sudo rsync -a rootfs/ mnt/root/
sudo cp -a firmware-master/hardfp/opt/vc mnt/root/opt/
```
Populate BOOT
```
sudo cp rootfs/boot/* mnt/boot/
sudo cp firmware-master/boot/{*bin,*dat,*elf} mnt/boot/
sudo sh -c 'cat >mnt/boot/config.txt<<EOF
kernel=kernel.img
arm_freq=800
core_freq=250
sdram_freq=400
over_voltage=0
gpu_mem=16
EOF
'
sudo sh -c 'cat >mnt/boot/cmdline.txt<<EOF
dwc_otg.lpm_enable=0 root=/dev/mmcblk0p2 rootfstype=ext4 rootwait
EOF
'	
```
Unmount partitions
```
sudo umount mnt/{boot,root}
```
Compress image
```
bzip2 -9 rpi-lcnc.img
```
Install image
```
sudo sh -c 'bzcat rpi-lcnc.img.bz2 > /dev/<sdcard>'
```
# Pre-built Image #
[rpi-lcnc.2.1.img.bz2](https://docs.google.com/file/d/0B4Qqa1lYhaVBRnNCMlVfdkQ2Y00)
```
md5sum:		cb92f875a313c75c5d47a219defe33c7
username:	pi
password:	raspberry
```
# Post-install #
Configure locale
```
sudo dpkg-reconfigure locales
```
Configure timezone
```
sudo dpkg-reconfigure tzdata
```
Generate new ssh keys
```
sudo rm /etc/ssh/ssh_host_*
sudo dpkg-reconfigure openssh-server
```
# Testing #
SSH to RPi
```
ssh -X -l rpi <ip address>
```
Xenomai regression test
```
for a in irqbench rtdmtest switchtest timerbench rtipc posix klat ; \
do sudo modprobe xeno_$a; \
done
sudo xeno-regression-test -l "/usr/lib/xenomai/dohell -m /tmp 100" -t 2
```
Xenomai latency test
```
sudo sh -c 'echo 3500 > /proc/xenomai/latency'
/usr/xenomai/bin/latency
```
LinuxCNC latency test
```
latency-test 100us 1ms
```
# References #
http://wiki.debian.org/Debootstrap

http://www.blaess.fr/christophe/2012/08/27/xenomai-sur-raspberry-pi/

http://www.raspberrypi.org/phpBB3/viewtopic.php?f=41&t=12368

http://www.raspberrypi.org/phpBB3/viewtopic.php?f=37&t=14122

http://wrttn.in/59e640
