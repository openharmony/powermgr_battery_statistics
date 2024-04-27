/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifdef HAS_BATTERYSTATS_BLUETOOTH_PART
#include "bluetooth_def.h"
#endif

#ifdef HAS_BATTERYSTATS_CALL_MANAGER_PART
#include "call_manager_inner_type.h"
#endif

#ifdef HAS_BATTERYSTATS_DISPLAY_MANAGER_PART
#include "display_power_info.h"
#endif

#ifdef HAS_BATTERYSTATS_WIFI_PART
#include "wifi_msg.h"
#endif

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
    auto statsService = BatteryStatsService::GetInstance();
    auto detector = statsService->GetBatteryStatsDetector();
    StatsUtils::StatsData data;
    data.eventDebugInfo.clear();
    if (eventName == StatsHiSysEvent::POWER_RUNNINGLOCK) {
        ProcessWakelockEvent(data, root);
    } else if (eventName == StatsHiSysEvent::SCREEN_STATE || eventName == StatsHiSysEvent::BRIGHTNESS_NIT ||
        eventName == StatsHiSysEvent::BACKLIGHT_DISCOUNT || eventName == StatsHiSysEvent::AMBIENT_LIGHT) {
        ProcessDispalyEvent(data, root, eventName);
    } else if (eventName == StatsHiSysEvent::BATTERY_CHANGED) {
        ProcessBatteryEvent(data, root);
    } else if (eventName == StatsHiSysEvent::POWER_TEMPERATURE ||
        eventName == StatsHiSysEvent::THERMAL_LEVEL_CHANGED ||
        eventName == StatsHiSysEvent::THERMAL_ACTION_TRIGGERED) {
        ProcessThermalEvent(data, root);
    } else if (eventName == StatsHiSysEvent::POWER_WORKSCHEDULER || eventName == StatsHiSysEvent::WORK_ADD ||
        eventName == StatsHiSysEvent::WORK_REMOVE || eventName == StatsHiSysEvent::WORK_START ||
        eventName == StatsHiSysEvent::WORK_STOP) {
        ProcessWorkschedulerEvent(data, root);
    } else if (eventName == StatsHiSysEvent::CALL_STATE || eventName == StatsHiSysEvent::DATA_CONNECTION_STATE) {
        ProcessPhoneEvent(data, root, eventName);
    } else if (eventName == StatsHiSysEvent::TORCH_STATE) {
        ProcessFlashlightEvent(data, root);
    } else if (eventName == StatsHiSysEvent::CAMERA_CONNECT || eventName == StatsHiSysEvent::CAMERA_DISCONNECT ||
        eventName == StatsHiSysEvent::FLASHLIGHT_ON || eventName == StatsHiSysEvent::FLASHLIGHT_OFF) {
        ProcessCameraEvent(data, root, eventName);
    } else if (eventName == StatsHiSysEvent::STREAM_CHANGE) {
        ProcessAudioEvent(data, root);
    } else if (eventName == StatsHiSysEvent::POWER_SENSOR_GRAVITY ||
        eventName == StatsHiSysEvent::POWER_SENSOR_PROXIMITY) {
        ProcessSensorEvent(data, root, eventName);
    } else if (eventName == StatsHiSysEvent::GNSS_STATE) {
        ProcessGnssEvent(data, root);
    } else if (eventName == StatsHiSysEvent::BR_SWITCH_STATE || eventName == StatsHiSysEvent::DISCOVERY_STATE ||
        eventName == StatsHiSysEvent::BLE_SWITCH_STATE || eventName == StatsHiSysEvent::BLE_SCAN_START ||
        eventName == StatsHiSysEvent::BLE_SCAN_STOP) {
        ProcessBluetoothEvent(data, root, eventName);
    } else if (eventName == StatsHiSysEvent::WIFI_CONNECTION || eventName == StatsHiSysEvent::WIFI_SCAN) {
        ProcessWifiEvent(data, root, eventName);
    } else if (eventName == StatsHiSysEvent::START_REMOTE_ABILITY) {
        ProcessDistributedSchedulerEvent(data, root);
    } else if (eventName == StatsHiSysEvent::MISC_TIME_STATISTIC_REPORT) {
        ProcessAlarmEvent(data, root);
    }
    detector->HandleStatsChangedEvent(data);
}

