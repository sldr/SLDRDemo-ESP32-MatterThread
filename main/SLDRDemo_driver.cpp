#include <esp_log.h>
#include <esp_matter.h>

#include "SLDRDemo_driver.h"

#define TAG "SLDRDemo_driver"

static led_strip_handle_t led_strip;
static bool enable_blink = true;
static uint8_t s_led_state = 0;
static TaskHandle_t led_blink_task_handle = NULL;
static uint16_t on_off_light_endpoint_id = UINT16_MAX;
static bool identification_running = false;
static constexpr auto blink_ms = 1000;
static constexpr auto identification_blink_ms = blink_ms / 4;

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
    /* Toggle the LED state */
    s_led_state = !s_led_state;
}

static void blink_led_task(void *arg)
{
    // Blink the LED in a loop
    static TickType_t delay_ms = blink_ms;
    ESP_LOGI(TAG, "Starting LED blink loop");
    while (true) {
        if (identification_running) {
            //ESP_LOGI(TAG, "ID ON");
            delay_ms = identification_blink_ms;
        } else {
            delay_ms = blink_ms;
        }
        if (!enable_blink && !identification_running) {
            // Always off if not enabled and not identifying (Don't move to after delay)
            s_led_state = false;
        }
        vTaskDelay(delay_ms / portTICK_PERIOD_MS);
        if (enable_blink || identification_running) {
            if (s_led_state) {
                //ESP_LOGI(TAG, "LED ON");
            } else {
                //ESP_LOGI(TAG, "LED OFF");
            }
        } else {
            //ESP_LOGI(TAG, "LED Diabled");
        }
        blink_led();
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

void SLDRDemo_clreate_blink_led_task(void)
{
    ESP_LOGI(TAG, "calling xTaskCreate for led_blink_task");
    xTaskCreate(blink_led_task, "led_blink_task", 2048, NULL, tskIDLE_PRIORITY, &led_blink_task_handle);
}

void SLDRDemo_set_on_off_light_endpoint_id(uint16_t endpoint_id)
{
    on_off_light_endpoint_id = endpoint_id;
}

esp_err_t SLDRDemo_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;
    if (endpoint_id == on_off_light_endpoint_id && cluster_id == chip::app::Clusters::OnOff::Id && attribute_id == chip::app::Clusters::OnOff::Attributes::OnOff::Id) {
        enable_blink = val->val.b;
        ESP_LOGI(TAG, "On/Off attribute updated to %s", enable_blink ? "ON" : "OFF");
        if (!identification_running) {
            // Force it to the new state immediately if not in identification mode
            s_led_state = enable_blink; // Set led state to on/off to to go to that immediately
            xTaskAbortDelay(led_blink_task_handle); // Wake up the blink task immediately (ignore return status if not in delay state)
        }
    }
    return err;
}

esp_err_t SLDRDemo_identification_start(uint16_t endpoint_id, uint8_t effect_id, uint8_t effect_variant, void *priv_data)
{
    if (endpoint_id == on_off_light_endpoint_id) {
        ESP_LOGI(TAG, "Identification Start requested");
        identification_running = true;
        // Force it to the new state immediately
        s_led_state = true; // Set led state to on to to go to that immediately
        xTaskAbortDelay(led_blink_task_handle); // Wake up the blink task immediately (ignore return status if not in delay state)
    }
    return ESP_OK;
}

esp_err_t SLDRDemo_identification_stop(uint16_t endpoint_id, uint8_t effect_id, uint8_t effect_variant, void *priv_data)
{
    if (endpoint_id == on_off_light_endpoint_id) {
        ESP_LOGI(TAG, "Identification Stop requested");
        identification_running = false;
        if (!enable_blink) {
            s_led_state = false; // Set led state to off to to go to that immediately
            xTaskAbortDelay(led_blink_task_handle); // Wake up the blink task immediately (ignore return status if not in delay state)
        }
    }
    return ESP_OK;
}
