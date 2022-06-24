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
#include "battery_stats_core.h"

#include <fstream>
#include <map>

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
#include "stats_log.h"

#include "xcollie.h"

namespace OHOS {
namespace PowerMgr {
namespace {
static const std::string BATTERY_STATS_JSON = "/data/service/el0/stats/battery_stats.json";
} // namespace
void BatteryStatsCore::CreatePartEntity()
{
    if (bluetoothEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created bluetooth entity");
        bluetoothEntity_ = std::make_shared<BluetoothEntity>();
    }
    if (idleEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created idle entity");
        idleEntity_ = std::make_shared<IdleEntity>();
    }
    if (phoneEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created phone entity");
        phoneEntity_ = std::make_shared<PhoneEntity>();
    }
    if (radioEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created radio entity");
        radioEntity_ = std::make_shared<RadioEntity>();
    }
    if (screenEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created screen entity");
        screenEntity_ = std::make_shared<ScreenEntity>();
    }
    if (wifiEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created wifi entity");
        wifiEntity_ = std::make_shared<WifiEntity>();
    }
}

void BatteryStatsCore::CreateAppEntity()
{
    if (audioEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created audio entity");
        audioEntity_ = std::make_shared<AudioEntity>();
    }
    if (cameraEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created camera entity");
        cameraEntity_ = std::make_shared<CameraEntity>();
    }
    if (flashlightEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created flashlight entity");
        flashlightEntity_ = std::make_shared<FlashlightEntity>();
    }
    if (gpsEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created gps entity");
        gpsEntity_ = std::make_shared<GpsEntity>();
    }
    if (sensorEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created sensor entity");
        sensorEntity_ = std::make_shared<SensorEntity>();
    }
    if (uidEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created uid entity");
        uidEntity_ = std::make_shared<UidEntity>();
    }
    if (userEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created user entity");
        userEntity_ = std::make_shared<UserEntity>();
    }
    if (wakelockEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created wakelock entity");
        wakelockEntity_ = std::make_shared<WakelockEntity>();
    }
    if (cpuEntity_ == nullptr) {
        STATS_HILOGD(COMP_SVC, "Created cpu entity");
        cpuEntity_ = std::make_shared<CpuEntity>();
    }
}

bool BatteryStatsCore::Init()
{
    STATS_HILOGI(COMP_SVC, "Battery stats core init");
    CreateAppEntity();
    CreatePartEntity();
    auto& batterySrvClient = BatterySrvClient::GetInstance();
    BatteryPluggedType plugType = batterySrvClient.GetPluggedType();
    if (plugType == BatteryPluggedType::PLUGGED_TYPE_NONE || plugType == BatteryPluggedType::PLUGGED_TYPE_BUTT) {
        STATS_HILOGI(COMP_SVC, "Device is not charing");
        StatsHelper::SetOnBattery(true);
    } else {
        STATS_HILOGI(COMP_SVC, "Device is charing");
        StatsHelper::SetOnBattery(false);
    }

    if (!LoadBatteryStatsData()) {
        STATS_HILOGE(COMP_SVC, "Load battery stats failed");
    }
    return true;
}

void BatteryStatsCore::ComputePower()
{
    STATS_HILOGD(COMP_SVC, "Calculate battery stats");
    const int DFX_DELAY_MS = 10000;
    int id = HiviewDFX::XCollie::GetInstance().SetTimer("BatteryStatsCoreComputePower", DFX_DELAY_MS, nullptr, nullptr,
        HiviewDFX::XCOLLIE_FLAG_NOOP);

    BatteryStatsEntity::ResetStatsEntity();
    uidEntity_->Calculate();
    bluetoothEntity_->Calculate();
    idleEntity_->Calculate();
    phoneEntity_->Calculate();
    radioEntity_->Calculate();
    screenEntity_->Calculate();
    wifiEntity_->Calculate();
    userEntity_->Calculate();

    HiviewDFX::XCollie::GetInstance().CancelTimer(id);
}

BatteryStatsInfoList BatteryStatsCore::GetBatteryStats()
{
    return BatteryStatsEntity::GetStatsInfoList();
}

std::shared_ptr<BatteryStatsEntity> BatteryStatsCore::GetEntity(const BatteryStatsInfo::ConsumptionType& type)
{
    STATS_HILOGD(COMP_SVC, "Get %{public}s entity", BatteryStatsInfo::ConvertConsumptionType(type).c_str());
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
    STATS_HILOGD(COMP_SVC,
        "Update for duration, statsType: %{public}s, uid: %{public}d, time: %{public}ld, data: %{public}ld",
        StatsUtils::ConvertStatsType(statsType).c_str(), uid, time, data);
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
}

void BatteryStatsCore::UpdateConnectivityStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state,
    int32_t uid)
{
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
}

void BatteryStatsCore::UpdateCommonStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state, int32_t uid)
{
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
}

