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

#include "stats_utils.h"

#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
const std::string StatsUtils::CURRENT_INVALID = "invalid";
const std::string StatsUtils::CURRENT_BLUETOOTH_ON = "bluetooth_on";
const std::string StatsUtils::CURRENT_BLUETOOTH_SCAN = "bluetooth_scan";
const std::string StatsUtils::CURRENT_BLUETOOTH_RX = "bluetooth_rx";
const std::string StatsUtils::CURRENT_BLUETOOTH_TX = "bluetooth_tx";
const std::string StatsUtils::CURRENT_WIFI_ON = "wifi_on";
const std::string StatsUtils::CURRENT_WIFI_SCAN = "wifi_scan";
const std::string StatsUtils::CURRENT_WIFI_RX = "wifi_rx";
const std::string StatsUtils::CURRENT_WIFI_TX = "wifi_tx";
const std::string StatsUtils::CURRENT_RADIO_ON = "radio_on";
const std::string StatsUtils::CURRENT_RADIO_SCAN = "radio_scan";
const std::string StatsUtils::CURRENT_RADIO_ACTIVE = "radio_active";
const std::string StatsUtils::CURRENT_RADIO_RX = "radio_rx";
const std::string StatsUtils::CURRENT_RADIO_TX = "radio_tx";
const std::string StatsUtils::CURRENT_CAMERA_ON = "camera_on";
const std::string StatsUtils::CURRENT_FLASHLIGHT_ON = "flashlight_on";
const std::string StatsUtils::CURRENT_GPS_ON = "gps_on";
const std::string StatsUtils::CURRENT_SENSOR_GRAVITY = "sensor_gravity_on";
const std::string StatsUtils::CURRENT_SENSOR_PROXIMITY = "sensor_proximity_on";
const std::string StatsUtils::CURRENT_AUDIO_ON = "audio_on";
const std::string StatsUtils::CURRENT_SCREEN_ON = "screen_on";
const std::string StatsUtils::CURRENT_SCREEN_BRIGHTNESS = "screen_brightness";
const std::string StatsUtils::CURRENT_CPU_AWAKE = "cpu_awake";
const std::string StatsUtils::CURRENT_CPU_IDLE = "cpu_idle";
const std::string StatsUtils::CURRENT_CPU_CLUSTER = "cpu_clusters";
const std::string StatsUtils::CURRENT_CPU_SPEED = "cpu_speed_cluster";
const std::string StatsUtils::CURRENT_CPU_ACTIVE = "cpu_active";
const std::string StatsUtils::CURRENT_CPU_SUSPEND = "cpu_suspend";

std::map<std::string, StatsUtils::StatsType> StatsUtils::statsTypeMap_ = {
    {StatsUtils::CURRENT_INVALID, STATS_TYPE_INVALID},
    {StatsUtils::CURRENT_BLUETOOTH_ON, STATS_TYPE_BLUETOOTH_ON},
    {StatsUtils::CURRENT_BLUETOOTH_SCAN, STATS_TYPE_BLUETOOTH_SCAN},
    {StatsUtils::CURRENT_BLUETOOTH_RX, STATS_TYPE_BLUETOOTH_RX},
    {StatsUtils::CURRENT_BLUETOOTH_TX, STATS_TYPE_BLUETOOTH_TX},
    {StatsUtils::CURRENT_WIFI_ON, STATS_TYPE_WIFI_ON},
    {StatsUtils::CURRENT_WIFI_SCAN, STATS_TYPE_WIFI_SCAN},
    {StatsUtils::CURRENT_WIFI_RX, STATS_TYPE_WIFI_RX},
    {StatsUtils::CURRENT_WIFI_TX, STATS_TYPE_WIFI_TX},
    {StatsUtils::CURRENT_RADIO_ON, STATS_TYPE_RADIO_ON},
    {StatsUtils::CURRENT_RADIO_SCAN, STATS_TYPE_RADIO_SCAN},
    {StatsUtils::CURRENT_RADIO_ACTIVE, STATS_TYPE_PHONE_ACTIVE},
    {StatsUtils::CURRENT_RADIO_RX, STATS_TYPE_RADIO_RX},
    {StatsUtils::CURRENT_RADIO_TX, STATS_TYPE_RADIO_TX},
    {StatsUtils::CURRENT_CAMERA_ON, STATS_TYPE_CAMERA_ON},
    {StatsUtils::CURRENT_FLASHLIGHT_ON, STATS_TYPE_FLASHLIGHT_ON},
    {StatsUtils::CURRENT_GPS_ON, STATS_TYPE_GPS_ON},
    {StatsUtils::CURRENT_SENSOR_GRAVITY, STATS_TYPE_SENSOR_GRAVITY_ON},
    {StatsUtils::CURRENT_SENSOR_PROXIMITY, STATS_TYPE_SENSOR_PROXIMITY_ON},
    {StatsUtils::CURRENT_AUDIO_ON, STATS_TYPE_AUDIO_ON},
    {StatsUtils::CURRENT_SCREEN_ON, STATS_TYPE_SCREEN_ON},
    {StatsUtils::CURRENT_SCREEN_BRIGHTNESS, STATS_TYPE_SCREEN_BRIGHTNESS},
    {StatsUtils::CURRENT_CPU_AWAKE, STATS_TYPE_WAKELOCK_HOLD},
    {StatsUtils::CURRENT_CPU_IDLE, STATS_TYPE_PHONE_IDLE},
    {StatsUtils::CURRENT_CPU_CLUSTER, STATS_TYPE_CPU_CLUSTER},
    {StatsUtils::CURRENT_CPU_SPEED, STATS_TYPE_CPU_SPEED},
    {StatsUtils::CURRENT_CPU_ACTIVE, STATS_TYPE_CPU_ACTIVE},
    {StatsUtils::CURRENT_CPU_SUSPEND, STATS_TYPE_CPU_SUSPEND}
};

