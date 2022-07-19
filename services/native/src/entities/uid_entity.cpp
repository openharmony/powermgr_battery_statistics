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

#include "entities/uid_entity.h"

#include <bundle_constants.h>
#include <bundle_mgr_interface.h>
#include <ohos_account_kits_impl.h>
#include <system_ability_definition.h>
#include <sys_mgr_client.h>

#include "battery_stats_service.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

UidEntity::UidEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_APP;
}

void UidEntity::UpdateUidMap(int32_t uid)
{
    if (uid > StatsUtils::INVALID_VALUE) {
        auto iter = uidPowerMap_.find(uid);
        if (iter != uidPowerMap_.end()) {
            STATS_HILOGD(COMP_SVC, "Uid has already been added, ignore");
        } else {
            STATS_HILOGD(COMP_SVC, "Update %{public}d to uid power map", uid);
            uidPowerMap_.insert(std::pair<int32_t, double>(uid, StatsUtils::DEFAULT_VALUE));
        }
    }
}

std::vector<int32_t> UidEntity::GetUids()
{
    std::vector<int32_t> uids;
    for (auto& iter : uidPowerMap_) {
        uids.push_back(iter.first);
    }
    return uids;
}

double UidEntity::CalculateForConnectivity(int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto core = g_statsService->GetBatteryStatsCore();
    auto bluetoothEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    auto radioEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    auto wifiEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);

    // Calculate bluetooth power consumption
    bluetoothEntity->Calculate(uid);
    power += bluetoothEntity->GetEntityPowerMah(uid);
    // Calculate radio power consumption
    radioEntity->Calculate(uid);
    power += radioEntity->GetEntityPowerMah(uid);
    // Calculate wifi power consumption
    wifiEntity->Calculate(uid);
    power += wifiEntity->GetEntityPowerMah(uid);
    STATS_HILOGD(COMP_SVC, "Connectivity power consumption: %{public}lfmAh for uid: %{public}d", power, uid);
    return power;
}

double UidEntity::CalculateForCommon(int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto core = g_statsService->GetBatteryStatsCore();
    auto cameraEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA);
    auto flashlightEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT);
    auto audioEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO);
    auto sensorEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR);
    auto gpsEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_GPS);
    auto cpuEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CPU);
    auto wakelockEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK);
    auto alarmEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_ALARM);

    // Calculate camera power consumption
    cameraEntity->Calculate(uid);
    power += cameraEntity->GetEntityPowerMah(uid);
    // Calculate flashlight power consumption
    flashlightEntity->Calculate(uid);
    power += flashlightEntity->GetEntityPowerMah(uid);
    // Calculate audio power consumption
    audioEntity->Calculate(uid);
    power += audioEntity->GetEntityPowerMah(uid);
    // Calculate sensor power consumption
    sensorEntity->Calculate(uid);
    power += sensorEntity->GetEntityPowerMah(uid);
    // Calculate gps power consumption
    gpsEntity->Calculate(uid);
    power += gpsEntity->GetEntityPowerMah(uid);
    // Calculate cpu power consumption
    cpuEntity->Calculate(uid);
    power += cpuEntity->GetEntityPowerMah(uid);
    // Calculate cpu power consumption
    wakelockEntity->Calculate(uid);
    power += wakelockEntity->GetEntityPowerMah(uid);
    // Calculate alarm power consumption
    alarmEntity->Calculate(uid);
    power += alarmEntity->GetEntityPowerMah(uid);

    STATS_HILOGD(COMP_SVC, "Common power consumption: %{public}lfmAh for uid: %{public}d", power, uid);
    return power;
}

void UidEntity::Calculate(int32_t uid)
{
    auto core = g_statsService->GetBatteryStatsCore();
    auto userEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_USER);
    for (auto& iter : uidPowerMap_) {
        double power = StatsUtils::DEFAULT_VALUE;
        power += CalculateForConnectivity(iter.first);
        power += CalculateForCommon(iter.first);
        iter.second = power;
        totalPowerMah_ += power;
        AddtoStatsList(iter.first, power);
        int32_t uid = iter.first;
        int32_t userId = AccountSA::OhosAccountKits::GetInstance().GetDeviceAccountIdByUID(uid);
        if (userEntity != nullptr) {
            userEntity->AggregateUserPowerMah(userId, power);
        }
    }
}

void UidEntity::AddtoStatsList(int32_t uid, double power)
{
    std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
    statsInfo->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
    statsInfo->SetUid(uid);
    statsInfo->SetPower(power);
    statsInfoList_.push_back(statsInfo);
}

double UidEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = uidPowerMap_.find(uidOrUserId);
    if (iter != uidPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGD(COMP_SVC, "Get app uid power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGD(COMP_SVC,
            "No app uid power consumption related to uid: %{public}d was found, return 0", uidOrUserId);
    }
    return power;
}

double UidEntity::GetPowerForConnectivity(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto core = g_statsService->GetBatteryStatsCore();
    auto bluetoothEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    auto radioEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    auto wifiEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);

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
    }
    return power;
}

double UidEntity::GetPowerForCommon(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto core = g_statsService->GetBatteryStatsCore();
    auto cameraEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA);
    auto flashlightEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT);
    auto audioEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO);
    auto sensorEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR);
    auto gpsEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_GPS);
    auto cpuEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CPU);
    auto wakelockEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK);
    auto alarmEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_ALARM);

    if (statsType == StatsUtils::STATS_TYPE_CAMERA_ON) {
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
    } else if (statsType == StatsUtils::STATS_TYPE_ALARM) {
        power = alarmEntity->GetStatsPowerMah(StatsUtils::STATS_TYPE_ALARM, uid);
    }
    return power;
}

double UidEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;

    switch (statsType) {
        case StatsUtils::STATS_TYPE_BLUETOOTH_SCAN:
        case StatsUtils::STATS_TYPE_BLUETOOTH_RX:
        case StatsUtils::STATS_TYPE_BLUETOOTH_TX:
        case StatsUtils::STATS_TYPE_WIFI_SCAN:
        case StatsUtils::STATS_TYPE_WIFI_RX:
        case StatsUtils::STATS_TYPE_WIFI_TX:
        case StatsUtils::STATS_TYPE_RADIO_RX:
        case StatsUtils::STATS_TYPE_RADIO_TX:
            power = GetPowerForConnectivity(statsType, uid);
            break;
        case StatsUtils::STATS_TYPE_CAMERA_ON:
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON:
        case StatsUtils::STATS_TYPE_GPS_ON:
        case StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON:
        case StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON:
        case StatsUtils::STATS_TYPE_AUDIO_ON:
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD:
        case StatsUtils::STATS_TYPE_CPU_CLUSTER:
        case StatsUtils::STATS_TYPE_CPU_SPEED:
        case StatsUtils::STATS_TYPE_CPU_ACTIVE:
        case StatsUtils::STATS_TYPE_ALARM:
            power = GetPowerForCommon(statsType, uid);
            break;
        default:
            STATS_HILOGW(COMP_SVC, "Invalid or illegal type got, return 0");
            break;
    }

    STATS_HILOGD(COMP_SVC, "Get %{public}s power: %{public}lfmAh for uid: %{public}d",
        StatsUtils::ConvertStatsType(statsType).c_str(), power, uid);
    return power;
}

void UidEntity::Reset()
{
    // Reset app Uid total power consumption
    for (auto& iter : uidPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }
}

void UidEntity::DumpForBluetooth(int32_t uid, std::string& result)
{
    // Dump for bluetooth realted info
    auto core = g_statsService->GetBatteryStatsCore();
    int64_t bluetoothScanTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_BLUETOOTH_SCAN);
    int64_t bluetoothRxTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_BLUETOOTH_RX);
    int64_t bluetoothTxTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_BLUETOOTH_TX);
    int64_t bluetoothRxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_BLUETOOTH_RX, uid);
    int64_t bluetoothTxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_BLUETOOTH_TX, uid);

    result.append("Bluetooth scan time: ")
        .append(ToString(bluetoothScanTime))
        .append("ms\n")
        .append("Bluetooth RX time: ")
        .append(ToString(bluetoothRxTime))
        .append("ms\n")
        .append("Bluetooth TX time: ")
        .append(ToString(bluetoothTxTime))
        .append("ms\n")
        .append("Bluetooth RX data: ")
        .append(ToString(bluetoothRxData))
        .append("bytes\n")
        .append("Bluetooth TX data: ")
        .append(ToString(bluetoothTxData))
        .append("bytes\n")
        .append("Bluetooth scan time: ")
        .append(ToString(bluetoothScanTime))
        .append("ms\n");
}

void UidEntity::DumpForWifi(int32_t uid, std::string& result)
{
    // Dump for wifi realted info
    auto core = g_statsService->GetBatteryStatsCore();
    int64_t wifiScanTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_WIFI_SCAN);
    int64_t wifiRxTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_WIFI_RX);
    int64_t wifiTxTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_WIFI_TX);
    int64_t wifiRxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_WIFI_RX, uid);
    int64_t wifiTxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_WIFI_TX, uid);

    result.append("Wifi scan time: ")
        .append(ToString(wifiScanTime))
        .append("ms\n")
        .append("Wifi RX time: ")
        .append(ToString(wifiRxTime))
        .append("ms\n")
        .append("Wifi TX time: ")
        .append(ToString(wifiTxTime))
        .append("ms\n")
        .append("Wifi RX data: ")
        .append(ToString(wifiRxData))
        .append("bytes\n")
        .append("Wifi TX data: ")
        .append(ToString(wifiTxData))
        .append("bytes\n");
}

