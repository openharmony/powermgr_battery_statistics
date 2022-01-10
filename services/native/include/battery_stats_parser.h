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

#ifndef BATTERY_STATS_PARSER_H
#define BATTERY_STATS_PARSER_H

#include <map>
#include <string>
#include <vector>

#include "battery_stats_service.h"
#include "battery_stats_utils.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsService;
class BatteryStatsParser {
public:
    explicit BatteryStatsParser(const wptr<BatteryStatsService>& bss) : bss_(bss)
    {
        STATS_HILOGD(STATS_MODULE_SERVICE, "BatteryStatsParser instance is created.");
    }
    ~BatteryStatsParser() = default;
    double GetAveragePower(std::string type);
    double GetAveragePower(std::string type, uint16_t level);
    uint16_t GetClusterNum();
    uint16_t GetSpeedNum(uint16_t cluster);
    bool Init();
private:
    bool LoadAveragePowerFromFile();
    std::map <std::string, double> averageMap_;
    std::map <std::string, std::vector<double>> averageVecMap_;
    const wptr<BatteryStatsService> bss_;
    uint16_t clusterNum_ = 0;
    std::vector<uint16_t> speedNum_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_PARSER_H