StatsUtils::StatsType StatsUtils::ConvertToStatsType(std::string type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    StatsType statsDataType = STATS_TYPE_INVALID;
    auto iter = statsTypeMap_.find(type);
    if (iter != statsTypeMap_.end()) {
        statsDataType = iter->second;
        STATS_HILOGD(STATS_MODULE_INNERKIT, "Convert %{public}s to %{public}d", type.c_str(), statsDataType);
    } else {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Convert failed for %{public}s, return %{public}d", type.c_str(),
            statsDataType);
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return statsDataType;
}

std::string StatsUtils::ConvertStatsType(StatsType statsType)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGE(STATS_MODULE_INNERKIT, "Got stats data type = %{public}d", statsType);
    std::string type = "";
    switch (statsType) {
        case STATS_TYPE_BLUETOOTH_ON:
            type = CURRENT_BLUETOOTH_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_BLUETOOTH_SCAN:
            type = CURRENT_BLUETOOTH_SCAN;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_BLUETOOTH_RX:
            type = CURRENT_BLUETOOTH_RX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_BLUETOOTH_TX:
            type = CURRENT_BLUETOOTH_TX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_WIFI_ON:
            type = CURRENT_WIFI_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_WIFI_SCAN:
            type = CURRENT_WIFI_SCAN;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_WIFI_RX:
            type = CURRENT_WIFI_RX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_WIFI_TX:
            type = CURRENT_WIFI_TX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_RADIO_ON:
            type = CURRENT_RADIO_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_RADIO_SCAN:
            type = CURRENT_RADIO_SCAN;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_PHONE_ACTIVE:
            type = CURRENT_RADIO_ACTIVE;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_RADIO_RX:
            type = CURRENT_RADIO_RX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_RADIO_TX:
            type = CURRENT_RADIO_TX;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_CAMERA_ON:
            type = CURRENT_CAMERA_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_FLASHLIGHT_ON:
            type = CURRENT_FLASHLIGHT_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_GPS_ON:
            type = CURRENT_GPS_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_SENSOR_GRAVITY_ON:
            type = CURRENT_SENSOR_GRAVITY;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_SENSOR_PROXIMITY_ON:
            type = CURRENT_SENSOR_PROXIMITY;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_AUDIO_ON:
            type = CURRENT_AUDIO_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_SCREEN_ON:
            type = CURRENT_SCREEN_ON;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_SCREEN_BRIGHTNESS:
            type = CURRENT_SCREEN_BRIGHTNESS;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_WAKELOCK_HOLD:
            type = CURRENT_CPU_AWAKE;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_INVALID:
            type = CURRENT_INVALID;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_PHONE_IDLE:
            type = CURRENT_CPU_IDLE;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_CPU_CLUSTER:
            type = CURRENT_CPU_CLUSTER;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_CPU_SPEED:
            type = CURRENT_CPU_SPEED;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_CPU_ACTIVE:
            type = CURRENT_CPU_ACTIVE;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        case STATS_TYPE_CPU_SUSPEND:
            type = CURRENT_CPU_SUSPEND;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to type: %{public}s", type.c_str());
            break;
        default:
            STATS_HILOGE(STATS_MODULE_INNERKIT, "Convert failed due to illegal stats data type, return empty string");
            break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return type;
}
} // namespace PowerMgr
} // namespace OHOS