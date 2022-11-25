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
#include "call_manager_inner_type.h"
#include "display_power_info.h"
#include "wifi_hisysevent.h"

#include "battery_stats_service.h"
#include "stats_hisysevent.h"
#include "stats_log.h"
#include "stats_types.h"

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr int32_t THERMAL_RATIO_BEGIN = 0;
constexpr int32_t THERMAL_RATIO_LENGTH = 4;
}
void BatteryStatsListener::OnEvent(std::shared_ptr<HiviewDFX::HiSysEventRecord> sysEvent)
{
    if (sysEvent == nullptr) {
        return;
    }
    std::string eventName = sysEvent->GetEventName();
    std::string eventDetail = sysEvent->AsJson();
    STATS_HILOGD(COMP_SVC, "EventDetail: %{public}s", eventDetail.c_str());
    if (!StatsHiSysEvent::CheckHiSysEvent(eventName)) {
        return;
    }
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    std::istrstream is(eventDetail.c_str());
    if (parseFromStream(reader, is, &root, &errors)) {
        ProcessHiSysEvent(eventName, root);
    } else {
        STATS_HILOGW(COMP_SVC, "Parse hisysevent data failed");
    }
}

void BatteryStatsListener::ProcessHiSysEvent(const std::string& eventName, const Json::Value& root)
{
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
    auto detector = statsService->GetBatteryStatsDetector();
    StatsUtils::StatsData data;
    data.eventDebugInfo.clear();
    if (eventName == "POWER_RUNNINGLOCK") {
        ProcessWakelockEvent(data, root);
    } else if (eventName == "SCREEN_STATE" || eventName == "BRIGHTNESS_NIT" ||  eventName == "BACKLIGHT_DISCOUNT"
        || eventName == "AMBIENT_LIGHT") {
        ProcessDispalyEvent(data, root, eventName);
    } else if (eventName == "BATTERY_CHANGED") {
        ProcessBatteryEvent(data, root);
    } else if (eventName == "POWER_TEMPERATURE" || eventName == "THERMAL_LEVEL_CHANGED" ||
        eventName == "THERMAL_ACTION_TRIGGERED") {
        ProcessThermalEvent(data, root);
    } else if (eventName == "POWER_WORKSCHEDULER" || eventName == "WORK_ADD" || eventName == "WORK_REMOVE" ||
        eventName == "WORK_START" || eventName == "WORK_STOP") {
        ProcessWorkschedulerEvent(data, root);
    } else if (eventName == "CALL_STATE" || eventName == "DATA_CONNECTION_STATE") {
        ProcessPhoneEvent(data, root, eventName);
    } else if (eventName == "TORCH_STATE") {
        ProcessFlashlightEvent(data, root);
    } else if (eventName == "CAMERA_CONNECT" || eventName == "CAMERA_DISCONNECT" ||
        eventName == "FLASHLIGHT_ON" || eventName == "FLASHLIGHT_OFF") {
        ProcessCameraEvent(data, root, eventName);
    } else if (eventName == "AUDIO_STREAM_CHANGE") {
        ProcessAudioEvent(data, root);
    } else if (eventName == "POWER_SENSOR_GRAVITY" || eventName == "POWER_SENSOR_PROXIMITY") {
        ProcessSensorEvent(data, root, eventName);
    } else if (eventName == "GNSS_STATE") {
        ProcessGnssEvent(data, root);
    } else if (eventName == "BLUETOOTH_BR_SWITCH_STATE" || eventName == "BLUETOOTH_DISCOVERY_STATE" ||
        eventName == "BLUETOOTH_BLE_STATE" || eventName == "BLUETOOTH_BLE_SCAN_START" ||
        eventName == "BLUETOOTH_BLE_SCAN_STOP") {
        ProcessBluetoothEvent(data, root, eventName);
    } else if (eventName == "WIFI_CONNECTION" || eventName == "WIFI_SCAN") {
        ProcessWifiEvent(data, root, eventName);
    } else if (eventName == "START_REMOTE_ABILITY") {
        ProcessDistributedSchedulerEvent(data, root);
    } else if (eventName == "MISC_TIME_STATISTIC_REPORT") {
        ProcessAlarmEvent(data, root);
    }
    detector->HandleStatsChangedEvent(data);
}

