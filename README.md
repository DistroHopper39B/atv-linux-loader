# Apple TV 1st Generation Example Code
This project is intended as a reference implementation of everything involved in getting code to execute on an original
Apple TV. Unlike [atv-bootloader](https://github.com/davilla/atv-bootloader), this project is permissively licensed
and will compile with the latest versions of Clang without issue.

## What's included
* Standard C string and memory manipulation functions based on [Baselibc](https://github.com/PetteriAimonen/Baselibc)
* 8x16 font and text console support with 32-bit color, line wrap, and scrolling (these functions are slow and not 
double buffered but get the job done)
* Full printf support with [tinyprintf](http://www.sparetimelabs.com/tinyprintf/tinyprintf.php)

## What's not included
* Dynamic memory allocator
* IDE or USB drivers
* Free coffee

## Testing this on your Apple TV
**Note: Due to Linux's superior handing of GPT disks with non-standard partition types I highly recommend doing development
of anything targeting the Apple TV on Linux.**

### Build `atvlib`
#### macOS
* Install the Xcode CLI tools: `xcode-select --install`
* Clone this repo and `cd` into it
* Type `make` (ignore any warnings about deprecated 32-bit support)

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
* Type `make` (ignore any warnings about `/System/Library/Frameworks`)
#### Windows
Use WSL or a VM or something, I don't know. Or just dual boot Linux.

### Format a USB drive (This should be done on Linux)
* Install [GParted](https://gparted.org/) for your distro.
* Connect a USB drive to your computer.
* Open GParted and select your USB drive.
* Go to `Device -> Create Partition Table`. If necessary, unmount existing partitions.
* Set the partition table type to `gpt` and click Apply. **WARNING: THIS WILL DELETE ALL DATA ON YOUR USB FLASH DRIVE!!!**
* Go to Partition -> New. Set the filesystem to fat32 and the label to boot.
* Apply the changes.
* Select your new partition and go to Partition -> Flags, then check the atvrecv box.
* Close GParted. The disk should show up in your file manager's device list. If it doesn't, disconnect and reconnect it.

### Gather and copy necessary files (This should be done on Linux)
* `boot.efi`:
  * Install `p7zip`
  * Download an Apple TV update image: `wget https://mesu.apple.com/data/OS/061-7495.20100210.TAVfr/2Z694-6013-013.dmg`
    (~235MB in size, SHA-1 hash of `97623d8d21bb59b0f4dc9d1b1c037f25c9fe09c3`)
  * Extract `boot.efi` from the image: `7z e 2Z694-6013-013.dmg OSBoot/System/Library/CoreServices/boot.efi`
  * Copy `boot.efi` to the root of the USB drive
* `Dummy.kext`:
  * Grab `Dummy.kext.zip` from https://github.com/DistroHopper39B/DummyKext/releases and extract it
  * On the root of the USB drive, create the `System/Library/Extensions` folders
  * Copy `Dummy.kext` to the `Extensions` folder you just created
* Copy `com.apple.Boot.plist` and `mach_kernel` to the root directory
* Connect the USB drive to the TV and power cycle it

## Configuring the Boot Process (`com.apple.Boot.plist`)
Boot-time configuration is done within the [com.apple.Boot.plist](com.apple.Boot.plist) file. The following
keys are defined within this file:

`Background Color`: Controls the background color. This is a 24-bit RGB value that must stored in decimal form, meaning
that 0 is black and 16777215 (0xFFFFFF) is white. If you completely remove this key, the default Apple grey is used.

`Boot Logo`: This is an image file displayed on the boot screen. If you completely remove this key, a grey Apple logo is
shown.

`Kernel`: This is the name of the kernel file to be loaded by `boot.efi`. `mach_kernel` is the default name.

`Kernel Flags`: This is the command line text sent to `atvlib`. Its maximum length is 1024 bytes.

