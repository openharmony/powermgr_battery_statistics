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

#include "entities/idle_entity.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

IdleEntity::IdleEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_IDLE;
}

long IdleEntity::GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long activeTimeMs = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_PHONE_IDLE) {
        activeTimeMs = StatsHelper::GetOnBatteryUpTimeMs();
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got cpu idle time: %{public}ldms", activeTimeMs);
    } else if (statsType == StatsUtils::STATS_TYPE_CPU_SUSPEND) {
        activeTimeMs = StatsHelper::GetOnBatteryBootTimeMs();
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got cpu suspend time: %{public}ldms", activeTimeMs);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return activeTimeMs;
}

void IdleEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto cpuSuspendPower = CalculateCpuSuspendPower();
    auto cpuIdlePower = CalculateCpuIdlePower();
    idleTotalPowerMah_ = cpuSuspendPower + cpuIdlePower;
    totalPowerMah_ += idleTotalPowerMah_;
    std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
    statsInfo->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_IDLE);
    statsInfo->SetPower(idleTotalPowerMah_);
    statsInfoList_.push_back(statsInfo);

    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate idle total power consumption: %{public}lfmAh", idleTotalPowerMah_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double IdleEntity::CalculateCpuSuspendPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto cpuSuspendAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_CPU_SUSPEND);
    auto bootOnBatteryTimeMs = GetActiveTimeMs(StatsUtils::STATS_TYPE_CPU_SUSPEND);
    auto cpuSuspendPowerMah = cpuSuspendAverageMa * bootOnBatteryTimeMs / StatsUtils::MS_IN_HOUR;
    cpuSuspendPowerMah_ = cpuSuspendPowerMah;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu suspend average power: %{public}lfma", cpuSuspendAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate boot on battery time: %{public}ldms", bootOnBatteryTimeMs);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu suspend power consumption: %{public}lfmAh", cpuSuspendPowerMah);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return cpuSuspendPowerMah_;
}

double IdleEntity::CalculateCpuIdlePower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto cpuIdleAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_CPU_IDLE);
    auto upOnBatteryTimeMs = GetActiveTimeMs(StatsUtils::STATS_TYPE_PHONE_IDLE);
    auto cpuIdlePowerMah = cpuIdleAverageMa * upOnBatteryTimeMs / StatsUtils::MS_IN_HOUR;
    cpuIdlePowerMah_ = cpuIdlePowerMah;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu idle average power: %{public}lfma", cpuIdleAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate up on battery time: %{public}ldms", upOnBatteryTimeMs);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu idle power consumption: %{public}lfmAh", cpuIdlePowerMah);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return cpuIdlePowerMah_;
}

double IdleEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got app idle power consumption: %{public}lfmAh", idleTotalPowerMah_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return idleTotalPowerMah_;
}

double IdleEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_PHONE_IDLE) {
        power = cpuIdlePowerMah_;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got cpu idle power consumption: %{public}lfmAh", power);
    } else if (statsType == StatsUtils::STATS_TYPE_CPU_SUSPEND) {
        power = cpuSuspendPowerMah_;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got cpu suspend power consumption: %{public}lfmAh", power);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

void IdleEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset Idle total power consumption
    idleTotalPowerMah_ = StatsUtils::DEFAULT_VALUE;

    // Reset cpu idle power consumption
    cpuIdlePowerMah_ = StatsUtils::DEFAULT_VALUE;

    // Reset cpu suspend power consumption
    cpuSuspendPowerMah_ = StatsUtils::DEFAULT_VALUE;

    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void IdleEntity::DumpInfo(std::string& result, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long upTime = GetActiveTimeMs(StatsUtils::STATS_TYPE_PHONE_IDLE);
    long bootTime = GetActiveTimeMs(StatsUtils::STATS_TYPE_CPU_SUSPEND);
    result.append("Idle dump:\n")
        .append("Phone boot time: ")
        .append(ToString(bootTime))
        .append("ms")
        .append("\n")
        .append("Phone up time: ")
        .append(ToString(upTime))
        .append("ms")
        .append("\n");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS