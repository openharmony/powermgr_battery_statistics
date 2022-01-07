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

#ifndef BATTERY_STATS_DETECTOR_H
#define BATTERY_STATS_DETECTOR_H

#include <memory>
#include <string>

#include "battery_stats_utils.h"
#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsService;
class BatteryStatsDetector {
public:
    explicit BatteryStatsDetector(const wptr<BatteryStatsService>& bss) : bss_(bss)
    {
        STATS_HILOGD(STATS_MODULE_SERVICE, "BatteryStatsDetector instance is created.");
    }
    ~BatteryStatsDetector() = default;
    void HandleStatsChangedEvent(BatteryStatsUtils::StatsData data);
    bool Init();
private:
    const wptr<BatteryStatsService> bss_;
    bool isTimeRelated(BatteryStatsUtils::StatsDataType type);
    bool isDurationRelated(BatteryStatsUtils::StatsDataType type);
    bool isLevelRelated(BatteryStatsUtils::StatsDataType type);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_DETECTOR_H