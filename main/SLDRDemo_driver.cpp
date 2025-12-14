#include <esp_log.h>
#include <esp_matter.h>

#include "SLDRDemo_driver.h"

#define TAG "SLDRDemo_driver"

static led_strip_handle_t led_strip;
static bool enable_blink = true;
static uint8_t s_led_state = 0;
static TaskHandle_t led_blink_task_handle = NULL;
static uint16_t on_off_light_endpoint_id = UINT16_MAX;

static void blink_led(void)
{
    /* If the addressable LED is enabled */
    if (enable_blink && s_led_state) {
        led_strip_set_pixel(led_strip, 0, 22, 22, 22);
        led_strip_refresh(led_strip); // Refresh the strip to send data
    } else {
        /* Set all LED off to clear all pixels */
        led_strip_clear(led_strip);
    }
}

static void led_blink_task(void *arg)
{
    // Blink the LED in a loop
    ESP_LOGI(TAG, "Starting LED blink loop");
    while (true) {
        if (enable_blink) {
            if (s_led_state) {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                //ESP_LOGI(TAG, "LED ON");
            } else {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                //ESP_LOGI(TAG, "LED OFF");
            }
        } else {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
    }
}

void SLDRDemo_configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED using RMT_CONFIG!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = CONFIG_BSP_LED_RGB_GPIO,
        .max_leds = 1, // at least one LED on board
    };
    led_strip_rmt_config_t rmt_config = {};
    rmt_config.resolution_hz = 10 * 1000 * 1000; // 10MHz
    rmt_config.flags.with_dma = false;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

void SLDRDemo_clreate_led_blink_task(void)
{
    ESP_LOGI(TAG, "calling xTaskCreate for led_blink_task");
    xTaskCreate(led_blink_task, "led_blink_task", 2048, NULL, tskIDLE_PRIORITY, &led_blink_task_handle);
}

void SLDRDemo_set_on_off_light_endpoint_id(uint16_t endpoint_id)
{
    on_off_light_endpoint_id = endpoint_id;
}

esp_err_t SLDRDemo_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == on_off_light_endpoint_id && cluster_id == chip::app::Clusters::OnOff::Id && attribute_id == chip::app::Clusters::OnOff::Attributes::OnOff::Id) {
        bool on_off = val->val.b;
        ESP_LOGI(TAG, "On/Off attribute updated to %s", on_off ? "ON" : "OFF");
        enable_blink = on_off;
        if (on_off) {
            s_led_state = false; // So it turns on in next blink cycle
        } else {
            blink_led(); // Turn off immediately
        }
    }
    return err;
}

esp_err_t SLDRDemo_identification_start(uint16_t endpoint_id, uint8_t effect_id, uint8_t effect_variant, void *priv_data)
{
    if (endpoint_id == on_off_light_endpoint_id) {
        ESP_LOGI(TAG, "Identification Start requested");
        // Do Nothing for now
    }
    return ESP_OK;
}

esp_err_t SLDRDemo_identification_stop(uint16_t endpoint_id, uint8_t effect_id, uint8_t effect_variant, void *priv_data)
{
    if (endpoint_id == on_off_light_endpoint_id) {
        ESP_LOGI(TAG, "Identification Stop requested");
        // Do Nothing for now
    }
    return ESP_OK;
}
