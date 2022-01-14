/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "battery_stats_info.h"
#include <parcel.h>
#include <string_ex.h>
#include "stats_hilog_wrapper.h"
#include "stats_common.h"

namespace OHOS {
namespace PowerMgr {
const std::map<std::string, BatteryStatsInfo::ConsumptionType> BatteryStatsInfo::statsTypeMap_ = {
    {StatsUtils::CURRENT_BLUETOOTH_ON, BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH},
    {StatsUtils::CURRENT_WIFI_ON, BatteryStatsInfo::CONSUMPTION_TYPE_WIFI},
    {StatsUtils::CURRENT_RADIO_ON, BatteryStatsInfo::CONSUMPTION_TYPE_RADIO},
    {StatsUtils::CURRENT_RADIO_SCAN, BatteryStatsInfo::CONSUMPTION_TYPE_RADIO},
    {StatsUtils::CURRENT_RADIO_ACTIVE, BatteryStatsInfo::CONSUMPTION_TYPE_PHONE},
    {StatsUtils::CURRENT_SCREEN_ON, BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN},
    {StatsUtils::CURRENT_SCREEN_BRIGHTNESS, BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN},
    {StatsUtils::CURRENT_CPU_IDLE, BatteryStatsInfo::CONSUMPTION_TYPE_IDLE}
};

bool BatteryStatsInfo::Marshalling(Parcel& parcel) const
{
    STATS_WRITE_PARCEL_WITH_RET(parcel, Int32, uid_, false);
    STATS_WRITE_PARCEL_WITH_RET(parcel, Int32, static_cast<int32_t>(type_), false);
    STATS_WRITE_PARCEL_WITH_RET(parcel, Double, totalPowerMah_, false);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "uid: %{public}d.", uid_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "type: %{public}d.", type_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "power: %{public}lf.", totalPowerMah_);
    return true;
}

std::shared_ptr<BatteryStatsInfo> BatteryStatsInfo::Unmarshalling(Parcel& parcel)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
    if (statsInfo == nullptr) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "BatteryStatsInfo object creating is failed.");
        statsInfo = nullptr;
    }
    if (!statsInfo->ReadFromParcel(parcel)) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "ReadFromParcel failed.");
        statsInfo = nullptr;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return statsInfo;
}

bool BatteryStatsInfo::ReadFromParcel(Parcel &parcel)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_READ_PARCEL_WITH_RET(parcel, Int32, uid_, false);
    int32_t type = static_cast<int32_t>(0);
    STATS_READ_PARCEL_WITH_RET(parcel, Int32, type, false);
    type_ = static_cast<ConsumptionType>(type);
    STATS_READ_PARCEL_WITH_RET(parcel, Double, totalPowerMah_, false);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "uid: %{public}d.", uid_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "type: %{public}d.", type_);
    STATS_HILOGD(STATS_MODULE_INNERKIT, "power: %{public}lf.", totalPowerMah_);
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return true;
}

