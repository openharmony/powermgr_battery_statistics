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

#include "battery_stats_detector.h"

#include "battery_stats_service.h"

namespace OHOS {
namespace PowerMgr {
void BatteryStatsDetector::HandleStatsChangedEvent(StatsUtils::StatsData data)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");

    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle type: %{public}s", StatsUtils::ConvertStatsType(data.type).c_str());
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle state: %{public}d", data.state);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle level: %{public}d", data.level);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle uid: %{public}d", data.uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle time: %{public}ld", data.time);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle traffic: %{public}ld", data.traffic);

    auto bss = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
    if (bss == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Got Battery stats service failed");
        return;
    }
    auto core = bss->GetBatteryStatsCore();
    if (isDurationRelated(data.type)) {
        // Update related timer with reported time
        // The traffic won't participate the power consumption calculation, just for dump info
        core->UpdateStats(data.type, data.time, data.traffic, data.uid);
    } else if (isStateRelated(data.type)) {
        // Update related timer based on state or level
        core->UpdateStats(data.type, data.state, data.level, data.uid);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Got invalid type");
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

bool BatteryStatsDetector::isDurationRelated(StatsUtils::StatsType type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle type: %{public}d", type);
    bool isMatch = false;
    switch (type) {
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX:
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX:
        case StatsUtils::STATS_TYPE_WIFI_RX:
        case StatsUtils::STATS_TYPE_WIFI_TX:
        case StatsUtils::STATS_TYPE_RADIO_RX:
        case StatsUtils::STATS_TYPE_RADIO_TX:
            // Realated with duration
            isMatch = true;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Type: %{public}d is duration related", type);
            break;
        default:
            STATS_HILOGE(STATS_MODULE_SERVICE, "Got invalid type");
            break;
    }
    return isMatch;
}

bool BatteryStatsDetector::isStateRelated(StatsUtils::StatsType type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle type: %{public}d", type);
    bool isMatch = false;
    switch (type) {
        case StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON:
        case StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON:
        case StatsUtils::STATS_TYPE_SCREEN_ON:
        case StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS:
        case StatsUtils::STATS_TYPE_BLUETOOTH_ON:
        case StatsUtils::STATS_TYPE_BLUETOOTH_SCAN:
        case StatsUtils::STATS_TYPE_WIFI_ON:
        case StatsUtils::STATS_TYPE_WIFI_SCAN:
        case StatsUtils::STATS_TYPE_RADIO_ON:
        case StatsUtils::STATS_TYPE_RADIO_SCAN:
        case StatsUtils::STATS_TYPE_PHONE_ACTIVE:
        case StatsUtils::STATS_TYPE_CAMERA_ON:
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON:
        case StatsUtils::STATS_TYPE_GPS_ON:
        case StatsUtils::STATS_TYPE_AUDIO_ON:
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD:
            // Related with level
            isMatch = true;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Type: %{public}d is level related", type);
            break;
        default:
            STATS_HILOGE(STATS_MODULE_SERVICE, "Got invalid type");
            break;
    }
    return isMatch;
}
} // namespace PowerMgr
} // namespace OHOS
