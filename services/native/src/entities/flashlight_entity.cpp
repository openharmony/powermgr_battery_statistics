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

#include "entities/flashlight_entity.h"

#include <cinttypes>

#include "battery_stats_service.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

FlashlightEntity::FlashlightEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT;
}

int64_t FlashlightEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    int64_t activeTimeMs = StatsUtils::DEFAULT_VALUE;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON: {
            auto iter = flashlightTimerMap_.find(uid);
            if (iter != flashlightTimerMap_.end()) {
                activeTimeMs = iter->second->GetRunningTimeMs();
                STATS_HILOGD(COMP_SVC, "Get flashlight on time: %{public}" PRId64 "ms for uid: %{public}d",
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

void FlashlightEntity::Calculate(int32_t uid)
{
    auto flashlightOnAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    auto flashlightOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_FLASHLIGHT_ON);
    auto flashlightOnPowerMah = flashlightOnAverageMa * flashlightOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto iter = flashlightPowerMap_.find(uid);
    if (iter != flashlightPowerMap_.end()) {
        STATS_HILOGD(COMP_SVC, "Update flashlight on power consumption: %{public}lfmAh for uid: %{public}d",
            flashlightOnAverageMa, uid);
        iter->second = flashlightOnPowerMah;
    } else {
        STATS_HILOGD(COMP_SVC, "Create flashlight on power consumption: %{public}lfmAh for uid: %{public}d",
            flashlightOnAverageMa, uid);
        flashlightPowerMap_.insert(std::pair<int32_t, double>(uid, flashlightOnPowerMah));
    }
}

double FlashlightEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = flashlightPowerMap_.find(uidOrUserId);
    if (iter != flashlightPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGD(COMP_SVC, "Get app flashlight power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGD(COMP_SVC,
            "No app flashlight power consumption related to uid: %{public}d was found, return 0", uidOrUserId);
    }
    return power;
}

double FlashlightEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_FLASHLIGHT_ON) {
        auto flashlightOnIter = flashlightPowerMap_.find(uid);
        if (flashlightOnIter != flashlightPowerMap_.end()) {
            power = flashlightOnIter->second;
            STATS_HILOGD(COMP_SVC,
                "Get flashlight on power consumption: %{public}lfmAh for uid: %{public}d", power, uid);
        } else {
            STATS_HILOGD(COMP_SVC,
                "No flashlight on power consumption related to uid: %{public}d was found, return 0", uid);
        }
    }
    return power;
}

std::shared_ptr<StatsHelper::ActiveTimer> FlashlightEntity::GetOrCreateTimer(int32_t uid,
    StatsUtils::StatsType statsType, int16_t level)
{
    std::shared_ptr<StatsHelper::ActiveTimer> timer = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_FLASHLIGHT_ON: {
            auto flashlightOnIter = flashlightTimerMap_.find(uid);
            if (flashlightOnIter != flashlightTimerMap_.end()) {
                STATS_HILOGD(COMP_SVC, "Get flashlight on timer for uid: %{public}d", uid);
                timer = flashlightOnIter->second;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create flashlight on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> flashTimer = std::make_shared<StatsHelper::ActiveTimer>();
            flashlightTimerMap_.insert(
                std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, flashTimer));
            timer = flashTimer;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create active timer failed");
            break;
    }
    return timer;
}

void FlashlightEntity::Reset()
{
    // Reset app Flashlight on total power consumption
    for (auto& iter : flashlightPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Flashlight on timer
    for (auto& iter : flashlightTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
}
} // namespace PowerMgr
} // namespace OHOS