void BatteryStatsCore::UpdateStats(StatsUtils::StatsType statsType, StatsUtils::StatsState state, int16_t level,
    int32_t uid)
{
    STATS_HILOGD(COMP_SVC,
        "Update for state, statsType: %{public}s, uid: %{public}d, state: %{public}d, level: %{public}d",
        StatsUtils::ConvertStatsType(statsType).c_str(), uid, state, level);
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
            UpdateConnectivityStats(statsType, state, uid);
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
}

void BatteryStatsCore::UpdateRadioStats(StatsUtils::StatsState state, int16_t level)
{
    STATS_HILOGD(COMP_SVC, "Last signal level: %{public}d", lastSignalLevel_);
    auto scanTimer = radioEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_RADIO_SCAN);

    if (state == StatsUtils::STATS_STATE_NETWORK_SEARCH) {
        isScanning_ = true;
        STATS_HILOGI(COMP_SVC, "Updated %{public}s timer for state: %{public}d",
            StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_SCAN).c_str(), state);
        scanTimer->StartRunning();
    } else {
        STATS_HILOGD(COMP_SVC, "Stop network search");
        isScanning_ = false;
    }
    if (!isScanning_) {
        STATS_HILOGI(COMP_SVC, "Updated %{public}s timer for state: %{public}d",
            StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_SCAN).c_str(), state);
        scanTimer->StopRunning();
    }

    if (lastSignalLevel_ <= StatsUtils::INVALID_VALUE ||
        (level > StatsUtils::INVALID_VALUE && level == lastSignalLevel_)) {
        auto signalTimer = radioEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_RADIO_ON, level);
        STATS_HILOGI(COMP_SVC, "Start %{public}s timer for first/same level: %{public}d",
            StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_ON).c_str(), level);
        signalTimer->StartRunning();
    } else if (lastSignalLevel_ != level) {
        auto oldTimer = radioEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_RADIO_ON, lastSignalLevel_);
        if (oldTimer != nullptr) {
            STATS_HILOGI(COMP_SVC, "Stop %{public}s timer for last level: %{public}d",
                StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_ON).c_str(), lastSignalLevel_);
            oldTimer->StopRunning();
        } else {
            STATS_HILOGW(COMP_SVC, "Found no %{public}s timer, update level: %{public}d failed",
                StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_ON).c_str(), lastSignalLevel_);
        }
        auto newTimer = radioEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_RADIO_ON, level);
        if (newTimer != nullptr) {
            STATS_HILOGI(COMP_SVC, "Start %{public}s timer for latest level: %{public}d",
                StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_ON).c_str(), level);
            newTimer->StartRunning();
        } else {
            STATS_HILOGW(COMP_SVC, "Found no %{public}s timer for level: %{public}d, update failed",
                StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_RADIO_ON).c_str(), level);
        }
    }
    lastSignalLevel_ = level;
}

void BatteryStatsCore::UpdateScreenStats(StatsUtils::StatsState state, int16_t level)
{
    STATS_HILOGD(COMP_SVC, "Last brightness level: %{public}d", lastBrightnessLevel_);
    auto onTimer = screenEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_SCREEN_ON);
    if (state == StatsUtils::STATS_STATE_DISPLAY_OFF) {
        STATS_HILOGI(COMP_SVC, "Stop %{public}s timer for state: %{public}d",
            StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_SCREEN_ON).c_str(), state);
        onTimer->StopRunning();
        if (lastBrightnessLevel_ > StatsUtils::INVALID_VALUE) {
            auto brightnessTimer = screenEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS,
                lastBrightnessLevel_);
            STATS_HILOGI(COMP_SVC, "Stop timer for last level: %{public}d", lastBrightnessLevel_);
            brightnessTimer->StopRunning();
        }
    } else {
        STATS_HILOGI(COMP_SVC, "Start %{public}s timer for state: %{public}d",
            StatsUtils::ConvertStatsType(StatsUtils::STATS_TYPE_SCREEN_ON).c_str(), state);
        onTimer->StartRunning();
        if (lastBrightnessLevel_ <= StatsUtils::INVALID_VALUE ||
            (level > StatsUtils::INVALID_VALUE && level == lastBrightnessLevel_)) {
            auto brightnessTimer = screenEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS,
                level);
            STATS_HILOGI(COMP_SVC, "Start screen_brightness timer for first/same level: %{public}d", level);
            brightnessTimer->StartRunning();
        } else if (lastBrightnessLevel_ != level) {
            auto oldTimer = screenEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS,
                lastBrightnessLevel_);
            auto newTimer = screenEntity_->GetOrCreateTimer(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS, level);
            if (oldTimer != nullptr) {
                STATS_HILOGI(COMP_SVC, "Stop screen_brightness timer for last level: %{public}d",
                    lastBrightnessLevel_);
                oldTimer->StopRunning();
            } else {
                STATS_HILOGW(COMP_SVC,
                    "Found no screen_brightness timer for level: %{public}d, update failed", lastBrightnessLevel_);
            }
            if (newTimer != nullptr) {
                STATS_HILOGI(COMP_SVC, "Start screen_brightness timer for latest level: %{public}d", level);
                newTimer->StartRunning();
            } else {
                STATS_HILOGW(COMP_SVC, "Found no timer for level: %{public}d, update failed", level);
            }
        }
    }
    lastBrightnessLevel_ = level;
}

void BatteryStatsCore::UpdateTimer(std::shared_ptr<BatteryStatsEntity> entity, StatsUtils::StatsType statsType,
    StatsUtils::StatsState state, int32_t uid)
{
    STATS_HILOGD(COMP_SVC,
        "entity: %{public}s, statsType: %{public}s, state: %{public}d, uid: %{public}d",
        BatteryStatsInfo::ConvertConsumptionType(entity->GetConsumptionType()).c_str(),
        StatsUtils::ConvertStatsType(statsType).c_str(),
        state,
        uid);
    std::shared_ptr<StatsHelper::ActiveTimer> timer;
    if (uid > StatsUtils::INVALID_VALUE) {
        timer = entity->GetOrCreateTimer(uid, statsType);
    } else {
        timer = entity->GetOrCreateTimer(statsType);
    }

    if (timer == nullptr) {
        STATS_HILOGE(COMP_SVC, "Timer is null, return");
        return;
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
}

int64_t BatteryStatsCore::GetTotalTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGD(COMP_SVC, "Handle statsType: %{public}s, level: %{public}d",
        StatsUtils::ConvertStatsType(statsType).c_str(), level);
    int64_t time = StatsUtils::DEFAULT_VALUE;
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
    STATS_HILOGD(COMP_SVC, "Get active time: %{public}sms for %{public}s", std::to_string(time).c_str(),
        StatsUtils::ConvertStatsType(statsType).c_str());
    return time;
}

void BatteryStatsCore::DumpInfo(std::string& result)
{
    result.append("BATTERY STATS DUMP (statistics dump -batterystats):\n");
    result.append("\n");
    if (bluetoothEntity_) {
        bluetoothEntity_->DumpInfo(result);
        result.append("\n");
    }
    if (idleEntity_) {
        idleEntity_->DumpInfo(result);
        result.append("\n");
    }
    if (phoneEntity_) {
        phoneEntity_->DumpInfo(result);
        result.append("\n");
    }
    if (radioEntity_) {
        radioEntity_->DumpInfo(result);
        result.append("\n");
    }
    if (screenEntity_) {
        screenEntity_->DumpInfo(result);
        result.append("\n");
    }
    if (wifiEntity_) {
        wifiEntity_->DumpInfo(result);
        result.append("\n");
    }
    if (uidEntity_) {
        uidEntity_->DumpInfo(result);
        result.append("\n");
    }
    GetDebugInfo(result);
}

