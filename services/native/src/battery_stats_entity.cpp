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

#include "battery_stats_entity.h"
#include "stats_hilog_wrapper.h"

#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {

void BatteryStatsEntity::SetType(BatteryStatsInfo::BatteryStatsType type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (type != BatteryStatsInfo::STATS_TYPE_INVALID) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Set type to: %{public}d", type);
        type_ = type;
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Invalid type");
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void BatteryStatsEntity::SetPower(double power)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (power >= BatteryStatsUtils::DEFAULT_VALUE) {
        totalPowerMah_ = power / 3600000;
        if (type_ == BatteryStatsInfo::STATS_TYPE_APP) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lf to type: %{public}d for uid: %{public}d",
                totalPowerMah_, type_, GetUid());
        } else if (type_ == BatteryStatsInfo::STATS_TYPE_USER) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lf to type: %{public}d for user id: %{public}d",
                totalPowerMah_, type_, GetUid());
        } else {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Set power: %{public}lf to type: %{public}d", totalPowerMah_, type_);
        }
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Invalid power");
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

int32_t BatteryStatsEntity::GetUid()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "No uid related, so return INVALID_UID = %{public}d", BatteryStatsUtils::INVALID_VALUE);
    return BatteryStatsUtils::INVALID_VALUE;
}

void BatteryStatsEntity::AddUidRelatedUserPower(double power)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "No need to add uid related user power");
}

BatteryStatsInfo::BatteryStatsType BatteryStatsEntity::GetType()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got type = %{public}d", type_);
    return type_;
}

double BatteryStatsEntity::GetTotalPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got total Power in Mah  = %{public}lf", totalPowerMah_);
    return totalPowerMah_;
}

void BatteryStatsEntity::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter entity reset");
    SetPower(BatteryStatsUtils::DEFAULT_VALUE);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit entity reset");
}
} // namespace PowerMgr
} // namespace OHOS