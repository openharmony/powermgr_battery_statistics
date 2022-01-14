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

#ifndef BATTERY_STATS_INFO_H
#define BATTERY_STATS_INFO_H

#include <list>
#include <memory>
#include <parcel.h>
#include <string>

#include "battery_stats_info.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsInfo : Parcelable {
public:
    enum ConsumptionType {
        CONSUMPTION_TYPE_INVALID = -17,
        CONSUMPTION_TYPE_APP,
        CONSUMPTION_TYPE_BLUETOOTH,
        CONSUMPTION_TYPE_IDLE,
        CONSUMPTION_TYPE_PHONE,
        CONSUMPTION_TYPE_RADIO,
        CONSUMPTION_TYPE_SCREEN,
        CONSUMPTION_TYPE_USER,
        CONSUMPTION_TYPE_WIFI,
        CONSUMPTION_TYPE_CAMERA,
        CONSUMPTION_TYPE_FLASHLIGHT,
        CONSUMPTION_TYPE_AUDIO,
        CONSUMPTION_TYPE_SENSOR,
        CONSUMPTION_TYPE_GPS,
        CONSUMPTION_TYPE_CPU,
        CONSUMPTION_TYPE_WAKELOCK
    };

    bool Marshalling(Parcel &parcel) const override;
    static std::shared_ptr<BatteryStatsInfo> Unmarshalling(Parcel &parcel);
    bool ReadFromParcel(Parcel &parcel);
    void SetUid(int32_t uid);
    void SetUserId(int32_t userId);
    void SetConsumptioType(ConsumptionType type);
    void SetPower(double power, StatsUtils::StatsType type = StatsUtils::STATS_TYPE_INVALID);
    void SetTime(long time, StatsUtils::StatsType type);
    void SetTraffic(long data, StatsUtils::StatsType type);
    int32_t GetUid();
    int32_t GetUserId();
    ConsumptionType GetConsumptionType();
    double GetPower(StatsUtils::StatsType type = StatsUtils::STATS_TYPE_INVALID);
    long GetTime(StatsUtils::StatsType type);
    long GetTraffic(StatsUtils::StatsType type);
    static ConsumptionType ConvertToConsumptionType(std::string type);
    static std::string ConvertConsumptionType(ConsumptionType type);
private:
    int32_t uid_ = StatsUtils::INVALID_VALUE;
    int32_t userId_ = StatsUtils::INVALID_VALUE;
    ConsumptionType type_ = CONSUMPTION_TYPE_INVALID;
    static const std::map<std::string, ConsumptionType> statsTypeMap_;

    // Power
    double totalPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double audioPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double cameraPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double flashlightPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double gpsPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double gravityPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double proximityPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double bluetoothScanPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double bluetoothRxPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double bluetoothTxPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double wifiScanPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double wifiRxPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double wifiTxPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double radioRxPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double radioTxPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double wakelockPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double cpuClustersPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double cpuSpeedsPowerMah_ = StatsUtils::DEFAULT_VALUE;

    // Time
    long audioTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long cameraTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long flashlightTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long gpsTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long gravityTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long proximityTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long bluetoothOnTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long bluetoothRxTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long bluetoothTxTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long bluetoothScanTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long wifiOnTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long wifiRxTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long wifiTxTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long wifiScanTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long radioOnTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long radioRxTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long radioTxTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long radioScanTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long screenOnTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long wakelockTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long cpuIdleTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long cpuClustersTimeMs_ = StatsUtils::DEFAULT_VALUE;
    long cpuSpeedsTimeMs_ = StatsUtils::DEFAULT_VALUE;

    // Traffic
    long bluetoothRxBytes = StatsUtils::DEFAULT_VALUE;
    long bluetoothTxBytes = StatsUtils::DEFAULT_VALUE;
    long wifiRxBytes = StatsUtils::DEFAULT_VALUE;
    long wifiTxBytes = StatsUtils::DEFAULT_VALUE;
    long radioRxBytes = StatsUtils::DEFAULT_VALUE;
    long radioTxBytes = StatsUtils::DEFAULT_VALUE;
};
using BatteryStatsInfoList = std::list<std::shared_ptr<BatteryStatsInfo>>;
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_INFO_H