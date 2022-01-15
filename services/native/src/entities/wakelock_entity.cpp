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

#include "entities/wakelock_entity.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

WakelockEntity::WakelockEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK;
}

long WakelockEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long activeTimeMs = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_WAKELOCK_HOLD) {
        auto iter = wakelockTimerMap_.find(uid);
        if (iter != wakelockTimerMap_.end()) {
            activeTimeMs = iter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wakelock on time: %{public}ldms for uid: %{public}d", activeTimeMs,
                uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Didn't find related timer for uid: %{public}d, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return activeTimeMs;
}

void WakelockEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto wakelockOnAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_CPU_AWAKE);
    auto wakelockOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_WAKELOCK_HOLD);
    auto wakelockOnPowerMah = wakelockOnAverageMa * wakelockOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto iter = wakelockPowerMap_.find(uid);
    if (iter != wakelockPowerMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update wakelock on power consumption: %{public}lfmAh for uid: %{public}d",
            wakelockOnAverageMa, uid);
        iter->second = wakelockOnPowerMah;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create wakelock on power consumption: %{public}lfmAh for uid: %{public}d",
            wakelockOnAverageMa, uid);
        wakelockPowerMap_.insert(std::pair<int32_t, double>(uid, wakelockOnPowerMah));
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wakelock on average power: %{public}lfma", wakelockOnAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wakelock on time: %{public}ldms for uid: %{public}d",
        wakelockOnTimeMs, uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wakelock on power consumption: %{public}lfmAh for uid: %{public}d",
        wakelockOnPowerMah, uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double WakelockEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = wakelockPowerMap_.find(uidOrUserId);
    if (iter != wakelockPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got app wakelock power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE,
            "No app wakelock power consumption related with uid: %{public}d found, return 0", uidOrUserId);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

double WakelockEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_WAKELOCK_HOLD) {
        auto wakelockOnIter = wakelockPowerMap_.find(uid);
        if (wakelockOnIter != wakelockPowerMap_.end()) {
            power = wakelockOnIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wakelock on power consumption: %{public}lfmAh for uid: %{public}d",
                power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No wakelock on power consumption related with uid: %{public}d found, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

std::shared_ptr<StatsHelper::ActiveTimer> WakelockEntity::GetOrCreateTimer(int32_t uid, StatsUtils::StatsType statsType,
    int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_WAKELOCK_HOLD) {
        auto wakelockOnIter = wakelockTimerMap_.find(uid);
        if (wakelockOnIter != wakelockTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got wakelock on timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return wakelockOnIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create wakelock on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            wakelockTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create active timer failed");
        return nullptr;
    }
}

void WakelockEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset app Wakelock on total power consumption
    for (auto &iter : wakelockPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Wakelock on timer
    for (auto &iter : wakelockTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS