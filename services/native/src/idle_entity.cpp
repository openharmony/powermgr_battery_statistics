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

#include "idle_entity.h"
#include <time.h>
#include "battery_stats_parser.h"
#include "battery_stats_service.h"

#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

IdleEntity::IdleEntity()
{
    SetType(BatteryStatsInfo::STATS_TYPE_IDLE);
}

void IdleEntity::Calculate()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto parser = statsService->GetBatteryStatsParser();
    auto cpuIdleAverage = parser->GetAveragePower(BatteryStatsUtils::TYPE_CPU_IDLE);
    auto cpuIdleTime = GetCpuIdleTimeMs();
    auto cpuIdlePower = cpuIdleAverage * cpuIdleTime;
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu idle time: %{public}ld", cpuIdleTime);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu idle average: %{public}lf", cpuIdleAverage);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate cpu idle power: %{public}lf", cpuIdlePower);
    SetPower(cpuIdlePower);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

long IdleEntity::GetCpuIdleTimeMs()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    long idleTime = BatteryStatsUtils::DEFAULT_VALUE;
    struct timespec rawRealTime;
    int errCode = clock_gettime(CLOCK_BOOTTIME, &rawRealTime);
    if (errCode != 0) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Get boot time failed");
    } else {
        long bootTimeMs = (long) (rawRealTime.tv_nsec / 1000000);
        long screenOnTime =
            (long) statsService->GetBatteryStatsCore()->GetTotalTimeMs(BatteryStatsUtils::TYPE_SCREEN_ON);
        if (bootTimeMs >= screenOnTime) {
            idleTime = bootTimeMs - screenOnTime;
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Got idle time: %{public}ld", idleTime);
    return idleTime;
}
} // namespace PowerMgr
} // namespace OHOS
