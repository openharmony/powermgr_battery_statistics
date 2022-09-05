/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef STATS_HISYSEVENT_H
#define STATS_HISYSEVENT_H

#include <string>

namespace OHOS {
namespace PowerMgr {
class StatsHiSysEvent {
public:
    enum HiSysEventType {
        HISYSEVENT_TYPE_INVALID = -1,
        HISYSEVENT_TYPE_POWER_RUNNINGLOCK,
        HISYSEVENT_TYPE_SCREEN_STATE,
        HISYSEVENT_TYPE_BRIGHTNESS_NIT,
        HISYSEVENT_TYPE_BACKLIGHT_DISCOUNT,
        HISYSEVENT_TYPE_AMBIENT_LIGHT,
        HISYSEVENT_TYPE_BATTERY_CHANGED,
        HISYSEVENT_TYPE_POWER_TEMPERATURE,
        HISYSEVENT_TYPE_THERMAL_LEVEL_CHANGED,
        HISYSEVENT_TYPE_POWER_WORKSCHEDULER,
        HISYSEVENT_TYPE_WORK_ADD,
        HISYSEVENT_TYPE_WORK_REMOVE,
        HISYSEVENT_TYPE_WORK_START,
        HISYSEVENT_TYPE_WORK_STOP,
        HISYSEVENT_TYPE_TORCH_STATE,
        HISYSEVENT_TYPE_CAMERA_CONNECT,
        HISYSEVENT_TYPE_CAMERA_DISCONNECT,
        HISYSEVENT_TYPE_FLASHLIGHT_ON,
        HISYSEVENT_TYPE_FLASHLIGHT_OFF,
        HISYSEVENT_TYPE_AUDIO_STREAM_CHANGE,
        HISYSEVENT_TYPE_POWER_SENSOR_GRAVITY,
        HISYSEVENT_TYPE_POWER_SENSOR_PROXIMITY,
        HISYSEVENT_TYPE_GNSS_STATE,
        HISYSEVENT_TYPE_BLUETOOTH_BR_SWITCH_STATE,
        HISYSEVENT_TYPE_BLUETOOTH_DISCOVERY_STATE,
        HISYSEVENT_TYPE_BLUETOOTH_BLE_STATE,
        HISYSEVENT_TYPE_BLUETOOTH_BLE_SCAN_START,
        HISYSEVENT_TYPE_BLUETOOTH_BLE_SCAN_STOP,
        HISYSEVENT_TYPE_WIFI_CONNECTION,
        HISYSEVENT_TYPE_WIFI_SCAN,
        HISYSEVENT_TYPE_START_REMOTE_ABILITY,
        HISYSEVENT_TYPE_MISC_TIME_STATISTIC_REPORT,
        HISYSEVENT_TYPE_THERMAL_ACTION_TRIGGERED,
        HISYSEVENT_TYPE_CALL_STATE,
        HISYSEVENT_TYPE_DATA_CONNECTION_STATE,
        HISYSEVENT_TYPE_END
    };

    static constexpr const char* HISYSEVENT_LIST[HISYSEVENT_TYPE_END] = {
        "POWER_RUNNINGLOCK",
        "SCREEN_STATE",
        "BRIGHTNESS_NIT",
        "BACKLIGHT_DISCOUNT",
        "AMBIENT_LIGHT",
        "BATTERY_CHANGED",
        "POWER_TEMPERATURE",
        "THERMAL_LEVEL_CHANGED",
        "POWER_WORKSCHEDULER",
        "WORK_ADD",
        "WORK_REMOVE",
        "WORK_START",
        "WORK_STOP",
        "TORCH_STATE",
        "CAMERA_CONNECT",
        "CAMERA_DISCONNECT",
        "FLASHLIGHT_ON",
        "FLASHLIGHT_OFF",
        "AUDIO_STREAM_CHANGE",
        "POWER_SENSOR_GRAVITY",
        "POWER_SENSOR_PROXIMITY",
        "GNSS_STATE",
        "BLUETOOTH_BR_SWITCH_STATE",
        "BLUETOOTH_DISCOVERY_STATE",
        "BLUETOOTH_BLE_STATE",
        "BLUETOOTH_BLE_SCAN_START",
        "BLUETOOTH_BLE_SCAN_STOP",
        "WIFI_CONNECTION",
        "WIFI_SCAN",
        "START_REMOTE_ABILITY",
        "MISC_TIME_STATISTIC_REPORT",
        "THERMAL_ACTION_TRIGGERED",
        "CALL_STATE",
        "DATA_CONNECTION_STATE",
    };

    static bool CheckHiSysEvent(const std::string& eventName);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_HISYSEVENT_H