#include <led_strip.h>
#include <esp_matter.h>

void SLDRDemo_configure_led(void);
void SLDRDemo_clreate_led_blink_task(void);
void SLDRDemo_set_on_off_light_endpoint_id(uint16_t endpoint_id);
esp_err_t SLDRDemo_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data);
esp_err_t SLDRDemo_identification_start(uint16_t endpoint_id, uint8_t effect_id, uint8_t effect_variant, void *priv_data);
esp_err_t SLDRDemo_identification_stop(uint16_t endpoint_id, uint8_t effect_id, uint8_t effect_variant, void *priv_data);

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
