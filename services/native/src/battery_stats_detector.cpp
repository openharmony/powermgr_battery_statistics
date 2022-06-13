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
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle pid: %{public}d", data.pid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle eventDataName: %{public}s", data.eventDataName.c_str());
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle eventDataType: %{public}d", data.eventDataType);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle eventDataExtra: %{public}d", data.eventDataExtra);
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
    handleDebugInfo(data);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

bool BatteryStatsDetector::isDurationRelated(StatsUtils::StatsType type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle type: %{public}s", StatsUtils::ConvertStatsType(type).c_str());
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
            STATS_HILOGI(STATS_MODULE_SERVICE, "Type: %{public}s is duration related",
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        default:
            STATS_HILOGE(STATS_MODULE_SERVICE, "Got invalid type");
            break;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return isMatch;
}

bool BatteryStatsDetector::isStateRelated(StatsUtils::StatsType type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle type: %{public}s", StatsUtils::ConvertStatsType(type).c_str());
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
            // Related with state
            isMatch = true;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Type: %{public}s is state related",
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        default:
            STATS_HILOGE(STATS_MODULE_SERVICE, "Got invalid type");
            break;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return isMatch;
}

void BatteryStatsDetector::handleThermalInfo(StatsUtils::StatsData data, long bootTimeMs, std::string& debugInfo)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    debugInfo.append("Thermal event: Part name = ")
        .append(data.eventDataName)
        .append(", temperature = ")
        .append(ToString(data.eventDataExtra))
        .append("degrees Celsius, boot time after boot = ")
        .append(ToString(bootTimeMs))
        .append("ms\n");
    if (!data.eventDebugInfo.empty()) {
        debugInfo.append("Additional debug info: ")
            .append(data.eventDebugInfo);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsDetector::handleBatteryInfo(StatsUtils::StatsData data, long bootTimeMs, std::string& debugInfo)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    debugInfo.append("Battery event: Battery level = ")
        .append(ToString(data.level))
        .append(", Charger type = ")
        .append(ToString(data.eventDataExtra))
        .append("ma, boot time after boot = ")
        .append(ToString(bootTimeMs))
        .append("ms\n");
    if (!data.eventDebugInfo.empty()) {
        debugInfo.append("Additional debug info: ")
            .append(data.eventDebugInfo);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsDetector::handleDispalyInfo(StatsUtils::StatsData data, long bootTimeMs, std::string& debugInfo)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::string screenState;
    if (data.state == StatsUtils::STATS_STATE_DISPLAY_OFF) {
        screenState = "off";
    } else if (data.state == StatsUtils::STATS_STATE_DISPLAY_ON) {
        screenState = "on";
    } else if (data.state == StatsUtils::STATS_STATE_DISPLAY_DIM) {
        screenState = "dim";
    } else if (data.state == StatsUtils::STATS_STATE_DISPLAY_SUSPEND) {
        screenState = "suspend";
    } else {
        screenState = "unknown state";
    }

    debugInfo.append("Display event: Screen is in ")
        .append(screenState)
        .append(" state, brigntness level = ")
        .append(ToString(data.level))
        .append(", boot time after boot = ")
        .append(ToString(bootTimeMs))
        .append("ms\n");
    if (!data.eventDebugInfo.empty()) {
        debugInfo.append("Additional debug info: ")
            .append(data.eventDebugInfo);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsDetector::handleWakelockInfo(StatsUtils::StatsData data, long bootTimeMs, std::string& debugInfo)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::string eventState;
    if (data.state == StatsUtils::STATS_STATE_ACTIVATED) {
        eventState = "LOCK";
    } else {
        eventState = "UNLOCK";
    }
    debugInfo.append("Wakelock event: UID = ")
        .append(ToString(data.uid))
        .append(", PID = ")
        .append(ToString(data.pid))
        .append(", wakelock type = ")
        .append(ToString(data.eventDataType))
        .append(", wakelock name = ")
        .append(data.eventDataName)
        .append(", wakelock state = ")
        .append(eventState)
        .append(", boot time after boot = ")
        .append(ToString(bootTimeMs))
        .append("ms\n");
    if (!data.eventDebugInfo.empty()) {
        debugInfo.append("Additional debug info: ")
            .append(data.eventDebugInfo);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsDetector::handleWorkschedulerInfo(StatsUtils::StatsData data, long bootTimeMs, std::string& debugInfo)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    debugInfo.append("WorkScheduler event: UID = ")
        .append(ToString(data.uid))
        .append(", PID = ")
        .append(ToString(data.pid))
        .append(", work type = ")
        .append(ToString(data.eventDataType))
        .append(", work interval = ")
        .append(ToString(data.eventDataExtra))
        .append(", work state = ")
        .append(ToString(data.state))
        .append(", boot time after boot = ")
        .append(ToString(bootTimeMs))
        .append("ms\n");
    if (!data.eventDebugInfo.empty()) {
        debugInfo.append("Additional debug info: ")
            .append(data.eventDebugInfo);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsDetector::handlePhoneInfo(StatsUtils::StatsData data, long bootTimeMs, std::string& debugInfo)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::string eventState;
    if (data.state == StatsUtils::STATS_STATE_ACTIVATED) {
        eventState = "Call active";
    } else {
        eventState = "Call terminated";
    }
    debugInfo.append("Phone event: phone state = ")
        .append(eventState)
        .append(", boot time after boot = ")
        .append(ToString(bootTimeMs))
        .append("ms\n");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsDetector::handleFlashlightInfo(StatsUtils::StatsData data, long bootTimeMs, std::string& debugInfo)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::string eventState;
    if (data.state == StatsUtils::STATS_STATE_ACTIVATED) {
        eventState = "ON";
    } else {
        eventState = "OFF";
    }
    debugInfo.append("Flashlight event: UID = ")
        .append(ToString(data.uid))
        .append(", PID = ")
        .append(ToString(data.pid))
        .append(", flashlight state = ")
        .append(eventState)
        .append(", boot time after boot = ")
        .append(ToString(bootTimeMs))
        .append("ms\n");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsDetector::handleDebugInfo(StatsUtils::StatsData data)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle type: %{public}s", StatsUtils::ConvertStatsType(data.type).c_str());
    long bootTimeMs = StatsHelper::GetBootTimeMs();
    auto core = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance()->GetBatteryStatsCore();
    std::string debugInfo;
    switch (data.type) {
        case StatsUtils::STATS_TYPE_THERMAL:
            handleThermalInfo(data, bootTimeMs, debugInfo);
            break;
        case StatsUtils::STATS_TYPE_BATTERY:
            handleBatteryInfo(data, bootTimeMs, debugInfo);
            break;
        case StatsUtils::STATS_TYPE_WORKSCHEDULER:
            handleWorkschedulerInfo(data, bootTimeMs, debugInfo);
            break;
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD:
            handleWakelockInfo(data, bootTimeMs, debugInfo);
            break;
        case StatsUtils::STATS_TYPE_SCREEN_ON:
            handleDispalyInfo(data, bootTimeMs, debugInfo);
            break;
        case StatsUtils::STATS_TYPE_PHONE_ACTIVE:
            handlePhoneInfo(data, bootTimeMs, debugInfo);
            break;
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON:
            handleFlashlightInfo(data, bootTimeMs, debugInfo);
            break;
        default:
            STATS_HILOGE(STATS_MODULE_SERVICE, "Got invalid type");
            break;
    }
    core->UpdateDebugInfo(debugInfo);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS
