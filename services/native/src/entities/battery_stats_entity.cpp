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

#include "entities/battery_stats_entity.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
double BatteryStatsEntity::totalPowerMah_ = StatsUtils::DEFAULT_VALUE;
BatteryStatsInfoList BatteryStatsEntity::statsInfoList_;

void BatteryStatsEntity::AggregateUserPowerMah(int32_t userId, double power)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to add app power to related user");
}

void BatteryStatsEntity::Calculate(int32_t uid)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to calculate");
}

long BatteryStatsEntity::GetTrafficByte(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to get traffic");
    return StatsUtils::DEFAULT_VALUE;
}

BatteryStatsInfoList BatteryStatsEntity::GetStatsInfoList()
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "Get stats info list");
    return statsInfoList_;
}

void BatteryStatsEntity::UpdateStatsInfoList(std::shared_ptr<BatteryStatsInfo> info)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "Enter");
    statsInfoList_.push_back(info);
    STATS_HILOGE(STATS_MODULE_SERVICE, "Exit");
}

long BatteryStatsEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to get active time, return 0");
    return StatsUtils::DEFAULT_VALUE;
}

long BatteryStatsEntity::GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to get active time, return 0");
    return StatsUtils::DEFAULT_VALUE;
}

void BatteryStatsEntity::DumpInfo(std::string& result, int32_t uid)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to dump");
}

void BatteryStatsEntity::UpdateUidMap(int32_t uid)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to update uid");
}

long BatteryStatsEntity::GetCpuTimeMs(int32_t uid)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to get cpu time, return 0");
    return StatsUtils::DEFAULT_VALUE;
}

void BatteryStatsEntity::UpdateCpuTime()
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to update cpu time");
}

double BatteryStatsEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to get stats power, return 0");
    return StatsUtils::DEFAULT_VALUE;
}

std::shared_ptr<StatsHelper::ActiveTimer> BatteryStatsEntity::GetOrCreateTimer(StatsUtils::StatsType statsType,
    int16_t level)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to get or create timer, return nullptr");
    return nullptr;
}

std::shared_ptr<StatsHelper::ActiveTimer> BatteryStatsEntity::GetOrCreateTimer(int32_t uid,
    StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to get or create timer, return nullptr");
    return nullptr;
}

std::shared_ptr<StatsHelper::Counter> BatteryStatsEntity::GetOrCreateCounter(StatsUtils::StatsType statsType,
    int32_t uid)
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "No need to get or create counter, return nullptr");
    return nullptr;
}

BatteryStatsInfo::ConsumptionType BatteryStatsEntity::GetConsumptionType()
{
    std::string typeStr = BatteryStatsInfo::ConvertConsumptionType(consumptionType_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got %{public}s: %{public}d", typeStr.c_str(), consumptionType_);
    return consumptionType_;
}

double BatteryStatsEntity::GetTotalPowerMah()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got total consumption power = %{public}lfMaH", totalPowerMah_);
    return totalPowerMah_;
}

void BatteryStatsEntity::ResetStatsEntity()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Reset total consumption power value to 0");
    totalPowerMah_ = StatsUtils::DEFAULT_VALUE;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Reset stats power list to 0");
    statsInfoList_.clear();
}
} // namespace PowerMgr
} // namespace OHOS