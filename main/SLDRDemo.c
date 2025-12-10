#include <stdio.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <esp_system.h>
#include <led_strip.h>

#define TAG "SLDRDemo"

static led_strip_handle_t led_strip;
static uint8_t s_led_state = 0;

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        led_strip_set_pixel(led_strip, 0, 22, 22, 22);
        led_strip_refresh(led_strip); // Refresh the strip to send data
    } else {
        /* Set all LED off to clear all pixels */
        led_strip_clear(led_strip);
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED using RMT_CONFIG!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = CONFIG_BSP_LED_RGB_GPIO,
        .max_leds = 1, // at least one LED on board
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

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

    // Configure the LED
    configure_led();
    ESP_LOGI(TAG, "LED configured");

    // Blink the LED in a loop
    ESP_LOGI(TAG, "Starting LED blink loop");
    while (true) {
        if (s_led_state) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            //ESP_LOGI(TAG, "LED ON");
        } else {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            //ESP_LOGI(TAG, "LED OFF");
        }
        blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
    }

    ESP_LOGI(TAG, "app_main finished");
}
