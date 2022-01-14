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

#include "entities/screen_entity.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}
ScreenEntity::ScreenEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN;
}

long ScreenEntity::GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long activeTimeMs = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_SCREEN_ON) {
        if (screenOnTimer_ != nullptr) {
            activeTimeMs = screenOnTimer_->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got screen on time: %{public}ldms", activeTimeMs);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Didn't find related timer, return 0");
        }
    } else if (statsType == StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS) {
        auto iter = screenBrightnessTimerMap_.find(level);
        if (iter != screenBrightnessTimerMap_.end() && iter->second != nullptr) {
            activeTimeMs = iter->second->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE,
                "Got screen brightness time: %{public}ldms of brightness level: %{public}d", activeTimeMs, level);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No screen brightness timer found, return 0");
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return activeTimeMs;
}

void ScreenEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto screenOnAverageMa =
        statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_ON);
    auto screenBrightnessAverageMa =
        statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_SCREEN_BRIGHTNESS);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen on average power: %{public}lfma", screenOnAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen brightness average power: %{public}lfma",
        screenBrightnessAverageMa);

    double screenPowerMah = StatsUtils::DEFAULT_VALUE;
    for (auto &iter : screenBrightnessTimerMap_) {
        if (iter.second != nullptr) {
            auto averageMa = screenBrightnessAverageMa * iter.first + screenOnAverageMa;
            auto timeMs = GetActiveTimeMs(StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS, iter.first);
            screenPowerMah += averageMa * timeMs / StatsUtils::MS_IN_HOUR;
            STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen average power: %{public}lfma", averageMa);
            STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen active time: %{public}ldms for level: %{public}d",
                timeMs, iter.first);
        }
    }
    screenPowerMah_ = screenPowerMah;
    totalPowerMah_ += screenPowerMah_;
    std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
    statsInfo->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    statsInfo->SetPower(screenPowerMah_);
    statsInfo->SetTime(GetActiveTimeMs(StatsUtils::STATS_TYPE_SCREEN_ON), StatsUtils::STATS_TYPE_SCREEN_ON);
    statsInfoList_.push_back(statsInfo);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen active power consumption: %{public}lfmAh", screenPowerMah);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double ScreenEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got screen power consumption: %{public}lfmAh", screenPowerMah_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return screenPowerMah_;
}

double ScreenEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got screen brightness power consumption: %{public}lfmAh", screenPowerMah_);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return screenPowerMah_;
}

std::shared_ptr<StatsHelper::ActiveTimer> ScreenEntity::GetOrCreateTimer(StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_SCREEN_ON) {
        if (screenOnTimer_ != nullptr) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got screen on timer");
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return screenOnTimer_;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create screen on timer");
            screenOnTimer_ = std::make_shared<StatsHelper::ActiveTimer>();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return screenOnTimer_;
        }
    } else if (statsType == StatsUtils::STATS_TYPE_SCREEN_BRIGHTNESS) {
        auto iter = screenBrightnessTimerMap_.find(level);
        if (iter != screenBrightnessTimerMap_.end() && iter->second != nullptr) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got screen brightness timer of brightness level: %{public}d", level);
            return iter->second;
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create screen brightness timer of brightness level: %{public}d", level);
            std::shared_ptr<StatsHelper::ActiveTimer> timer = std::make_shared<StatsHelper::ActiveTimer>();
            screenBrightnessTimerMap_.
                insert(std::pair<int32_t, std::shared_ptr<StatsHelper::ActiveTimer>>(level, timer));
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return timer;
        }
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create active timer failed");
        return nullptr;
    }
}

void ScreenEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset app Screen total power consumption
    screenPowerMah_ = StatsUtils::DEFAULT_VALUE;

    // Reset Screen on timer
    if (screenOnTimer_ != nullptr) {
        screenOnTimer_->Reset();
    }

    // Reset Screen brightness timer
    for (auto &iter : screenBrightnessTimerMap_) {
        if (iter.second != nullptr) {
            iter.second->Reset();
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void ScreenEntity::DumpInfo(std::string& result)
{
    long onTime = GetActiveTimeMs(StatsUtils::STATS_TYPE_SCREEN_ON);
    result.append("Screen on time: ")
        .append(ToString(onTime))
        .append("ms")
        .append("\n");
}
} // namespace PowerMgr
} // namespace OHOS