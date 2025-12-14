#include <string>
#include <esp_matter.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_chip_info.h>
#include <esp_flash.h>
#include <common_macros.h>
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include <platform/ESP32/OpenthreadLauncher.h>
#endif

#include "SLDRDemo_driver.h"

#define TAG "SLDRDemo"

constexpr auto k_timeout_seconds = 300;

std::string DeviceEventTypeInfo(const uint16_t eventType)
{
    std::string ret;
    if (chip::DeviceLayer::DeviceEventType::IsInternal(eventType)) {
        ret += "internal ";
    } else {
        ret += "public ";
    }
    if (chip::DeviceLayer::DeviceEventType::IsPlatformSpecific(eventType)) {
        ret += "platform-specific";
    } else {
        ret += "platform-generic";
    }
    return ret;
}

// This callback is invoked when clients interact with the Identify Cluster.
// In the callback implementation, an endpoint can identify itself. (e.g., by flashing an LED or light).
static esp_err_t SLDRDemo_identification_cb(esp_matter::identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id,
                                       uint8_t effect_variant, void *priv_data)
{
    esp_err_t err = ESP_OK;
    switch (type) {
        case esp_matter::identification::START:
            ESP_LOGI(TAG, "START identification on endpoint_id %u", endpoint_id);
            err = SLDRDemo_identification_start(endpoint_id, effect_id, effect_variant, priv_data);
            break;
        case esp_matter::identification::STOP:
            ESP_LOGI(TAG, "STOP identification on endpoint_id %u", endpoint_id);
            err = SLDRDemo_identification_stop(endpoint_id, effect_id, effect_variant, priv_data);
            break;
        case esp_matter::identification::EFFECT:
            ESP_LOGI(TAG, "TRIGGER EFFECT on endpoint_id %u effect_id %u effect_variant %u", endpoint_id, effect_id, effect_variant);
            break;
        default:
            ESP_LOGW(TAG, "Unknown identification CB type (%u) on endpoint_id %u", type, endpoint_id);
            break;
    }
    return err;
}


// This callback is called for every attribute update. The callback implementation shall
// handle the desired attributes and return an appropriate error code. If the attribute
// is not of your interest, please do not return an error code and strictly return ESP_OK.
static esp_err_t SLDRDemo_attribute_update_cb(esp_matter::attribute::callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                         uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
{
    esp_err_t err = ESP_OK;
    switch (type) {
        case esp_matter::attribute::PRE_UPDATE:
            ESP_LOGI(TAG, "Attribute Pre Update CB: endpoint_id=%d, cluster_id=0x%04x, attribute_id=0x%04x", endpoint_id, cluster_id, attribute_id);
            err = SLDRDemo_attribute_update(endpoint_id, cluster_id, attribute_id, val, priv_data);
            break;
        case esp_matter::attribute::POST_UPDATE:
            ESP_LOGI(TAG, "Attribute Post Update CB: endpoint_id=%d, cluster_id=0x%04x, attribute_id=0x%04x", endpoint_id, cluster_id, attribute_id);
            break;
        default:
            ESP_LOGW(TAG, "Unknown or unexpected attribute update CB type (%d): endpoint_id=%d, cluster_id=0x%04x, attribute_id=0x%04x", type, endpoint_id, cluster_id, attribute_id);
            break;
    }
    return err;
}

static void matter_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP Address changed");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        break;

    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGI(TAG, "Commissioning failed, fail safe timer expired");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning session stopped");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning window opened");
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
        break;

    case chip::DeviceLayer::DeviceEventType::kFabricRemoved:
        {
            ESP_LOGI(TAG, "Fabric removed successfully");
            if (chip::Server::GetInstance().GetFabricTable().FabricCount() == 0)
            {
                chip::CommissioningWindowManager & commissionMgr = chip::Server::GetInstance().GetCommissioningWindowManager();
                constexpr auto kTimeoutSeconds = chip::System::Clock::Seconds16(k_timeout_seconds);
                if (!commissionMgr.IsCommissioningWindowOpen())
                {
                    /* After removing last fabric, this example does not remove the Wi-Fi credentials
                     * and still has IP connectivity so, only advertising on DNS-SD.
                     */
                    CHIP_ERROR err = commissionMgr.OpenBasicCommissioningWindow(kTimeoutSeconds,
                                                    chip::CommissioningWindowAdvertisement::kDnssdOnly);
                    if (err != CHIP_NO_ERROR)
                    {
                        ESP_LOGE(TAG, "Failed to open commissioning window, err:%" CHIP_ERROR_FORMAT, err.Format());
                    }
                }
            }
        break;
        }

    case chip::DeviceLayer::DeviceEventType::kFabricWillBeRemoved:
        ESP_LOGI(TAG, "Fabric will be removed");
        break;

    case chip::DeviceLayer::DeviceEventType::kFabricUpdated:
        ESP_LOGI(TAG, "Fabric is updated");
        break;

    case chip::DeviceLayer::DeviceEventType::kFabricCommitted:
        ESP_LOGI(TAG, "Fabric is committed");
        break;

    case chip::DeviceLayer::DeviceEventType::kBLEDeinitialized:
        ESP_LOGI(TAG, "BLE deinitialized and memory reclaimed");
        break;

