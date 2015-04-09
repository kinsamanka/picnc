![![](http://imageshack.us/scaled/landing/268/img00000012gd.jpg)](http://img268.imageshack.us/img268/5665/img00000012gd.jpg)

# Disclaimer #
The instructions posted here can potentially ruin your RPI if you messed up the GPIO connections. Do this at your own risk.
### Introduction ###
OpenOCD together with RPi GPIO is used to program the PIC32 chip for those who doesn't have the Pickit2/3 programmer.

The following steps were performed within RPi using Raspbian image.
### Installing OpenOCD ###
Install required software
```
sudo apt-get install libjim-dev libtool automake texinfo
```
Make **/usr/src** directory writable to users
```
sudo chmod a+rw /usr/src
```
Download OpenOCD
```
cd /usr/src
git clone git://git.code.sf.net/p/openocd/code openocd-code
```
Compile and install
```
cd openocd-code
git checkout fe97fab -b rpi
./bootstrap
./configure --enable-maintainer-mode --disable-werror --enable-sysfsgpio
make
sudo make install
```
Create configuration files
```
sudo sh -c 'cat>/usr/local/share/openocd/scripts/interface/rpi.cfg<<EOF
#
# Config for using RaspberryPi'"'"'s expansion header
#
# This is best used with a fast enough buffer but also
# is suitable for direct connection if the target voltage
# matches RPi'"'"'s 3.3V
#
# Do not forget the GND connection, pin 6 of the expansion header.
#

interface sysfsgpio

# Each of the JTAG lines need a gpio number set: tck tms tdi tdo
# Header pin numbers: 19 26 21 23
sysfsgpio_jtag_nums 10 7 9 11

# At least one of srst or trst needs to be specified
# Header pin numbers: TRST - 15
sysfsgpio_trst_num 22
EOF'
sudo sh -c 'cat>/usr/local/share/openocd/scripts/board/pic32mx150f128b.cfg<<EOF
set CPUTAPID 0x14D06053
source [find target/pic32mx.cfg]
EOF'
sudo sh -c 'cat>/usr/local/share/openocd/scripts/board/pic32mx764f128h.cfg<<EOF
set CPUTAPID 0x1440B053
source [find target/pic32mx.cfg]
EOF'
```
### JTAG Connections ###
It is preferable that the PIC32 is programmed outside of the circuit using the recommended minimal circuit:

![http://imageshack.us/scaled/landing/23/minimalpic32.png](http://imageshack.us/scaled/landing/23/minimalpic32.png)


Connect the RPi P1 header pins to the target PIC32 using the following table:


| **JTAG** | **P1 Header** | **PIC32MX1xxB** | **64 pin PIC32** |
|:---------|:--------------|:----------------|:-----------------|
| tck | 19 | 17 | 27 |
| tms | 26 | 22 | 23 |
| tdi | 21 | 16 | 28 |
| tdo | 23 | 18 | 24 |
| trst | 15 |  1 | 7 |
| VDD | 17 | 13,28 | 10,19,26,38,57 |
| VSS | 25 | 8,19,27| 9,20,25,41 |


RPi GPIO Pinout


![http://elinux.org/images/6/61/RPi_P1_header.png](http://elinux.org/images/6/61/RPi_P1_header.png)

![http://elinux.org/images/2/2a/GPIOs.png](http://elinux.org/images/2/2a/GPIOs.png)


### Programming ###
Blank chips can be easily programmed using OpenOCD. But chips that are already programmed with the JTAG disabled in the main code (normally used to release the IO's used by JTAG) are tricky. The following bash script is needed to program both new and used chips:

---

_**NOTE:**
Replace the BRD\_CFG value with_ **pic32mx764f128h.cfg** _if you are programming the original PICnc board_

---

```
cat>pic32openocd<<EOF
#!/bin/sh
TRST_GPIO=22
BRD_CFG=pic32mx150f128b.cfg

if [ -z \$1 ]
then
  echo "firmware file is missing!"
  echo "Usage: pic32openocd <hexfile>"
  exit
fi

echo \$TRST_GPIO > /sys/class/gpio/unexport 2>/dev/null
echo \$TRST_GPIO > /sys/class/gpio/export
echo "low" > /sys/class/gpio/gpio\${TRST_GPIO}/direction
echo  "1" > /sys/class/gpio/gpio\${TRST_GPIO}/active_low

openocd  -f interface/rpi.cfg -f board/\${BRD_CFG} &
sleep 1

(
echo 'reset init'
sleep 1
echo "high" > /sys/class/gpio/gpio\${TRST_GPIO}/direction
echo  "0" > /sys/class/gpio/gpio\${TRST_GPIO}/active_low
sleep 1
echo pic32mx unlock 0
sleep 1
echo reset init
echo program \$1
) | nc localhost 4444 > /dev/null
EOF
chmod +x ./pic32openocd
sudo cp pic32openocd /usr/local/bin
```
### Caveats ###
Do not disable JTAG using the configuration bits:
```
#pragma config JTAGEN = ON         // Enable JTAG. Do not turn off!
```
Once disabled, only ICSP programming is allowed.
### Sample Session ###
```
rpi@rpi:~$ sudo /usr/local/bin/pic32openocd test.hex
Open On-Chip Debugger 0.7.0-dev-00207-gfe97fab (2013-03-31-09:49)
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.sourceforge.net/doc/doxygen/bugs.html
Info : only one transport option; autoselect 'jtag'
SysfsGPIO nums: tck = 10, tms = 7, tdi = 9, tdi = 11
SysfsGPIO num: trst = 22
adapter_nsrst_delay: 100
jtag_ntrst_delay: 100
Info : SysfsGPIO JTAG bitbang driver
Warn : gpio 22 is already exported
Info : This adapter doesn't support configurable speed
Warn : writing srst failed
Info : JTAG tap: pic32mx.cpu tap/device found: 0x14d06053 (mfg: 0x029, part: 0x4d06, ver: 0x1)
Info : accepting 'telnet' connection from 4444
Info : JTAG tap: pic32mx.cpu tap/device found: 0x14d06053 (mfg: 0x029, part: 0x4d06, ver: 0x1)
target state: halted
target halted in MIPS32 mode due to debug-request, pc: 0xbfc00000
Info : device id = 0x14d06053 (manuf 0x029 dev 0x4d06, ver 0x01)
Info : flash size = 3kbytes
pic32mx is already unlocked, erasing anyway
pic32mx unlocked.
INFO: a reset or power cycle is required for the new settings to take effect.
Info : JTAG tap: pic32mx.cpu tap/device found: 0x14d06053 (mfg: 0x029, part: 0x4d06, ver: 0x1)
target state: halted
target halted in MIPS32 mode due to debug-request, pc: 0xbfc00000
Info : JTAG tap: pic32mx.cpu tap/device found: 0x14d06053 (mfg: 0x029, part: 0x4d06, ver: 0x1)
target state: halted
target halted in MIPS32 mode due to debug-request, pc: 0xbfc00000
** Programming Started **
auto erase enabled
Info : device id = 0x14d06053 (manuf 0x029 dev 0x4d06, ver 0x01)
Info : flash size = 128kbytes
Info : Padding image section 5 with 125400 bytes
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
Info : Padding image section 7 with 880 bytes
Info : Padding image section 8 with 2148 bytes
target state: halted
target halted in MIPS32 mode due to target-not-halted, pc: 0xa000088c
wrote 131072 bytes from file test.hex in 40.309155s (3.175 KiB/s)
** Programming Finished **
shutdown command invoked
```

# Precompiled Binaries #

Untar the following file:

https://drive.google.com/uc?id=0B4Qqa1lYhaVBZ3p6RFZ0WC00elE&export=download



```
rpi@rpi:~$ sudo tar xvf openocd.tar.gz -C /
```
