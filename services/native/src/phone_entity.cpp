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

#include "phone_entity.h"
#include "battery_stats_parser.h"
#include "battery_stats_service.h"

#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

PhoneEntity::PhoneEntity()
{
    SetType(BatteryStatsInfo::STATS_TYPE_PHONE);
}

void PhoneEntity::Calculate()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto phoneAverage = statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_RADIO_ACTIVE);
    auto phoneTime = statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_RADIO_ACTIVE);
    auto phonePower = phoneAverage * phoneTime;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio active time: %{public}ld", phoneTime);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio active average: %{public}lf", phoneAverage);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio active power: %{public}lf", phonePower);
    SetPower(phonePower);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS