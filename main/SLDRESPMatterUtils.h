#include <string_view>
#include <esp_matter.h>

namespace sldr {

    std::string DeviceEventTypeInfo(const uint16_t eventType);
    std::string_view GetDeviceEventTypeName(const ChipDeviceEvent* event);
    std::string GetDeviceEventTypeDetails(const ChipDeviceEvent* event);

} // namespace sldr 