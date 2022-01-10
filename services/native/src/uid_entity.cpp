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

#include "uid_entity.h"
#include "battery_stats_parser.h"
#include "battery_stats_service.h"

#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

UidEntity::UidEntity(int32_t uid)
{
    if (uid > BatteryStatsUtils::INVALID_VALUE) {
        uid_ = uid;
        SetType(BatteryStatsInfo::STATS_TYPE_APP);
    }
}

int32_t UidEntity::GetUid()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got uid: %{public}d", uid_);
    return uid_;
}

void UidEntity::Calculate()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto totalPower = CalculateCameraPower() + CalculateFlashlightPower() +
        CalculateCpuPower() + CalculateRunningLockPower() + CalculateMobileRadioPower() + CalculateWifiPower() +
        CalculateGpsPower() + CalculateSensorPower();
    SetPower(totalPower);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate total power: %{public}lf for uid: %{public}d", totalPower, uid_);
}

double UidEntity::CalculateCameraPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto cameraOnAverage = statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_CAMERA_ON);
    auto cameraOnTime = statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_CAMERA_ON, uid_);
    auto cameraOnPower = cameraOnAverage * cameraOnTime;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate camera time: %{public}ld for uid: %{public}d", cameraOnTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate camera average: %{public}lf for uid: %{public}d",
        cameraOnAverage, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate camera power: %{public}lf for uid: %{public}d",
        cameraOnPower, uid_);
    return cameraOnPower;
}

double UidEntity::CalculateFlashlightPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto flashlightOnAverage =
        statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_FLASHLIGHT_ON);
    auto flashlightOnTime =
        statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_FLASHLIGHT_ON, uid_);
    auto flashlightOnPower = flashlightOnAverage * flashlightOnTime;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate flashlight time: %{public}ld for uid: %{public}d",
        flashlightOnTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate flashlight average: %{public}lf for uid: %{public}d",
        flashlightOnAverage, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate flashlight power: %{public}lf for uid: %{public}d",
        flashlightOnPower, uid_);
    return flashlightOnPower;
}

double UidEntity::CalculateCpuPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double cpuPower = BatteryStatsUtils::DEFAULT_VALUE;
    // TO-DO, using new CPU stats functions
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got cpu power: %{public}lf", cpuPower);
    return cpuPower;
}

double UidEntity::CalculateRunningLockPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto runningLockAverage =
        statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_CPU_AWAKE);
    auto runningLockTime =
        statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_CPU_AWAKE, uid_);
    auto runningLockPower = runningLockAverage * runningLockTime;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate runninglock time: %{public}ld for uid: %{public}d",
        runningLockTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate runninglock average: %{public}lf for uid: %{public}d",
        runningLockAverage, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate runninglock power: %{public}lf for uid: %{public}d",
        runningLockPower, uid_);
    return runningLockPower;
}

double UidEntity::CalculateMobileRadioPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Calculate radio transfer power
    auto radioRxAverage = statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_RADIO_RX);
    auto radioRxTime = statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_RADIO_RX, uid_);
    auto radioRxPower = radioRxAverage * radioRxTime;

    auto radioTxAverage = statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_RADIO_TX);
    auto radioTxTime = statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_RADIO_TX, uid_);
    auto radioTxPower = radioTxAverage * radioTxTime;

    // Calculate radio total power
    auto mobileRadioPower = radioRxPower + radioTxPower;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio rx time: %{public}ld for uid: %{public}d", radioRxTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio rx average: %{public}lf for uid: %{public}d", radioRxAverage,
        uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio rx power: %{public}lf for uid: %{public}d",
        radioRxPower, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio tx time: %{public}ld for uid: %{public}d", radioTxTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio tx average: %{public}lf for uid: %{public}d", radioTxAverage,
        uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio tx power: %{public}lf for uid: %{public}d",
        radioTxPower, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio total power: %{public}lf for uid: %{public}d",
        mobileRadioPower, uid_);
    return mobileRadioPower;
}

double UidEntity::CalculateWifiPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Calculate wifi transfer power
    auto wifiRxAverage = statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_WIFI_RX);
    auto wifiRxTime = statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_WIFI_RX, uid_);
    auto wifiRxPower = wifiRxAverage * wifiRxTime;

    auto wifiTxAverage = statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_WIFI_TX);
    auto wifiTxTime = statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_WIFI_TX, uid_);
    auto wifiTxPower = wifiTxAverage * wifiTxTime;

    // Calculate wifi scan power
    auto wifiScanAverage = statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_WIFI_SCAN);
    auto wifiScanTime = statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_WIFI_SCAN, uid_);
    auto wifiScanPower = wifiScanAverage * wifiScanTime;

    // Calculate wifi total power
    auto wifiPower = wifiRxPower + wifiTxPower + wifiScanPower;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi scan time: %{public}ld for uid: %{public}d",
        wifiScanTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi scan average: %{public}lf for uid: %{public}d", wifiScanAverage,
        uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi scan power: %{public}lf for uid: %{public}d", wifiScanPower,
        uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi rx time: %{public}ld for uid: %{public}d", wifiRxTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi rx average: %{public}lf for uid: %{public}d", wifiRxAverage,
        uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi rx power: %{public}lf for uid: %{public}d", wifiRxPower, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi tx time: %{public}ld for uid: %{public}d", wifiTxTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi tx average: %{public}lf for uid: %{public}d", wifiTxAverage,
        uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi tx power: %{public}lf for uid: %{public}d", wifiTxPower, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi total power: %{public}lf for uid: %{public}d", wifiPower, uid_);
    return wifiPower;
}

double UidEntity::CalculateBluetoothPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Calculate bluetooth transfer power
    auto bluetoothRxAverage =
        statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_BLUETOOTH_RX);
    auto bluetoothRxTime =
        statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_BLUETOOTH_RX, uid_);
    auto bluetoothRxPower = bluetoothRxAverage * bluetoothRxTime;

    auto bluetoothTxAverage =
        statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_BLUETOOTH_TX);
    auto bluetoothTxTime =
        statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_BLUETOOTH_TX, uid_);
    auto bluetoothTxPower = bluetoothTxAverage * bluetoothTxTime;

    // Calculate bluetooth scan power
    auto bluetoothScanAverage =
        statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_BLUETOOTH_SCAN);
    auto bluetoothScanTime =
        statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_BLUETOOTH_SCAN, uid_);
    auto bluetoothScanPower = bluetoothScanAverage * bluetoothScanTime;

    // Calculate bluetooth total power
    auto bluetoothPower = bluetoothRxPower + bluetoothTxPower + bluetoothScanPower;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth scan time: %{public}ld for uid: %{public}d",
        bluetoothScanTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth scan average: %{public}lf for uid: %{public}d",
        bluetoothScanAverage,uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth scan power: %{public}lf for uid: %{public}d",
        bluetoothScanPower, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth rx time: %{public}ld for uid: %{public}d",
        bluetoothRxTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth rx average: %{public}lf for uid: %{public}d",
        bluetoothRxAverage, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth rx power: %{public}lf for uid: %{public}d",
        bluetoothRxPower, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth tx time: %{public}ld for uid: %{public}d",
        bluetoothTxTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth tx average: %{public}lf for uid: %{public}d",
        bluetoothTxAverage, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth tx power: %{public}lf for uid: %{public}d",
        bluetoothTxPower, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth total power: %{public}lf for uid: %{public}d",
        bluetoothPower, uid_);
    return bluetoothPower;
}

double UidEntity::CalculateGpsPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto gpsOnAverage = statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_GPS_ON);
    auto gpsOnTime = statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_GPS_ON, uid_);
    auto gpsOnPower = gpsOnAverage * gpsOnTime;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate gps time: %{public}ld for uid: %{public}d", gpsOnTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate gps average: %{public}lf for uid: %{public}d", gpsOnAverage, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate gps power: %{public}lf for uid: %{public}d", gpsOnPower, uid_);
    return gpsOnPower;
}

double UidEntity::CalculateSensorPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double totalSensorPower = BatteryStatsUtils::DEFAULT_VALUE;
    // Calculate gravity sensor power
    auto gravityOnAverage =
        statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_SENSOR_GRAVITY);
    auto gravityOnTime =
        statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_SENSOR_GRAVITY, uid_);
    auto gravityOnPower = gravityOnAverage * gravityOnTime;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate gravity time: %{public}ld for uid: %{public}d", gravityOnTime, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate gravity average: %{public}lf for uid: %{public}d", gravityOnAverage,
        uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate gravity power: %{public}lf for uid: %{public}d", gravityOnPower,
        uid_);

    // Calculate proximity sensor power
    auto proximityOnAverage =
        statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_SENSOR_PROXIMITY);
    auto proximityOnTime =
        statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_SENSOR_PROXIMITY, uid_);
    auto proximityOnPower = proximityOnAverage * proximityOnTime;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate proximity time: %{public}ld for uid: %{public}d", proximityOnTime,
        uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate proximity average: %{public}lf for uid: %{public}d",
        proximityOnAverage, uid_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate proximity power: %{public}lf for uid: %{public}d", proximityOnPower,
        uid_);

    // New sensors calculation can be added below.
    // Calculate total sensor power
    totalSensorPower = gravityOnPower + proximityOnPower;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate sensor power: %{public}lf for uid: %{public}d", totalSensorPower,
        uid_);
    return totalSensorPower;
}
} // namespace PowerMgr
} // namespace OHOS