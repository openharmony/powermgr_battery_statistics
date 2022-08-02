/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STATS_UTILS_H
#define STATS_UTILS_H

#include <string>
#include <iosfwd>

namespace OHOS {
namespace PowerMgr {
#define GET_VARIABLE_NAME(name) #name
class StatsUtils {
public:
    static constexpr uint8_t DEFAULT_VALUE = 0;
    static constexpr uint8_t SCREEN_BRIGHTNESS_BIN = 255;
    static constexpr uint8_t RADIO_SIGNAL_BIN = 5;
    static constexpr int8_t INVALID_VALUE = -1;
    static constexpr uint32_t MS_IN_HOUR = 3600000;
    static constexpr uint32_t MS_IN_SECOND = 1000;
    static constexpr uint32_t NS_IN_MS = 1000000;
    static constexpr uint32_t US_IN_MS = 1000;

    static constexpr const char* CURRENT_INVALID = "invalid";
    static constexpr const char* CURRENT_BLUETOOTH_BR_ON = "bluetooth_br_on";
    static constexpr const char* CURRENT_BLUETOOTH_BR_SCAN = "bluetooth_br_scan";
    static constexpr const char* CURRENT_BLUETOOTH_BLE_ON = "bluetooth_ble_on";
    static constexpr const char* CURRENT_BLUETOOTH_BLE_SCAN = "bluetooth_ble_scan";
    static constexpr const char* CURRENT_WIFI_ON = "wifi_on";
    static constexpr const char* CURRENT_WIFI_SCAN = "wifi_scan";
    static constexpr const char* CURRENT_RADIO_ON = "radio_on";
    static constexpr const char* CURRENT_RADIO_DATA = "radio_data";
    static constexpr const char* CURRENT_CAMERA_ON = "camera_on";
    static constexpr const char* CURRENT_FLASHLIGHT_ON = "flashlight_on";
    static constexpr const char* CURRENT_GNSS_ON = "gnss_on";
    static constexpr const char* CURRENT_SENSOR_GRAVITY = "sensor_gravity_on";
    static constexpr const char* CURRENT_SENSOR_PROXIMITY = "sensor_proximity_on";
    static constexpr const char* CURRENT_AUDIO_ON = "audio_on";
    static constexpr const char* CURRENT_SCREEN_ON = "screen_on";
    static constexpr const char* CURRENT_SCREEN_BRIGHTNESS = "screen_brightness";
    static constexpr const char* CURRENT_CPU_AWAKE = "cpu_awake";
    static constexpr const char* CURRENT_CPU_IDLE = "cpu_idle";
    static constexpr const char* CURRENT_CPU_CLUSTER = "cpu_clusters";
    static constexpr const char* CURRENT_CPU_SPEED = "cpu_speed_cluster";
    static constexpr const char* CURRENT_CPU_ACTIVE = "cpu_active";
    static constexpr const char* CURRENT_CPU_SUSPEND = "cpu_suspend";
    static constexpr const char* CURRENT_ALARM_ON = "alarm_on";

    enum StatsType {
        STATS_TYPE_INVALID = -1,
        STATS_TYPE_BLUETOOTH_BR_ON,
        STATS_TYPE_BLUETOOTH_BR_SCAN,
        STATS_TYPE_BLUETOOTH_BLE_ON,
        STATS_TYPE_BLUETOOTH_BLE_SCAN,
        STATS_TYPE_WIFI_ON,
        STATS_TYPE_WIFI_SCAN,
        STATS_TYPE_PHONE_ACTIVE,
        STATS_TYPE_PHONE_DATA,
        STATS_TYPE_CAMERA_ON,
        STATS_TYPE_CAMERA_FLASHLIGHT_ON,
        STATS_TYPE_FLASHLIGHT_ON,
        STATS_TYPE_GNSS_ON,
        STATS_TYPE_SENSOR_GRAVITY_ON,
        STATS_TYPE_SENSOR_PROXIMITY_ON,
        STATS_TYPE_AUDIO_ON,
        STATS_TYPE_DISPLAY,
        STATS_TYPE_SCREEN_ON,
        STATS_TYPE_SCREEN_BRIGHTNESS,
        STATS_TYPE_WAKELOCK_HOLD,
        STATS_TYPE_PHONE_IDLE,
        STATS_TYPE_CPU_CLUSTER,
        STATS_TYPE_CPU_SPEED,
        STATS_TYPE_CPU_ACTIVE,
        STATS_TYPE_CPU_SUSPEND,
        STATS_TYPE_BATTERY,
        STATS_TYPE_WORKSCHEDULER,
        STATS_TYPE_THERMAL,
        STATS_TYPE_DISTRIBUTEDSCHEDULER,
        STATS_TYPE_ALARM,
    };

    enum StatsState {
        STATS_STATE_INVALID = -1, // Indicates an invalid state
        STATS_STATE_ACTIVATED, // Indicates subsystem is activated
        STATS_STATE_DEACTIVATED, // Indicates subsystem is deactivated
        STATS_STATE_WORKSCHEDULER_SCHEDULED, // Indicates work is scheduled
        STATS_STATE_WORKSCHEDULER_EXECUTED, // Indicates work is executed
    };

    struct StatsData {
        StatsType type = STATS_TYPE_INVALID;
        StatsState state = STATS_STATE_INVALID;
        int32_t uid = INVALID_VALUE;
        int32_t pid = INVALID_VALUE;
        std::string eventDataName = "INVALID";
        std::string eventDebugInfo = "INVALID";
        int32_t eventDataType = INVALID_VALUE;
        int32_t eventDataExtra = INVALID_VALUE;
        int16_t level = INVALID_VALUE;
        int64_t time = DEFAULT_VALUE;
        int64_t traffic = DEFAULT_VALUE;
        std::string deviceId = "";
    };

    static std::string ConvertStatsType(StatsType statsType);
private:
    static std::string ConvertTypeForConn(StatsType statsType);
    static std::string ConvertTypeForCpu(StatsType statsType);
    static std::string ConvertTypeForCommon(StatsType statsType);
    static std::string ConvertTypeForDebug(StatsType statsType);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_UTILS_H