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

#ifndef BATTERY_STATS_DETECTOR_H
#define BATTERY_STATS_DETECTOR_H

#include <memory>
#include <string>

#include "refbase.h"

#include "stats_log.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsDetector {
public:
    explicit BatteryStatsDetector()
    {
        STATS_HILOGI(COMP_SVC, "BatteryStatsDetector instance is created");
    }
    ~BatteryStatsDetector() = default;
    void HandleStatsChangedEvent(StatsUtils::StatsData data);
private:
    bool IsDurationRelated(StatsUtils::StatsType type);
    bool IsStateRelated(StatsUtils::StatsType type);
    void HandleDebugInfo(StatsUtils::StatsData data);
    void HandleThermalInfo(StatsUtils::StatsData data, int64_t bootTimeMs, std::string& debugInfo);
    void HandleBatteryInfo(StatsUtils::StatsData data, int64_t bootTimeMs, std::string& debugInfo);
    void HandleDispalyInfo(StatsUtils::StatsData data, int64_t bootTimeMs, std::string& debugInfo);
    void HandleWakelockInfo(StatsUtils::StatsData data, int64_t bootTimeMs, std::string& debugInfo);
    void HandleWorkschedulerInfo(StatsUtils::StatsData data, int64_t bootTimeMs, std::string& debugInfo);
    void HandlePhoneInfo(StatsUtils::StatsData data, int64_t bootTimeMs, std::string& debugInfo);
    void HandleFlashlightInfo(StatsUtils::StatsData data, int64_t bootTimeMs, std::string& debugInfo);
    void HandleDistributedSchedulerInfo(StatsUtils::StatsData data, int64_t bootTimeMs, std::string& debugInfo);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_DETECTOR_H