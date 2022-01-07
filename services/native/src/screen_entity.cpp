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

#include "screen_entity.h"
#include "battery_stats_parser.h"
#include "battery_stats_service.h"

#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}
ScreenEntity::ScreenEntity()
{
    SetType(BatteryStatsInfo::STATS_TYPE_SCREEN);
}

void ScreenEntity::Calculate()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double totalScreenPower = BatteryStatsUtils::DEFAULT_VALUE;
    // Calculate screen on power
    auto screenOnAverage = statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_SCREEN_ON);
    auto screenOnTime = statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_SCREEN_ON);
    auto screenOnPower = screenOnAverage * screenOnTime;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen on power: %{public}lf", screenOnPower);

    // Calculate screen brightness power
    auto screenBrightnessFullAverage =
        statsService->GetBatteryStatsParser()->GetAveragePower(BatteryStatsUtils::TYPE_SCREEN_BRIGHTNESS);
    double screenBrightnessPower = BatteryStatsUtils::DEFAULT_VALUE;
    for (int i = 0; i < BatteryStatsUtils::DEFAULT_BIN; i++) {
        double screenBrightnessBinAverage =
            screenBrightnessFullAverage * (i + 0.5f) / BatteryStatsUtils::DEFAULT_BIN;
        auto screenBrightnessbinTime = GetScreenBrightnessTime(i);
        double screenBrightnessBinPower = screenBrightnessBinAverage * screenBrightnessbinTime;
        STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen brightness bin: %{public}d time: %{public}ld", i,
            screenBrightnessbinTime);
        STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen brightness bin: %{public}d average: %{public}lf", i,
            screenBrightnessBinPower);
        STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen brightness bin: %{public}d power: %{public}lf", i,
            screenBrightnessBinPower);
        screenBrightnessPower += screenBrightnessBinPower;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate radio screen brightness power: %{public}lf", screenBrightnessPower);

    // Sum the total screen power
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen on time: %{public}ld", screenOnTime);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen on average: %{public}lf", screenOnAverage);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen on power: %{public}lf", screenOnPower);
    totalScreenPower = screenOnPower + screenBrightnessPower;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate screen total power: %{public}lf", totalScreenPower);
    SetPower(totalScreenPower);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

long ScreenEntity::GetScreenBrightnessTime(int32_t bin)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long time = BatteryStatsUtils::DEFAULT_VALUE;
    if ((bin > BatteryStatsUtils::DEFAULT_VALUE) && (bin < BatteryStatsUtils::DEFAULT_BIN)) {
        time = statsService->GetBatteryStatsCore()->GetTotalTimeMs(bin, BatteryStatsUtils::TYPE_SCREEN_BRIGHTNESS);
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got brightness bin: %{public}d time: %{public}ld", bin, time);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Illegal brightness bin");
    }
    return time;
}
} // namespace PowerMgr
} // namespace OHOS
