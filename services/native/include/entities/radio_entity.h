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

#ifndef RADIO_ENTITY_H
#define RADIO_ENTITY_H

#include <map>

#include "entities/battery_stats_entity.h"

namespace OHOS {
namespace PowerMgr {
class RadioEntity : public BatteryStatsEntity {
public:
    RadioEntity();
    ~RadioEntity() = default;
    void Calculate(int32_t uid = StatsUtils::INVALID_VALUE) override;
    long GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level = StatsUtils::INVALID_VALUE) override;
    long GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType,
        int16_t level = StatsUtils::INVALID_VALUE) override;
    double GetEntityPowerMah(int32_t uidOrUserId = StatsUtils::INVALID_VALUE) override;
    double GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid = StatsUtils::INVALID_VALUE) override;
    void Reset() override;
    void DumpInfo(std::string& result, int32_t uid = StatsUtils::INVALID_VALUE) override;
    long GetTrafficByte(StatsUtils::StatsType statsType, int32_t uid = StatsUtils::INVALID_VALUE) override;
    std::shared_ptr<StatsHelper::ActiveTimer> GetOrCreateTimer(StatsUtils::StatsType statsType,
        int16_t level = StatsUtils::INVALID_VALUE) override;
    std::shared_ptr<StatsHelper::ActiveTimer> GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
        int16_t level = StatsUtils::INVALID_VALUE) override;
    std::shared_ptr<StatsHelper::Counter> GetOrCreateCounter(StatsUtils::StatsType statsType,
        int32_t uid = StatsUtils::INVALID_VALUE) override;
private:
    double radioPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double radioOnPowerMah_ = StatsUtils::DEFAULT_VALUE;
    double radioScanPowerMah_ = StatsUtils::DEFAULT_VALUE;
    std::map<int32_t, double> appRadioPowerMap_;
    std::map<int32_t, double> appRadioRxPowerMap_;
    std::map<int32_t, double> appRadioTxPowerMap_;
    std::map<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>> radioOnTimerMap_;
    std::shared_ptr<StatsHelper::ActiveTimer> radioScanTimer_;
    std::map<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>> appRadioRxTimerMap_;
    std::map<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>> appRadioTxTimerMap_;
    std::map<int32_t, std::shared_ptr<StatsHelper::Counter>> appRadioRxCounterMap_;
    std::map<int32_t, std::shared_ptr<StatsHelper::Counter>> appRadioTxCounterMap_;
    void CalculateRadioPower();
    void CalculateRadioPowerForApp(int32_t uid);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // RADIO_ENTITY_H