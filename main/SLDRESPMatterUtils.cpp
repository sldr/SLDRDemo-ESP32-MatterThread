#include <sstream>

#include "SLDRESPMatterUtils.h"

namespace sldr {

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

    std::string_view GetDeviceEventTypeName(const ChipDeviceEvent* event)
    {
        switch (event->Type) {
            case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
                return "Commissioning session started";
            case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
                return "Commissioning session stopped";
            case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
                return "Commissioning window opened";
            case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
                return "Commissioning window closed";
            case chip::DeviceLayer::DeviceEventType::kFabricRemoved:
                return "Fabric removed successfully";
            case chip::DeviceLayer::DeviceEventType::kFabricWillBeRemoved:
                return "Fabric will be removed";
            case chip::DeviceLayer::DeviceEventType::kFabricUpdated:
                return "Fabric is updated";
            case chip::DeviceLayer::DeviceEventType::kFabricCommitted:
                return "Fabric is committed";
#ifdef CONFIG_ENABLE_ESP32_BLE_CONTROLLER
            case chip::DeviceLayer::DeviceEventType::kPlatformESP32Event:
                return "kPlatformESP32Event";
            case chip::DeviceLayer::DeviceEventType::kPlatformESP32BLECentralConnected:
                return "kPlatformESP32BLECentralConnected";
            case chip::DeviceLayer::DeviceEventType::kPlatformESP32BLECentralConnectFailed:
                return "kPlatformESP32BLECentralConnectFailed";
            case chip::DeviceLayer::DeviceEventType::kPlatformESP32BLEWriteComplete:
                return "kPlatformESP32BLEWriteComplete";
            case chip::DeviceLayer::DeviceEventType::kPlatformESP32BLESubscribeOpComplete:
                return "kPlatformESP32BLESubscribeOpComplete";
            case chip::DeviceLayer::DeviceEventType::kPlatformESP32BLEIndicationReceived:
                return "kPlatformESP32BLEIndicationReceived";
#endif // CONFIG_ENABLE_ESP32_BLE_CONTROLLER
            case chip::DeviceLayer::DeviceEventType::kWiFiConnectivityChange:
                return "WiFi Connectivity changed";
            case chip::DeviceLayer::DeviceEventType::kThreadConnectivityChange:
                return "Thread Connectivity changed";
            case chip::DeviceLayer::DeviceEventType::kInternetConnectivityChange:
                return "Internet Connectivity changed";
            case chip::DeviceLayer::DeviceEventType::kServiceConnectivityChange:
                return "Service Connectivity changed";
            case chip::DeviceLayer::DeviceEventType::kServiceProvisioningChange:
                return "Service Provisioning changed";
            case chip::DeviceLayer::DeviceEventType::kTimeSyncChange:
                return "Time Sync changed";
            case chip::DeviceLayer::DeviceEventType::kCHIPoBLEConnectionEstablished:
                return "CHIPoBLE Connection Established";
            case chip::DeviceLayer::DeviceEventType::kCHIPoBLEConnectionClosed:
                return "CHIPoBLE Connection Closed";
            case chip::DeviceLayer::DeviceEventType::kCloseAllBleConnections:
                return "Request to close all BLE connections";
            case chip::DeviceLayer::DeviceEventType::kWiFiDeviceAvailable:
                return "WiFi Device Available";
            case chip::DeviceLayer::DeviceEventType::kOperationalNetworkStarted:
                return "Operational Network Started";
            case chip::DeviceLayer::DeviceEventType::kThreadStateChange:
                return "Thread State changed";
            case chip::DeviceLayer::DeviceEventType::kThreadInterfaceStateChange:
                return "Thread Interface State changed";
            case chip::DeviceLayer::DeviceEventType::kCHIPoBLEAdvertisingChange:
                return "CHIPoBLE Advertising changed";
            case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
                return "Interface IP Address changed";
            case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
                return "Commissioning complete";
            case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
                return "Commissioning failed, fail safe timer expired";
            case chip::DeviceLayer::DeviceEventType::kOperationalNetworkEnabled:
                return "Operational Network Enabled";
            case chip::DeviceLayer::DeviceEventType::kDnssdInitialized:
                return "DNS-SD Initialized";
            case chip::DeviceLayer::DeviceEventType::kDnssdRestartNeeded:
                return "DNS-SD Restart Needed";
            case chip::DeviceLayer::DeviceEventType::kBindingsChangedViaCluster:
                return "Bindings Changed Via Cluster";
            case chip::DeviceLayer::DeviceEventType::kOtaStateChanged:
                return "OTA State Changed";
            case chip::DeviceLayer::DeviceEventType::kServerReady:
                return "Server Ready";
            case chip::DeviceLayer::DeviceEventType::kBLEDeinitialized:
                return "BLE deinitialized and memory reclaimed";
            case chip::DeviceLayer::DeviceEventType::kESPSystemEvent:
                return "ESP System Event"; // Call <function_???> to decode furthor
            default:
                return "";
        }
    }

    std::string GetDeviceEventTypeDetails(const ChipDeviceEvent* event)
    {
        std::string ret{GetDeviceEventTypeName(event)};
        if (ret.empty()) {
            std::stringstream ss;
            ss << "Unknown or unexpected " << DeviceEventTypeInfo(event->Type) << " chip::DeviceLayer::DeviceEventType (" << event->Type <<")";
            return ss.str();
        }
        switch (event->Type) {
            case chip::DeviceLayer::DeviceEventType::kESPSystemEvent:
                {
                    std::stringstream ss;
                    ss << "Public " << ret << "(event.Platform.ESPSystemEvent.Base=" << event->Platform.ESPSystemEvent.Base << " event.Platform.ESPSystemEvent.Id=" << event->Platform.ESPSystemEvent.Id << ")";
                    return ss.str();
                }
            default:
                return ret;
        }
    }

} // namespace sldr 