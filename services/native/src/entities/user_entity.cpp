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

#include "entities/user_entity.h"

#include "ohos_account_kits_impl.h"
#include "stats_hilog_wrapper.h"

#include "battery_stats_parser.h"
#include "battery_stats_service.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

UserEntity::UserEntity()
{
    consumptionType_ = BatteryStatsInfo::CONSUMPTION_TYPE_USER;
}

double UserEntity::GetEntityPowerMah(int32_t uidOrUserId)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double power = StatsUtils::DEFAULT_VALUE;
    auto iter = userPowerMap_.find(uidOrUserId);
    if (iter != userPowerMap_.end()) {
        power = iter->second;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Got user power consumption: %{public}lfmAh for user id: %{public}d",
            power, uidOrUserId);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE,
            "No user power consumption related with user id: %{public}d found, return 0", uidOrUserId);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return power;
}

void UserEntity::AggregateUserPowerMah(int32_t userId, double power)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto iter = userPowerMap_.find(userId);
    if (iter != userPowerMap_.end()) {
        iter->second += power;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Add user power consumption: %{public}lfmAh for user id: %{public}d",
            power, userId);
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Create user power consumption: %{public}lfmAh for user id: %{public}d",
            power, userId);
        userPowerMap_.insert(std::pair<int32_t, double>(userId, power));
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void UserEntity::Calculate(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    for (auto &iter : userPowerMap_) {
        std::shared_ptr<BatteryStatsInfo> statsInfo = std::make_shared<BatteryStatsInfo>();
        statsInfo->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_USER);
        statsInfo->SetUserId(iter.first);
        statsInfo->SetPower(iter.second);
        statsInfoList_.push_back(statsInfo);
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void UserEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Reset app user total power consumption
    for (auto &iter : userPowerMap_) {
        iter.second = StatsUtils::DEFAULT_VALUE;
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS

