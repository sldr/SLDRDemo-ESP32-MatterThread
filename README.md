# SLDRDemo-ESP32-MatterThread
This walks thru creating a project from scratch using the ESP-IDF and ESP-Matter.

## Building
My platform is Ubuntu 24.04 LTS (specifically "24.04.3 LTS (Noble Numbat)").

*I tried to get Windows to work but it would not work. I beleive it may just be long path names (ESP Matter requires it) but I have not jumped back on that.*
### Prerequsites
Install ESPRESSIF SDKs along with their prerequsites.

1. First install the prerequisites for ESP IDF by following the instructions at [https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/get-started/linux-macos-setup.html](https://docs.espressif.com/projects/esp-idf/en/v5.4.1/esp32c6/get-started/linux-macos-setup.html). NOTE: The link is for 5.4.1 but newer 5.4.x releases exist so you may want to use the later release (at this writing 5.5.1 is latest stable).

   **Known Working SDKs**
   |**SDK**|**Version**|
   |:--|:-:|
   |ESP IDF   |5.4.1|
   |ESP Matter|1.4.2|

1. Install the ESP-IDF and the ESP-Matter SDKs by following the instructions at [https://docs.espressif.com/projects/esp-matter/en/release-v1.4.2/esp32c6/developing.html](https://docs.espressif.com/projects/esp-matter/en/release-v1.4.2/esp32c6/developing.html) for all of section 1 (ESP-IDF Setup) and 2 (ESP Matter Setup) - 2.1.2 (Configuring the Environment).

   **NOTES:**
   * Section 2.1.2 will need to be run every time you login (or do the needfull).
   * Using `ccache` will speed up your compiles and it worked for me.
   * Read all of 2.1.1 before running any commands. I prefered not to do the `--depth 1` to git it all. This *will* take longer. I added `--jobs <n>` to the clone command to help.

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

