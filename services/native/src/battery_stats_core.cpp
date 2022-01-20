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
#include "battery_stats_core.h"

#include <fstream>
#include <map>

#include <json/json.h>
#include <ohos_account_kits_impl.h>

#include "battery_info.h"
#include "battery_srv_client.h"
#include "battery_stats_info.h"
#include "entities/audio_entity.h"
#include "entities/bluetooth_entity.h"
#include "entities/camera_entity.h"
#include "entities/cpu_entity.h"
#include "entities/flashlight_entity.h"
#include "entities/gps_entity.h"
#include "entities/idle_entity.h"
#include "entities/phone_entity.h"
#include "entities/radio_entity.h"
#include "entities/screen_entity.h"
#include "entities/sensor_entity.h"
#include "entities/uid_entity.h"
#include "entities/user_entity.h"
#include "entities/wifi_entity.h"
#include "entities/wakelock_entity.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
static const std::string BATTERY_STATS_JSON = "/data/system/battery_stats.json";
} // namespace
void BatteryStatsCore::CreatePartEntity()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (bluetoothEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created bluetooth entity");
        bluetoothEntity_ = std::make_shared<BluetoothEntity>();
    }
    if (idleEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created idle entity");
        idleEntity_ = std::make_shared<IdleEntity>();
    }
    if (phoneEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created phone entity");
        phoneEntity_ = std::make_shared<PhoneEntity>();
    }
    if (radioEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created radio entity");
        radioEntity_ = std::make_shared<RadioEntity>();
    }
    if (screenEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created screen entity");
        screenEntity_ = std::make_shared<ScreenEntity>();
    }
    if (wifiEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created wifi entity");
        wifiEntity_ = std::make_shared<WifiEntity>();
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsCore::CreateAppEntity()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (audioEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created audio entity");
        audioEntity_ = std::make_shared<AudioEntity>();
    }
    if (cameraEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created camera entity");
        cameraEntity_ = std::make_shared<CameraEntity>();
    }
    if (flashlightEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created flashlight entity");
        flashlightEntity_ = std::make_shared<FlashlightEntity>();
    }
    if (gpsEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created gps entity");
        gpsEntity_ = std::make_shared<GpsEntity>();
    }
    if (sensorEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created sensor entity");
        sensorEntity_ = std::make_shared<SensorEntity>();
    }
    if (uidEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created uid entity");
        uidEntity_ = std::make_shared<UidEntity>();
    }
    if (userEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created user entity");
        userEntity_ = std::make_shared<UserEntity>();
    }
    if (wakelockEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created wakelock entity");
        wakelockEntity_ = std::make_shared<WakelockEntity>();
    }
    if (cpuEntity_ == nullptr) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Created cpu entity");
        cpuEntity_ = std::make_shared<CpuEntity>();
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

bool BatteryStatsCore::Init()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    CreateAppEntity();
    CreatePartEntity();

    auto& batterySrvClient = BatterySrvClient::GetInstance();
    BatteryPluggedType plugType = batterySrvClient.GetPluggedType();
    if (plugType == BatteryPluggedType::PLUGGED_TYPE_NONE || plugType == BatteryPluggedType::PLUGGED_TYPE_BUTT) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Device is not charing.");
        StatsHelper::SetOnBattery(true);
    } else {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Device is charing.");
        StatsHelper::SetOnBattery(false);
    }

    if (!LoadBatteryStatsData()) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Load battery stats failed");
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

void BatteryStatsCore::ComputePower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");

    BatteryStatsEntity::ResetStatsEntity();
    uidEntity_->Calculate();
    bluetoothEntity_->Calculate();
    idleEntity_->Calculate();
    phoneEntity_->Calculate();
    radioEntity_->Calculate();
    screenEntity_->Calculate();
    wifiEntity_->Calculate();
    userEntity_->Calculate();
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

BatteryStatsInfoList BatteryStatsCore::GetBatteryStats()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    return BatteryStatsEntity::GetStatsInfoList();
}

std::shared_ptr<BatteryStatsEntity> BatteryStatsCore::GetEntity(const BatteryStatsInfo::ConsumptionType& type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Get %{public}s entity", BatteryStatsInfo::ConvertConsumptionType(type).c_str());
    switch (type) {
        case BatteryStatsInfo::CONSUMPTION_TYPE_APP:
            return uidEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH:
            return bluetoothEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_IDLE:
            return idleEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_PHONE:
            return phoneEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_RADIO:
            return radioEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN:
            return screenEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_USER:
            return userEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_WIFI:
            return wifiEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA:
            return cameraEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT:
            return flashlightEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO:
            return audioEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR:
            return sensorEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_GPS:
            return gpsEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_CPU:
            return cpuEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK:
            return wakelockEntity_;
        case BatteryStatsInfo::CONSUMPTION_TYPE_INVALID:
        default:
            return nullptr;
    }
}

