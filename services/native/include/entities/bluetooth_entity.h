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

#ifndef BLUETOOTH_ENTITY_H
#define BLUETOOTH_ENTITY_H

#include <map>

#include "entities/battery_stats_entity.h"

namespace OHOS {
namespace PowerMgr {
class BluetoothEntity : public BatteryStatsEntity {
public:
    enum PowerType {
        POWER_TYPE_BR = 0,
        POWER_TYPE_BLE,
        POWER_TYPE_ALL,
    };
    BluetoothEntity();
    ~BluetoothEntity() = default;
    void Calculate(int32_t uid = StatsUtils::INVALID_VALUE) override;
    int64_t GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level = StatsUtils::INVALID_VALUE) override;
    int64_t GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType,
        int16_t level = StatsUtils::INVALID_VALUE) override;
    double GetEntityPowerMah(int32_t uidOrUserId = StatsUtils::INVALID_VALUE) override;
    double GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid = StatsUtils::INVALID_VALUE) override;
    void Reset() override;
    void DumpInfo(std::string& result, int32_t uid = StatsUtils::INVALID_VALUE) override;
    std::shared_ptr<StatsHelper::ActiveTimer> GetOrCreateTimer(StatsUtils::StatsType statsType,
        int16_t level = StatsUtils::INVALID_VALUE) override;
    std::shared_ptr<StatsHelper::ActiveTimer> GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
        int16_t level = StatsUtils::INVALID_VALUE) override;

private:
    double GetBluetoothUidPower();
    void CalculateBtPower();
    void CalculateBtPowerForApp(int32_t uid);
    void UpdateAppBluetoothBlePower(PowerType type, int32_t uid, double powerMah);
    double bluetoothBrPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double bluetoothBlePowerMah_ = StatsUtils::DEFAULT_VALUE;
    double bluetoothPowerMah_ = StatsUtils::DEFAULT_VALUE;
    std::map<int32_t, double> appBluetoothBrPowerMap_;
    std::map<int32_t, double> appBluetoothBlePowerMap_;
    std::map<int32_t, double> appBluetoothPowerMap_;
    std::shared_ptr<StatsHelper::ActiveTimer> bluetoothBrOnTimer_;
    std::shared_ptr<StatsHelper::ActiveTimer> bluetoothBleOnTimer_;
    std::map<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>> appBluetoothBrScanTimerMap_;
    std::map<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>> appBluetoothBleScanTimerMap_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BLUETOOTH_ENTITY_H