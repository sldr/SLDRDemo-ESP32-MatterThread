#include <stdio.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <esp_system.h>

#define TAG "SLDRDemo"

void app_main(void)
{
    ESP_LOGI(TAG, "app_main starting");

    // Log chip information
    esp_chip_info_t chip_info;
    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    uint32_t id, size;
    esp_chip_info(&chip_info);
    ESP_LOGI(TAG, "Using %s chip with %d CPU core(s), %s%s%s%s, silicon revision v%d.%d", 
        CONFIG_IDF_TARGET,
        chip_info.cores,
        (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
        (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
        (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
        (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "",
        major_rev,
        minor_rev
    );
    // Log Memory information
    ESP_ERROR_CHECK(esp_flash_read_id(NULL, &id));
    ESP_ERROR_CHECK(esp_flash_get_size(NULL, &size));
    ESP_LOGI(TAG, "Flash ID %" PRIu32 ", Flash Size %" PRIu32 "MB", id, size / (uint32_t)(1024 * 1024));
    ESP_LOGI(TAG, "Minimum free heap size: %" PRIu32 " bytes", esp_get_minimum_free_heap_size());

    // Initialize NVS (Non-Volatile Storage)
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "nvs_flash_init completed");

    ESP_LOGI(TAG, "app_main finished");
}