void BatteryStatsCore::UpdateStats(StatsUtils::StatsType statsType, long time, long data, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter for data and duration updating");
    STATS_HILOGI(STATS_MODULE_SERVICE, "statsType: %{public}s, uid: %{public}d",
        StatsUtils::ConvertStatsType(statsType).c_str(), uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "time: %{public}ld, data: %{public}ld", time, data);

    if (uid > StatsUtils::INVALID_VALUE) {
        uidEntity_->UpdateUidMap(uid);
    }

    switch (statsType) {
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX:
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX: {
            auto timer = bluetoothEntity_->GetOrCreateTimer(uid, statsType);
            auto counter = bluetoothEntity_->GetOrCreateCounter(statsType, uid);

            timer->AddRunningTimeMs(time);
            counter->AddCount(data);
            break;
        }
        case StatsUtils::STATS_TYPE_WIFI_RX:
        case StatsUtils::STATS_TYPE_WIFI_TX: {
            auto timer = wifiEntity_->GetOrCreateTimer(uid, statsType);
            auto counter = wifiEntity_->GetOrCreateCounter(statsType, uid);

            timer->AddRunningTimeMs(time);
            counter->AddCount(data);
            break;
        }
        case StatsUtils::STATS_TYPE_RADIO_RX:
        case StatsUtils::STATS_TYPE_RADIO_TX: {
            auto timer = radioEntity_->GetOrCreateTimer(uid, statsType);
            auto counter = radioEntity_->GetOrCreateCounter(statsType, uid);

            timer->AddRunningTimeMs(time);
            counter->AddCount(data);
            break;
        }
        default:
            break;
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit for data and duration updating");
}

void BatteryStatsCore::UpdateConnectiviyStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state,
    int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    switch (statsType) {
        case StatsUtils::STATS_TYPE_BLUETOOTH_ON:
            UpdateTimer(bluetoothEntity_, statsType, state);
            break;
        case StatsUtils::STATS_TYPE_WIFI_ON:
            UpdateTimer(wifiEntity_, statsType, state);
            break;
        case StatsUtils::STATS_TYPE_PHONE_ACTIVE:
            UpdateTimer(phoneEntity_, statsType, state);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_SCAN:
            UpdateTimer(bluetoothEntity_, statsType, state, uid);
            break;
        case StatsUtils::STATS_TYPE_WIFI_SCAN:
            UpdateTimer(wifiEntity_, statsType, state, uid);
            break;
        default:
            break;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsCore::UpdateCommonStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    switch (statsType) {
        case StatsUtils::STATS_TYPE_CAMERA_ON:
            UpdateTimer(cameraEntity_, statsType, state, uid);
            break;
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON:
            UpdateTimer(flashlightEntity_, statsType, state, uid);
            break;
        case StatsUtils::STATS_TYPE_GPS_ON:
            UpdateTimer(gpsEntity_, statsType, state, uid);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON:
            UpdateTimer(sensorEntity_, statsType, state, uid);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON:
            UpdateTimer(sensorEntity_, statsType, state, uid);
            break;
        case StatsUtils::STATS_TYPE_AUDIO_ON:
            UpdateTimer(audioEntity_, statsType, state, uid);
            break;
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD:
            UpdateTimer(wakelockEntity_, statsType, state, uid);
            break;
        default:
            break;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsCore::UpdateStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state, int16_t level,
    int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter for state updating");
    STATS_HILOGI(STATS_MODULE_SERVICE, "statsType: %{public}s, uid: %{public}d",
        StatsUtils::ConvertStatsType(statsType).c_str(), uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "state: %{public}d, level: %{public}d", state, level);

    if (uid > StatsUtils::INVALID_VALUE) {
        uidEntity_->UpdateUidMap(uid);
    }

    switch (statsType) {
        case StatsUtils::STATS_TYPE_RADIO_ON:
        case StatsUtils::STATS_TYPE_RADIO_SCAN:
            UpdateRadioStats(state, level);
            break;
        case StatsUtils::STATS_TYPE_SCREEN_ON:
        case StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS:
            UpdateScreenStats(state, level);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_ON:
        case StatsUtils::STATS_TYPE_WIFI_ON:
        case StatsUtils::STATS_TYPE_PHONE_ACTIVE:
        case StatsUtils::STATS_TYPE_BLUETOOTH_SCAN:
        case StatsUtils::STATS_TYPE_WIFI_SCAN:
            UpdateConnectiviyStats(statsType, state, uid);
            break;
        case StatsUtils::STATS_TYPE_CAMERA_ON:
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON:
        case StatsUtils::STATS_TYPE_GPS_ON:
        case StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON:
        case StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON:
        case StatsUtils::STATS_TYPE_AUDIO_ON:
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD:
            UpdateCommonStats(statsType, state, uid);
            break;
        default:
            break;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit for state updating");
}

void BatteryStatsCore::UpdateRadioStats(StatsUtils::StatsState state, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");

    bool isScanning = false;
    auto scanTimer = radioEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_RADIO_SCAN);
    if (scanTimer == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Found no related timer, update failed");
        return;
    }
    if (state == StatsUtils::STATS_STATE_NETWORK_SEARCH) {
        isScanning = true;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Updated %{public}s timer for state: %{public}d",
            StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_SCAN).c_str(), state);
        scanTimer->StartRunning();
    }
    if (!isScanning) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Updated %{public}s timer for state: %{public}d",
            StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_SCAN).c_str(), state);
        scanTimer->StopRunning();
    }

    if (lastSignalLevel_ != level) {
        if (lastSignalLevel_ > StatsUtils::INVALID_VALUE) {
            auto oldTimer = radioEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_RADIO_ON, lastSignalLevel_);
            if (oldTimer != nullptr) {
                STATS_HILOGI(STATS_MODULE_SERVICE, "Stop %{public}s timer for last level: %{public}d",
                    StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_ON).c_str(), lastSignalLevel_);
                oldTimer->StopRunning();
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE, "Found no %{public}s timer, update level: %{public}d failed",
                    StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_ON).c_str(), lastSignalLevel_);
            }
        }
        auto newTimer = radioEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_RADIO_ON, level);
        if (newTimer != nullptr) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Start %{public}s timer for latest level: %{public}d",
                StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_ON).c_str(), level);
            newTimer->StartRunning();
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Found no %{public}s timer for level: %{public}d, update failed",
                StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_ON).c_str(), level);
        }
        lastSignalLevel_ = level;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsCore::UpdateScreenStats(StatsUtils::StatsState state, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto onTimer = screenEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_SCREEN_ON);
    if (onTimer == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Found no %{public}s timer, update failed",
            StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_SCREEN_ON).c_str());
        return;
    }
    if (state == StatsUtils::STATS_STATE_DISPLAY_OFF) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Stop %{public}s timer for state: %{public}d",
            StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_SCREEN_ON).c_str(), state);
        onTimer->StopRunning();
        if (lastBrightnessLevel_ != StatsUtils::INVALID_VALUE) {
            auto brightnessTimer = screenEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS,
                lastBrightnessLevel_);
            brightnessTimer->StopRunning();
        }
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Start %{public}s timer for state: %{public}d",
            StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_SCREEN_ON).c_str(), state);
        onTimer->StartRunning();
        if (lastBrightnessLevel_ == StatsUtils::INVALID_VALUE) {
            auto brightnessTimer = screenEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS,
                level);
            brightnessTimer->StartRunning();
        } else if (lastBrightnessLevel_ != level) {
            auto oldTimer = screenEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS,
                lastBrightnessLevel_);
            auto newTimer = screenEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS, level);
            if (oldTimer != nullptr) {
                STATS_HILOGI(STATS_MODULE_SERVICE, "Stop screen_brightness timer for last level: %{public}d",
                    lastBrightnessLevel_);
                oldTimer->StopRunning();
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE,
                    "Found no screen_brightness timer for level: %{public}d, update failed", lastBrightnessLevel_);
            }
            if (newTimer != nullptr) {
                STATS_HILOGI(STATS_MODULE_SERVICE, "Start screen_brightness timer for latest level: %{public}d",
                    level);
                newTimer->StartRunning();
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE,
                    "Found no screen_brightness timer for level: %{public}d, update failed", level);
            }
        }
    }
    lastBrightnessLevel_ = level;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsCore::UpdateTimer(std::shared_ptr<BatteryStatsEntity> entity, StatsUtils::StatsType statsType,
    StatsUtils::StatsState state, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "entity: %{public}s",
        BatteryStatsInfo::ConvertConsumptionType(entity->GetConsumptionType()).c_str());
    STATS_HILOGI(STATS_MODULE_SERVICE, "statsType: %{public}s", StatsUtils::ConvertStatsType(statsType).c_str());
    STATS_HILOGI(STATS_MODULE_SERVICE, "state: %{public}d, uid: %{public}d", state, uid);
    std::shared_ptr<StatsHelper::ActiveTimer> timer;
    if (uid > StatsUtils::INVALID_VALUE) {
        timer = entity->GetOrCreateTimer(uid, statsType);
    } else {
        timer = entity->GetOrCreateTimer(statsType);
    }

    switch (state) {
        case StatsUtils::STATS_STATE_ACTIVATED:
            timer->StartRunning();
            break;
        case StatsUtils::STATS_STATE_DEACTIVATED:
            timer->StopRunning();
            break;
        default:
            break;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

long BatteryStatsCore::GetTotalTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long time = StatsUtils::DEFAULT_VALUE;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle level: %{public}d", level);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle statsType: %{public}s", StatsUtils::ConvertStatsType(statsType).c_str());

    switch (statsType) {
        case StatsUtils::STATS_TYPE_RADIO_ON:
            time = radioEntity_->GetActiveTimeMs(statsType, level);
            break;
        case StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS:
            time = screenEntity_->GetActiveTimeMs(statsType, level);
            break;
        case StatsUtils::STATS_TYPE_RADIO_SCAN:
            time = radioEntity_->GetActiveTimeMs(statsType);
            break;
        case StatsUtils::STATS_TYPE_SCREEN_ON:
            time = screenEntity_->GetActiveTimeMs(statsType);
            break;
        case StatsUtils::STATS_TYPE_BLUETOOTH_ON:
            time = bluetoothEntity_->GetActiveTimeMs(statsType);
            break;
        case StatsUtils::STATS_TYPE_WIFI_ON:
            time = wifiEntity_->GetActiveTimeMs(statsType);
            break;
        case StatsUtils::STATS_TYPE_PHONE_ACTIVE:
            time = phoneEntity_->GetActiveTimeMs(statsType);
            break;
        case StatsUtils::STATS_TYPE_PHONE_IDLE:
        case StatsUtils::STATS_TYPE_CPU_SUSPEND:
            time = idleEntity_->GetActiveTimeMs(statsType);
            break;
        default:
            break;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Get active time: %{public}ldms for %{public}s", time,
        StatsUtils::ConvertStatsType(statsType).c_str());
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return time;
}

void BatteryStatsCore::DumpInfo(std::string& result)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    result.append("BATTERY STATS DUMP (hidumper -batterystats):\n");
    if (bluetoothEntity_) {
        bluetoothEntity_->DumpInfo(result);
    }
    if (idleEntity_) {
        idleEntity_->DumpInfo(result);
    }
    if (phoneEntity_) {
        phoneEntity_->DumpInfo(result);
    }
    if (radioEntity_) {
        radioEntity_->DumpInfo(result);
    }
    if (screenEntity_) {
        screenEntity_->DumpInfo(result);
    }
    if (wifiEntity_) {
        wifiEntity_->DumpInfo(result);
    }
    if (uidEntity_) {
        uidEntity_->DumpInfo(result);
    }
    GetDebugInfo(result);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsCore::UpdateDebugInfo(const std::string& info)
{
    debugInfo_.append(info);
}

void BatteryStatsCore::GetDebugInfo(std::string& result)
{
    result = debugInfo_;
}

long BatteryStatsCore::GetTotalTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle statsType: %{public}s", StatsUtils::ConvertStatsType(statsType).c_str());
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle uid: %{public}d", uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle level: %{public}d", level);
    long time = StatsUtils::DEFAULT_VALUE;

    switch (statsType) {
        case StatsUtils::STATS_TYPE_CAMERA_ON:
            time = cameraEntity_->GetActiveTimeMs(uid, statsType);
            break;
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON:
            time = flashlightEntity_->GetActiveTimeMs(uid, statsType);
            break;
        case StatsUtils::STATS_TYPE_GPS_ON:
            time = gpsEntity_->GetActiveTimeMs(uid, statsType);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON:
            time = sensorEntity_->GetActiveTimeMs(uid, statsType);
            break;
        case StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON:
            time = sensorEntity_->GetActiveTimeMs(uid, statsType);
            break;
        case StatsUtils::STATS_TYPE_AUDIO_ON:
            time = audioEntity_->GetActiveTimeMs(uid, statsType);
            break;
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD:
            time = wakelockEntity_->GetActiveTimeMs(uid, statsType);
            break;
        case StatsUtils::STATS_TYPE_CPU_CLUSTER:
        case StatsUtils::STATS_TYPE_CPU_SPEED:
        case StatsUtils::STATS_TYPE_CPU_ACTIVE:
            time = cpuEntity_->GetCpuTimeMs(uid);
            break;
        default:
            break;
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Get active time: %{public}ldms for %{public}s of uid: %{public}d", time,
        StatsUtils::ConvertStatsType(statsType).c_str(), uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return time;
}

long BatteryStatsCore::GetTotalDataCount(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle statsType: %{public}s", StatsUtils::ConvertStatsType(statsType).c_str());
    STATS_HILOGI(STATS_MODULE_SERVICE, "Handle uid: %{public}d", uid);
    long data = StatsUtils::DEFAULT_VALUE;

    switch (statsType) {
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX:
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX:
            data = bluetoothEntity_->GetTrafficByte(statsType, uid);
            break;
        case StatsUtils::STATS_TYPE_WIFI_RX:
        case StatsUtils::STATS_TYPE_WIFI_TX:
            data = bluetoothEntity_->GetTrafficByte(statsType, uid);
            break;
        case StatsUtils::STATS_TYPE_RADIO_RX:
        case StatsUtils::STATS_TYPE_RADIO_TX:
            data = bluetoothEntity_->GetTrafficByte(statsType, uid);
            break;
        default:
            break;
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Get traffic data bytes: %{public}ld of %{public}s for uid: %{public}d", data,
        StatsUtils::ConvertStatsType(statsType).c_str(), uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return data;
}

double BatteryStatsCore::GetAppStatsMah(const int32_t& uid)
{
    double appStatsMah = StatsUtils::DEFAULT_VALUE;
    auto statsInfoList = GetBatteryStats();
    for (auto iter = statsInfoList.begin(); iter != statsInfoList.end(); iter++) {
        if ((*iter)->GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_APP) {
            if ((*iter)->GetUid() == uid) {
                appStatsMah = (*iter)->GetPower();
                break;
            }
        }
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "Got stats mah: %{public}lf for uid: %{public}d", appStatsMah, uid);
    return appStatsMah;
}

double BatteryStatsCore::GetAppStatsPercent(const int32_t& uid)
{
    double appStatsPercent = StatsUtils::DEFAULT_VALUE;
    auto statsInfoList = GetBatteryStats();
    auto totalConsumption = BatteryStatsEntity::GetTotalPowerMah();
    if (totalConsumption <= StatsUtils::DEFAULT_VALUE) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "No consumption got, return 0");
        return appStatsPercent;
    }
    for (auto iter = statsInfoList.begin(); iter != statsInfoList.end(); iter++) {
        if ((*iter)->GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_APP) {
            if ((*iter)->GetUid() == uid && totalConsumption != StatsUtils::DEFAULT_VALUE) {
                appStatsPercent = (*iter)->GetPower() / totalConsumption;
                break;
            }
        }
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "Got stats percent: %{public}lf for uid: %{public}d", appStatsPercent, uid);
    return appStatsPercent;
}

double BatteryStatsCore::GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& type)
{
    double partStatsMah = StatsUtils::DEFAULT_VALUE;
    auto statsInfoList = GetBatteryStats();
    for (auto iter = statsInfoList.begin(); iter != statsInfoList.end(); iter++) {
        if ((*iter)->GetConsumptionType() == type) {
            partStatsMah = (*iter)->GetPower();
            break;
        }
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "Got stats mah: %{public}lf for type: %{public}d", partStatsMah, type);
    return partStatsMah;
}

double BatteryStatsCore::GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& type)
{
    double partStatsPercent = StatsUtils::DEFAULT_VALUE;
    auto statsInfoList = GetBatteryStats();
    auto totalConsumption = BatteryStatsEntity::GetTotalPowerMah();
    for (auto iter = statsInfoList.begin(); iter != statsInfoList.end(); iter++) {
        if ((*iter)->GetConsumptionType() == type && totalConsumption != StatsUtils::DEFAULT_VALUE) {
            partStatsPercent = (*iter)->GetPower() / totalConsumption;
            break;
        }
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "Got stats percent: %{public}lf for type: %{public}d", partStatsPercent, type);
    return partStatsPercent;
}

bool BatteryStatsCore::SaveBatteryStatsData()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    ComputePower();
    Json::Value root;
    auto statsInfoList = BatteryStatsEntity::GetStatsInfoList();
    for (auto iter = statsInfoList.begin(); iter != statsInfoList.end(); iter++) {
        if ((*iter)->GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_APP) {
            std::string name = std::to_string((*iter)->GetUid());
            root[name] = Json::Value((*iter)->GetPower());
            STATS_HILOGD(STATS_MODULE_SERVICE, "Saved power: %{public}lf for uid: %{public}s", (*iter)->GetPower(),
                name.c_str());
        } else if ((*iter)->GetConsumptionType() != BatteryStatsInfo::CONSUMPTION_TYPE_USER) {
            std::string name = std::to_string((*iter)->GetConsumptionType());
            root[name] = Json::Value((*iter)->GetPower());
            STATS_HILOGD(STATS_MODULE_SERVICE, "Saved power: %{public}lf for type: %{public}s", (*iter)->GetPower(),
                name.c_str());
        }
    }
    Json::StreamWriterBuilder swb;
    std::ofstream ofs;
    ofs.open(BATTERY_STATS_JSON);
    if (!ofs.is_open()) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Opening json file failed");
        return false;
    }
    swb.newStreamWriter()->write(root, &ofs);
    ofs.close();
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

