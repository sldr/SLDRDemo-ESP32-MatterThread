#pragma once

// Override vendor name reported by CHIP/ Matter (not in sdkconfig)
#define CHIP_DEVICE_CONFIG_DEVICE_VENDOR_NAME "SLDR"

// Override product name reported by CHIP/ Matter (not in sdkconfig)
#define CHIP_DEVICE_CONFIG_DEVICE_PRODUCT_NAME "SLDR Demo Light"

// Override hardware version reported by CHIP/ Matter (not in sdkconfig but CONFIG_DEFAULT_DEVICE_HARDWARE_VERSION is)
#if defined(CONFIG_IDF_TARGET_ESP32H2)
#define CHIP_DEVICE_CONFIG_DEFAULT_DEVICE_HARDWARE_VERSION_STRING "ESP32-H2 1.1"
#elif defined(CONFIG_IDF_TARGET_ESP32C6)
#define CHIP_DEVICE_CONFIG_DEFAULT_DEVICE_HARDWARE_VERSION_STRING "ESP32-C6 1.1"
#else
#define CHIP_DEVICE_CONFIG_DEFAULT_DEVICE_HARDWARE_VERSION_STRING "ESP32-?? 1.1"
#endif
