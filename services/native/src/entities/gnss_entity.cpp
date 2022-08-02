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

#include "entities/gnss_entity.h"

#include <cinttypes>

#include "battery_stats_service.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

GnssEntity::GnssEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_GNSS;
}

int64_t GnssEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    int64_t activeTimeMs = StatsUtils::DEFAULT_VALUE;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_GNSS_ON: {
            auto iter = gnssTimerMap_.find(uid);
            if (iter != gnssTimerMap_.end()) {
                activeTimeMs = iter->second->GetRunningTimeMs();
                STATS_HILOGD(COMP_SVC, "Get gnss on time: %{public}" PRId64 "ms for uid: %{public}d", activeTimeMs,
                    uid);
                break;
            }
            STATS_HILOGD(COMP_SVC, "Didn't find related timer for uid: %{public}d, return 0", uid);
            break;
        }
        default:
            break;
    }
    return activeTimeMs;
}

void GnssEntity::Calculate(int32_t uid)
{
    auto gnssOnAverageMa = g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_GNSS_ON);
    auto gnssOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_GNSS_ON);
    auto gnssOnPowerMah = gnssOnAverageMa * gnssOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto iter = gnssPowerMap_.find(uid);
    if (iter != gnssPowerMap_.end()) {
        STATS_HILOGD(COMP_SVC, "Update gnss on power consumption: %{public}lfmAh for uid: %{public}d",
            gnssOnAverageMa, uid);
        iter->second = gnssOnPowerMah;
    } else {
        STATS_HILOGD(COMP_SVC, "Create gnss on power consumption: %{public}lfmAh for uid: %{public}d",
            gnssOnAverageMa, uid);
        gnssPowerMap_.insert(std::pair<int32_t, double>(uid, gnssOnPowerMah));
    }
}

double GnssEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = gnssPowerMap_.find(uidOrUserId);
    if (iter != gnssPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGD(COMP_SVC, "Get app gnss power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGD(COMP_SVC,
            "No app gnss power consumption related to uid: %{public}d was found, return 0", uidOrUserId);
    }
    return power;
}

double GnssEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_GNSS_ON) {
        auto gnssOnIter = gnssPowerMap_.find(uid);
        if (gnssOnIter != gnssPowerMap_.end()) {
            power = gnssOnIter->second;
            STATS_HILOGD(COMP_SVC, "Get gnss on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No gnss on power consumption related to uid: %{public}d was found, return 0", uid);
        }
    }
    return power;
}

std::shared_ptr<StatsHelper::ActiveTimer> GnssEntity::GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
    int16_t level)
{
    std::shared_ptr<StatsHelper::ActiveTimer> timer = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_GNSS_ON: {
            auto gnssOnIter = gnssTimerMap_.find(uid);
            if (gnssOnIter != gnssTimerMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get gnss on timer for uid: %{public}d", uid);
                timer = gnssOnIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create gnss on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> gnssTimer = std::make_shared<StatsHelper::ActiveTimer>();
            gnssTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, gnssTimer));
            timer = gnssTimer;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create active timer failed");
            break;
    }
    return timer;
}

void GnssEntity::Reset()
{
    // Reset app Gnss on total power consumption
    for (auto& iter : gnssPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Gnss on timer
    for (auto& iter : gnssTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
}
} // namespace PowerMgr
} // namespace OHOS