void BatteryStatsCore::UpdateDebugInfo(const std::string& info)
{
    debugInfo_.append(info);
}

void BatteryStatsCore::GetDebugInfo(std::string& result)
{
    if (debugInfo_.size() > 0) {
        STATS_HILOGI(COMP_SVC, "Get debug info");
        result.append("Misc stats info dump:\n");
        result.append(debugInfo_);
    } else {
        STATS_HILOGI(COMP_SVC, "There's no debug info collected yet");
    }
}

int64_t BatteryStatsCore::GetTotalTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGD(COMP_SVC, "Handle statsType: %{public}s, uid: %{public}d, level: %{public}d",
        StatsUtils::ConvertStatsType(statsType).c_str(), uid, level);
    int64_t time = StatsUtils::DEFAULT_VALUE;

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
    STATS_HILOGD(COMP_SVC, "Get active time: %{public}sms for %{public}s of uid: %{public}d",
        std::to_string(time).c_str(), StatsUtils::ConvertStatsType(statsType).c_str(), uid);
    return time;
}

long BatteryStatsCore::GetTotalDataCount(StatsUtils::StatsType statsType, int32_t uid)
{
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
    STATS_HILOGD(COMP_SVC, "Get traffic data bytes: %{public}ld of %{public}s for uid: %{public}d", data,
        StatsUtils::ConvertStatsType(statsType).c_str(), uid);
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
    STATS_HILOGD(COMP_SVC, "Got stats mah: %{public}lf for uid: %{public}d", appStatsMah, uid);
    return appStatsMah;
}

double BatteryStatsCore::GetAppStatsPercent(const int32_t& uid)
{
    double appStatsPercent = StatsUtils::DEFAULT_VALUE;
    auto statsInfoList = GetBatteryStats();
    auto totalConsumption = BatteryStatsEntity::GetTotalPowerMah();
    if (totalConsumption <= StatsUtils::DEFAULT_VALUE) {
        STATS_HILOGE(COMP_SVC, "No consumption got, return 0");
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
    STATS_HILOGD(COMP_SVC, "Got stats percent: %{public}lf for uid: %{public}d", appStatsPercent, uid);
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
    STATS_HILOGD(COMP_SVC, "Got stats mah: %{public}lf for type: %{public}d", partStatsMah, type);
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
    STATS_HILOGD(COMP_SVC, "Got stats percent: %{public}lf for type: %{public}d", partStatsPercent, type);
    return partStatsPercent;
}

void BatteryStatsCore::SaveForHardware(Json::Value& root)
{
    STATS_HILOGD(COMP_SVC, "Save hardware battery stats");
    // Save for Bluetooth
    root["Hardware"]["bluetooth_on"] =
        Json::Value(GetTotalTimeMs(StatsUtils::STATS_TYPE_BLUETOOTH_ON));

    // Save for Screen
    root["Hardware"]["screen_on"] =
        Json::Value(GetTotalTimeMs(StatsUtils::STATS_TYPE_SCREEN_ON));
    for (uint16_t brightNess = 0; brightNess <= StatsUtils::SCREEN_BRIGHTNESS_BIN; brightNess++) {
        root["Hardware"]["screen_brightness"][brightNess] =
            Json::Value(GetTotalTimeMs(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS, brightNess));
    }

    // Save for Wifi
    root["Hardware"]["wifi_on"] =
        Json::Value(GetTotalTimeMs(StatsUtils::STATS_TYPE_WIFI_ON));

    // Save for CPU idle
    root["Hardware"]["cpu_idle"] =
        Json::Value(GetTotalTimeMs(StatsUtils::STATS_TYPE_PHONE_IDLE));

    // Save for Phone
    root["Hardware"]["radio_active"] =
        Json::Value(GetTotalTimeMs(StatsUtils::STATS_TYPE_PHONE_ACTIVE));

    // Save for Radio
    root["Hardware"]["radio_scan"] =
        Json::Value(GetTotalTimeMs(StatsUtils::STATS_TYPE_RADIO_SCAN));
    for (uint16_t signal = 0; signal <= StatsUtils::RADIO_SIGNAL_BIN; signal++) {
        root["Hardware"]["radio_on"][signal] =
            Json::Value(GetTotalTimeMs(StatsUtils::STATS_TYPE_RADIO_ON, signal));
    }
}

void BatteryStatsCore::SaveForSoftware(Json::Value& root)
{
    for (auto it : uidEntity_->GetUids()) {
        SaveForSoftwareCommon(root, it);
        SaveForSoftwareConnectivity(root, it);
    }
}

void BatteryStatsCore::SaveForSoftwareCommon(Json::Value& root, int32_t uid)
{
    STATS_HILOGD(COMP_SVC, "Save software common battery stats, uid: %{public}d", uid);
    // Save for camera related
    root["Software"][uid]["camera_on"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_CAMERA_ON));

    // Save for flashlight related
    root["Software"][uid]["flashlight_on"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_FLASHLIGHT_ON));

    // Save for gps related
    root["Software"][uid]["gps_on"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_GPS_ON));

    // Save for audio related
    root["Software"][uid]["audio_on"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_AUDIO_ON));

    // Save for wakelock related
    root["Software"][uid]["cpu_awake"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_WAKELOCK_HOLD));

    // Save for sensor related
    root["Software"][uid]["sensor_gravity"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON));
    root["Software"][uid]["sensor_proximity"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON));

    // Save for cpu related
    root["Software"][uid]["cpu_time"] = Json::Value(cpuEntity_->GetCpuTimeMs(uid));
}

void BatteryStatsCore::SaveForSoftwareConnectivity(Json::Value& root, int32_t uid)
{
    STATS_HILOGD(COMP_SVC, "Save software connectivity battery stats, uid: %{public}d", uid);
    // Save for Bluetooth related
    root["Software"][uid]["bluetooth_scan"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_BLUETOOTH_SCAN));
    root["Software"][uid]["bluetooth_rx"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_BLUETOOTH_RX));
    root["Software"][uid]["bluetooth_tx"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_BLUETOOTH_TX));
    int32_t bluetoothTxBytes = GetTotalDataCount(StatsUtils::STATS_TYPE_BLUETOOTH_TX, uid);
    int32_t bluetoothRxBytes = GetTotalDataCount(StatsUtils::STATS_TYPE_BLUETOOTH_RX, uid);
    root["Software"][uid]["bluetooth_byte"] = Json::Value(bluetoothTxBytes + bluetoothRxBytes);

    // Save for wifi related
    root["Software"][uid]["wifi_scan"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_WIFI_SCAN));
    root["Software"][uid]["wifi_rx"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_WIFI_RX));
    root["Software"][uid]["wifi_tx"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_WIFI_TX));
    int32_t wifiTxBytes = GetTotalDataCount(StatsUtils::STATS_TYPE_WIFI_TX, uid);
    int32_t wifiRxBytes = GetTotalDataCount(StatsUtils::STATS_TYPE_WIFI_RX, uid);
    root["Software"][uid]["wifi_byte"] = Json::Value(wifiTxBytes + wifiRxBytes);

    // Save for radio related
    root["Software"][uid]["radio_tx"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_RADIO_TX));
    root["Software"][uid]["radio_rx"] =
        Json::Value(GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_RADIO_RX));
    int32_t radioTxBytes = GetTotalDataCount(StatsUtils::STATS_TYPE_RADIO_TX, uid);
    int32_t radioRxBytes = GetTotalDataCount(StatsUtils::STATS_TYPE_RADIO_RX, uid);
    root["Software"][uid]["radio_byte"] = Json::Value(radioTxBytes + radioRxBytes);
}

