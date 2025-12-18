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
#include <app_reset.h>
#include <button_types.h>

#include "gen_dependencies_version_info.h"
#include "SLDRDemo_driver.h"
#include "SLDRESPMatterUtils.h"

#define TAG "SLDRDemo"

// The Matter data model is described in ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/data_model for each version. This was written based on the 1.4.2.
// The devices (aka endpoints) are described in ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/data_model/1.4.2/device_types/device_type_ids.json along
// with the xml files for each. Each endpoint has clusters that are described in ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/data_model/1.4.2/clusters/
// cluster_ids.json along with the xml files for each. Each cluster contains attributes that are described in the cluster's xml file. Each cluster may contain
// commands that are described in the cluster's xml file. At the top is a node that holds all the endpoints and each endpoint holds clusters and each cluster
// holds attributes and commands. Node->Endpoint(s)->Cluster(s)->Attribute(s)/Command(s).
//
// Notes:
// * When the node is created (via esp_matter::node::create()), the root node endpoint (endpoint 0) is also created automatically (internally via call to
// endpoint::root_node::create()).
// * As 1 or more endpoints are created, they will be added to the node and assigned a unique endpoint ID automatically. For example:
// esp_matter::endpoint_t* on_off_light_endpoint = esp_matter::endpoint::on_off_light::create(node, ...); // creates and adds the endpoint to node
// uint16_t on_off_light_endpoint_id = esp_matter::endpoint::get_id(on_off_light_endpoint);  // get the assigned endpoint ID
// * Cluster IDs and Attribute IDs are constants that can be found in the respective cluster's xml file. Each endpoint can have the same cluster id used (like
// the Descriptor cluster ID 29) but will have different values for their attributes.
// * The node, endpoint, cluster, and attribute tend have a config type and the resulting type pointer after creation. For example, esp_matter::node::config_t
// and esp_matter::node_t* for the node, esp_matter::endpoint::on_off_light::config_t and esp_matter::endpoint_t* for the endpoint,
// esp_matter::cluster::descriptor::config_t and esp_matter::cluster_t* for the descriptor cluster, etc.
// * It is possible to add custom attributes to standard clusters but some system that is connected to this Matter device may not recognize the custom attribute.
// * It is possible to add custom clusters to a standard endpoint but some system that is connected to this Matter device may not recognize the custom cluster.


constexpr auto k_timeout_seconds = 300;

// This callback is invoked when clients interact with the Identify Cluster.
// In the callback implementation, an endpoint can identify itself. (e.g., by flashing an LED or light).
static esp_err_t SLDRDemo_identification_cb(esp_matter::identification::callback_type_t type, chip::EndpointId endpoint_id, uint8_t effect_id,
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
static esp_err_t SLDRDemo_attribute_update_cb(esp_matter::attribute::callback_type_t type, chip::EndpointId endpoint_id, chip::ClusterId cluster_id,
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
    ESP_LOGI(TAG, "%s", sldr::GetDeviceEventTypeDetails(event).c_str());
    switch (event->Type) {
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
    // Log Other information
    ESP_LOGI(TAG, "FreeRTOS Version Number (task): %s", tskKERNEL_VERSION_NUMBER);
    ESP_LOGI(TAG, "ESP-Matter: V%s", GEN_DEP_ESP_MATTER_VERSION);

    // Initialize NVS (Non-Volatile Storage)
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "nvs_flash_init completed");

    // Configure the LED
    SLDRDemo_configure_led();
    ESP_LOGI(TAG, "LED configured");

    // Create the LED blink task
    SLDRDemo_clreate_blink_led_task();

    // Init the button and use long press for reset on button
    button_handle_t button_handle = SLDRDemo_button_init();
    app_reset_button_register(button_handle);
    
    // Create a Matter node and add the mandatory Root Node device type on endpoint 0
    esp_matter::node::config_t node_config;
    esp_matter::node_t *node = esp_matter::node::create(&node_config, SLDRDemo_attribute_update_cb, SLDRDemo_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    // Create an On/Off Light endpoint and add it to the node
    esp_matter::endpoint::on_off_light::config_t on_off_light_config = {};
    esp_matter::endpoint_t* on_off_light_endpoint = esp_matter::endpoint::on_off_light::create(node, &on_off_light_config, 0, nullptr);
    ABORT_APP_ON_FAILURE(on_off_light_endpoint != nullptr, ESP_LOGE(TAG, "Failed to create on off light endpoint"));

    // Get the On/Off Light endpoint ID
    chip::EndpointId on_off_light_endpoint_id = esp_matter::endpoint::get_id(on_off_light_endpoint);
    ESP_LOGI(TAG, "On Off Light created with endpoint_id %d", on_off_light_endpoint_id);
    SLDRDemo_set_on_off_light_endpoint_id(on_off_light_endpoint_id);

    // Mark deferred persistence for some attributes that might be changed rapidly
    esp_matter::attribute_t *attribute = esp_matter::attribute::get(on_off_light_endpoint_id, chip::app::Clusters::OnOff::Id, chip::app::Clusters::OnOff::Attributes::OnOff::Id);
    esp_matter::attribute::set_deferred_persistence(attribute);
    SLDRDemo_set_blink_led_enable(SLDRDemo_get_on_off_light(attribute));

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
