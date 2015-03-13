The PIC32 firmware is compiled using the compiler provided by MPIDE running on an x86 PC.

## Steps ##

  * Download the 20130626 version of the  [MPIDE compiler](http://chipkit.net/started/install-chipkit-software/). (Archive versions are located [here](http://chipkit.s3.amazonaws.com/index.html)).
  * Extract and move the compiler to `/usr/local`
```

tar --wildcards -xvzf mpide-0023-linux-*.tgz  'mpide-*/hardware/pic32/compiler/pic32-tools'
sudo mv mpide-*/hardware/pic32/compiler/pic32-tools /usr/local
rm  mpide-*/ -r
```
  * Done!