bool BatteryStatsCore::LoadBatteryStatsData()
{
    Json::CharReaderBuilder reader;
    Json::Value root;
    std::string errors;
    std::ifstream ifs(BATTERY_STATS_JSON, std::ios::binary);
    if (!ifs.is_open()) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Json file doesn't exist");
        return false;
    }
    if (!parseFromStream(reader, ifs, &root, &errors)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Parsing json file failed");
        return false;
    }
    ifs.close();
    BatteryStatsEntity::ResetStatsEntity();
    Json::Value::Members member = root.getMemberNames();
    std::map<int32_t, double> tmpUserPowerMap;
    for (auto iter = member.begin(); iter != member.end(); iter++) {
        auto id = std::stoi(*iter);
        std::shared_ptr<BatteryStatsInfo> info = std::make_shared<BatteryStatsInfo>();
        if (id > StatsUtils::INVALID_VALUE) {
            info->SetUid(id);
            info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
            info->SetPower(root[*iter].asDouble());
            int32_t usr = AccountSA::OhosAccountKits::GetInstance().GetDeviceAccountIdByUID(id);
            auto iter = tmpUserPowerMap.find(usr);
            if (iter != tmpUserPowerMap.end()) {
                iter->second += info->GetPower();
            } else {
                tmpUserPowerMap.insert(std::pair<int32_t, double>(usr, info->GetPower()));
            }
            STATS_HILOGI(STATS_MODULE_SERVICE, "Update power: %{public}lfmAh, user: %{public}d", info->GetPower(), usr);
        } else if (id < StatsUtils::INVALID_VALUE && id > BatteryStatsInfo::CONSUMPTION_TYPE_INVALID) {
            info->SetUid(StatsUtils::INVALID_VALUE);
            info->SetConsumptioType(static_cast<BatteryStatsInfo::ConsumptionType>(id));
            info->SetPower(root[*iter].asDouble());
        }
        STATS_HILOGD(STATS_MODULE_SERVICE, "Load power: %{public}lfmAh for id: %{public}d", info->GetPower(), id);
        BatteryStatsEntity::UpdateStatsInfoList(info);
    }
    for (auto &iter : tmpUserPowerMap) {
        std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
        statsInfo->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_USER);
        statsInfo->SetUserId(iter.first);
        statsInfo->SetPower(iter.second);
        BatteryStatsEntity::UpdateStatsInfoList(statsInfo);
    }
    return true;
}

void BatteryStatsCore::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    audioEntity_->Reset();
    bluetoothEntity_->Reset();
    cameraEntity_->Reset();
    cpuEntity_->Reset();
    flashlightEntity_->Reset();
    gpsEntity_->Reset();
    idleEntity_->Reset();
    phoneEntity_->Reset();
    radioEntity_->Reset();
    screenEntity_->Reset();
    sensorEntity_->Reset();
    uidEntity_->Reset();
    userEntity_->Reset();
    wifiEntity_->Reset();
    wakelockEntity_->Reset();
    BatteryStatsEntity::ResetStatsEntity();
    debugInfo_.clear();
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS