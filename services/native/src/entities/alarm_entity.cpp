/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "entities/alarm_entity.h"

#include <cinttypes>

#include "battery_stats_service.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

AlarmEntity::AlarmEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_ALARM;
}

int64_t AlarmEntity::GetConsumptionCount(StatsUtils::StatsType statsType, int32_t uid)
{
    int64_t count = StatsUtils::DEFAULT_VALUE;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_ALARM: {
            auto almIter = alarmCounterMap_.find(uid);
            if (almIter != alarmCounterMap_.end()) {
                count = almIter->second->GetCount();
                STATS_HILOGD(COMP_SVC, "Get alarm count: %{public}" PRId64 " for uid: %{public}d", count, uid);
                break;
            }
            STATS_HILOGD(COMP_SVC, "No alarm count related to uid: %{public}d was found, return 0", uid);
            break;
        }
        default:
            break;
    }
    return count;
}

void AlarmEntity::Calculate(int32_t uid)
{
    auto alarmOnAverageMa = g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_ALARM_ON);
    auto alarmOnCount = GetConsumptionCount(StatsUtils::STATS_TYPE_ALARM, uid);
    auto alarmOnPowerMah = alarmOnAverageMa * alarmOnCount;
    auto iter = alarmPowerMap_.find(uid);
    if (iter != alarmPowerMap_.end()) {
        STATS_HILOGD(COMP_SVC, "Update alarm on power consumption: %{public}lfmAh for uid: %{public}d",
            alarmOnAverageMa, uid);
        iter->second = alarmOnPowerMah;
    } else {
        STATS_HILOGD(COMP_SVC, "Create alarm on power consumption: %{public}lfmAh for uid: %{public}d",
            alarmOnAverageMa, uid);
        alarmPowerMap_.insert(std::pair<int32_t, double>(uid, alarmOnPowerMah));
    }
}

double AlarmEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = alarmPowerMap_.find(uidOrUserId);
    if (iter != alarmPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGD(COMP_SVC, "Get app alarm power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGD(COMP_SVC,
            "No app alarm power consumption related to uid: %{public}d was found, return 0", uidOrUserId);
    }
    return power;
}

double AlarmEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_ALARM) {
        auto alarmOnIter = alarmPowerMap_.find(uid);
        if (alarmOnIter != alarmPowerMap_.end()) {
            power = alarmOnIter->second;
            STATS_HILOGD(COMP_SVC, "Get alarm on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No alarm on power alarm related to uid: %{public}d was found, return 0", uid);
        }
    }
    return power;
}

std::shared_ptr<StatsHelper::Counter> AlarmEntity::GetOrCreateCounter(StatsUtils::StatsType statsType, int32_t uid)
{
    std::shared_ptr<StatsHelper::Counter> counter = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_ALARM: {
            auto alarmOnIter = alarmCounterMap_.find(uid);
            if (alarmOnIter != alarmCounterMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get alarm on counter for uid: %{public}d", uid);
                counter = alarmOnIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create alarm on counter for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::Counter> alarmConuter = std::make_shared<StatsHelper::Counter>();
            alarmCounterMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::Counter>>(uid, alarmConuter));
            counter = alarmConuter;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create active conuter failed");
            break;
    }
    return counter;
}

void AlarmEntity::Reset()
{
    // Reset app Alarm on total power consumption
    for (auto& iter : alarmPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Alarm on counter
    for (auto& iter : alarmCounterMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
}
} // namespace PowerMgr
} // namespace OHOS