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

#include "entities/phone_entity.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

PhoneEntity::PhoneEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_PHONE;
}

long PhoneEntity::GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long activeTimeMs = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_PHONE_ACTIVE) {
        if (phoneTimer_ != nullptr) {
            activeTimeMs = phoneTimer_->GetRunningTimeMs();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got phone active time: %{public}ldms", activeTimeMs);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Didn't find related timer, return 0");
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return activeTimeMs;
}

void PhoneEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto phoneOnAverageMa =
        g_statsService->GetBatteryStatsParser()->GetAveragePowerMa(StatsUtils::CURRENT_RADIO_ACTIVE);
    auto phoneOnTimeMs = GetActiveTimeMs(StatsUtils::STATS_TYPE_PHONE_ACTIVE);
    auto phoneOnPowerMah = phoneOnAverageMa * phoneOnTimeMs / StatsUtils::MS_IN_HOUR;
    phonePowerMah_ = phoneOnPowerMah;
    totalPowerMah_ += phonePowerMah_;
    std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
    statsInfo->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    statsInfo->SetPower(phonePowerMah_);
    statsInfoList_.push_back(statsInfo);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate phone active average power: %{public}lfma", phoneOnAverageMa);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate phone active time: %{public}ldms", phoneOnTimeMs);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate phone active power consumption: %{public}lfmAh", phoneOnPowerMah);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double PhoneEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got app phone power consumption: %{public}lfmAh", phonePowerMah_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return phonePowerMah_;
}

double PhoneEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_PHONE_ACTIVE) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got app phone active power consumption: %{public}lfmAh", phonePowerMah_);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return phonePowerMah_;
}

std::shared_ptr<StatsHelper::ActiveTimer> PhoneEntity::GetOrCreateTimer(StatsUtils::StatsType statsType, int16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (statsType == StatsUtils::STATS_TYPE_PHONE_ACTIVE) {
        if (phoneTimer_ != nullptr) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got phone active timer");
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return phoneTimer_;
        } else {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Create phone active timer");
            phoneTimer_ = std::make_shared<StatsHelper::ActiveTimer>();
            STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
            return phoneTimer_;
        }
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Create active timer failed");
        return nullptr;
    }
}

void PhoneEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset app Phone total power consumption
    phonePowerMah_ = StatsUtils::DEFAULT_VALUE;

    // Reset Phone active timer
    if (phoneTimer_) {
        phoneTimer_->Reset();
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void PhoneEntity::DumpInfo(std::string& result, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long time = GetActiveTimeMs(StatsUtils::STATS_TYPE_PHONE_ACTIVE);
    result.append("Phone dump:\n")
        .append("Phone active time: ")
        .append(ToString(time))
        .append("ms")
        .append("\n");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS