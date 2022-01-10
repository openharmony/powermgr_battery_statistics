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

#ifndef BATTERY_STATS_UTILS_H
#define BATTERY_STATS_UTILS_H

#include <string>
#include <map>

namespace OHOS {
namespace PowerMgr {
class BatteryStatsUtils {
public:
    static constexpr bool DEBUG = true;
    static constexpr uint8_t DEFAULT_VALUE = 0;
    static constexpr uint8_t DEFAULT_BIN = 5;
    static constexpr int8_t INVALID_VALUE = -1;

    static const std::string TYPE_BLUETOOTH_ON;
    static const std::string TYPE_BLUETOOTH_SCAN;
    static const std::string TYPE_BLUETOOTH_RX;
    static const std::string TYPE_BLUETOOTH_TX;
    static const std::string TYPE_WIFI_ON;
    static const std::string TYPE_WIFI_SCAN;
    static const std::string TYPE_WIFI_RX;
    static const std::string TYPE_WIFI_TX;
    static const std::string TYPE_RADIO_ON;
    static const std::string TYPE_RADIO_SCAN;
    static const std::string TYPE_RADIO_ACTIVE;
    static const std::string TYPE_RADIO_RX;
    static const std::string TYPE_RADIO_TX;
    static const std::string TYPE_CAMERA_ON;
    static const std::string TYPE_FLASHLIGHT_ON;
    static const std::string TYPE_GPS_ON;
    static const std::string TYPE_SENSOR_GRAVITY;
    static const std::string TYPE_SENSOR_PROXIMITY;
    static const std::string TYPE_AUDIO_ON;
    static const std::string TYPE_SCREEN_ON;
    static const std::string TYPE_SCREEN_BRIGHTNESS;
    static const std::string TYPE_CPU_AWAKE;
    static const std::string TYPE_CPU_IDLE;
    static const std::string TYPE_CPU_CLUSTER;
    static const std::string TYPE_CPU_SPEED;

    enum StatsDataType {
        DATA_TYPE_INVALID = -1,
        DATA_TYPE_BLUETOOTH_ON,
        DATA_TYPE_BLUETOOTH_SCAN,
        DATA_TYPE_BLUETOOTH_RX,
        DATA_TYPE_BLUETOOTH_TX,
        DATA_TYPE_WIFI_ON,
        DATA_TYPE_WIFI_SCAN,
        DATA_TYPE_WIFI_RX,
        DATA_TYPE_WIFI_TX,
        DATA_TYPE_RADIO_ON,
        DATA_TYPE_RADIO_SCAN,
        DATA_TYPE_RADIO_ACTIVE,
        DATA_TYPE_RADIO_RX,
        DATA_TYPE_RADIO_TX,
        DATA_TYPE_CAMERA_ON,
        DATA_TYPE_FLASHLIGHT_ON,
        DATA_TYPE_GPS_ON,
        DATA_TYPE_SENSOR_GRAVITY_ON,
        DATA_TYPE_SENSOR_PROXIMITY_ON,
        DATA_TYPE_AUDIO_ON,
        DATA_TYPE_SCREEN_ON,
        DATA_TYPE_SCREEN_BRIGHTNESS,
        DATA_TYPE_WAKELOCK_HOLD
    };

    enum StatsDataState {
        STATE_INVALID = -1, // Indicates an invalid state
        STATE_DISPLAY_OFF, // Indicates screen is in off state
        STATE_DISPLAY_DIM, // Indicates screen is in dim state
        STATE_DISPLAY_ON, // Indicates screen is in on state
        STATE_DISPLAY_SUSPEND, // Indicates screen is in suspend state
        STATE_DISPLAY_UNKNOWN, // Indicates screen is in unknown state
        STATE_NETWORK_UNKNOWN, // Indicates mobile network is in unknown state
        STATE_NETWORK_IN_SERVICE, // Indicates mobile network is in in-service state
        STATE_NETWORK_NO_SERVICE, // Indicates mobile network is in no-service state
        STATE_NETWORK_EMERGENCY_ONLY, // Indicates mobile network is in emergency-only state
        STATE_NETWORK_SEARCH, // Indicates mobile network is in searching state
        STATE_NETWORK_POWER_OFF, // Indicates radio of telephony is explicitly powered off
    };

    struct StatsData {
        StatsDataType type = DATA_TYPE_INVALID;
        StatsDataState state = STATE_INVALID;
        int32_t uid = INVALID_VALUE;
        bool activated = false;
        int32_t level = INVALID_VALUE;
        long time = DEFAULT_VALUE;
        long traffic = DEFAULT_VALUE;
    };

    static std::string CovertDataType(StatsDataType dataType);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_UTILS_H