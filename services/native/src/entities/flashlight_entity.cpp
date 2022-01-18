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

#include "entities/flashlight_entity.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

FlashlightEntity::FlashlightEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT;
}

long FlashlightEntity::GetActiveTimeMs(int32_t uid, StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long activeTimeMs = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_FLASHLIGHT_ON) {
        auto iter = flashlightTimerMap_.find(uid);
        if (iter != flashlightTimerMap_.end()) {
            activeTimeMs = iter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got flashlight on time: %{public}ldms for uid: %{public}d",
                activeTimeMs, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Didn't find related timer for uid: %{public}d, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return activeTimeMs;
}

void FlashlightEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto flashlightOnAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_FLASHLIGHT_ON);
    auto flashlightOnTimeMs = GetActiveTimeMs(uid, StatsUtils::STATS_TYPE_FLASHLIGHT_ON);
    auto flashlightOnPowerMah = flashlightOnAverageMa * flashlightOnTimeMs / StatsUtils::MS_IN_HOUR;
    auto iter = flashlightPowerMap_.find(uid);
    if (iter != flashlightPowerMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Update flashlight on power consumption: %{public}lfmAh for uid: %{public}d",
            flashlightOnAverageMa, uid);
        iter->second = flashlightOnPowerMah;
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create flashlight on power consumption: %{public}lfmAh for uid: %{public}d",
            flashlightOnAverageMa, uid);
        flashlightPowerMap_.insert(std::pair<int32_t, double>(uid, flashlightOnPowerMah));
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate flashlight on average power: %{public}lfma", flashlightOnAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate flashlight on time: %{public}ldms for uid: %{public}d",
        flashlightOnTimeMs, uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate flashlight on power consumption: %{public}lfmAh for uid: %{public}d",
        flashlightOnPowerMah, uid);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double FlashlightEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = flashlightPowerMap_.find(uidOrUserId);
    if (iter != flashlightPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got app flashlight power consumption: %{public}lfmAh for uid: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE,
            "No app flashlight power consumption related with uid: %{public}d found, return 0", uidOrUserId);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

double FlashlightEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_FLASHLIGHT_ON) {
        auto flashlightOnIter = flashlightPowerMap_.find(uid);
        if (flashlightOnIter != flashlightPowerMap_.end()) {
            power = flashlightOnIter->second;
            STATS_HILOGI(STATS_MODULE_SERVICE,
                "Got flashlight on power consumption: %{public}lfmAh for uid: %{public}d", power, uid);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE,
                "No flashlight on power consumption related with uid: %{public}d found, return 0", uid);
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

std::shared_ptr<StatsHelper::ActiveTimer> FlashlightEntity::GetOrCreateTimer(int32_t uid,
    StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_FLASHLIGHT_ON) {
        auto flashlightOnIter = flashlightTimerMap_.find(uid);
        if (flashlightOnIter != flashlightTimerMap_.end()) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got flashlight on timer for uid: %{public}d", uid);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return flashlightOnIter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create flashlight on timer for uid: %{public}d", uid);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            flashlightTimerMap_.insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(uid, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create active timer failed");
        return nullptr;
    }
}

void FlashlightEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset app Flashlight on total power consumption
    for (auto &iter : flashlightPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    // Reset Flashlight on timer
    for (auto &iter : flashlightTimerMap_) {
        if (iter.second) {
            iter.second->Reset();
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS