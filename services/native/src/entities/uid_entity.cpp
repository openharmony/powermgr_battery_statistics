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

#include "entities/uid_entity.h"

#include <ohos_account_kits_impl.h>

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

UidEntity::UidEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_APP;
}

void UidEntity::UpdateUidMap(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (uid > StatsUtils::INVALID_VALUE) {
        auto iter = uidPowerMap_.find(uid);
        if (iter != uidPowerMap_.end()) {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Uid has already been added, ignore");
        } else {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Update %{public}d to uid power map", uid);
            uidPowerMap_.insert(std::pair<int32_t, long>(uid, StatsUtils::DEFAULT_VALUE));
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void UidEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto core = statsService->GetBatteryStatsCore();
    auto bluetoothEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    auto radioEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    auto userEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_USER);
    auto wifiEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    auto cameraEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA);
    auto flashlightEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT);
    auto audioEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO);
    auto sensorEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR);
    auto gpsEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_GPS);
    auto cpuEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CPU);
    auto wakelockEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK);
    for (auto& iter : uidPowerMap_) {
        double power = StatsUtils::DEFAULT_VALUE;
        // Calculate bluetooth power consumption
        bluetoothEntity->Calculate(iter.first);
        power += bluetoothEntity->GetEntityPowerMah(iter.first);
        // Calculate radio power consumption
        radioEntity->Calculate(iter.first);
        power += radioEntity->GetEntityPowerMah(iter.first);
        // Calculate wifi power consumption
        wifiEntity->Calculate(iter.first);
        power += wifiEntity->GetEntityPowerMah(iter.first);
        // Calculate camera power consumption
        cameraEntity->Calculate(iter.first);
        power += cameraEntity->GetEntityPowerMah(iter.first);
        // Calculate flashlight power consumption
        flashlightEntity->Calculate(iter.first);
        power += flashlightEntity->GetEntityPowerMah(iter.first);
        // Calculate audio power consumption
        audioEntity->Calculate(iter.first);
        power += audioEntity->GetEntityPowerMah(iter.first);
        // Calculate sensor power consumption
        sensorEntity->Calculate(iter.first);
        power += sensorEntity->GetEntityPowerMah(iter.first);
        // Calculate gps power consumption
        gpsEntity->Calculate(iter.first);
        power += gpsEntity->GetEntityPowerMah(iter.first);
        // Calculate cpu power consumption
        cpuEntity->Calculate(iter.first);
        power += cpuEntity->GetEntityPowerMah(iter.first);
        // Calculate cpu power consumption
        wakelockEntity->Calculate(iter.first);
        power += wakelockEntity->GetEntityPowerMah(iter.first);
        iter.second = power;
        totalPowerMah_ += power;
        std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
        statsInfo->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
        statsInfo->SetUid(iter.first);
        statsInfo->SetPower(iter.second);
        statsInfo->SetTime(bluetoothEntity->GetActiveTimeMs(iter.first, StatsUtils::STATS_TYPE_BLUETOOTH_SCAN),
            StatsUtils::STATS_TYPE_BLUETOOTH_SCAN);
        statsInfo->SetTraffic(bluetoothEntity->GetTrafficByte(StatsUtils::STATS_TYPE_BLUETOOTH_RX, iter.first),
            StatsUtils::STATS_TYPE_BLUETOOTH_RX);
        // TO-DO, set other stats info(time/traffic)
        statsInfoList_.push_back(statsInfo);
        STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate uid power consumption: %{public}lfmAh for uid: %{public}d",
            power, iter.first);
        int32_t tmpUid = iter.first;
        int32_t userId = AccountSA::OhosAccountKits::GetInstance().GetDeviceAccountIdByUID(tmpUid);
        if (userEntity != nullptr) {
            userEntity->AggregateUserPowerMah(userId, power);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double UidEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = uidPowerMap_.find(uidOrUserId);
    if (iter != uidPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got app uid power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE,
            "No app uid power consumption related with uid: %{public}d found, return 0", uidOrUserId);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

double UidEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    auto core = statsService->GetBatteryStatsCore();
    auto bluetoothEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    auto radioEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    auto userEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_USER);
    auto wifiEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    auto cameraEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA);
    auto flashlightEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT);
    auto audioEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO);
    auto sensorEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR);
    auto gpsEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_GPS);
    auto cpuEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CPU);
    auto wakelockEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK);

    if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_SCAN) {
        power = bluetoothEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_BLUETOOTH_SCAN, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_RX) {
        power = bluetoothEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_BLUETOOTH_RX, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_BLUETOOTH_TX) {
        power = bluetoothEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_BLUETOOTH_TX, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_SCAN) {
        power = wifiEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_WIFI_SCAN, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_RX) {
        power = wifiEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_WIFI_RX, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_WIFI_TX) {
        power = wifiEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_WIFI_TX, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_RADIO_RX) {
        power = radioEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_RADIO_RX, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_RADIO_TX) {
        power = radioEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_RADIO_TX, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_CAMERA_ON) {
        power = cameraEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CAMERA_ON, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_FLASHLIGHT_ON) {
        power = flashlightEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_FLASHLIGHT_ON, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_GPS_ON) {
        power = gpsEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_GPS_ON, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON) {
        power = sensorEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON) {
        power = sensorEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_AUDIO_ON) {
        power = audioEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_AUDIO_ON, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_WAKELOCK_HOLD) {
        power = wakelockEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_WAKELOCK_HOLD, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_CPU_CLUSTER) {
        power = cpuEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CPU_CLUSTER, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_CPU_SPEED) {
        power = cpuEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CPU_SPEED, uid);
    } else if (statsType == StatsUtils::STATS_TYPE_CPU_ACTIVE) {
        power = cpuEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_CPU_ACTIVE, uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

void UidEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset app Uid total power consumption
    for (auto &iter : uidPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void UidEntity::DumpInfo(std::string& result)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto core = statsService->GetBatteryStatsCore();
    for (auto &iter : uidPowerMap_) {
        // Dump for bluetooth realted info
        long bluetoothScanTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_BLUETOOTH_SCAN);
        long bluetoothRxTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_BLUETOOTH_RX);
        long bluetoothTxTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_BLUETOOTH_TX);
        long bluetoothRxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_BLUETOOTH_RX, iter.first);
        long bluetoothTxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_BLUETOOTH_TX, iter.first);

        // Dump for wifi realted info
        long wifiScanTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_WIFI_SCAN);
        long wifiRxTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_WIFI_RX);
        long wifiTxTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_WIFI_TX);
        long wifiRxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_WIFI_RX, iter.first);
        long wifiTxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_WIFI_TX, iter.first);

        // Dump for radio realted info
        long radioRxTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_RADIO_RX);
        long radioTxTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_RADIO_TX);
        long radioRxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_RADIO_RX, iter.first);
        long radioTxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_RADIO_TX, iter.first);

        // Dump for camera related info
        long cameraTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_CAMERA_ON);

        // Dump for flashlight related info
        long flashlightTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_FLASHLIGHT_ON);

        // Dump for gps related info
        long gpsTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_GPS_ON);

        // Dump for gravity sensor related info
        long gravityTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON);

        // Dump for proximity sensor related info
        long proximityTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON);

        // Dump for audio related info
        long audioTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_AUDIO_ON);

        // Dump for wakelock related info
        long wakelockTime = core->GetTotalTimeMs(iter.first, StatsUtils::STATS_TYPE_WAKELOCK_HOLD);

        result.append(ToString(iter.first))
            .append(":")
            .append("\n")
            .append("Bluetooth scan time: ")
            .append(ToString(bluetoothScanTime))
            .append("ms")
            .append("\n")
            .append("Bluetooth RX time: ")
            .append(ToString(bluetoothRxTime))
            .append("ms")
            .append("\n")
            .append("Bluetooth TX time: ")
            .append(ToString(bluetoothTxTime))
            .append("ms")
            .append("\n")
            .append("Bluetooth RX data: ")
            .append(ToString(bluetoothRxData))
            .append("bytes")
            .append("\n")
            .append("Bluetooth TX data: ")
            .append(ToString(bluetoothTxData))
            .append("bytes")
            .append("\n")
            .append("Bluetooth scan time: ")
            .append(ToString(bluetoothScanTime))
            .append("ms")
            .append("\n")
            .append("Wifi scan time: ")
            .append(ToString(wifiScanTime))
            .append("ms")
            .append("\n")
            .append("Wifi RX time: ")
            .append(ToString(wifiRxTime))
            .append("ms")
            .append("\n")
            .append("Wifi TX time: ")
            .append(ToString(wifiTxTime))
            .append("ms")
            .append("\n")
            .append("Wifi RX data: ")
            .append(ToString(wifiRxData))
            .append("ms")
            .append("\n")
            .append("Wifi TX data: ")
            .append(ToString(wifiTxData))
            .append("ms")
            .append("\n")
            .append("Radio RX time: ")
            .append(ToString(radioRxTime))
            .append("ms")
            .append("\n")
            .append("Radio TX time: ")
            .append(ToString(radioTxTime))
            .append("ms")
            .append("\n")
            .append("Radio RX data: ")
            .append(ToString(radioRxData))
            .append("ms")
            .append("\n")
            .append("Radio TX data: ")
            .append(ToString(radioTxData))
            .append("ms")
            .append("\n")
            .append("Camera on time: ")
            .append(ToString(cameraTime))
            .append("ms")
            .append("\n")
            .append("Flashlight scan time: ")
            .append(ToString(flashlightTime))
            .append("ms")
            .append("\n")
            .append("GPS scan time: ")
            .append(ToString(gpsTime))
            .append("ms")
            .append("\n")
            .append("Gravity sensor on time: ")
            .append(ToString(gravityTime))
            .append("ms")
            .append("\n")
            .append("Proximity sensor on time: ")
            .append(ToString(proximityTime))
            .append("ms")
            .append("\n")
            .append("Audio on time: ")
            .append(ToString(audioTime))
            .append("ms")
            .append("\n")
            .append("Wakelock hold time: ")
            .append(ToString(wakelockTime))
            .append("ms")
            .append("\n");
            core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CPU)->DumpInfo(result);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS