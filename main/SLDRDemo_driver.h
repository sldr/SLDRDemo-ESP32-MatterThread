#include <led_strip.h>
#include <esp_matter.h>
#include <button_types.h>

void SLDRDemo_configure_led(void);
void SLDRDemo_clreate_blink_led_task(void);
void SLDRDemo_set_on_off_light_endpoint_id(chip::EndpointId endpoint_id);
void SLDRDemo_set_blink_led_enable(bool enable);
esp_err_t SLDRDemo_attribute_update(chip::EndpointId endpoint_id, chip::ClusterId cluster_id, chip::AttributeId attribute_id, esp_matter_attr_val_t *val, void *priv_data);
esp_err_t SLDRDemo_identification_start(chip::EndpointId endpoint_id, uint8_t effect_id, uint8_t effect_variant, void *priv_data);
esp_err_t SLDRDemo_identification_stop(chip::EndpointId endpoint_id, uint8_t effect_id, uint8_t effect_variant, void *priv_data);
button_handle_t SLDRDemo_button_init();
bool SLDRDemo_get_on_off_light(esp_matter::attribute_t *attribute);
bool SLDRDemo_get_on_off_light(void);
void SLDRDemo_set_on_off_light(esp_matter::attribute_t *attribute, bool on);
void SLDRDemo_set_on_off_light(bool on);
void SLDRDemo_on_off_light_toggle(esp_matter::attribute_t *attribute);
void SLDRDemo_on_off_light_toggle(void);

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG()                                           \
    {                                                                                   \
        .radio_mode = RADIO_MODE_NATIVE,                                                \
    }

#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG()                                            \
    {                                                                                   \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE,                              \
    }

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG()                                            \
    {                                                                                   \
        .storage_partition_name = "nvs", .netif_queue_size = 10, .task_queue_size = 10, \
    }
#endif
