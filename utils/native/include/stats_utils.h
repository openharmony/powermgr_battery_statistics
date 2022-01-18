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

namespace OHOS {
namespace PowerMgr {
#define GET_VARIABLE_NAME(name) #name
class StatsUtils {
public:
    static constexpr uint8_t DEFAULT_VALUE = 0;
    static constexpr uint8_t SCREEN_BRIGHTNESS_BIN = 5;
    static constexpr uint8_t RADIO_SIGNAL_BIN = 5;
    static constexpr int8_t INVALID_VALUE = -1;
    static constexpr uint32_t MS_IN_HOUR = 3600000;
    static constexpr uint32_t MS_IN_SECOND = 1000;
    static constexpr uint32_t NS_IN_MS = 1000000;
    static constexpr uint32_t US_IN_MS = 1000;

    static const std::string CURRENT_INVALID;
    static const std::string CURRENT_BLUETOOTH_ON;
    static const std::string CURRENT_BLUETOOTH_SCAN;
    static const std::string CURRENT_BLUETOOTH_RX;
    static const std::string CURRENT_BLUETOOTH_TX;
    static const std::string CURRENT_WIFI_ON;
    static const std::string CURRENT_WIFI_SCAN;
    static const std::string CURRENT_WIFI_RX;
    static const std::string CURRENT_WIFI_TX;
    static const std::string CURRENT_RADIO_ON;
    static const std::string CURRENT_RADIO_SCAN;
    static const std::string CURRENT_RADIO_ACTIVE;
    static const std::string CURRENT_RADIO_RX;
    static const std::string CURRENT_RADIO_TX;
    static const std::string CURRENT_CAMERA_ON;
    static const std::string CURRENT_FLASHLIGHT_ON;
    static const std::string CURRENT_GPS_ON;
    static const std::string CURRENT_SENSOR_GRAVITY;
    static const std::string CURRENT_SENSOR_PROXIMITY;
    static const std::string CURRENT_AUDIO_ON;
    static const std::string CURRENT_SCREEN_ON;
    static const std::string CURRENT_SCREEN_BRIGHTNESS;
    static const std::string CURRENT_CPU_AWAKE;
    static const std::string CURRENT_CPU_IDLE;
    static const std::string CURRENT_CPU_CLUSTER;
    static const std::string CURRENT_CPU_SPEED;
    static const std::string CURRENT_CPU_ACTIVE;
    static const std::string CURRENT_CPU_SUSPEND;

    enum StatsType {
        STATS_TYPE_INVALID = -1,
        STATS_TYPE_BLUETOOTH_ON,
        STATS_TYPE_BLUETOOTH_SCAN,
        STATS_TYPE_BLUETOOTH_RX,
        STATS_TYPE_BLUETOOTH_TX,
        STATS_TYPE_WIFI_ON,
        STATS_TYPE_WIFI_SCAN,
        STATS_TYPE_WIFI_RX,
        STATS_TYPE_WIFI_TX,
        STATS_TYPE_RADIO_ON,
        STATS_TYPE_RADIO_SCAN,
        STATS_TYPE_PHONE_ACTIVE,
        STATS_TYPE_RADIO_RX,
        STATS_TYPE_RADIO_TX,
        STATS_TYPE_CAMERA_ON,
        STATS_TYPE_FLASHLIGHT_ON,
        STATS_TYPE_GPS_ON,
        STATS_TYPE_SENSOR_GRAVITY_ON,
        STATS_TYPE_SENSOR_PROXIMITY_ON,
        STATS_TYPE_AUDIO_ON,
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
    };

    enum StatsState {
        STATS_STATE_INVALID = -1, // Indicates an invalid state
        STATS_STATE_ACTIVATED, // Indicates subsystem is activated
        STATS_STATE_DEACTIVATED, // Indicates subsystem is deactivated
        STATS_STATE_DISPLAY_OFF, // Indicates screen is in off state
        STATS_STATE_DISPLAY_DIM, // Indicates screen is in dim state
        STATS_STATE_DISPLAY_ON, // Indicates screen is in on state
        STATS_STATE_DISPLAY_SUSPEND, // Indicates screen is in suspend state
        STATS_STATE_DISPLAY_UNKNOWN, // Indicates screen is in unknown state
        STATS_STATE_NETWORK_UNKNOWN, // Indicates mobile network is in unknown state
        STATS_STATE_NETWORK_IN_SERVICE, // Indicates mobile network is in in-service state
        STATS_STATE_NETWORK_NO_SERVICE, // Indicates mobile network is in no-service state
        STATS_STATE_NETWORK_EMERGENCY_ONLY, // Indicates mobile network is in emergency-only state
        STATS_STATE_NETWORK_SEARCH, // Indicates mobile network is in searching state
        STATS_STATE_NETWORK_POWER_OFF, // Indicates radio of telephony is explicitly powered off
        STATS_STATE_WORKSCHEDULER_SCHEDULED, // Indicates work is scheduled
        STATS_STATE_WORKSCHEDULER_EXECUTED, // Indicates work is executed
    };

    struct StatsData {
        StatsType type = STATS_TYPE_INVALID;
        StatsState state = STATS_STATE_INVALID;
        int32_t uid = INVALID_VALUE;
        int32_t pid = INVALID_VALUE;
        std::string eventDataName = "INVALID";
        int32_t eventDataType = INVALID_VALUE;
        int32_t eventDataExtra = INVALID_VALUE;
        int16_t level = INVALID_VALUE;
        long time = DEFAULT_VALUE;
        long traffic = DEFAULT_VALUE;
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