void BatteryStatsInfo::SetUid(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    if (uid > StatsUtils::INVALID_VALUE) {
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Set uid: %{public}d", uid);
        uid_ = uid;
    } else {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Got illegal uid: %{public}d, ignore", uid);
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

void BatteryStatsInfo::SetUserId(int32_t userId)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    if (userId > StatsUtils::INVALID_VALUE) {
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Set uid: %{public}d", userId);
        userId_ = userId;
    } else {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Got illegal user id: %{public}d, ignore", userId);
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

void BatteryStatsInfo::SetConsumptioType(ConsumptionType type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Set type: %{public}d", type);
    type_ = type;
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

void BatteryStatsInfo::SetTime(long time, StatsUtils::StatsType type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    switch (type) {
        case StatsUtils::STATS_TYPE_AUDIO_ON:
            audioTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CAMERA_ON:
            cameraTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON:
            flashlightTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_GPS_ON:
            gpsTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON:
            gravityTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON:
            proximityTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_ON:
            bluetoothOnTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_SCAN:
            bluetoothScanTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX:
            bluetoothRxTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX:
            bluetoothTxTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_ON:
            wifiOnTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        case StatsUtils::STATS_TYPE_WIFI_SCAN:
            wifiScanTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_RX:
            wifiRxTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_TX:
            wifiTxTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_SCREEN_ON:
            screenOnTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        case StatsUtils::STATS_TYPE_RADIO_ON:
            radioOnTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        case StatsUtils::STATS_TYPE_RADIO_SCAN:
            radioScanTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        case StatsUtils::STATS_TYPE_RADIO_RX:
            radioRxTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_RADIO_TX:
            radioTxTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD:
            wakelockTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CPU_CLUSTER:
            cpuClustersTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CPU_SPEED:
            cpuSpeedsTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_PHONE_IDLE:
            cpuIdleTimeMs_ = time;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                ConvertConsumptionType(type_).c_str());
            break;
        default:
            STATS_HILOGE(STATS_MODULE_INNERKIT, "Ilegal or invalid type got, return 0");
            break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

void BatteryStatsInfo::SetTraffic(long data, StatsUtils::StatsType type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    switch (type) {
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX:
            bluetoothRxBytes = data;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX:
            bluetoothTxBytes = data;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_RX:
            wifiRxBytes = data;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_TX:
            wifiTxBytes = data;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_RADIO_RX:
            radioRxBytes = data;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_RADIO_TX:
            radioTxBytes = data;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        default:
            STATS_HILOGE(STATS_MODULE_INNERKIT, "Ilegal or invalid type got, return 0");
            break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

void BatteryStatsInfo::SetPower(double power, StatsUtils::StatsType type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    switch (type) {
        case StatsUtils::STATS_TYPE_AUDIO_ON:
            audioPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CAMERA_ON:
            cameraPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON:
            flashlightPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_GPS_ON:
            gpsPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON:
            gravityPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON:
            proximityPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_SCAN:
            bluetoothScanPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX:
            bluetoothRxPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX:
            bluetoothTxPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_SCAN:
            wifiScanPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_RX:
            wifiRxPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_TX:
            wifiTxPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_RADIO_RX:
            radioRxPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_RADIO_TX:
            radioTxPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD:
            wakelockPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CPU_CLUSTER:
            cpuClustersPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CPU_SPEED:
            cpuSpeedsPowerMah_ = power;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_INVALID:
            totalPowerMah_ = power;
            if (type_ == CONSUMPTION_TYPE_APP) {
                STATS_HILOGI(STATS_MODULE_SERVICE, "Set total power: %{public}lfmAh for uid: %{public}d",
                power, uid_);
            } else if (type_ == CONSUMPTION_TYPE_USER) {
                STATS_HILOGI(STATS_MODULE_SERVICE, "Set total power: %{public}lfmAh for user id: %{public}d",
                power, userId_);
            } else {
                STATS_HILOGI(STATS_MODULE_INNERKIT, "Set power: %{public}lfmAh for %{public}s", power,
                    ConvertConsumptionType(type_).c_str());
            }
            break;
        default:
            STATS_HILOGE(STATS_MODULE_INNERKIT, "Ilegal type got, ignore");
            break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

int32_t BatteryStatsInfo::GetUid()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Got uid: %{public}d", uid_);
    return uid_;
}

int32_t BatteryStatsInfo::GetUserId()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Got user id: %{public}d", userId_);
    return userId_;
}

BatteryStatsInfo::ConsumptionType BatteryStatsInfo::GetConsumptionType()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Got type: %{public}d", type_);
    return type_;
}

long BatteryStatsInfo::GetTime(StatsUtils::StatsType type = StatsUtils::STATS_TYPE_INVALID)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    long time = StatsUtils::DEFAULT_VALUE;
    switch (type) {
        case StatsUtils::STATS_TYPE_AUDIO_ON:
            time = audioTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CAMERA_ON:
            time = cameraTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON:
            time = flashlightTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_GPS_ON:
            time = gpsTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON:
            time = gravityTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON:
            time = proximityTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_ON:
            time = bluetoothOnTimeMs_;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_SCAN:
            time = bluetoothScanTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX:
            time = bluetoothRxTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX:
            time = bluetoothTxTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_ON:
            time = wifiOnTimeMs_;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        case StatsUtils::STATS_TYPE_WIFI_SCAN:
            time = wifiScanTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_RX:
            time = wifiRxTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_TX:
            time = wifiTxTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_SCREEN_ON:
            time = screenOnTimeMs_;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        case StatsUtils::STATS_TYPE_RADIO_ON:
            time = radioOnTimeMs_;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        case StatsUtils::STATS_TYPE_RADIO_SCAN:
            time = radioScanTimeMs_;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                StatsUtils::ConvertStatsType(type).c_str());
            break;
        case StatsUtils::STATS_TYPE_RADIO_RX:
            time = radioRxTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_RADIO_TX:
            time = radioTxTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD:
            time = wakelockTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CPU_CLUSTER:
            time = cpuClustersTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CPU_SPEED:
            time = cpuSpeedsTimeMs_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got time: %{public}ldms for %{public}s of uid: %{public}d", time,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_PHONE_IDLE:
            time = cpuIdleTimeMs_;
            STATS_HILOGI(STATS_MODULE_INNERKIT, "Got time: %{public}ldms for %{public}s", time,
                ConvertConsumptionType(type_).c_str());
            break;
        default:
            STATS_HILOGE(STATS_MODULE_INNERKIT, "Ilegal or invalid type got, return 0");
            break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return time;
}

long BatteryStatsInfo::GetTraffic(StatsUtils::StatsType type = StatsUtils::STATS_TYPE_INVALID)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    long data = StatsUtils::DEFAULT_VALUE;
    switch (type) {
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX:
            data = bluetoothRxBytes;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX:
            data = bluetoothTxBytes;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_RX:
            data = wifiRxBytes;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_TX:
            data = wifiTxBytes;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_RADIO_RX:
            data = radioRxBytes;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_RADIO_TX:
            data = radioTxBytes;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got data: %{public}ldbytes for %{public}s of uid: %{public}d", data,
                StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        default:
            STATS_HILOGE(STATS_MODULE_INNERKIT, "Ilegal or invalid type got, return 0");
            break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return data;
}

double BatteryStatsInfo::GetPower(StatsUtils::StatsType type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    switch (type) {
        case StatsUtils::STATS_TYPE_AUDIO_ON:
            power = audioPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CAMERA_ON:
            power = cameraPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON:
            power = flashlightPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_GPS_ON:
            power = gpsPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON:
            power = gravityPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON:
            power = proximityPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_SCAN:
            power = bluetoothScanPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX:
            power = bluetoothRxPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX:
            power = bluetoothTxPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_SCAN:
            power = wifiScanPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_RX:
            power = wifiRxPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WIFI_TX:
            power = wifiTxPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_RADIO_RX:
            power = radioRxPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_RADIO_TX:
            power = radioTxPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD:
            power = wakelockPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CPU_CLUSTER:
            power = cpuClustersPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_CPU_SPEED:
            power = cpuSpeedsPowerMah_;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got power: %{public}lfmAh for %{public}s of uid: %{public}d",
                power, StatsUtils::ConvertStatsType(type).c_str(), uid_);
            break;
        case StatsUtils::STATS_TYPE_INVALID:
            power = totalPowerMah_;
            if (type_ == CONSUMPTION_TYPE_APP) {
                STATS_HILOGI(STATS_MODULE_SERVICE, "Got total power: %{public}lfmAh of uid: %{public}d",
                power, uid_);
            } else if (type_ == CONSUMPTION_TYPE_USER) {
                STATS_HILOGI(STATS_MODULE_SERVICE, "Got total power: %{public}lfmAh of user id: %{public}d",
                power, userId_);
            } else {
                STATS_HILOGI(STATS_MODULE_INNERKIT, "Got power: %{public}lfmAh of %{public}s", power,
                    ConvertConsumptionType(type_).c_str());
            }
            break;
        default:
            STATS_HILOGE(STATS_MODULE_INNERKIT, "Ilegal type got, return 0");
            break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return power;
}

BatteryStatsInfo::ConsumptionType BatteryStatsInfo::ConvertToConsumptionType(std::string type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    ConsumptionType statsType = CONSUMPTION_TYPE_INVALID;
    auto iter = statsTypeMap_.find(type);
    if (iter != statsTypeMap_.end()) {
        statsType = iter->second;
        STATS_HILOGD(STATS_MODULE_INNERKIT, "Convert %{public}s to %{public}d", type.c_str(), statsType);
    } else {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Convert failed for %{public}s, return %{public}d", type.c_str(), statsType);
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return statsType;
}

std::string BatteryStatsInfo::ConvertConsumptionType(ConsumptionType type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    std::string result = "";
    switch (type) {
    case CONSUMPTION_TYPE_APP:
        result = "CONSUMPTION_TYPE_APP";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_BLUETOOTH:
        result = "CONSUMPTION_TYPE_BLUETOOTH";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_IDLE:
        result = "CONSUMPTION_TYPE_IDLE";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_PHONE:
        result = "CONSUMPTION_TYPE_PHONE";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_RADIO:
        result = "CONSUMPTION_TYPE_RADIO";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_SCREEN:
        result = "CONSUMPTION_TYPE_SCREEN";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_USER:
        result = "CONSUMPTION_TYPE_USER";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_WIFI:
        result = "CONSUMPTION_TYPE_WIFI";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_INVALID:
        result = "CONSUMPTION_TYPE_INVALID";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_CAMERA:
        result = "CONSUMPTION_TYPE_CAMERA";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_FLASHLIGHT:
        result = "CONSUMPTION_TYPE_FLASHLIGHT";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_AUDIO:
        result = "CONSUMPTION_TYPE_AUDIO";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_SENSOR:
        result = "CONSUMPTION_TYPE_SENSOR";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_GPS:
        result = "CONSUMPTION_TYPE_GPS";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_CPU:
        result = "CONSUMPTION_TYPE_CPU";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    case CONSUMPTION_TYPE_WAKELOCK:
        result = "CONSUMPTION_TYPE_WAKELOCK";
        STATS_HILOGI(STATS_MODULE_INNERKIT, "Convert to %{public}s", result.c_str());
        break;
    default:
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Illegal ConsumptionType got");
        break;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return result;
}
} // namespace PowerMgr
} // namespace OHOS
