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

#ifndef CPU_ENTITY_H
#define CPU_ENTITY_H

#include <map>

#include "cpu_time_reader.h"
#include "entities/battery_stats_entity.h"
#include "stats_helper.h"

namespace OHOS {
namespace PowerMgr {
class CpuEntity : public BatteryStatsEntity {
public:
    CpuEntity();
    ~CpuEntity() = default;
    void Calculate(int32_t uid = StatsUtils::INVALID_VALUE) override;
    double GetEntityPowerMah(int32_t uidOrUserId = StatsUtils::INVALID_VALUE) override;
    double GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid = StatsUtils::INVALID_VALUE) override;
    int64_t GetCpuTimeMs(int32_t uid) override;
    void Reset() override;
    void DumpInfo(std::string& result, int32_t uid = StatsUtils::INVALID_VALUE) override;
    void UpdateCpuTime() override;
private:
    std::shared_ptr<CpuTimeReader> cpuReader_;
    std::map<int32_t, int64_t> cpuTimeMap_;
    std::map<int32_t, double> cpuTotalPowerMap_;
    std::map<int32_t, double> cpuActivePowerMap_;
    std::map<int32_t, double> cpuClusterPowerMap_;
    std::map<int32_t, double> cpuSpeedPowerMap_;
    double CalculateCpuActivePower(int32_t uid);
    double CalculateCpuClusterPower(int32_t uid);
    double CalculateCpuSpeedPower(int32_t uid);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // CPU_ENTITY_H