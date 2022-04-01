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

#include "bt_def.h"
#include "display_power_info.h"
#include "network_search_types.h"
#include "wifi_hisysevent.h"

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
    data.eventDebugInfo.clear();
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    std::istrstream is(eventDetail.c_str());
    if (parseFromStream(reader, is, &root, &errors)) {
        std::string eventName = root["name_"].asString();
        if (eventName == "POWER_RUNNINGLOCK") {
            processWakelockEvent(data, root);
        } else if (eventName == "POWER_SCREEN" || eventName == "SCREEN_STATE" || eventName == "BRIGHTNESS_NIT" ||
            eventName == "BACKLIGHT_DISCOUNT" || eventName == "AMBIENT_LIGHT") {
            processDispalyEvent(data, root);
        } else if (eventName == "POWER_BATTERY" || eventName == "BATTERY_CHANGED") {
            processBatteryEvent(data, root);
        } else if (eventName == "POWER_TEMPERATURE" || eventName == "TEMPERATURE_CHANGED") {
            processThermalEvent(data, root);
        } else if (eventName == "POWER_WORKSCHEDULER" || eventName == "WORK_ADD" || eventName == "WORK_REMOVE" ||
            eventName == "WORK_START" || eventName == "WORK_STOP") {
            processWorkschedulerEvent(data, root);
        } else if (eventName == "POWER_PHONE") {
            processPhoneEvent(data, root);
        } else if (eventName == "POWER_FLASHLIGHT") {
            processFlashlightEvent(data, root);
        } else if (eventName == "POWER_CAMERA") {
            processCameraEvent(data, root);
        } else if (eventName == "POWER_AUDIO") {
            processAudioEvent(data, root);
        } else if (eventName == "POWER_SENSOR_GRAVITY" || eventName == "POWER_SENSOR_PROXIMITY") {
            processSensorEvent(data, root);
        } else if (eventName == "POWER_RADIO") {
            processRadioEvent(data, root);
        } else if (eventName == "GNSS_STATE") {
            processGpsEvent(data, root);
        } else if (eventName == "BLUETOOTH_BR_STATE" || eventName == "BLUETOOTH_SCAN_STATE") {
            processBluetoothEvent(data, root);
        } else if (eventName == "WIFI_STATE" || eventName == "WIFI_SCAN") {
            processWifiEvent(data, root);
        }
        detector->HandleStatsChangedEvent(data);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Parse hisysevent data failed");
    }
}

void BatteryStatsListener::processCameraEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_CAMERA_ON;
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

void BatteryStatsListener::processAudioEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_AUDIO_ON;
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

void BatteryStatsListener::processSensorEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (root["name_"].asString() == "POWER_SENSOR_GRAVITY") {
        data.type = StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON;
    } else if (root["name_"].asString() == "POWER_SENSOR_PROXIMITY") {
        data.type = StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON;
    }

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

void BatteryStatsListener::processRadioEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_RADIO_ON;
    if (!root["STATE"].asString().empty()) {
        Telephony::RegServiceState radioState = Telephony::RegServiceState(stoi(root["STATE"].asString()));
        switch (radioState) {
            case Telephony::RegServiceState::REG_STATE_UNKNOWN:
                data.state = StatsUtils::STATS_STATE_NETWORK_UNKNOWN;
                break;
            case Telephony::RegServiceState::REG_STATE_IN_SERVICE:
                data.state = StatsUtils::STATS_STATE_NETWORK_IN_SERVICE;
                break;
            case Telephony::RegServiceState::REG_STATE_NO_SERVICE:
                data.state = StatsUtils::STATS_STATE_NETWORK_NO_SERVICE;
                break;
            case Telephony::RegServiceState::REG_STATE_EMERGENCY_ONLY:
                data.state = StatsUtils::STATS_STATE_NETWORK_EMERGENCY_ONLY;
                break;
            case Telephony::RegServiceState::REG_STATE_SEARCH:
                data.state = StatsUtils::STATS_STATE_NETWORK_SEARCH;
                break;
            default:
                break;
        }
    }
    if (!root["SIGNAL"].asString().empty()) {
        data.level = stoi(root["SIGNAL"].asString());
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::processGpsEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_GPS_ON;
    if (!root["UID"].asString().empty()) {
        data.uid = stoi(root["UID"].asString());
    }
    if (!root["PID"].asString().empty()) {
        data.pid = stoi(root["PID"].asString());
    }
    if (!root["STATE"].asString().empty()) {
        if (root["STATE"].asString() == "start") {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (root["STATE"].asString() == "stop") {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::processBluetoothEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (root["name_"].asString() == "BLUETOOTH_BR_STATE") {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_ON;
        if (!root["BR_STATE"].asString().empty()) {
            if (stoi(root["BR_STATE"].asString()) == bluetooth::BTStateID::STATE_TURN_ON) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (stoi(root["BR_STATE"].asString()) == bluetooth::BTStateID::STATE_TURN_OFF) {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
            }
        }
    } else if (root["name_"].asString() == "BLUETOOTH_SCAN_STATE") {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_SCAN;
        if (!root["BR_SCAN_STATE"].asString().empty()) {
            if (stoi(root["BR_SCAN_STATE"].asString()) == bluetooth::DISCOVERY_STARTED) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (stoi(root["BR_SCAN_STATE"].asString()) == bluetooth::DISCOVERY_STOPED) {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
            }
        }
        if (!root["UID"].asString().empty()) {
            data.uid = stoi(root["UID"].asString());
        }
        if (!root["PID"].asString().empty()) {
            data.pid = stoi(root["PID"].asString());
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::processWifiEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_WIFI_ON;
    int32_t wifiEnable = static_cast<int32_t>(Wifi::WifiOperType::ENABLE);
    int32_t wifiDisable = static_cast<int32_t>(Wifi::WifiOperType::DISABLE);
    if (!root["OPER_TYPE"].asString().empty()) {
        if (stoi(root["OPER_TYPE"].asString()) == wifiEnable) {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (stoi(root["OPER_TYPE"].asString()) == wifiDisable) {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
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
    if (!root["LOG_LEVEL"].asString().empty()) {
        data.eventDebugInfo.append(" LOG_LEVEL = ").append(root["LOG_LEVEL"].asString());
    }
    if (!root["TAG"].asString().empty()) {
        data.eventDebugInfo.append(" TAG = ").append(root["TAG"].asString());
    }
    if (!root["MESSAGE"].asString().empty()) {
        data.eventDebugInfo.append(" MESSAGE = ").append(root["MESSAGE"].asString());
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::processDispalyDebugInfo(StatsUtils::StatsData& data, const Json::Value& root)
{
    if (root["name_"].asString().empty()) {
        data.eventDebugInfo.append(root["name_"].asString()).append(":");
    }
    if (!root["STATE"].asString().empty()) {
        data.eventDebugInfo.append(" Screen state = ").append(root["STATE"].asString());
    }
    if (!root["BRIGHTNESS"].asString().empty()) {
        data.eventDebugInfo.append(" Screen brightness = ").append(root["BRIGHTNESS"].asString());
    }
    if (!root["REASON"].asString().empty()) {
        data.eventDebugInfo.append(" Brightness reason = ").append(root["REASON"].asString());
    }
    if (!root["NIT"].asString().empty()) {
        data.eventDebugInfo.append(" Brightness nit = ").append(root["NIT"].asString());
    }
    if (!root["RATIO"].asString().empty()) {
        data.eventDebugInfo.append(" Ratio = ").append(root["RATIO"].asString());
    }
    if (!root["TYPE"].asString().empty()) {
        data.eventDebugInfo.append(" Ambient type = ").append(root["TYPE"].asString());
    }
    if (!root["LEVEL"].asString().empty()) {
        data.eventDebugInfo.append(" Ambient brightness = ").append(root["LEVEL"].asString());
    }
}

void BatteryStatsListener::processDispalyEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    data.type = StatsUtils::STATS_TYPE_SCREEN_ON;
    if (root["name_"].asString() == "POWER_SCREEN") {
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
    } else {
        processDispalyDebugInfo(data, root);
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
    if (!root["LEVEL"].asString().empty()) {
        data.eventDebugInfo.append(" Battery level = ").append(root["LEVEL"].asString());
    }
    if (!root["CHARGER"].asString().empty()) {
        data.eventDebugInfo.append(" Charger type = ").append(root["CHARGER"].asString());
    }
    if (!root["VOLTAGE"].asString().empty()) {
        data.eventDebugInfo.append(" Voltage = ").append(root["VOLTAGE"].asString());
    }
    if (!root["TEMP"].asString().empty()) {
        data.eventDebugInfo.append(" Temperature = ").append(root["TEMP"].asString());
    }
    if (!root["HEALTH"].asString().empty()) {
        data.eventDebugInfo.append(" Health = ").append(root["HEALTH"].asString());
    }
    if (!root["CURRENT"].asString().empty()) {
        data.eventDebugInfo.append(" Current = ").append(root["CURRENT"].asString());
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
    if (!root["LEVEL"].asString().empty()) {
        data.eventDebugInfo.append(" Temperature level = ").append(root["LEVEL"].asString());
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsListener::processWorkschedulerEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_WORKSCHEDULER;
    if (root["name_"].asString() == "POWER_WORKSCHEDULER") {
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
    } else {
        if (!root["name_"].asString().empty()) {
            data.eventDebugInfo.append(root["name_"].asString()).append(":");
        }
        if (!root["UID"].asString().empty()) {
            data.uid = stoi(root["UID"].asString());
        }
        if (!root["PID"].asString().empty()) {
            data.pid = stoi(root["PID"].asString());
        }
        if (!root["NAME"].asString().empty()) {
            data.eventDebugInfo.append(" Bundle name = ").append(root["NAME"].asString());
        }
        if (!root["WORKID"].asString().empty()) {
            data.eventDebugInfo.append(" Work ID = ").append(root["WORKID"].asString());
        }
        if (!root["TRIGGER"].asString().empty()) {
            data.eventDebugInfo.append(" Trigger conditions = ").append(root["TRIGGER"].asString());
        }
        if (!root["TYPE"].asString().empty()) {
            data.eventDebugInfo.append(" Work type = ").append(root["TYPE"].asString());
        }
        if (!root["INTERVAL"].asString().empty()) {
            data.eventDebugInfo.append(" Interval = ").append(root["INTERVAL"].asString());
        }
    }
}

void BatteryStatsListener::OnServiceDied()
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "Service disconnected");
    exit(0);
}
} // namespace PowerMgr
} // namespace OHOS