void UidEntity::DumpForRadio(int32_t uid, std::string& result)
{
    // Dump for radio realted info
    auto core = g_statsService->GetBatteryStatsCore();
    int64_t radioRxTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_RADIO_RX);
    int64_t radioTxTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_RADIO_TX);
    int64_t radioRxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_RADIO_RX, uid);
    int64_t radioTxData = core->GetTotalDataCount(StatsUtils::STATS_TYPE_RADIO_TX, uid);

    result.append("Radio RX time: ")
        .append(ToString(radioRxTime))
        .append("ms\n")
        .append("Radio TX time: ")
        .append(ToString(radioTxTime))
        .append("ms\n")
        .append("Radio RX data: ")
        .append(ToString(radioRxData))
        .append("bytes\n")
        .append("Radio TX data: ")
        .append(ToString(radioTxData))
        .append("ms\n");
}

void UidEntity::DumpForCommon(int32_t uid, std::string& result)
{
    auto core = g_statsService->GetBatteryStatsCore();
    // Dump for camera related info
    int64_t cameraTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_CAMERA_ON);

    // Dump for flashlight related info
    int64_t flashlightTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_FLASHLIGHT_ON);

    // Dump for gps related info
    int64_t gpsTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_GPS_ON);

    // Dump for gravity sensor related info
    int64_t gravityTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_SENSOR_GRAVITY_ON);

    // Dump for proximity sensor related info
    int64_t proximityTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_SENSOR_PROXIMITY_ON);

    // Dump for audio related info
    int64_t audioTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_AUDIO_ON);

    // Dump for wakelock related info
    int64_t wakelockTime = core->GetTotalTimeMs(uid, StatsUtils::STATS_TYPE_WAKELOCK_HOLD);

    // Dump for alarm related info
    int64_t alarmCount = core->GetTotalDataCount(StatsUtils::STATS_TYPE_ALARM, uid);

    result.append("Camera on time: ")
        .append(ToString(cameraTime))
        .append("ms\n")
        .append("Flashlight scan time: ")
        .append(ToString(flashlightTime))
        .append("ms\n")
        .append("GPS scan time: ")
        .append(ToString(gpsTime))
        .append("ms\n")
        .append("Gravity sensor on time: ")
        .append(ToString(gravityTime))
        .append("ms\n")
        .append("Proximity sensor on time: ")
        .append(ToString(proximityTime))
        .append("ms\n")
        .append("Audio on time: ")
        .append(ToString(audioTime))
        .append("ms\n")
        .append("Wakelock hold time: ")
        .append(ToString(wakelockTime))
        .append("ms\n")
        .append("Alarm trigger count: ")
        .append(ToString(alarmCount))
        .append("ms\n");
}

void UidEntity::DumpInfo(std::string& result, int32_t uid)
{
    auto core = g_statsService->GetBatteryStatsCore();
    for (auto& iter : uidPowerMap_) {
        std::string bundleName = "NULL";
        auto bundleObj =
            DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()
                ->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (bundleObj == nullptr) {
            STATS_HILOGE(COMP_SVC, "Failed to get bundle manager service");
        } else {
            sptr<AppExecFwk::IBundleMgr> bmgr = iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
            if (bmgr == nullptr) {
                STATS_HILOGE(COMP_SVC, "Failed to get bundle manager proxy");
            } else {
                bool res = bmgr->GetBundleNameForUid(iter.first, bundleName);
                if (!res) {
                    STATS_HILOGE(COMP_SVC, "Failed to get bundle name for uid: %{public}d", iter.first);
                }
            }
        }
        result.append("\n")
            .append(ToString(iter.first))
            .append("(Bundle name: ")
            .append(bundleName)
            .append(")")
            .append(":")
            .append("\n");
        DumpForBluetooth(iter.first, result);
        DumpForWifi(iter.first, result);
        DumpForRadio(iter.first, result);
        DumpForCommon(iter.first, result);
        auto cpuEntity = core->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_CPU);
        if (cpuEntity) {
            cpuEntity->DumpInfo(result, iter.first);
        }
    }
}
} // namespace PowerMgr
} // namespace OHOS