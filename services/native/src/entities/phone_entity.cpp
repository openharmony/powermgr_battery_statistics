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

#include "entities/phone_entity.h"

#include <cinttypes>

#include "battery_stats_service.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

PhoneEntity::PhoneEntity()
    : phoneTimer_(nullptr), phonePowerMah_(StatsUtils::DEFAULT_VALUE)
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_PHONE;
}

int64_t PhoneEntity::GetActiveTimeMs(StatsUtils::StatsType statsType, int16_t level)
{
    int64_t activeTimeMs = StatsUtils::DEFAULT_VALUE;
    if (statsType == StatsUtils::STATS_TYPE_PHONE_ACTIVE) {
        if (phoneTimer_ != nullptr) {
            activeTimeMs = phoneTimer_->GetRunningTimeMs();
            STATS_HILOGD(COMP_SVC, "Get phone active time: %{public}" PRId64 "ms", activeTimeMs);
        } else {
            STATS_HILOGD(COMP_SVC, "Didn't find related timer, return 0");
        }
    }
    return activeTimeMs;
}

void PhoneEntity::Calculate(int32_t uid)
{
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
    STATS_HILOGD(COMP_SVC, "Calculate phone active power consumption: %{public}lfmAh", phoneOnPowerMah);
}

double PhoneEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    return phonePowerMah_;
}

double PhoneEntity::GetStatsPowerMah(StatsUtils::StatsType statsType, int32_t uid)
{
    return phonePowerMah_;
}

std::shared_ptr<StatsHelper::ActiveTimer> PhoneEntity::GetOrCreateTimer(StatsUtils::StatsType statsType, int16_t level)
{
    std::shared_ptr<StatsHelper::ActiveTimer> timer = nullptr;
    switch (statsType) {
        case StatsUtils::STATS_TYPE_PHONE_ACTIVE: {
            if (phoneTimer_ != nullptr) {
                STATS_HILOGD(COMP_SVC, "Get phone active timer");
                timer = phoneTimer_;
                break;
            }
            STATS_HILOGD(COMP_SVC, "Create phone active timer");
            phoneTimer_ = std::make_shared<StatsHelper::ActiveTimer>();
            timer = phoneTimer_;
            break;
        }
        default:
            STATS_HILOGW(COMP_SVC, "Create active timer failed");
            break;
    }
    return timer;
}

void PhoneEntity::Reset()
{
    STATS_HILOGI(COMP_SVC, "Reset");
    // Reset app Phone total power consumption
    phonePowerMah_ = StatsUtils::DEFAULT_VALUE;

    // Reset Phone active timer
    if (phoneTimer_) {
        phoneTimer_->Reset();
    }
}

void PhoneEntity::DumpInfo(std::string& result, int32_t uid)
{
    int64_t time = GetActiveTimeMs(StatsUtils::STATS_TYPE_PHONE_ACTIVE);
    result.append("Phone dump:\n")
        .append("Phone active time: ")
        .append(ToString(time))
        .append("ms")
        .append("\n");
}
} // namespace PowerMgr
} // namespace OHOS