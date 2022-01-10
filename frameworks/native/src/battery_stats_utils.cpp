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

#include "battery_stats_utils.h"

#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
const std::string BatteryStatsUtils::TYPE_BLUETOOTH_ON = "bluetooth_on";
const std::string BatteryStatsUtils::TYPE_BLUETOOTH_SCAN = "bluetooth_scan";
const std::string BatteryStatsUtils::TYPE_BLUETOOTH_RX = "bluetooth_rx";
const std::string BatteryStatsUtils::TYPE_BLUETOOTH_TX = "bluetooth_tx";
const std::string BatteryStatsUtils::TYPE_WIFI_ON = "wifi_on";
const std::string BatteryStatsUtils::TYPE_WIFI_SCAN = "wifi_scan";
const std::string BatteryStatsUtils::TYPE_WIFI_RX = "wifi_rx";
const std::string BatteryStatsUtils::TYPE_WIFI_TX = "wifi_tx";
const std::string BatteryStatsUtils::TYPE_RADIO_ON = "radio_on";
const std::string BatteryStatsUtils::TYPE_RADIO_SCAN = "radio_scan";
const std::string BatteryStatsUtils::TYPE_RADIO_ACTIVE = "radio_active";
const std::string BatteryStatsUtils::TYPE_RADIO_RX = "radio_rx";
const std::string BatteryStatsUtils::TYPE_RADIO_TX = "radio_tx";
const std::string BatteryStatsUtils::TYPE_CAMERA_ON = "camera_on";
const std::string BatteryStatsUtils::TYPE_FLASHLIGHT_ON = "flashlight_on";
const std::string BatteryStatsUtils::TYPE_GPS_ON = "gps_on";
const std::string BatteryStatsUtils::TYPE_SENSOR_GRAVITY = "sensor_gravity_on";
const std::string BatteryStatsUtils::TYPE_SENSOR_PROXIMITY = "sensor_proximity_on";
const std::string BatteryStatsUtils::TYPE_AUDIO_ON = "audio_on";
const std::string BatteryStatsUtils::TYPE_SCREEN_ON = "screen_on";
const std::string BatteryStatsUtils::TYPE_SCREEN_BRIGHTNESS = "screen_brightness";
const std::string BatteryStatsUtils::TYPE_CPU_AWAKE = "cpu_awake";
const std::string BatteryStatsUtils::TYPE_CPU_IDLE = "cpu_idle";
const std::string BatteryStatsUtils::TYPE_CPU_CLUSTER = "cpu_clusters";
const std::string BatteryStatsUtils::TYPE_CPU_SPEED = "cpu_speed_cluster";

std::string BatteryStatsUtils::CovertDataType(StatsDataType dataType)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGE(STATS_MODULE_INNERKIT, "Got stats data type = %{public}d", dataType);
    std::string type = "";
    switch (dataType) {
        case DATA_TYPE_BLUETOOTH_ON:
            type = TYPE_BLUETOOTH_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_BLUETOOTH_SCAN:
            type = TYPE_BLUETOOTH_SCAN;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_BLUETOOTH_RX:
            type = TYPE_BLUETOOTH_RX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_BLUETOOTH_TX:
            type = TYPE_BLUETOOTH_TX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_WIFI_ON:
            type = TYPE_WIFI_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_WIFI_SCAN:
            type = TYPE_WIFI_SCAN;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_WIFI_RX:
            type = TYPE_WIFI_RX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_WIFI_TX:
            type = TYPE_WIFI_TX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_RADIO_ON:
            type = TYPE_RADIO_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_RADIO_SCAN:
            type = TYPE_RADIO_SCAN;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_RADIO_ACTIVE:
            type = TYPE_RADIO_ACTIVE;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_RADIO_RX:
            type = TYPE_RADIO_RX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_RADIO_TX:
            type = TYPE_RADIO_TX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_CAMERA_ON:
            type = TYPE_CAMERA_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_FLASHLIGHT_ON:
            type = TYPE_FLASHLIGHT_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_GPS_ON:
            type = TYPE_GPS_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_SENSOR_GRAVITY_ON:
            type = TYPE_SENSOR_GRAVITY;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_SENSOR_PROXIMITY_ON:
            type = TYPE_SENSOR_PROXIMITY;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_AUDIO_ON:
            type = TYPE_AUDIO_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_SCREEN_ON:
            type = TYPE_SCREEN_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_SCREEN_BRIGHTNESS:
            type = TYPE_SCREEN_BRIGHTNESS;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_WAKELOCK_HOLD:
            type = TYPE_CPU_AWAKE;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Covert to type: %{public}s", type.c_str());
            break;
        case DATA_TYPE_INVALID:
        default:
            STATS_HILOGE(STATS_MODULE_INNERKIT, "Covert failed due to invalid stats data type, return empty string");
            break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return type;
}
} // namespace PowerMgr
} // namespace OHOS