void BatteryStatsListener::ProcessCameraEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    if (eventName == StatsHiSysEvent::CAMERA_CONNECT || eventName == StatsHiSysEvent::CAMERA_DISCONNECT) {
        data.type = StatsUtils::STATS_TYPE_CAMERA_ON;
        if (root["UID"].isInt()) {
            data.uid = root["UID"].asInt();
        }
        if (root["PID"].isInt()) {
            data.pid = root["PID"].asInt();
        }
        if (root["ID"].isString() && !root["ID"].asString().empty()) {
            data.deviceId = root["ID"].asString();
        }
        if (eventName == StatsHiSysEvent::CAMERA_CONNECT) {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    } else if (eventName == StatsHiSysEvent::FLASHLIGHT_ON || eventName == StatsHiSysEvent::FLASHLIGHT_OFF) {
        data.type = StatsUtils::STATS_TYPE_CAMERA_FLASHLIGHT_ON;
        if (eventName == StatsHiSysEvent::FLASHLIGHT_ON) {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
}

void BatteryStatsListener::ProcessAudioEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_AUDIO_ON;
    if (root["UID"].isInt()) {
        data.uid = root["UID"].asInt();
    }
    if (root["PID"].isInt()) {
        data.pid = root["PID"].asInt();
    }
    if (root["STATE"].isInt()) {
        AudioState audioState = AudioState(root["STATE"].asInt());
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
    if (eventName == StatsHiSysEvent::POWER_SENSOR_GRAVITY) {
        data.type = StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON;
    } else if (eventName == StatsHiSysEvent::POWER_SENSOR_PROXIMITY) {
        data.type = StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON;
    }

    if (root["UID"].isInt()) {
        data.uid = root["UID"].asInt();
    }
    if (root["PID"].isInt()) {
        data.pid = root["PID"].asInt();
    }
    if (root["STATE"].isInt()) {
        if (root["STATE"].asInt() == 1) {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (root["STATE"].asInt() == 0) {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
}

void BatteryStatsListener::ProcessGnssEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_GNSS_ON;
    if (root["UID"].isInt()) {
        data.uid = root["UID"].asInt();
    }
    if (root["PID"].isInt()) {
        data.pid = root["PID"].asInt();
    }
    if (root["STATE"].isString() && !root["STATE"].asString().empty()) {
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
    if (eventName == StatsHiSysEvent::BR_SWITCH_STATE) {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_BR_ON;
        if (root["STATE"].isInt()) {
#ifdef HAS_BATTERYSTATS_BLUETOOTH_PART
            if (root["STATE"].asInt() == Bluetooth::BTStateID::STATE_TURN_ON) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (root["STATE"].asInt() == Bluetooth::BTStateID::STATE_TURN_OFF) {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
            }
#endif
        }
    } else if (eventName == StatsHiSysEvent::DISCOVERY_STATE) {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_BR_SCAN;
        if (root["STATE"].isInt()) {
#ifdef HAS_BATTERYSTATS_BLUETOOTH_PART
            if (root["STATE"].asInt() == Bluetooth::DISCOVERY_STARTED) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (root["STATE"].asInt() == Bluetooth::DISCOVERY_STOPED) {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
            }
#endif
        }
        if (root["UID"].isInt()) {
            data.uid = root["UID"].asInt();
        }
        if (root["PID"].isInt()) {
            data.pid = root["PID"].asInt();
        }
    }
}

void BatteryStatsListener::ProcessBluetoothBleEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    if (eventName == StatsHiSysEvent::BLE_SWITCH_STATE) {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_BLE_ON;
        if (root["STATE"].isInt()) {
#ifdef HAS_BATTERYSTATS_BLUETOOTH_PART
            if (root["STATE"].asInt() == Bluetooth::BTStateID::STATE_TURN_ON) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (root["STATE"].asInt() == Bluetooth::BTStateID::STATE_TURN_OFF) {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
            }
#endif
        }
    } else if (eventName == StatsHiSysEvent::BLE_SCAN_START || eventName == StatsHiSysEvent::BLE_SCAN_STOP) {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_BLE_SCAN;
        if (eventName == StatsHiSysEvent::BLE_SCAN_START) {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (eventName == StatsHiSysEvent::BLE_SCAN_STOP) {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
        if (root["UID"].isInt()) {
            data.uid = root["UID"].asInt();
        }
        if (root["PID"].isInt()) {
            data.pid = root["PID"].asInt();
        }
    }
}

void BatteryStatsListener::ProcessBluetoothEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    if (eventName == StatsHiSysEvent::BR_SWITCH_STATE || eventName == StatsHiSysEvent::DISCOVERY_STATE) {
        ProcessBluetoothBrEvent(data, root, eventName);
    } else if (eventName == StatsHiSysEvent::BLE_SWITCH_STATE ||eventName == StatsHiSysEvent::BLE_SCAN_START ||
        eventName == StatsHiSysEvent::BLE_SCAN_STOP) {
        ProcessBluetoothBleEvent(data, root, eventName);
    }
}

void BatteryStatsListener::ProcessWifiEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    if (eventName == StatsHiSysEvent::WIFI_CONNECTION) {
        data.type = StatsUtils::STATS_TYPE_WIFI_ON;
        if (root["TYPE"].isInt()) {
#ifdef HAS_BATTERYSTATS_WIFI_PART
            Wifi::ConnState connectionType = Wifi::ConnState(root["TYPE"].asInt());
            switch (connectionType) {
                case Wifi::ConnState::CONNECTED:
                    data.state = StatsUtils::STATS_STATE_ACTIVATED;
                    break;
                case Wifi::ConnState::DISCONNECTED:
                    data.state = StatsUtils::STATS_STATE_DEACTIVATED;
                    break;
                default:
                    break;
            }
#endif
        }
    } else if (eventName == StatsHiSysEvent::WIFI_SCAN) {
        data.type = StatsUtils::STATS_TYPE_WIFI_SCAN;
        data.traffic = 1;
    }
}

void BatteryStatsListener::ProcessPhoneDebugInfo(StatsUtils::StatsData& data, const Json::Value& root)
{
    if (root["name_"].isString() && !root["name_"].asString().empty()) {
        data.eventDebugInfo.append("Event name = ").append(root["name_"].asString());
    }
    if (root["STATE"].isInt()) {
        data.eventDebugInfo.append(" State = ").append(std::to_string(root["STATE"].asInt()));
    }
    if (root["SLOT_ID"].isInt()) {
        data.eventDebugInfo.append(" Slot ID = ").append(std::to_string(root["SLOT_ID"].asInt()));
    }
    if (root["INDEX_ID"].isInt()) {
        data.eventDebugInfo.append(" Index ID = ").append(std::to_string(root["INDEX_ID"].asInt()));
    }
}

void BatteryStatsListener::ProcessPhoneEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    if (eventName == StatsHiSysEvent::CALL_STATE) {
        data.type = StatsUtils::STATS_TYPE_PHONE_ACTIVE;
        if (root["STATE"].isInt()) {
#ifdef HAS_BATTERYSTATS_CALL_MANAGER_PART
            Telephony::TelCallState callState = Telephony::TelCallState(root["STATE"].asInt());
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
#endif
        }
    } else if (eventName == StatsHiSysEvent::DATA_CONNECTION_STATE) {
        data.type = StatsUtils::STATS_TYPE_PHONE_DATA;
        if (root["STATE"].isInt()) {
            if (root["STATE"].asInt() == 1) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (root["STATE"].asInt() == 0) {
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
    if (root["UID"].isInt()) {
        data.uid = root["UID"].asInt();
    }
    if (root["PID"].isInt()) {
        data.pid = root["PID"].asInt();
    }
    if (root["STATE"].isInt()) {
        if (root["STATE"].asInt() == 1) {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (root["STATE"].asInt() == 0) {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
}

void BatteryStatsListener::ProcessWakelockEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_WAKELOCK_HOLD;
    if (root["UID"].isInt()) {
        data.uid = root["UID"].asInt();
    }
    if (root["PID"].isInt()) {
        data.pid = root["PID"].asInt();
    }
    if (root["STATE"].isInt()) {
        RunningLockState lockState = RunningLockState(root["STATE"].asInt());
        std::string stateLabel = "";
        switch (lockState) {
            case RunningLockState::RUNNINGLOCK_STATE_DISABLE: {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
                stateLabel = "Disable";
                break;
            }
            case RunningLockState::RUNNINGLOCK_STATE_ENABLE: {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
                stateLabel = "Enable";
                break;
            }
            case RunningLockState::RUNNINGLOCK_STATE_PROXIED:
            case RunningLockState::RUNNINGLOCK_STATE_UNPROXIED_RESTORE: {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
                stateLabel = "Proxied";
                break;
            }
            default:
                break;
        }
        data.eventDebugInfo.append(" STATE = ").append(stateLabel);
    }
    if (root["TYPE"].isInt()) {
        data.eventDataType = root["TYPE"].asInt();
    }
    if (root["NAME"].isString() && !root["NAME"].asString().empty()) {
        data.eventDataName = root["NAME"].asString();
    }
    if (root["LOG_LEVEL"].isInt()) {
        data.eventDebugInfo.append(" LOG_LEVEL = ").append(std::to_string(root["LOG_LEVEL"].asInt()));
    }
    if (root["TAG"].isString() && !root["TAG"].asString().empty()) {
        data.eventDebugInfo.append(" TAG = ").append(root["TAG"].asString());
    }
    if (root["MESSAGE"].isString() && !root["MESSAGE"].asString().empty()) {
        data.eventDebugInfo.append(" MESSAGE = ").append(root["MESSAGE"].asString());
    }
}

void BatteryStatsListener::ProcessDispalyDebugInfo(StatsUtils::StatsData& data, const Json::Value& root)
{
    if (root["name_"].isString() && !root["name_"].asString().empty()) {
        data.eventDebugInfo.append("Event name = ").append(root["name_"].asString());
    }
    if (root["STATE"].isInt()) {
        data.eventDebugInfo.append(" Screen state = ").append(std::to_string(root["STATE"].asInt()));
    }
    if (root["BRIGHTNESS"].isInt()) {
        data.eventDebugInfo.append(" Screen brightness = ").append(std::to_string(root["BRIGHTNESS"].asInt()));
    }
    if (root["REASON"].isString() && !root["REASON"].asString().empty()) {
        data.eventDebugInfo.append(" Brightness reason = ").append(root["REASON"].asString());
    }
    if (root["NIT"].isInt()) {
        data.eventDebugInfo.append(" Brightness nit = ").append(std::to_string(root["NIT"].asInt()));
    }
    if (root["RATIO"].isInt()) {
        data.eventDebugInfo.append(" Ratio = ").append(std::to_string(root["RATIO"].asInt()));
    }
    if (root["TYPE"].isInt()) {
        data.eventDebugInfo.append(" Ambient type = ").append(std::to_string(root["TYPE"].asInt()));
    }
    if (root["LEVEL"].isInt()) {
        data.eventDebugInfo.append(" Ambient brightness = ").append(std::to_string(root["LEVEL"].asInt()));
    }
}

void BatteryStatsListener::ProcessDispalyEvent(StatsUtils::StatsData& data, const Json::Value& root,
    const std::string& eventName)
{
    data.type = StatsUtils::STATS_TYPE_DISPLAY;
    if (eventName == StatsHiSysEvent::SCREEN_STATE) {
        data.type = StatsUtils::STATS_TYPE_SCREEN_ON;
#ifdef HAS_BATTERYSTATS_DISPLAY_MANAGER_PART
        if (root["STATE"].isInt()) {
            DisplayPowerMgr::DisplayState displayState = DisplayPowerMgr::DisplayState(root["STATE"].asInt());
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
#endif
    } else if (eventName == StatsHiSysEvent::BRIGHTNESS_NIT) {
        data.type = StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS;
        if (root["BRIGHTNESS"].isInt()) {
            data.level = root["BRIGHTNESS"].asInt();
        }
    }
    ProcessDispalyDebugInfo(data, root);
}

void BatteryStatsListener::ProcessBatteryEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_BATTERY;
    if (root["LEVEL"].isInt()) {
        data.level = root["LEVEL"].asInt();
    }
    if (root["CHARGER"].isInt()) {
        data.eventDataExtra = root["CHARGER"].asInt();
    }
    if (root["VOLTAGE"].isInt()) {
        data.eventDebugInfo.append(" Voltage = ").append(std::to_string(root["VOLTAGE"].asInt()));
    }
    if (root["HEALTH"].isInt()) {
        data.eventDebugInfo.append(" Health = ").append(std::to_string(root["HEALTH"].asInt()));
    }
    if (root["TEMPERATURE"].isInt()) {
        data.eventDebugInfo.append(" Temperature = ").append(std::to_string(root["TEMPERATURE"].asInt()));
    }
}

void BatteryStatsListener::ProcessThermalEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_THERMAL;
    if (root["name_"].isString() && !root["name_"].asString().empty()) {
        data.eventDebugInfo.append("Event name = ").append(root["name_"].asString());
    }
    if (root["NAME"].isString() && !root["NAME"].asString().empty()) {
        data.eventDebugInfo.append(" Name = ").append(root["NAME"].asString());
    }
    if (root["TEMPERATURE"].isInt()) {
        data.eventDebugInfo.append(" Temperature = ").append(std::to_string(root["TEMPERATURE"].asInt()));
    }
    if (root["LEVEL"].isInt()) {
        data.eventDebugInfo.append(" Temperature level = ").append(std::to_string(root["LEVEL"].asInt()));
    }
    if (root["ACTION"].isString() && !root["ACTION"].asString().empty()) {
        data.eventDebugInfo.append(" Action name = ").append(root["ACTION"].asString());
    }
    if (root["VALUE"].isInt()) {
        data.eventDebugInfo.append(" Value = ").append(std::to_string(root["VALUE"].asInt()));
    }
    if (root["RATIO"].isNumeric()) {
        std::string ratio = std::to_string(root["RATIO"].asFloat()).substr(THERMAL_RATIO_BEGIN, THERMAL_RATIO_LENGTH);
        data.eventDebugInfo.append(" Ratio = ").append(ratio);
    }
}

void BatteryStatsListener::ProcessPowerWorkschedulerEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
        data.type = StatsUtils::STATS_TYPE_WORKSCHEDULER;
        if (root["UID"].isInt()) {
            data.uid = root["UID"].asInt();
        }
        if (root["PID"].isInt()) {
            data.pid = root["PID"].asInt();
        }
        if (root["STATE"].isInt()) {
            data.state = StatsUtils::StatsState(root["STATE"].asInt());
        }
        if (root["TYPE"].isInt()) {
            data.eventDataType = root["TYPE"].asInt();
        }
        if (root["INTERVAL"].isInt()) {
            data.eventDataExtra = root["INTERVAL"].asInt();
        }
}

void BatteryStatsListener::ProcessOthersWorkschedulerEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
        data.type = StatsUtils::STATS_TYPE_WORKSCHEDULER;
        if (root["name_"].isString() && !root["name_"].asString().empty()) {
            data.eventDebugInfo.append(root["name_"].asString()).append(":");
        }
        if (root["UID"].isInt()) {
            data.uid = root["UID"].asInt();
        }
        if (root["PID"].isInt()) {
            data.pid = root["PID"].asInt();
        }
        if (root["NAME"].isString() && !root["NAME"].asString().empty()) {
            data.eventDebugInfo.append(" Bundle name = ").append(root["NAME"].asString());
        }
        if (root["WORKID"].isString() && !root["WORKID"].asString().empty()) {
            data.eventDebugInfo.append(" Work ID = ").append(root["WORKID"].asString());
        }
        if (root["TRIGGER"].isString() && !root["TRIGGER"].asString().empty()) {
            data.eventDebugInfo.append(" Trigger conditions = ").append(root["TRIGGER"].asString());
        }
        if (root["TYPE"].isString() && !root["TYPE"].asString().empty()) {
            data.eventDebugInfo.append(" Work type = ").append(root["TYPE"].asString());
        }
        if (root["INTERVAL"].isInt()) {
            data.eventDebugInfo.append(" Interval = ").append(std::to_string(root["INTERVAL"].asInt()));
        }
}

void BatteryStatsListener::ProcessWorkschedulerEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    if (!root["name_"].isString() || root["name_"].asString().empty()) {
        return;
    }
    if (root["name_"].asString() == StatsHiSysEvent::POWER_WORKSCHEDULER) {
        ProcessPowerWorkschedulerEvent(data, root);
    } else {
        ProcessOthersWorkschedulerEvent(data, root);
    }
}

void BatteryStatsListener::ProcessDistributedSchedulerEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_DISTRIBUTEDSCHEDULER;
    if (root["name_"].isString() && !root["name_"].asString().empty()) {
        data.eventDebugInfo.append("Event name = ").append(root["name_"].asString());
    }
    if (root["CALLING_TYPE"].isString() && !root["CALLING_TYPE"].asString().empty()) {
        data.eventDebugInfo.append(" Calling Type = ").append(root["CALLING_TYPE"].asString());
    }
    if (root["CALLING_UID"].isInt()) {
        data.eventDebugInfo.append(" Calling Uid = ").append(std::to_string(root["CALLING_UID"].asInt()));
    }
    if (root["CALLING_PID"].isInt()) {
        data.eventDebugInfo.append(" Calling Pid = ").append(std::to_string(root["CALLING_PID"].asInt()));
    }
    if (root["TARGET_BUNDLE"].isString() && !root["TARGET_BUNDLE"].asString().empty()) {
        data.eventDebugInfo.append(" Target Bundle Name = ").append(root["TARGET_BUNDLE"].asString());
    }
    if (root["TARGET_ABILITY"].isString() && !root["TARGET_ABILITY"].asString().empty()) {
        data.eventDebugInfo.append(" Target Ability Name = ").append(root["TARGET_ABILITY"].asString());
    }
    if (root["CALLING_APP_UID"].isInt()) {
        data.eventDebugInfo.append(" Calling App Uid = ").append(std::to_string(root["CALLING_APP_UID"].asInt()));
    }
    if (root["RESULT"].isInt()) {
        data.eventDebugInfo.append(" RESULT = ").append(std::to_string(root["RESULT"].asInt()));
    }
}

void BatteryStatsListener::ProcessAlarmEvent(StatsUtils::StatsData& data, const Json::Value& root)
{
    data.type = StatsUtils::STATS_TYPE_ALARM;
    data.traffic = 1;
    if (root["CALLER_UID"].isInt()) {
        data.uid = root["CALLER_UID"].asInt();
    }
    if (root["CALLER_PID"].isInt()) {
        data.pid = root["CALLER_PID"].asInt();
    }
}

void BatteryStatsListener::OnServiceDied()
{
    STATS_HILOGE(COMP_SVC, "Service disconnected");
}
} // namespace PowerMgr
} // namespace OHOS
