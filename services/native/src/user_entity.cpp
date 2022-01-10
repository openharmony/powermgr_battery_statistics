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

#include "user_entity.h"
#include "battery_stats_parser.h"
#include "battery_stats_service.h"

#include "stats_hilog_wrapper.h"
#include "ohos_account_kits_impl.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto battryCore = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance()->GetBatteryStatsCore();
}

UserEntity::UserEntity(int32_t userId)
{
    if (userId > BatteryStatsUtils::INVALID_VALUE) {
        userId_ = userId;
        SetType(BatteryStatsInfo::STATS_TYPE_USER);
    }
}

int32_t UserEntity::GetUid()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got user id: %{public}d", userId_);
    return userId_;
}

void UserEntity::Calculate()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate power: %{public}lf for user id: %{public}d",
        uidRelatedUserPower_, userId_);
    SetPower(uidRelatedUserPower_);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void UserEntity::AddUidRelatedUserPower(double power)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (power >= BatteryStatsUtils::DEFAULT_VALUE) {
        double powerMams = power * 3600000;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Add uid related user power: %{public}lf to user id: %{public}d",
            powerMams, userId_);
        uidRelatedUserPower_ += powerMams;
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Invalid uid related user power");
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void UserEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter entity reset");
    SetPower(BatteryStatsUtils::DEFAULT_VALUE);
    uidRelatedUserPower_ = BatteryStatsUtils::DEFAULT_VALUE;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit entity reset");
}
} // namespace PowerMgr
} // namespace OHOS

