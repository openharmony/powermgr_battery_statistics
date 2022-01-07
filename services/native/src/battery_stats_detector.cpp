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

namespace OHOS {
namespace PowerMgr {
bool BatteryStatsDetector::Init()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto pmsptr = bss_.promote();
    if (pmsptr == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Initialization failed: promoting failure");
        return false;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Initialization succeeded");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

void BatteryStatsDetector::HandleStatsChangedEvent(BatteryStatsUtils::StatsData data)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");

    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle type: %{public}d", data.type);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle state: %{public}d", data.state);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle uid: %{public}d", data.uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle activated: %{public}d", data.activated);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle time: %{public}ld", data.time);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle traffic: %{public}ld", data.traffic);

    auto bssptr = bss_.promote();
    if (bssptr == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Promoting failure");
        return;
    }
    auto core = bssptr->GetBatteryStatsCore();
    auto typeStr = BatteryStatsUtils::CovertDataType(data.type);
    if (isTimeRelated(data.type)) {
        // Update related timer based on activation flag
        core->UpdateStats(typeStr, data.activated, data.uid);
    } else if (isDurationRelated(data.type)) {
        // Update related timer with reported time
        // The traffic won't participate the power consumption calculation, just for dump info
        core->UpdateStats(typeStr, data.time, data.traffic, data.uid);
    } else if (isLevelRelated(data.type)) {
        // Update related timer based on level, there're more than 1 level to consider
        core->UpdateStats(typeStr, data.state, data.level, data.uid);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Got invalid type");
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

bool BatteryStatsDetector::isTimeRelated(BatteryStatsUtils::StatsDataType type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle type: %{public}d", type);
    bool isMatch = false;
    switch (type) {
        case BatteryStatsUtils::DATA_TYPE_BLUETOOTH_ON:
        case BatteryStatsUtils::DATA_TYPE_BLUETOOTH_SCAN:
        case BatteryStatsUtils::DATA_TYPE_WIFI_ON:
        case BatteryStatsUtils::DATA_TYPE_WIFI_SCAN:
        case BatteryStatsUtils::DATA_TYPE_RADIO_SCAN:
        case BatteryStatsUtils::DATA_TYPE_RADIO_ACTIVE:
        case BatteryStatsUtils::DATA_TYPE_CAMERA_ON:
        case BatteryStatsUtils::DATA_TYPE_FLASHLIGHT_ON:
        case BatteryStatsUtils::DATA_TYPE_GPS_ON:
        case BatteryStatsUtils::DATA_TYPE_AUDIO_ON:
        case BatteryStatsUtils::DATA_TYPE_WAKELOCK_HOLD:
            // Related with activation flag
            isMatch = true;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Type: %{public}d is time related", type);
            break;
        case BatteryStatsUtils::DATA_TYPE_INVALID:
        default:
            STATS_HILOGE(STATS_MODULE_SERVICE, "Got invalid type");
            break;
    }
    return isMatch;
}

bool BatteryStatsDetector::isDurationRelated(BatteryStatsUtils::StatsDataType type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle type: %{public}d", type);
    bool isMatch = false;
    switch (type) {
        case BatteryStatsUtils::DATA_TYPE_BLUETOOTH_RX:
        case BatteryStatsUtils::DATA_TYPE_BLUETOOTH_TX:
        case BatteryStatsUtils::DATA_TYPE_WIFI_RX:
        case BatteryStatsUtils::DATA_TYPE_WIFI_TX:
        case BatteryStatsUtils::DATA_TYPE_RADIO_RX:
        case BatteryStatsUtils::DATA_TYPE_RADIO_TX:
            // Realated with duration
            isMatch = true;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Type: %{public}d is duration related", type);
            break;
        case BatteryStatsUtils::DATA_TYPE_INVALID:
        default:
            STATS_HILOGE(STATS_MODULE_SERVICE, "Got invalid type");
            break;
    }
    return isMatch;
}

bool BatteryStatsDetector::isLevelRelated(BatteryStatsUtils::StatsDataType type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle type: %{public}d", type);
    bool isMatch = false;
    switch (type) {
        case BatteryStatsUtils::DATA_TYPE_RADIO_ON:
        case BatteryStatsUtils::DATA_TYPE_SCREEN_BRIGHTNESS:
            // Related with level
            isMatch = true;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Type: %{public}d is level related", type);
            break;
        case BatteryStatsUtils::DATA_TYPE_INVALID:
        default:
            STATS_HILOGE(STATS_MODULE_SERVICE, "Got invalid type");
            break;
    }
    return isMatch;
}
} // namespace PowerMgr
} // namespace OHOS
