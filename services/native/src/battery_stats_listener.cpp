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

    cJSON* root = cJSON_Parse(eventDetail.c_str());
    if (root != nullptr) {
        if (!cJSON_IsObject(root)) {
            STATS_HILOGD(COMP_SVC, "json root is not an object");
            cJSON_Delete(root);
            return;
        }
        ProcessHiSysEvent(eventName, root);
        cJSON_Delete(root);
    } else {
        STATS_HILOGW(COMP_SVC, "Parse hisysevent data failed");
    }
}

void BatteryStatsListener::ProcessHiSysEvent(const std::string& eventName, const cJSON* root)
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
    } else {
        ProcessHiSysEventInternal(data, eventName, root);
    }
    detector->HandleStatsChangedEvent(data);
}

void BatteryStatsListener::ProcessHiSysEventInternal(StatsUtils::StatsData& data,
    const std::string& eventName, const cJSON* root)
{
    if (eventName == StatsHiSysEvent::TORCH_STATE) {
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
}

void BatteryStatsListener::ProcessCameraEvent(StatsUtils::StatsData& data, const cJSON* root,
    const std::string& eventName)
{
    if (eventName == StatsHiSysEvent::CAMERA_CONNECT || eventName == StatsHiSysEvent::CAMERA_DISCONNECT) {
        data.type = StatsUtils::STATS_TYPE_CAMERA_ON;
        cJSON* uidItem = cJSON_GetObjectItemCaseSensitive(root, "UID");
        if (uidItem && cJSON_IsNumber(uidItem)) {
            data.uid = static_cast<int32_t>(uidItem->valueint);
        }

        cJSON* pidItem = cJSON_GetObjectItemCaseSensitive(root, "PID");
        if (pidItem && cJSON_IsNumber(pidItem)) {
            data.pid = static_cast<int32_t>(pidItem->valueint);
        }

        cJSON* idItem = cJSON_GetObjectItemCaseSensitive(root, "ID");
        if (idItem && cJSON_IsString(idItem) && idItem->valuestring != nullptr &&
            strlen(idItem->valuestring) > 0) {
            data.deviceId = idItem->valuestring;
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

void BatteryStatsListener::ProcessAudioEvent(StatsUtils::StatsData& data, const cJSON* root)
{
    data.type = StatsUtils::STATS_TYPE_AUDIO_ON;
    cJSON* uidItem = cJSON_GetObjectItemCaseSensitive(root, "UID");
    if (uidItem && cJSON_IsNumber(uidItem)) {
        data.uid = static_cast<int32_t>(uidItem->valueint);
    }

    cJSON* pidItem = cJSON_GetObjectItemCaseSensitive(root, "PID");
    if (pidItem && cJSON_IsNumber(pidItem)) {
        data.pid = static_cast<int32_t>(pidItem->valueint);
    }

    cJSON* stateItem = cJSON_GetObjectItemCaseSensitive(root, "STATE");
    if (stateItem && cJSON_IsNumber(stateItem)) {
        AudioState state = static_cast<AudioState>(stateItem->valueint);
        switch (state) {
            case AudioState::AUDIO_STATE_RUNNING:
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
                break;
            case AudioState::AUDIO_STATE_STOPPED:
            case AudioState::AUDIO_STATE_RELEASED:
            case AudioState::AUDIO_STATE_PAUSED:
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
                break;
            default:
                data.state = StatsUtils::STATS_STATE_INVALID;
                break;
        }
    }
}

void BatteryStatsListener::ProcessSensorEvent(StatsUtils::StatsData& data, const cJSON* root,
    const std::string& eventName)
{
    if (eventName == StatsHiSysEvent::POWER_SENSOR_GRAVITY) {
        data.type = StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON;
    } else if (eventName == StatsHiSysEvent::POWER_SENSOR_PROXIMITY) {
        data.type = StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON;
    }

    cJSON* uidItem = cJSON_GetObjectItemCaseSensitive(root, "UID");
    if (uidItem && cJSON_IsNumber(uidItem)) {
        data.uid = static_cast<int32_t>(uidItem->valueint);
    }

    cJSON* pidItem = cJSON_GetObjectItemCaseSensitive(root, "PID");
    if (pidItem && cJSON_IsNumber(pidItem)) {
        data.pid = static_cast<int32_t>(pidItem->valueint);
    }

    cJSON* stateItem = cJSON_GetObjectItemCaseSensitive(root, "STATE");
    if (stateItem && cJSON_IsNumber(stateItem)) {
        if (stateItem->valueint == 1) {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (stateItem->valueint == 0) {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
}

void BatteryStatsListener::ProcessGnssEvent(StatsUtils::StatsData& data, const cJSON* root)
{
    data.type = StatsUtils::STATS_TYPE_GNSS_ON;
    cJSON* uidItem = cJSON_GetObjectItemCaseSensitive(root, "UID");
    if (uidItem && cJSON_IsNumber(uidItem)) {
        data.uid = static_cast<int32_t>(uidItem->valueint);
    }

    cJSON* pidItem = cJSON_GetObjectItemCaseSensitive(root, "PID");
    if (pidItem && cJSON_IsNumber(pidItem)) {
        data.pid = static_cast<int32_t>(pidItem->valueint);
    }

    cJSON* stateItem = cJSON_GetObjectItemCaseSensitive(root, "STATE");
    if (stateItem && cJSON_IsString(stateItem) && stateItem->valuestring != nullptr &&
        strlen(stateItem->valuestring) > 0) {
        const char* stateStr = stateItem->valuestring;
        if (strcmp(stateStr, "start") == 0) {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (strcmp(stateStr, "stop") == 0) {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
}

void BatteryStatsListener::ProcessBluetoothBrEvent(StatsUtils::StatsData& data, const cJSON* root,
    const std::string& eventName)
{
    cJSON* stateItem = cJSON_GetObjectItemCaseSensitive(root, "STATE");
    if (eventName == StatsHiSysEvent::BR_SWITCH_STATE) {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_BR_ON;
        if (stateItem && cJSON_IsNumber(stateItem)) {
#ifdef HAS_BATTERYSTATS_BLUETOOTH_PART
            if (stateItem->valueint == Bluetooth::BTStateID::STATE_TURN_ON) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (stateItem->valueint == Bluetooth::BTStateID::STATE_TURN_OFF) {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
            }
#endif
        }
    } else if (eventName == StatsHiSysEvent::DISCOVERY_STATE) {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_BR_SCAN;
        if (stateItem && cJSON_IsNumber(stateItem)) {
#ifdef HAS_BATTERYSTATS_BLUETOOTH_PART
            if (stateItem->valueint == Bluetooth::DISCOVERY_STARTED) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (stateItem->valueint == Bluetooth::DISCOVERY_STOPED) {
                data.state = StatsUtils::STATS_STATE_DEACTIVATED;
            }
#endif
        }
        cJSON* uidItem = cJSON_GetObjectItemCaseSensitive(root, "UID");
        if (uidItem && cJSON_IsNumber(uidItem)) {
            data.uid = static_cast<int32_t>(uidItem->valueint);
        }

        cJSON* pidItem = cJSON_GetObjectItemCaseSensitive(root, "PID");
        if (pidItem && cJSON_IsNumber(pidItem)) {
            data.pid = static_cast<int32_t>(pidItem->valueint);
        }
    }
}

void BatteryStatsListener::ProcessBluetoothBleEvent(StatsUtils::StatsData& data, const cJSON* root,
    const std::string& eventName)
{
    if (eventName == StatsHiSysEvent::BLE_SWITCH_STATE) {
        data.type = StatsUtils::STATS_TYPE_BLUETOOTH_BLE_ON;
        cJSON* stateItem = cJSON_GetObjectItemCaseSensitive(root, "STATE");
        if (stateItem && cJSON_IsNumber(stateItem)) {
#ifdef HAS_BATTERYSTATS_BLUETOOTH_PART
            if (stateItem->valueint == Bluetooth::BTStateID::STATE_TURN_ON) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (stateItem->valueint == Bluetooth::BTStateID::STATE_TURN_OFF) {
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
        cJSON* uidItem = cJSON_GetObjectItemCaseSensitive(root, "UID");
        if (uidItem && cJSON_IsNumber(uidItem)) {
            data.uid = static_cast<int32_t>(uidItem->valueint);
        }

        cJSON* pidItem = cJSON_GetObjectItemCaseSensitive(root, "PID");
        if (pidItem && cJSON_IsNumber(pidItem)) {
            data.pid = static_cast<int32_t>(pidItem->valueint);
        }
    }
}

void BatteryStatsListener::ProcessBluetoothEvent(StatsUtils::StatsData& data, const cJSON* root,
    const std::string& eventName)
{
    if (eventName == StatsHiSysEvent::BR_SWITCH_STATE || eventName == StatsHiSysEvent::DISCOVERY_STATE) {
        ProcessBluetoothBrEvent(data, root, eventName);
    } else if (eventName == StatsHiSysEvent::BLE_SWITCH_STATE ||eventName == StatsHiSysEvent::BLE_SCAN_START ||
        eventName == StatsHiSysEvent::BLE_SCAN_STOP) {
        ProcessBluetoothBleEvent(data, root, eventName);
    }
}

void BatteryStatsListener::ProcessWifiEvent(StatsUtils::StatsData& data, const cJSON* root,
    const std::string& eventName)
{
    if (eventName == StatsHiSysEvent::WIFI_CONNECTION) {
        data.type = StatsUtils::STATS_TYPE_WIFI_ON;
        cJSON* typeItem = cJSON_GetObjectItemCaseSensitive(root, "TYPE");
        if (typeItem && cJSON_IsNumber(typeItem)) {
#ifdef HAS_BATTERYSTATS_WIFI_PART
            int connectionTypeInt = typeItem->valueint;
            switch (static_cast<Wifi::ConnState>(connectionTypeInt)) {
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

void BatteryStatsListener::ProcessPhoneDebugInfo(StatsUtils::StatsData& data, const cJSON* root)
{
    cJSON* item = nullptr;

    item = cJSON_GetObjectItemCaseSensitive(root, "name_");
    if (item && cJSON_IsString(item) && item->valuestring != nullptr && strlen(item->valuestring) > 0) {
        data.eventDebugInfo.append("Event name = ").append(item->valuestring);
    }

    item = cJSON_GetObjectItemCaseSensitive(root, "STATE");
    if (item && cJSON_IsNumber(item)) {
        data.eventDebugInfo.append(" State = ").append(std::to_string(item->valueint));
    }

    item = cJSON_GetObjectItemCaseSensitive(root, "SLOT_ID");
    if (item && cJSON_IsNumber(item)) {
        data.eventDebugInfo.append(" Slot ID = ").append(std::to_string(item->valueint));
    }

    item = cJSON_GetObjectItemCaseSensitive(root, "INDEX_ID");
    if (item && cJSON_IsNumber(item)) {
        data.eventDebugInfo.append(" Index ID = ").append(std::to_string(item->valueint));
    }
}

void BatteryStatsListener::ProcessPhoneEvent(StatsUtils::StatsData& data, const cJSON* root,
    const std::string& eventName)
{
    cJSON* stateItem = cJSON_GetObjectItemCaseSensitive(root, "STATE");
    if (eventName == StatsHiSysEvent::CALL_STATE) {
        data.type = StatsUtils::STATS_TYPE_PHONE_ACTIVE;
        if (stateItem && cJSON_IsNumber(stateItem)) {
#ifdef HAS_BATTERYSTATS_CALL_MANAGER_PART
            int callStateInt = stateItem->valueint;
            switch (static_cast<Telephony::TelCallState>(callStateInt)) {
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
        if (stateItem && cJSON_IsNumber(stateItem)) {
            if (stateItem->valueint == 1) {
                data.state = StatsUtils::STATS_STATE_ACTIVATED;
            } else if (stateItem->valueint == 0) {
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

void BatteryStatsListener::ProcessFlashlightEvent(StatsUtils::StatsData& data, const cJSON* root)
{
    data.type = StatsUtils::STATS_TYPE_FLASHLIGHT_ON;
    cJSON* uidItem = cJSON_GetObjectItemCaseSensitive(root, "UID");
    if (uidItem && cJSON_IsNumber(uidItem)) {
        data.uid = static_cast<int32_t>(uidItem->valueint);
    }

    cJSON* pidItem = cJSON_GetObjectItemCaseSensitive(root, "PID");
    if (pidItem && cJSON_IsNumber(pidItem)) {
        data.pid = static_cast<int32_t>(pidItem->valueint);
    }

    cJSON* stateItem = cJSON_GetObjectItemCaseSensitive(root, "STATE");
    if (stateItem && cJSON_IsNumber(stateItem)) {
        if (stateItem->valueint == 1) {
            data.state = StatsUtils::STATS_STATE_ACTIVATED;
        } else if (stateItem->valueint == 0) {
            data.state = StatsUtils::STATS_STATE_DEACTIVATED;
        }
    }
}

void BatteryStatsListener::ProcessWakelockEvent(StatsUtils::StatsData& data, const cJSON* root)
{
    data.type = StatsUtils::STATS_TYPE_WAKELOCK_HOLD;
    cJSON* uidItem = cJSON_GetObjectItemCaseSensitive(root, "UID");
    if (uidItem && cJSON_IsNumber(uidItem)) {
        data.uid = static_cast<int32_t>(uidItem->valueint);
    }

    cJSON* pidItem = cJSON_GetObjectItemCaseSensitive(root, "PID");
    if (pidItem && cJSON_IsNumber(pidItem)) {
        data.pid = static_cast<int32_t>(pidItem->valueint);
    }
    cJSON* stateItem = cJSON_GetObjectItemCaseSensitive(root, "STATE");
    if (stateItem && cJSON_IsNumber(stateItem)) {
        int lockStateInt = stateItem->valueint;
        std::string stateLabel = "";
        switch (static_cast<RunningLockState>(lockStateInt)) {
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

    ProcessWakelockEventInternal(data, root);
}

void BatteryStatsListener::ProcessWakelockEventInternal(StatsUtils::StatsData& data, const cJSON* root)
{
    cJSON* typeItem = cJSON_GetObjectItemCaseSensitive(root, "TYPE");
    if (typeItem && cJSON_IsNumber(typeItem)) {
        data.eventDataType = static_cast<int32_t>(typeItem->valueint);
    }

    cJSON* nameItem = cJSON_GetObjectItemCaseSensitive(root, "NAME");
    if (nameItem && cJSON_IsString(nameItem) && nameItem->valuestring != nullptr &&
        strlen(nameItem->valuestring) > 0) {
        data.eventDataName = nameItem->valuestring;
    }

    cJSON* logLevelItem = cJSON_GetObjectItemCaseSensitive(root, "LOG_LEVEL");
    if (logLevelItem && cJSON_IsNumber(logLevelItem)) {
        data.eventDebugInfo.append(" LOG_LEVEL = ").append(std::to_string(logLevelItem->valueint));
    }

    cJSON* tagItem = cJSON_GetObjectItemCaseSensitive(root, "TAG");
    if (tagItem && cJSON_IsString(tagItem) && tagItem->valuestring != nullptr &&
        strlen(tagItem->valuestring) > 0) {
        data.eventDebugInfo.append(" TAG = ").append(tagItem->valuestring);
    }

    cJSON* messageItem = cJSON_GetObjectItemCaseSensitive(root, "MESSAGE");
    if (messageItem && cJSON_IsString(messageItem) && messageItem->valuestring != nullptr &&
        strlen(messageItem->valuestring) > 0) {
        data.eventDebugInfo.append(" MESSAGE = ").append(messageItem->valuestring);
    }
}

void BatteryStatsListener::ProcessDispalyDebugInfo(StatsUtils::StatsData& data, const cJSON* root)
{
    cJSON* nameItem = cJSON_GetObjectItemCaseSensitive(root, "name_");
    if (nameItem && cJSON_IsString(nameItem) && nameItem->valuestring != nullptr &&
        strlen(nameItem->valuestring) > 0) {
        data.eventDebugInfo.append("Event name = ").append(nameItem->valuestring);
    }

    cJSON* stateItem = cJSON_GetObjectItemCaseSensitive(root, "STATE");
    if (stateItem && cJSON_IsNumber(stateItem)) {
        data.eventDebugInfo.append(" Screen state = ").append(std::to_string(stateItem->valueint));
    }

    cJSON* brightnessItem = cJSON_GetObjectItemCaseSensitive(root, "BRIGHTNESS");
    if (brightnessItem && cJSON_IsNumber(brightnessItem)) {
        data.eventDebugInfo.append(" Screen brightness = ").append(std::to_string(brightnessItem->valueint));
    }

    cJSON* reasonItem = cJSON_GetObjectItemCaseSensitive(root, "REASON");
    if (reasonItem && cJSON_IsString(reasonItem) && reasonItem->valuestring != nullptr &&
        strlen(reasonItem->valuestring) > 0) {
        data.eventDebugInfo.append(" Brightness reason = ").append(reasonItem->valuestring);
    }
    ProcessDispalyDebugInfoInternal(data, root);
}

void BatteryStatsListener::ProcessDispalyDebugInfoInternal(StatsUtils::StatsData& data, const cJSON* root)
{
    cJSON* nitItem = cJSON_GetObjectItemCaseSensitive(root, "NIT");
    if (nitItem && cJSON_IsNumber(nitItem)) {
        data.eventDebugInfo.append(" Brightness nit = ").append(std::to_string(nitItem->valueint));
    }

    cJSON* ratioItem = cJSON_GetObjectItemCaseSensitive(root, "RATIO");
    if (ratioItem && cJSON_IsNumber(ratioItem)) {
        data.eventDebugInfo.append(" Ratio = ").append(std::to_string(ratioItem->valueint));
    }

    cJSON* typeItem = cJSON_GetObjectItemCaseSensitive(root, "TYPE");
    if (typeItem && cJSON_IsNumber(typeItem)) {
        data.eventDebugInfo.append(" Ambient type = ").append(std::to_string(typeItem->valueint));
    }

    cJSON* levelItem = cJSON_GetObjectItemCaseSensitive(root, "LEVEL");
    if (levelItem && cJSON_IsNumber(levelItem)) {
        data.eventDebugInfo.append(" Ambient brightness = ").append(std::to_string(levelItem->valueint));
    }
}

void BatteryStatsListener::ProcessDispalyEvent(StatsUtils::StatsData& data, const cJSON* root,
    const std::string& eventName)
{
    data.type = StatsUtils::STATS_TYPE_DISPLAY;
    if (eventName == StatsHiSysEvent::SCREEN_STATE) {
        data.type = StatsUtils::STATS_TYPE_SCREEN_ON;
#ifdef HAS_BATTERYSTATS_DISPLAY_MANAGER_PART
        cJSON* stateItem = cJSON_GetObjectItemCaseSensitive(root, "STATE");
        if (stateItem && cJSON_IsNumber(stateItem)) {
            int displayStateInt = stateItem->valueint;
            switch (static_cast<DisplayPowerMgr::DisplayState>(displayStateInt)) {
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
        cJSON* brightnessItem = cJSON_GetObjectItemCaseSensitive(root, "BRIGHTNESS");
        if (brightnessItem && cJSON_IsNumber(brightnessItem)) {
            data.level = static_cast<int16_t>(brightnessItem->valueint);
        }
    }
    ProcessDispalyDebugInfo(data, root);
}

void BatteryStatsListener::ProcessBatteryEvent(StatsUtils::StatsData& data, const cJSON* root)
{
    data.type = StatsUtils::STATS_TYPE_BATTERY;

    cJSON* levelItem = cJSON_GetObjectItemCaseSensitive(root, "LEVEL");
    if (levelItem && cJSON_IsNumber(levelItem)) {
        data.level = static_cast<int16_t>(levelItem->valueint);
    }

    cJSON* chargerItem = cJSON_GetObjectItemCaseSensitive(root, "CHARGER");
    if (chargerItem && cJSON_IsNumber(chargerItem)) {
        data.eventDataExtra = static_cast<int32_t>(chargerItem->valueint);
    }

    cJSON* voltageItem = cJSON_GetObjectItemCaseSensitive(root, "VOLTAGE");
    if (voltageItem && cJSON_IsNumber(voltageItem)) {
        data.eventDebugInfo.append(" Voltage = ").append(std::to_string(voltageItem->valueint));
    }

    cJSON* healthItem = cJSON_GetObjectItemCaseSensitive(root, "HEALTH");
    if (healthItem && cJSON_IsNumber(healthItem)) {
        data.eventDebugInfo.append(" Health = ").append(std::to_string(healthItem->valueint));
    }

    cJSON* temperatureItem = cJSON_GetObjectItemCaseSensitive(root, "TEMPERATURE");
    if (temperatureItem && cJSON_IsNumber(temperatureItem)) {
        data.eventDebugInfo.append(" Temperature = ").append(std::to_string(temperatureItem->valueint));
    }
}

void BatteryStatsListener::ProcessThermalEvent(StatsUtils::StatsData& data, const cJSON* root)
{
    data.type = StatsUtils::STATS_TYPE_THERMAL;

    cJSON* nameItem = cJSON_GetObjectItemCaseSensitive(root, "name_");
    if (nameItem && cJSON_IsString(nameItem) && nameItem->valuestring != nullptr &&
        strlen(nameItem->valuestring) > 0) {
        data.eventDebugInfo.append("Event name = ").append(nameItem->valuestring);
    }

    cJSON* bundleNameItem = cJSON_GetObjectItemCaseSensitive(root, "NAME");
    if (bundleNameItem && cJSON_IsString(bundleNameItem) && bundleNameItem->valuestring != nullptr &&
        strlen(bundleNameItem->valuestring) > 0) {
        data.eventDebugInfo.append(" Name = ").append(bundleNameItem->valuestring);
    }

    cJSON* temperatureItem = cJSON_GetObjectItemCaseSensitive(root, "TEMPERATURE");
    if (temperatureItem && cJSON_IsNumber(temperatureItem)) {
        data.eventDebugInfo.append(" Temperature = ").append(std::to_string(temperatureItem->valueint));
    }

    cJSON* levelItem = cJSON_GetObjectItemCaseSensitive(root, "LEVEL");
    if (levelItem && cJSON_IsNumber(levelItem)) {
        data.eventDebugInfo.append(" Temperature level = ").append(std::to_string(levelItem->valueint));
    }

    ProcessThermalEventInternal(data, root);
}

void BatteryStatsListener::ProcessThermalEventInternal(StatsUtils::StatsData& data, const cJSON* root)
{
    cJSON* actionItem = cJSON_GetObjectItemCaseSensitive(root, "ACTION");
    if (actionItem && cJSON_IsString(actionItem) && actionItem->valuestring != nullptr &&
        strlen(actionItem->valuestring) > 0) {
        data.eventDebugInfo.append(" Action name = ").append(actionItem->valuestring);
    }

    cJSON* valueItem = cJSON_GetObjectItemCaseSensitive(root, "VALUE");
    if (valueItem && cJSON_IsNumber(valueItem)) {
        data.eventDebugInfo.append(" Value = ").append(std::to_string(valueItem->valueint));
    }

    cJSON* ratioItem = cJSON_GetObjectItemCaseSensitive(root, "RATIO");
    if (ratioItem && cJSON_IsNumber(ratioItem)) {
        std::string ratio = std::to_string(static_cast<float>(ratioItem->valuedouble)).substr(THERMAL_RATIO_BEGIN,
            THERMAL_RATIO_LENGTH);
        data.eventDebugInfo.append(" Ratio = ").append(ratio);
    }
}

void BatteryStatsListener::ProcessPowerWorkschedulerEvent(StatsUtils::StatsData& data, const cJSON* root)
{
    data.type = StatsUtils::STATS_TYPE_WORKSCHEDULER;
    cJSON* uidItem = cJSON_GetObjectItemCaseSensitive(root, "UID");
    if (uidItem && cJSON_IsNumber(uidItem)) {
        data.uid = static_cast<int32_t>(uidItem->valueint);
    }

    cJSON* pidItem = cJSON_GetObjectItemCaseSensitive(root, "PID");
    if (pidItem && cJSON_IsNumber(pidItem)) {
        data.pid = static_cast<int32_t>(pidItem->valueint);
    }

    cJSON* stateItem = cJSON_GetObjectItemCaseSensitive(root, "STATE");
    if (stateItem && cJSON_IsNumber(stateItem)) {
        data.state = static_cast<StatsUtils::StatsState>(stateItem->valueint);
    }

    cJSON* typeItem = cJSON_GetObjectItemCaseSensitive(root, "TYPE");
    if (typeItem && cJSON_IsNumber(typeItem)) {
        data.eventDataType = static_cast<int32_t>(typeItem->valueint);
    }

    cJSON* intervalItem = cJSON_GetObjectItemCaseSensitive(root, "INTERVAL");
    if (intervalItem && cJSON_IsNumber(intervalItem)) {
        data.eventDataExtra = static_cast<int32_t>(intervalItem->valueint);
    }
}

void BatteryStatsListener::ProcessOthersWorkschedulerEvent(StatsUtils::StatsData& data, const cJSON* root)
{
    data.type = StatsUtils::STATS_TYPE_WORKSCHEDULER;
    cJSON* nameItem = cJSON_GetObjectItemCaseSensitive(root, "name_");
    if (nameItem && cJSON_IsString(nameItem) && nameItem->valuestring != nullptr &&
        strlen(nameItem->valuestring) > 0) {
        data.eventDebugInfo.append(nameItem->valuestring).append(":");
    }

    cJSON* uidItem = cJSON_GetObjectItemCaseSensitive(root, "UID");
    if (uidItem && cJSON_IsNumber(uidItem)) {
        data.uid = static_cast<int32_t>(uidItem->valueint);
    }

    cJSON* pidItem = cJSON_GetObjectItemCaseSensitive(root, "PID");
    if (pidItem && cJSON_IsNumber(pidItem)) {
        data.pid = static_cast<int32_t>(pidItem->valueint);
    }

    cJSON* bundleNameItem = cJSON_GetObjectItemCaseSensitive(root, "NAME");
    if (bundleNameItem && cJSON_IsString(bundleNameItem) && bundleNameItem->valuestring != nullptr &&
        strlen(bundleNameItem->valuestring) > 0) {
        data.eventDebugInfo.append(" Bundle name = ").append(bundleNameItem->valuestring);
    }
    ProcessOthersWorkschedulerEventInternal(data, root);
}

void BatteryStatsListener::ProcessOthersWorkschedulerEventInternal(StatsUtils::StatsData& data, const cJSON* root)
{
    cJSON* workIdItem = cJSON_GetObjectItemCaseSensitive(root, "WORKID");
    if (workIdItem && cJSON_IsString(workIdItem) && workIdItem->valuestring != nullptr &&
        strlen(workIdItem->valuestring) > 0) {
        data.eventDebugInfo.append(" Work ID = ").append(workIdItem->valuestring);
    }

    cJSON* triggerItem = cJSON_GetObjectItemCaseSensitive(root, "TRIGGER");
    if (triggerItem && cJSON_IsString(triggerItem) && triggerItem->valuestring != nullptr &&
        strlen(triggerItem->valuestring) > 0) {
        data.eventDebugInfo.append(" Trigger conditions = ").append(triggerItem->valuestring);
    }

    cJSON* typeItem = cJSON_GetObjectItemCaseSensitive(root, "TYPE");
    if (typeItem && cJSON_IsString(typeItem) && typeItem->valuestring != nullptr &&
        strlen(typeItem->valuestring) > 0) {
        data.eventDebugInfo.append(" Work type = ").append(typeItem->valuestring);
    }

    cJSON* intervalItem = cJSON_GetObjectItemCaseSensitive(root, "INTERVAL");
    if (intervalItem && cJSON_IsNumber(intervalItem)) {
        data.eventDebugInfo.append(" Interval = ").append(std::to_string(intervalItem->valueint));
    }
}

void BatteryStatsListener::ProcessWorkschedulerEvent(StatsUtils::StatsData& data, const cJSON* root)
{
    cJSON* nameItem = cJSON_GetObjectItemCaseSensitive(root, "name_");
    if (!nameItem || !cJSON_IsString(nameItem) || nameItem->valuestring == nullptr ||
        strlen(nameItem->valuestring) == 0) {
        return;
    }
    std::string eventName(nameItem->valuestring);
    if (eventName == StatsHiSysEvent::POWER_WORKSCHEDULER) {
        ProcessPowerWorkschedulerEvent(data, root);
    } else {
        ProcessOthersWorkschedulerEvent(data, root);
    }
}

void BatteryStatsListener::ProcessDistributedSchedulerEvent(StatsUtils::StatsData& data, const cJSON* root)
{
    data.type = StatsUtils::STATS_TYPE_DISTRIBUTEDSCHEDULER;
    cJSON* nameItem = cJSON_GetObjectItemCaseSensitive(root, "name_");
    if (nameItem && cJSON_IsString(nameItem) && nameItem->valuestring != nullptr &&
        strlen(nameItem->valuestring) > 0) {
        data.eventDebugInfo.append("Event name = ").append(nameItem->valuestring);
    }

    cJSON* callingTypeItem = cJSON_GetObjectItemCaseSensitive(root, "CALLING_TYPE");
    if (callingTypeItem && cJSON_IsString(callingTypeItem) && callingTypeItem->valuestring != nullptr &&
        strlen(callingTypeItem->valuestring) > 0) {
        data.eventDebugInfo.append(" Calling Type = ").append(callingTypeItem->valuestring);
    }

    cJSON* callingUidItem = cJSON_GetObjectItemCaseSensitive(root, "CALLING_UID");
    if (callingUidItem && cJSON_IsNumber(callingUidItem)) {
        data.eventDebugInfo.append(" Calling Uid = ").append(std::to_string(callingUidItem->valueint));
    }

    cJSON* callingPidItem = cJSON_GetObjectItemCaseSensitive(root, "CALLING_PID");
    if (callingPidItem && cJSON_IsNumber(callingPidItem)) {
        data.eventDebugInfo.append(" Calling Pid = ").append(std::to_string(callingPidItem->valueint));
    }

    ProcessDistributedSchedulerEventInternal(data, root);
}

void BatteryStatsListener::ProcessDistributedSchedulerEventInternal(StatsUtils::StatsData& data, const cJSON* root)
{
    cJSON* targetBundleItem = cJSON_GetObjectItemCaseSensitive(root, "TARGET_BUNDLE");
    if (targetBundleItem && cJSON_IsString(targetBundleItem) && targetBundleItem->valuestring != nullptr &&
        strlen(targetBundleItem->valuestring) > 0) {
        data.eventDebugInfo.append(" Target Bundle Name = ").append(targetBundleItem->valuestring);
    }

    cJSON* targetAbilityItem = cJSON_GetObjectItemCaseSensitive(root, "TARGET_ABILITY");
    if (targetAbilityItem && cJSON_IsString(targetAbilityItem) && targetAbilityItem->valuestring != nullptr &&
        strlen(targetAbilityItem->valuestring) > 0) {
        data.eventDebugInfo.append(" Target Ability Name = ").append(targetAbilityItem->valuestring);
    }

    cJSON* callingAppUidItem = cJSON_GetObjectItemCaseSensitive(root, "CALLING_APP_UID");
    if (callingAppUidItem && cJSON_IsNumber(callingAppUidItem)) {
        data.eventDebugInfo.append(" Calling App Uid = ").append(std::to_string(callingAppUidItem->valueint));
    }

    cJSON* resultItem = cJSON_GetObjectItemCaseSensitive(root, "RESULT");
    if (resultItem && cJSON_IsNumber(resultItem)) {
        data.eventDebugInfo.append(" RESULT = ").append(std::to_string(resultItem->valueint));
    }
}

void BatteryStatsListener::ProcessAlarmEvent(StatsUtils::StatsData& data, const cJSON* root)
{
    data.type = StatsUtils::STATS_TYPE_ALARM;
    data.traffic = 1;

    cJSON* uidItem = cJSON_GetObjectItemCaseSensitive(root, "CALLER_UID");
    if (uidItem && cJSON_IsNumber(uidItem)) {
        data.uid = static_cast<int32_t>(uidItem->valueint);
    }

    cJSON* pidItem = cJSON_GetObjectItemCaseSensitive(root, "CALLER_PID");
    if (pidItem && cJSON_IsNumber(pidItem)) {
        data.pid = static_cast<int32_t>(pidItem->valueint);
    }
}

void BatteryStatsListener::OnServiceDied()
{
    STATS_HILOGE(COMP_SVC, "Service disconnected");
}
} // namespace PowerMgr
} // namespace OHOS
