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

#include "battery_stats_listener.h"

#include <string>
#include <strstream>

#include "display_power_info.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
void BatteryStatsListener::OnHandle(const std::string& domain, const std::string& eventName,
    const int eventType, const std::string& eventDetail)
{
    STATS_HILOGD(STATS_MODULE_SERVICE, "EventDetail: %{public}s", eventDetail.c_str());
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
    auto detector = statsService->GetBatteryStatsDetector();
    StatsUtils::StatsData data;
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    std::istrstream is(eventDetail.c_str());
    if (parseFromStream(reader, is, &root, &errors)) {
        if (root["name_"].asString() == "POWER_RUNNINGLOCK") {
            processWakelockEvent(data, root);
        } else if (root["name_"].asString() == "POWER_SCREEN") {
            processDispalyEvent(data, root);
        } else if (root["name_"].asString() == "POWER_BATTERY") {
            processBatteryEvent(data, root);
        } else if (root["name_"].asString() == "POWER_TEMPERATURE") {
            processThermalEvent(data, root);
        } else if (root["name_"].asString() == "POWER_WORkSCHEDULER") {
            processWorkschedulerEvent(data, root);
        } else if (root["name_"].asString() == "POWER_PHONE") {
            processPhoneEvent(data, root);
        } else if (root["name_"].asString() == "POWER_FLASHLIGHT") {
            processFlashlightEvent(data, root);
        }
        detector->HandleStatsChangedEvent(data);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Parse hisysevent data failed");
    }
}

void BatteryStatsListener::processPhoneEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_PHONE_ACTIVE;
    if (!root["STATE"].asString().empty()) {
        if (root["STATE"].asString() == "1") {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (root["STATE"].asString() == "0") {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::processFlashlightEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_FLASHLIGHT_ON;
    if (!root["UID"].asString().empty()) {
        data.uid = stoi(root["UID"].asString());
    }
    if (!root["PID"].asString().empty()) {
        data.pid = stoi(root["PID"].asString());
    }
    if (!root["STATE"].asString().empty()) {
        if (root["STATE"].asString() == "1") {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (root["STATE"].asString() == "0") {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::processWakelockEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_WAKELOCK_HOLD;
    if (!root["UID"].asString().empty()) {
        data.uid = stoi(root["UID"].asString());
    }
    if (!root["PID"].asString().empty()) {
        data.pid = stoi(root["PID"].asString());
    }
    if (!root["STATE"].asString().empty()) {
        if (root["STATE"].asString() == "1") {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (root["STATE"].asString() == "0") {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
    if (!root["TYPE"].asString().empty()) {
        data.eventDataType = stoi(root["TYPE"].asString());
    }
    if (!root["NAME"].asString().empty()) {
        data.eventDataName = root["NAME"].asString();
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::processDispalyEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_SCREEN_ON;
    if (!root["STATE"].asString().empty()) {
        DisplayPowerMgr::DisplayState displayState = DisplayPowerMgr::DisplayState(stoi(root["STATE"].asString()));
        switch (displayState) {
            case DisplayPowerMgr::DisplayState::DISPLAY_OFF:
                data.state = StatsUtils::STATS_STATE_DISPLAY_OFF;
                break;
            case DisplayPowerMgr::DisplayState::DISPLAY_ON:
                data.state = StatsUtils::STATS_STATE_DISPLAY_ON;
                break;
            case DisplayPowerMgr::DisplayState::DISPLAY_DIM:
                data.state = StatsUtils::STATS_STATE_DISPLAY_DIM;
                break;
            case DisplayPowerMgr::DisplayState::DISPLAY_SUSPEND:
                data.state = StatsUtils::STATS_STATE_DISPLAY_SUSPEND;
                break;
            case DisplayPowerMgr::DisplayState::DISPLAY_UNKNOWN:
                data.state = StatsUtils::STATS_STATE_DISPLAY_UNKNOWN;
                break;
            default:
                break;
        }
    }
    if (!root["BRIGHTNESS"].asString().empty()) {
        data.level = stoi(root["BRIGHTNESS"].asString());
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::processBatteryEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_BATTERY;
    if (!root["BATTERY_LEVEL"].asString().empty()) {
        data.level = stoi(root["BATTERY_LEVEL"].asString());
    }
    if (!root["CURRENT_NOW"].asString().empty()) {
        data.eventDataExtra = stoi(root["CURRENT_NOW"].asString());
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::processThermalEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_THERMAL;
    if (!root["NAME"].asString().empty()) {
        data.eventDataName = root["NAME"].asString();
    }
    if (!root["TEMPERATURE"].asString().empty()) {
        data.eventDataExtra = stoi(root["TEMPERATURE"].asString());
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::processWorkschedulerEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_WORKSCHEDULER;
    if (!root["UID"].asString().empty()) {
        data.uid = stoi(root["UID"].asString());
    }
    if (!root["PID"].asString().empty()) {
        data.pid = stoi(root["PID"].asString());
    }
    if (!root["STATE"].asString().empty()) {
        data.state = StatsUtils::StatsState(stoi(root["STATE"].asString()));
    }
    if (!root["TYPE"].asString().empty()) {
        data.eventDataType = stoi(root["TYPE"].asString());
    }
    if (!root["INTERVAL"].asString().empty()) {
        data.eventDataExtra = stoi(root["INTERVAL"].asString());
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::OnServiceDied()
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "Service disconnected");
    exit(0);
}
} // namespace PowerMgr
} // namespace OHOS