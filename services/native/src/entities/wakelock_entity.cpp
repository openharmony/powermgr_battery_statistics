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

#include "entities/wakelock_entity.h"

#include <cinttypes>

#include "battery_stats_service.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

WakelockEntity::WakelockEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK;
}

int64_t WakelockEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    int64_t activeTimeMs = StatsUtils::DEFAULT_VALUE;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD: {
            auto iter = wakelockTimerMap_.find(uid);
            if (iter != wakelockTimerMap_.end()) {
                activeTimeMs = iter->second->GetRunningTimeMs();
                STATS_HILOGD(COMP_SVC, "Get wakelock on time: %{public}" PRId64 "ms for uid: %{public}d",
                    activeTimeMs, uid);
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

void WakelockEntity::Calculate(int32_t uid)
{
    auto wakelockOnAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_CPU_AWAKE);
    auto wakelockOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_WAKELOCK_HOLD);
    auto wakelockOnPowerMah = wakelockOnAverageMa * wakelockOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto iter = wakelockPowerMap_.find(uid);
    if (iter != wakelockPowerMap_.end()) {
        STATS_HILOGD(COMP_SVC, "Update wakelock on power consumption: %{public}lfmAh for uid: %{public}d",
            wakelockOnPowerMah, uid);
        iter->second = wakelockOnPowerMah;
    } else {
        STATS_HILOGD(COMP_SVC, "Create wakelock on power consumption: %{public}lfmAh for uid: %{public}d",
            wakelockOnPowerMah, uid);
        wakelockPowerMap_.insert(std::pair<int32_t, double>(uid, wakelockOnPowerMah));
    }
}

double WakelockEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = wakelockPowerMap_.find(uidOrUserId);
    if (iter != wakelockPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGD(COMP_SVC, "Get app wakelock power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGD(COMP_SVC,
            "No app wakelock power consumption related to uid: %{public}d was found, return 0", uidOrUserId);
    }
    return power;
}

double WakelockEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_WAKELOCK_HOLD) {
        auto wakelockOnIter = wakelockPowerMap_.find(uid);
        if (wakelockOnIter != wakelockPowerMap_.end()) {
            power = wakelockOnIter->second;
            STATS_HILOGD(COMP_SVC, "Get wakelock on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No wakelock on power consumption related to uid: %{public}d was found, return 0", uid);
        }
    }
    return power;
}

std::shared_ptr<StatsHelper::ActiveTimer> WakelockEntity::GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
    int16_t level)
{
    std::shared_ptr<StatsHelper::ActiveTimer> timer = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_WAKELOCK_HOLD: {
            auto wakelockOnIter = wakelockTimerMap_.find(uid);
            if (wakelockOnIter != wakelockTimerMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get wakelock on timer for uid: %{public}d", uid);
                timer = wakelockOnIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create wakelock on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> holdTimer = std::make_shared<StatsHelper::ActiveTimer>();
            wakelockTimerMap_.insert(
                std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, holdTimer));
            timer = holdTimer;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create active timer failed");
            break;
    }
    return timer;
}

void WakelockEntity::Reset()
{
    // Reset app Wakelock on total power consumption
    for (auto& iter : wakelockPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Wakelock on timer
    for (auto& iter : wakelockTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
}
} // namespace PowerMgr
} // namespace OHOS