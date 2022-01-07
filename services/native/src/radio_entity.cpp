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

#include "radio_entity.h"
#include "battery_stats_parser.h"
#include "battery_stats_service.h"

#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

RadioEntity::RadioEntity()
{
    SetType(BatteryStatsInfo::STATS_TYPE_RADIO);
}

void RadioEntity::Calculate()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double totalRadioPower = BatteryStatsUtils::DEFAULT_VALUE;
    // Calculate radio scanning power
    auto radioScanAverage = statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_RADIO_SCAN);
    auto radioScanTime = statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_RADIO_SCAN);
    auto radioScanPower = radioScanAverage * radioScanTime;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio scanning power: %{public}lf", radioScanPower);

    // Calculate radio signal level power
    double radioSignalPower = BatteryStatsUtils::DEFAULT_VALUE;
    for (int i = 0; i < BatteryStatsUtils::DEFAULT_BIN; i++) {
        double radioSignalAverage =
            statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_RADIO_ON, i);
        auto radioSignalTime = GetSignalLevelTime(i);
        double radioSignalLevelPower = radioSignalAverage * radioSignalTime;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio signal level: %{public}d time: %{public}ld", i, radioSignalTime);
        STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio signal level: %{public}d average: %{public}lf", i,
            radioSignalAverage);
        STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio signal level: %{public}d power: %{public}lf", i,
            radioSignalLevelPower);
        radioSignalPower += radioSignalLevelPower;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio radio signal power: %{public}lf", radioSignalPower);

    // Sum the total radio power
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio scan time: %{public}ld", radioScanTime);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio scan average: %{public}lf", radioScanAverage);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio scan power: %{public}lf", radioScanPower);
    totalRadioPower = radioScanPower + radioSignalPower;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio total power: %{public}lf", totalRadioPower);
    SetPower(totalRadioPower);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

long RadioEntity::GetSignalLevelTime(int32_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long time = BatteryStatsUtils::DEFAULT_VALUE;
    if ((level > BatteryStatsUtils::DEFAULT_VALUE) && (level < BatteryStatsUtils::DEFAULT_BIN)) {
        time = statsService->GetBatteryStatsCore()->GetTotalTimeMs(level, BatteryStatsUtils::TYPE_RADIO_ON);
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got signal level: %{public}d time: %{public}ld", level, time);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Illegal signal level");
    }
    return time;
}
} // namespace PowerMgr
} // namespace OHOS