void BatteryStatsListener::ProcessCameraEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
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

void BatteryStatsListener::ProcessSensorEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    if (eventName == "POWER_SENSOR_GRAVITY") {
        data.type = StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON;
    } else if (eventName == "POWER_SENSOR_PROXIMITY") {
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

void BatteryStatsListener::ProcessGnssEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_GNSS_ON;
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

void BatteryStatsListener::ProcessBluetoothBrEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    if (eventName == "BLUETOOTH_BR_SWITCH_STATE") {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_BR_ON;
        if (!root["STATE"].asString().empty()) {
            if (stoi(root["STATE"].asString()) == bluetooth::BTStateID::STATE_TURN_ON) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (stoi(root["STATE"].asString()) == bluetooth::BTStateID::STATE_TURN_OFF) {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
            }
        }
    } else if (eventName == "BLUETOOTH_DISCOVERY_STATE") {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_BR_SCAN;
        if (!root["STATE"].asString().empty()) {
            if (stoi(root["STATE"].asString()) == bluetooth::DISCOVERY_STARTED) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (stoi(root["STATE"].asString()) == bluetooth::DISCOVERY_STOPED) {
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

void BatteryStatsListener::ProcessBluetoothBleEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    if (eventName == "BLUETOOTH_BLE_STATE") {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_BLE_ON;
        if (!root["STATE"].asString().empty()) {
            if (stoi(root["STATE"].asString()) == bluetooth::BTStateID::STATE_TURN_ON) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (stoi(root["STATE"].asString()) == bluetooth::BTStateID::STATE_TURN_OFF) {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
            }
        }
    } else if (eventName == "BLUETOOTH_BLE_SCAN_START" || eventName == "BLUETOOTH_BLE_SCAN_STOP") {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_BLE_SCAN;
        if (eventName == "BLUETOOTH_BLE_SCAN_START") {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (eventName == "BLUETOOTH_BLE_SCAN_STOP") {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
        if (!root["UID"].asString().empty()) {
            data.uid = stoi(root["UID"].asString());
        }
        if (!root["PID"].asString().empty()) {
            data.pid = stoi(root["PID"].asString());
        }
    }
}

void BatteryStatsListener::ProcessBluetoothEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    if (eventName == "BLUETOOTH_BR_SWITCH_STATE" || eventName == "BLUETOOTH_DISCOVERY_STATE") {
        ProcessBluetoothBrEvent(data, root, eventName);
    } else if (eventName == "BLUETOOTH_BLE_STATE" ||eventName == "BLUETOOTH_BLE_SCAN_START" ||
        eventName == "BLUETOOTH_BLE_SCAN_STOP") {
        ProcessBluetoothBleEvent(data, root, eventName);
    }
}

void BatteryStatsListener::ProcessWifiEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    if (eventName == "WIFI_CONNECTION") {
        data.type = StatsUtils::STATS_TYPE_WIFI_ON;
        if (!root["TYPE"].asString().empty()) {
            Wifi::WifiConnectionType connectionType = Wifi::WifiConnectionType(stoi(root["TYPE"].asString()));
            switch (connectionType) {
                case Wifi::WifiConnectionType::CONNECT:
                    data.state = StatsUtils::STATS_STATE_ACTIVATED;
                    break;
                case Wifi::WifiConnectionType::DISCONNECT:
                    data.state = StatsUtils::STATS_STATE_DEACTIVATED;
                    break;
                default:
                    break;
            }
        }
    } else if (eventName == "WIFI_SCAN") {
        data.type = StatsUtils::STATS_TYPE_WIFI_SCAN;
        data.traffic = 1;
    }
}

void BatteryStatsListener::ProcessPhoneDebugInfo(StatsUtils::StatsData& data, const Json::Value& root)
{
    if (!root["name_"].asString().empty()) {
        data.eventDebugInfo.append("Event name = ").append(root["name_"].asString());
    }
    if (!root["STATE"].asString().empty()) {
        data.eventDebugInfo.append(" State = ").append(root["STATE"].asString());
    }
    if (!root["SLOT_ID"].asString().empty()) {
        data.eventDebugInfo.append(" Slot ID = ").append(root["SLOT_ID"].asString());
    }
    if (!root["INDEX_ID"].asString().empty()) {
        data.eventDebugInfo.append(" Index ID = ").append(root["INDEX_ID"].asString());
    }
}

void BatteryStatsListener::ProcessPhoneEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    if (eventName == "CALL_STATE") {
        data.type = StatsUtils::STATS_TYPE_PHONE_ACTIVE;
        if (!root["STATE"].asString().empty()) {
            Telephony::TelCallState callState = Telephony::TelCallState(stoi(root["STATE"].asString()));
            switch (callState) {
                case Telephony::TelCallState::CALL_STATUS_ACTIVE:
                    data.state = StatsUtils::STATS_STATE_ACTIVATED;
                    break;
                case Telephony::TelCallState::CALL_STATUS_DISCONNECTED:
                    data.state = StatsUtils::STATS_STATE_DEACTIVATED;
                    break;
                default:
                    break;
            }
        }
    } else if (eventName == "DATA_CONNECTION_STATE") {
        data.type = StatsUtils::STATS_TYPE_PHONE_DATA;
        if (!root["STATE"].asString().empty()) {
            if (root["STATE"].asString() == "1") {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (root["STATE"].asString() == "0") {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
            }
        }
    }

    /**
     * The average power consumption of phone call and phone data is divided by level
     * However, the Telephony event has no input level information, so use level 0
     */
    data.level = 0;
    ProcessPhoneDebugInfo(data, root);
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

void BatteryStatsListener::ProcessDispalyEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    data.type = StatsUtils::STATS_TYPE_DISPLAY;
    if (eventName == "SCREEN_STATE") {
        data.type = StatsUtils::STATS_TYPE_SCREEN_ON;
        if (!root["STATE"].asString().empty()) {
            DisplayPowerMgr::DisplayState displayState = DisplayPowerMgr::DisplayState(stoi(root["STATE"].asString()));
            switch (displayState) {
                case DisplayPowerMgr::DisplayState::DISPLAY_OFF:
                    data.state = StatsUtils::STATS_STATE_DEACTIVATED;
                    break;
                case DisplayPowerMgr::DisplayState::DISPLAY_ON:
                    data.state = StatsUtils::STATS_STATE_ACTIVATED;
                    break;
                default:
                    break;
            }
        }
    } else if (eventName == "BRIGHTNESS_NIT") {
        data.type = StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS;
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
    if (!root["name_"].asString().empty()) {
        data.eventDebugInfo.append("Event name = ").append(root["name_"].asString());
    }
    if (!root["NAME"].asString().empty()) {
        data.eventDebugInfo.append(" Name = ").append(root["NAME"].asString());
    }
    if (!root["TEMPERATURE"].asString().empty()) {
        data.eventDebugInfo.append(" Temperature = ").append(root["TEMPERATURE"].asString());
    }
    if (!root["LEVEL"].asString().empty()) {
        data.eventDebugInfo.append(" Temperature level = ").append(root["LEVEL"].asString());
    }
    if (!root["ACTION"].asString().empty()) {
        data.eventDebugInfo.append(" Action name = ").append(root["ACTION"].asString());
    }
    if (!root["VALUE"].asString().empty()) {
        data.eventDebugInfo.append(" Value = ").append(root["VALUE"].asString());
    }
    if (!root["RATIO"].asString().empty()) {
        std::string ratio = std::to_string(root["RATIO"].asFloat()).substr(THERMAL_RATIO_BEGIN, THERMAL_RATIO_LENGTH);
        data.eventDebugInfo.append(" Ratio = ").append(ratio);
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
    if (!root["CALLER_UID"].asString().empty()) {
        data.uid = stoi(root["CALLER_UID"].asString());
    }
    if (!root["CALLER_PID"].asString().empty()) {
        data.pid = stoi(root["CALLER_PID"].asString());
    }
}

void BatteryStatsListener::OnServiceDied()
{
    STATS_HILOGE(COMP_SVC, "Service disconnected");
}
} // namespace PowerMgr
} // namespace OHOS
