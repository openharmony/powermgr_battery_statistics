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

#include "battery_stats_listener.h"

#include <string>
#include <strstream>

#include "bt_def.h"
#include "display_power_info.h"
#include "network_search_types.h"
#include "wifi_hisysevent.h"

#include "battery_stats_service.h"
#include "stats_log.h"
#include "stats_types.h"

namespace OHOS {
namespace PowerMgr {
void BatteryStatsListener::OnHandle(const std::string& domain, const std::string& eventName,
    const int eventType, const std::string& eventDetail)
{
    STATS_HILOGD(COMP_SVC, "EventDetail: %{public}s", eventDetail.c_str());
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    std::istrstream is(eventDetail.c_str());
    if (parseFromStream(reader, is, &root, &errors)) {
        ProcessHiSysEvent(root);
    } else {
        STATS_HILOGD(COMP_SVC, "Parse hisysevent data failed");
    }
}

void BatteryStatsListener::ProcessHiSysEvent(const Json::Value& root)
{
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
    auto detector = statsService->GetBatteryStatsDetector();
    StatsUtils::StatsData data;
    data.eventDebugInfo.clear();
    std::string eventName = root["name_"].asString();
    if (eventName == "POWER_RUNNINGLOCK") {
        ProcessWakelockEvent(data, root);
    } else if (eventName == "POWER_SCREEN" || eventName == "SCREEN_STATE" || eventName == "BRIGHTNESS_NIT" ||
        eventName == "BACKLIGHT_DISCOUNT" || eventName == "AMBIENT_LIGHT") {
        ProcessDispalyEvent(data, root);
    } else if (eventName == "BATTERY_CHANGED") {
        ProcessBatteryEvent(data, root);
    } else if (eventName == "POWER_TEMPERATURE" || eventName == "THERMAL_LEVEL_CHANGED") {
        ProcessThermalEvent(data, root);
    } else if (eventName == "POWER_WORKSCHEDULER" || eventName == "WORK_ADD" || eventName == "WORK_REMOVE" ||
        eventName == "WORK_START" || eventName == "WORK_STOP") {
        ProcessWorkschedulerEvent(data, root);
    } else if (eventName == "POWER_PHONE") {
        ProcessPhoneEvent(data, root);
    } else if (eventName == "TORCH_STATE") {
        ProcessFlashlightEvent(data, root);
    } else if (eventName == "CAMERA_CONNECT" || eventName == "CAMERA_DISCONNECT" ||
        eventName == "FLASHLIGHT_ON" || eventName == "FLASHLIGHT_OFF") {
        ProcessCameraEvent(data, root);
    } else if (eventName == "AUDIO_STREAM_CHANGE") {
        ProcessAudioEvent(data, root);
    } else if (eventName == "POWER_SENSOR_GRAVITY" || eventName == "POWER_SENSOR_PROXIMITY") {
        ProcessSensorEvent(data, root);
    } else if (eventName == "POWER_RADIO") {
        ProcessRadioEvent(data, root);
    } else if (eventName == "GNSS_STATE") {
        ProcessGpsEvent(data, root);
    } else if (eventName == "BLUETOOTH_BR_STATE" || eventName == "BLUETOOTH_SCAN_STATE") {
        ProcessBluetoothEvent(data, root);
    } else if (eventName == "WIFI_STATE" || eventName == "WIFI_SCAN") {
        ProcessWifiEvent(data, root);
    } else if (eventName == "START_REMOTE_ABILITY") {
        ProcessDistributedSchedulerEvent(data, root);
    } else if (eventName == "ALARM_TRIGGER") {
        ProcessAlarmEvent(data, root);
    }
    detector->HandleStatsChangedEvent(data);
}

void BatteryStatsListener::ProcessCameraEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    std::string eventName = root["name_"].asString();
    if (eventName == "CAMERA_CONNECT" || eventName == "CAMERA_DISCONNECT") {
        data.type = StatsUtils::STATS_TYPE_CAMERA_ON;
        if (!root["UID"].asString().empty()) {
            data.uid = stoi(root["UID"].asString());
        }
        if (!root["PID"].asString().empty()) {
            data.pid = stoi(root["PID"].asString());
        }
        if (!root["ID"].asString().empty()) {
            data.deviceId = root["ID"].asString();
        }
        if (eventName == "CAMERA_CONNECT") {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    } else if (eventName == "FLASHLIGHT_ON" || eventName == "FLASHLIGHT_OFF") {
        data.type = StatsUtils::STATS_TYPE_CAMERA_FLASHLIGHT_ON;
        if (eventName == "FLASHLIGHT_ON") {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
}

void BatteryStatsListener::ProcessAudioEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_AUDIO_ON;
    if (!root["UID"].asString().empty()) {
        data.uid = stoi(root["UID"].asString());
    }
    if (!root["PID"].asString().empty()) {
        data.pid = stoi(root["PID"].asString());
    }
    if (!root["STATE"].asString().empty()) {
        AudioState audioState = AudioState(stoi(root["STATE"].asString()));
        switch (audioState) {
            case AudioState::AUDIO_STATE_RUNNING:
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
                break;
            case AudioState::AUDIO_STATE_STOPPED:
            case AudioState::AUDIO_STATE_RELEASED:
            case AudioState::AUDIO_STATE_PAUSED:
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
                break;
            default:
                break;
        }
    }
}

void BatteryStatsListener::ProcessSensorEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
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
}

void BatteryStatsListener::ProcessRadioEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
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
}

void BatteryStatsListener::ProcessGpsEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
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
}

void BatteryStatsListener::ProcessBluetoothEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
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
}

void BatteryStatsListener::ProcessWifiEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
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
}

void BatteryStatsListener::ProcessPhoneEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_PHONE_ACTIVE;
    if (!root["STATE"].asString().empty()) {
        if (root["STATE"].asString() == "1") {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (root["STATE"].asString() == "0") {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
}

void BatteryStatsListener::ProcessFlashlightEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
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
}

void BatteryStatsListener::ProcessWakelockEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
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
}

void BatteryStatsListener::ProcessDispalyDebugInfo(StatsUtils::StatsData& data, const Json::Value& root)
{
    if (!root["name_"].asString().empty()) {
        data.eventDebugInfo.append("Event name = ").append(root["name_"].asString());
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

void BatteryStatsListener::ProcessDispalyEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
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
    }
    ProcessDispalyDebugInfo(data, root);
}

void BatteryStatsListener::ProcessBatteryEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_BATTERY;
    if (!root["LEVEL"].asString().empty()) {
        data.level = stoi(root["LEVEL"].asString());
    }
    if (!root["CHARGER"].asString().empty()) {
        data.eventDataExtra = stoi(root["CHARGER"].asString());
    }
    if (!root["VOLTAGE"].asString().empty()) {
        data.eventDebugInfo.append(" Voltage = ").append(root["VOLTAGE"].asString());
    }
    if (!root["HEALTH"].asString().empty()) {
        data.eventDebugInfo.append(" Health = ").append(root["HEALTH"].asString());
    }
    if (!root["TEMPERATURE"].asString().empty()) {
        data.eventDebugInfo.append(" Temperature = ").append(root["TEMPERATURE"].asString());
    }
}

void BatteryStatsListener::ProcessThermalEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
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
}

void BatteryStatsListener::ProcessWorkschedulerEvent(StatsUtils::StatsData& data, const Json::Value& root)
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

void BatteryStatsListener::ProcessDistributedSchedulerEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_DISTRIBUTEDSCHEDULER;
    if (!root["name_"].asString().empty()) {
        data.eventDebugInfo.append("Event name = ").append(root["name_"].asString());
    }
    if (!root["CALLING_TYPE"].asString().empty()) {
        data.eventDebugInfo.append(" Calling Type = ").append(root["CALLING_TYPE"].asString());
    }
    if (!root["CALLING_UID"].asString().empty()) {
        data.eventDebugInfo.append(" Calling Uid = ").append(root["CALLING_UID"].asString());
    }
    if (!root["CALLING_PID"].asString().empty()) {
        data.eventDebugInfo.append(" Calling Pid = ").append(root["CALLING_PID"].asString());
    }
    if (!root["TARGET_BUNDLE"].asString().empty()) {
        data.eventDebugInfo.append(" Target Bundle Name = ").append(root["TARGET_BUNDLE"].asString());
    }
    if (!root["TARGET_ABILITY"].asString().empty()) {
        data.eventDebugInfo.append(" Target Ability Name = ").append(root["TARGET_ABILITY"].asString());
    }
    if (!root["CALLING_APP_UID"].asString().empty()) {
        data.eventDebugInfo.append(" Calling App Uid = ").append(root["CALLING_APP_UID"].asString());
    }
    if (!root["RESULT"].asString().empty()) {
        data.eventDebugInfo.append(" RESULT = ").append(root["RESULT"].asString());
    }
}

void BatteryStatsListener::ProcessAlarmEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_ALARM;
    data.traffic = 1;
    if (!root["UID"].asString().empty()) {
        data.uid = stoi(root["UID"].asString());
    }
    if (!root["PID"].asString().empty()) {
        data.pid = stoi(root["PID"].asString());
    }
}

void BatteryStatsListener::OnServiceDied()
{
    STATS_HILOGD(COMP_SVC, "Service disconnected");
    exit(0);
}
} // namespace PowerMgr
} // namespace OHOS
