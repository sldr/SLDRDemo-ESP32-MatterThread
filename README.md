# SLDRDemo-ESP32-MatterThread
This walks thru creating a project from scratch using the ESP-IDF (ESP IoT Development Framework) and ESP-Matter. If you go thru the commit history you will see the progression from the first `idf.py create-project SLDRDemo` command that created something that would flash to an ESP32-C6 board (it wouldn't do much and didn't have any Matter or Thread internals yet). All the way to a functioning Matter over Thread device.

The goal of this is to help other home users be able to learn and create their own home automation devices. The documentation is sparse from many of the specifications and technologies that had to come togather to create devices that can participate in home automation using Matter over Thread.

## Scope
This demo is scoped for a home user wanting to create a device that they can connect to the their home automation system. As such some security issues will be ignored like using a test product ID and test vendor ID as well as getting a comercial certificate (that would require passing verification tests etc). See the Security section for more security details that a home user should be aware of.

Comercial users are welcome to use this to learn but please understand that I didn't write this for making a comercially sold secure product and lots of changes would need to be done to turn this into a comercially sold secure product.

## Hardware
I have tested this on the following boards:
| Board Model | Board Maker | Flash Size (MB) | ESP Chip Model | Link | Notes |
|---|---|:-:|---|:-:|:-:|
ESP32-C6-DevKitM-1 | ESPRESSIF | 4 | ESP32-C6-MINI-1 | | |
ESP32-H2-DevKitM-1 | ESPRESSIF | 4 | ESP32-H2-MINI-1 | | |
135349G_Y96 (ESP32-C6-DevKitM-1) | DORHEA (??) | 8 | ESP32-C6-WROOM-1 | | 1 |
ESP32-C6-Zero | Wonrabai / Waveshare | 4 | ESP32-C6FH4 | [:link:](https://m.media-amazon.com/images/I/61oMTuD51EL._AC_SL1000_.jpg) | |

Notes:
1. The board PINs DO NOT match the ESPRESSIF ESP32-C6-DevKitM-1 board even thou DORHEA (maybe distributer) sells as board model ESP32-C6-DevKitM-1.

## Prerequsites
Install ESPRESSIF SDKs along with their prerequsites.

**Known Working SDKs**
|**SDK**|**Version**|
|:--|:-:|
|ESP IDF   |5.4.1|
|ESP Matter|1.4.2|

1. Install ESP IDF by following the instructions at [https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/get-started/linux-macos-setup.html](https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/get-started/linux-macos-setup.html). Stop right before you do Step 5.
 
   **NOTES:** 
   * The link is for 5.4.1 but newer 5.4.x releases exist so you may want to use the later release (at this writing 5.5.1 is latest stable).
   * On step 3 choose the right chip targets or do all `./install.sh all`.

1. Install ESP-Matter by following the instructions at [https://docs.espressif.com/projects/esp-matter/en/release-v1.4.2/esp32c6/developing.html](https://docs.espressif.com/projects/esp-matter/en/release-v1.4.2/esp32c6/developing.html) for all of section 1 (ESP-IDF Setup) and 2 (ESP Matter Setup) - 2.1.2 (Configuring the Environment).

   **NOTES:**
   * Section 2.1.2 will need to be run every time you login (or do the needfull).
   * Using `ccache` will speed up your compiles and it worked for me.
   * Read all of 2.1.1 before running any commands. I prefered not to do the `--depth 1` to git it all. This *will* take longer. I added `--jobs <n>` to the clone command to help.

## Building
My platform is Ubuntu 24.04 LTS (specifically "24.04.3 LTS (Noble Numbat)"). Other platforms should work but I only Ubuntu 24.04 LTS.

*I tried to get Windows to work but it would not work. I beleive it may just be long path names (ESP Matter requires it) but I have not jumped back on that.*
### Actual Build Commands
These are the commands you will use to build. Most of the time you will only need to run the `set-target` command once. If you modify the sdkconfig.defaults* you will need to run it again. If you change the ESP32-C6 to ESP32-H2 or reverse you will need to run it again.
```bash
idf.py set-target esp32c6
idf.py build
idf.py -p /dev/ttyACM0 erase-flash
idf.py -p /dev/ttyACM0 flash
idf.py -p /dev/ttyACM0 monitor
```
You can combine the last three commands into one:
```bash
idf.py -p /dev/ttyACM0 erase-flash flash monitor
```
**NOTE:** Change `/dev/ttyACM0` to the right device for your platform.

When you update the code and don't want to re-pair it again to your home automation system skip the `erase-flash` command and assuming you didn't change the partition information too it should come right up with the new code.

## Pairing

Use your standard procedure for adding a new Matter device to your home automation system. When you need to take a picture of the QR code for the new device take a picture of the image below:
![defult QR](https://docs.espressif.com/projects/esp-matter/en/release-v1.4.2/esp32c6/_images/matter_qrcode_20202021_3840.png)

More details on comissioning can be found at [https://docs.espressif.com/projects/esp-matter/en/release-v1.4.2/esp32c6/developing.html#commissioning](https://docs.espressif.com/projects/esp-matter/en/release-v1.4.2/esp32c6/developing.html#commissioning).

## Security
TBA

## What SLDRDemo does
Once paired the light on/off controls the boards built in LED. Off turns the LED light off. On turns on the blinking of the LED at 1 sec duration on followed by 1 sec duration off and repeat. Pressing the Identify diagnostic cause the LED to fast blink at 1/4 sec durations for the specified identify time duration.

A short press on the Reset button on the board causes the on/off to toggle it's state. A long press causes the board to reset (ie: reboot).

NOTE: The boot button is not monitored or controled by the program but pressing it will cause the board to reboot.