#ifdef CONFIG_ENABLE_ESP32_BLE_CONTROLLER
    case chip::DeviceLayer::DeviceEventType::kPlatformESP32BLECentralConnected:
        ESP_LOGI(TAG, "kPlatformESP32BLECentralConnected");
        break;

    case chip::DeviceLayer::DeviceEventType::kPlatformESP32BLECentralConnectFailed:
        ESP_LOGI(TAG, "kPlatformESP32BLECentralConnectFailed");
        break;

    case chip::DeviceLayer::DeviceEventType::kPlatformESP32BLEWriteComplete:
        ESP_LOGI(TAG, "kPlatformESP32BLEWriteComplete");
        break;

    case chip::DeviceLayer::DeviceEventType::kPlatformESP32BLESubscribeOpComplete:
        ESP_LOGI(TAG, "kPlatformESP32BLESubscribeOpComplete");
        break;

    case chip::DeviceLayer::DeviceEventType::kPlatformESP32BLEIndicationReceived:
        ESP_LOGI(TAG, "kPlatformESP32BLEIndicationReceived");
        break;
#endif // CONFIG_ENABLE_ESP32_BLE_CONTROLLER

    case chip::DeviceLayer::DeviceEventType::kESPSystemEvent:
        ESP_LOGW(TAG, "Unexpected public ESP System Event (event.Platform.ESPSystemEvent.Base=%s event.Platform.ESPSystemEvent.Id=%ld)", event->Platform.ESPSystemEvent.Base, event->Platform.ESPSystemEvent.Id);
        break;

    default:
        ESP_LOGW(TAG, "Unknown or unexpected %s chip::DeviceLayer::DeviceEventType (%d)", DeviceEventTypeInfo(event->Type).c_str(), event->Type);
        break;
    }
}

extern "C" void app_main(void)
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
    SLDRDemo_configure_led();
    ESP_LOGI(TAG, "LED configured");

    // Create the LED blink task
    SLDRDemo_clreate_led_blink_task();

    // Create a Matter node and add the mandatory Root Node device type on endpoint 0
    esp_matter::node::config_t node_config;
    esp_matter::node_t *node = esp_matter::node::create(&node_config, SLDRDemo_attribute_update_cb, SLDRDemo_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    // Create an On/Off Light endpoint and add it to the node
    esp_matter::endpoint::on_off_light::config_t on_off_light_config = {};
    esp_matter::endpoint_t* on_off_light_endpoint = esp_matter::endpoint::on_off_light::create(node, &on_off_light_config, 0, nullptr);
    ABORT_APP_ON_FAILURE(on_off_light_endpoint != nullptr, ESP_LOGE(TAG, "Failed to create on off light endpoint"));

    // Get the On/Off Light endpoint ID
    uint16_t on_off_light_endpoint_id = esp_matter::endpoint::get_id(on_off_light_endpoint);
    ESP_LOGI(TAG, "On Off Light created with endpoint_id %d", on_off_light_endpoint_id);
    SLDRDemo_set_on_off_light_endpoint_id(on_off_light_endpoint_id);

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
    /* Set OpenThread platform config */
    esp_openthread_platform_config_t config = {
        .radio_config = ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG(),
        .host_config = ESP_OPENTHREAD_DEFAULT_HOST_CONFIG(),
        .port_config = ESP_OPENTHREAD_DEFAULT_PORT_CONFIG(),
    };
    set_openthread_platform_config(&config);
#endif

    /* Matter start */
    esp_err_t err = esp_matter::start(matter_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK, ESP_LOGE(TAG, "Failed to start Matter, err:%d", err));


    ESP_LOGI(TAG, "app_main finished");
}
