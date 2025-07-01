# Modern Linux on the Original Apple TV
This bootloader will allow you to boot Linux on an Apple TV (1st generation) and Intel Macs with 32-bit EFI (though for
all but the MacBook2,1 consider using UEFI GRUB). Unlike [atv-bootloader](https://github.com/davilla/atv-bootloader) this
bootloader will compile with modern Clang and boot distributions with giant ramdisks such as [Debian](https://debian.org).

## How this loader works
The original Apple TV is an EFI-based machine like Intel Macs, but can only boot Apple EFI files. 

## Installing this on your Apple TV
**Note: Due to Linux's superior handing of GPT disks with non-standard partition types I highly recommend setting this
up on Linux.**

### Installing Linux
Use a VM to install a 32-bit Linux distro (I recommend Alpine Linux, but Debian will work too). Image it
over to (or directly install it to) either a USB flash drive or IDE hard drive.

### Formatting the USB/Hard Drive (this should be done on Linux)
* Install [GParted](https://gparted.org/) for your distro.
* Connect a USB drive to your computer.
* Open GParted and select your USB drive.
* Resize the existing Linux partition. 
* Go to `Partition -> New`. Set the filesystem to `fat32` and the label to `boot`.
* Apply the changes.
* Select your new partition and go to `Partition -> Flags`, then check the `atvrecv` box.
* Close GParted. The new partition should show up in your file manager's device list. If it doesn't, disconnect and
reconnect it.

### Build `linux-loader-appletv`
#### macOS
* Install the Xcode CLI tools: `xcode-select --install`
* Clone this repo and `cd` into it
* Type `make KERNEL=/path/to/vmlinuz INITRAMFS=/path/to/initrd`, using the paths to the Linux kernel and initramfs from
the installation you just made (ignore any warnings about deprecated 32-bit support)

#### Linux
* Install Clang, autoconf, automake, and libtool
* Compile version 986 of [cctools-port](https://github.com/tpoechtrager/cctools-port) (newer versions work but are more
complicated to set up)
```
git clone https://github.com/tpoechtrager/cctools-port.git -b 986-ld64-711
cd cctools-port/cctools
./configure --prefix=/opt/cross --target=i386-apple-darwin8
make -j$(nproc)
sudo make install
```
* Clone this repo and `cd` into it
* Type `make KERNEL=/path/to/vmlinuz INITRAMFS=/path/to/initrd`, using the paths to the Linux kernel and initramfs from
the installation you just made (ignore any warnings about `/System/Library/Frameworks`)
#### Windows
Use WSL or a VM or something, I don't know. Or just dual boot Linux.

### Gather and copy necessary files (This should be done on Linux)
* `boot.efi`:
  * Install `p7zip`
  * Download an Apple TV update image: `wget https://mesu.apple.com/data/OS/061-7495.20100210.TAVfr/2Z694-6013-013.dmg`
    (~235MB in size, SHA-1 hash of `97623d8d21bb59b0f4dc9d1b1c037f25c9fe09c3`)
  * Extract `boot.efi` from the image: `7z e 2Z694-6013-013.dmg OSBoot/System/Library/CoreServices/boot.efi`
  * Copy `boot.efi` to the boot partition you just created on the USB drive
* `Dummy.kext`:
  * Grab `Dummy.kext.zip` from https://github.com/DistroHopper39B/DummyKext/releases and extract it
  * On the boot partition on the USB drive, create the `System/Library/Extensions` folders
  * Copy `Dummy.kext` to the `Extensions` folder you just created
* Copy `com.apple.Boot.plist` and `mach_kernel` to the root of the partition

### Configure the kernel parameters
In order to tell Linux what disk to boot from, among other things, you'll need to copy the kernel parameters from the
stock bootloader to this one. These parameters are usually stored in `/boot/grub/grub.cfg` or `/boot/syslinux/syslinux.cfg`.
Find the line with `root=UUID=` or something like that, and copy everything after the path to the Linux kernel. Paste it
into the `Kernel Flags` key in `com.apple.Boot.plist`.

With all of this done, turn on your Apple TV and Linux should boot!

## Configuring the Boot Process (`com.apple.Boot.plist`)
Boot-time configuration is done within the [com.apple.Boot.plist](com.apple.Boot.plist) file. The following
keys are defined within this file:

`Background Color`: Controls the background color. This is a 24-bit RGB value that must stored in decimal form, meaning
that 0 is black and 16777215 (0xFFFFFF) is white. If you completely remove this key, the default Apple grey is used.

`Boot Logo`: This is an image file displayed on the boot screen. If you completely remove this key, a grey Apple logo is
shown.

`Kernel`: This is the name of the kernel file to be loaded by `boot.efi`. `mach_kernel` is the default name.

`Kernel Flags`: This is the command line text sent to the Linux kernel. Its maximum length is 1024 bytes.

