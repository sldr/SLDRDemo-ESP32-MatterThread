#include <stdio.h>
#include <esp_log.h>
#include <nvs_flash.h>

#define TAG "SLDRDemo"

void app_main(void)
{
    ESP_LOGI(TAG, "app_main starting");

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "nvs_flash_init completed");

    ESP_LOGI(TAG, "app_main finished");
}