void BatteryStatsCore::SaveForPower(Json::Value& root)
{
    STATS_HILOGD(COMP_SVC, "Save power battery stats");
    auto statsInfoList = BatteryStatsEntity::GetStatsInfoList();
    for (auto iter = statsInfoList.begin(); iter != statsInfoList.end(); iter++) {
        if ((*iter)->GetConsumptionType() == BatteryStatsInfo::CONSUMPTION_TYPE_APP) {
            std::string name = std::to_string((*iter)->GetUid());
            root["Power"][name] = Json::Value((*iter)->GetPower());
            STATS_HILOGD(COMP_SVC, "Saved power: %{public}lf for uid: %{public}s", (*iter)->GetPower(),
                name.c_str());
        } else if ((*iter)->GetConsumptionType() != BatteryStatsInfo::CONSUMPTION_TYPE_USER) {
            std::string name = std::to_string((*iter)->GetConsumptionType());
            root["Power"][name] = Json::Value((*iter)->GetPower());
            STATS_HILOGD(COMP_SVC, "Saved power: %{public}lf for type: %{public}s", (*iter)->GetPower(),
                name.c_str());
        }
    }
}

bool BatteryStatsCore::SaveBatteryStatsData()
{
    ComputePower();
    Json::Value root;

    // Save for power
    SaveForPower(root);

    // Save for hardware
    SaveForHardware(root);

    // Save for software
    SaveForSoftware(root);

    Json::StreamWriterBuilder swb;
    std::ofstream ofs;
    ofs.open(BATTERY_STATS_JSON);
    if (!ofs.is_open()) {
        STATS_HILOGE(COMP_SVC, "Opening json file failed");
        return false;
    }
    swb.newStreamWriter()->write(root, &ofs);
    ofs.close();
    return true;
}

bool BatteryStatsCore::LoadBatteryStatsData()
{
    Json::CharReaderBuilder reader;
    Json::Value root;
    std::string errors;
    std::ifstream ifs(BATTERY_STATS_JSON, std::ios::binary);
    if (!ifs.is_open()) {
        STATS_HILOGE(COMP_SVC, "Json file doesn't exist");
        return false;
    }
    if (!parseFromStream(reader, ifs, &root, &errors)) {
        STATS_HILOGE(COMP_SVC, "Parsing json file failed");
        return false;
    }
    ifs.close();
    BatteryStatsEntity::ResetStatsEntity();
    Json::Value::Members member = root["Power"].getMemberNames();
    std::map<int32_t, double> tmpUserPowerMap;
    for (auto iter = member.begin(); iter != member.end(); iter++) {
        auto id = std::stoi(*iter);
        std::shared_ptr<BatteryStatsInfo> info = std::make_shared<BatteryStatsInfo>();
        if (id > StatsUtils::INVALID_VALUE) {
            info->SetUid(id);
            info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
            info->SetPower(root["Power"][*iter].asDouble());
            int32_t usr = AccountSA::OhosAccountKits::GetInstance().GetDeviceAccountIdByUID(id);
            auto iter = tmpUserPowerMap.find(usr);
            if (iter != tmpUserPowerMap.end()) {
                iter->second += info->GetPower();
            } else {
                tmpUserPowerMap.insert(std::pair<int32_t, double>(usr, info->GetPower()));
            }
            STATS_HILOGI(COMP_SVC, "Update power: %{public}lfmAh, user: %{public}d", info->GetPower(), usr);
        } else if (id < StatsUtils::INVALID_VALUE && id > BatteryStatsInfo::CONSUMPTION_TYPE_INVALID) {
            info->SetUid(StatsUtils::INVALID_VALUE);
            info->SetConsumptioType(static_cast<BatteryStatsInfo::ConsumptionType>(id));
            info->SetPower(root["Power"][*iter].asDouble());
        }
        STATS_HILOGD(COMP_SVC, "Load power: %{public}lfmAh for id: %{public}d", info->GetPower(), id);
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
    STATS_HILOGD(COMP_SVC, "Reset");
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
}
} // namespace PowerMgr
} // namespace OHOS