/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "stats_log.h"

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

std::string StatsUtils::ConvertTypeForConn(StatsType statsType)
{
    std::string result = "";
    switch (statsType) {
        case STATS_TYPE_BLUETOOTH_ON:
            result = GET_VARIABLE_NAME(STATS_TYPE_BLUETOOTH_ON);
            break;
        case STATS_TYPE_BLUETOOTH_SCAN:
            result = GET_VARIABLE_NAME(STATS_TYPE_BLUETOOTH_SCAN);
            break;
        case STATS_TYPE_BLUETOOTH_RX:
            result = GET_VARIABLE_NAME(STATS_TYPE_BLUETOOTH_RX);
            break;
        case STATS_TYPE_BLUETOOTH_TX:
            result = GET_VARIABLE_NAME(STATS_TYPE_BLUETOOTH_TX);
            break;
        case STATS_TYPE_WIFI_ON:
            result = GET_VARIABLE_NAME(STATS_TYPE_WIFI_ON);
            break;
        case STATS_TYPE_WIFI_SCAN:
            result = GET_VARIABLE_NAME(STATS_TYPE_WIFI_SCAN);
            break;
        case STATS_TYPE_WIFI_RX:
            result = GET_VARIABLE_NAME(STATS_TYPE_WIFI_RX);
            break;
        case STATS_TYPE_WIFI_TX:
            result = GET_VARIABLE_NAME(STATS_TYPE_WIFI_TX);
            break;
        case STATS_TYPE_RADIO_ON:
            result = GET_VARIABLE_NAME(STATS_TYPE_RADIO_ON);
            break;
        case STATS_TYPE_RADIO_SCAN:
            result = GET_VARIABLE_NAME(STATS_TYPE_RADIO_SCAN);
            break;
        case STATS_TYPE_PHONE_ACTIVE:
            result = GET_VARIABLE_NAME(STATS_TYPE_PHONE_ACTIVE);
            break;
        case STATS_TYPE_RADIO_RX:
            result = GET_VARIABLE_NAME(STATS_TYPE_RADIO_RX);
            break;
        case STATS_TYPE_RADIO_TX:
            result = GET_VARIABLE_NAME(STATS_TYPE_RADIO_TX);
            break;
        default:
            break;
    }
    STATS_HILOGI(COMP_FWK, "Convert to %{public}s", result.c_str());
    return result;
}

std::string StatsUtils::ConvertTypeForCpu(StatsType statsType)
{
    std::string result = "";
    switch (statsType) {
        case STATS_TYPE_WAKELOCK_HOLD:
            result = GET_VARIABLE_NAME(STATS_TYPE_WAKELOCK_HOLD);
            break;
        case STATS_TYPE_PHONE_IDLE:
            result = GET_VARIABLE_NAME(STATS_TYPE_PHONE_IDLE);
            break;
        case STATS_TYPE_CPU_CLUSTER:
            result = GET_VARIABLE_NAME(STATS_TYPE_CPU_CLUSTER);
            break;
        case STATS_TYPE_CPU_SPEED:
            result = GET_VARIABLE_NAME(STATS_TYPE_CPU_SPEED);
            break;
        case STATS_TYPE_CPU_ACTIVE:
            result = GET_VARIABLE_NAME(STATS_TYPE_CPU_ACTIVE);
            break;
        case STATS_TYPE_CPU_SUSPEND:
            result = GET_VARIABLE_NAME(STATS_TYPE_CPU_SUSPEND);
            break;
        default:
            break;
    }
    STATS_HILOGI(COMP_FWK, "Convert to %{public}s", result.c_str());
    return result;
}

std::string StatsUtils::ConvertTypeForCommon(StatsType statsType)
{
    std::string result = "";
    switch (statsType) {
        case STATS_TYPE_CAMERA_ON:
            result = GET_VARIABLE_NAME(STATS_TYPE_CAMERA_ON);
            break;
        case STATS_TYPE_FLASHLIGHT_ON:
            result = GET_VARIABLE_NAME(STATS_TYPE_FLASHLIGHT_ON);
            break;
        case STATS_TYPE_GPS_ON:
            result = GET_VARIABLE_NAME(STATS_TYPE_GPS_ON);
            break;
        case STATS_TYPE_SENSOR_GRAVITY_ON:
            result = GET_VARIABLE_NAME(STATS_TYPE_SENSOR_GRAVITY_ON);
            break;
        case STATS_TYPE_SENSOR_PROXIMITY_ON:
            result = GET_VARIABLE_NAME(STATS_TYPE_SENSOR_PROXIMITY_ON);
            break;
        case STATS_TYPE_AUDIO_ON:
            result = GET_VARIABLE_NAME(STATS_TYPE_AUDIO_ON);
            break;
        case STATS_TYPE_SCREEN_ON:
            result = GET_VARIABLE_NAME(STATS_TYPE_SCREEN_ON);
            break;
        case STATS_TYPE_SCREEN_BRIGHTNESS:
            result = GET_VARIABLE_NAME(STATS_TYPE_SCREEN_BRIGHTNESS);
            break;
        default:
            break;
    }
    STATS_HILOGI(COMP_FWK, "Convert to %{public}s", result.c_str());
    return result;
}

std::string StatsUtils::ConvertTypeForDebug(StatsType statsType)
{
    std::string result = "";
    switch (statsType) {
        case STATS_TYPE_BATTERY:
            result = GET_VARIABLE_NAME(STATS_TYPE_BATTERY);
            break;
        case STATS_TYPE_WORKSCHEDULER:
            result = GET_VARIABLE_NAME(STATS_TYPE_WORKSCHEDULER);
            break;
        case STATS_TYPE_THERMAL:
            result = GET_VARIABLE_NAME(STATS_TYPE_THERMAL);
            break;
        case STATS_TYPE_DISTRIBUTEDSCHEDULER:
            result = GET_VARIABLE_NAME(STATS_TYPE_DISTRIBUTEDSCHEDULER);
            break;
        default:
            break;
    }
    STATS_HILOGI(COMP_FWK, "Convert to %{public}s", result.c_str());
    return result;
}

std::string StatsUtils::ConvertStatsType(StatsType statsType)
{
    std::string result = "";
    switch (statsType) {
        case STATS_TYPE_BLUETOOTH_ON:
        case STATS_TYPE_BLUETOOTH_SCAN:
        case STATS_TYPE_BLUETOOTH_RX:
        case STATS_TYPE_BLUETOOTH_TX:
        case STATS_TYPE_WIFI_ON:
        case STATS_TYPE_WIFI_SCAN:
        case STATS_TYPE_WIFI_RX:
        case STATS_TYPE_WIFI_TX:
        case STATS_TYPE_RADIO_ON:
        case STATS_TYPE_RADIO_SCAN:
        case STATS_TYPE_PHONE_ACTIVE:
        case STATS_TYPE_RADIO_RX:
        case STATS_TYPE_RADIO_TX:
            result = ConvertTypeForConn(statsType);
            break;
        case STATS_TYPE_CAMERA_ON:
        case STATS_TYPE_FLASHLIGHT_ON:
        case STATS_TYPE_GPS_ON:
        case STATS_TYPE_SENSOR_GRAVITY_ON:
        case STATS_TYPE_SENSOR_PROXIMITY_ON:
        case STATS_TYPE_AUDIO_ON:
        case STATS_TYPE_SCREEN_ON:
        case STATS_TYPE_SCREEN_BRIGHTNESS:
            result = ConvertTypeForCommon(statsType);
            break;
        case STATS_TYPE_WAKELOCK_HOLD:
        case STATS_TYPE_PHONE_IDLE:
        case STATS_TYPE_CPU_CLUSTER:
        case STATS_TYPE_CPU_SPEED:
        case STATS_TYPE_CPU_ACTIVE:
        case STATS_TYPE_CPU_SUSPEND:
            result = ConvertTypeForCommon(statsType);
            break;
        case STATS_TYPE_BATTERY:
        case STATS_TYPE_WORKSCHEDULER:
        case STATS_TYPE_THERMAL:
        case STATS_TYPE_DISTRIBUTEDSCHEDULER:
            result = ConvertTypeForDebug(statsType);
            break;
        default:
            STATS_HILOGE(COMP_FWK, "Convert failed due to illegal ConsumptionType, return empty string");
            break;
    }
    return result;
}
} // namespace PowerMgr
} // namespace OHOS