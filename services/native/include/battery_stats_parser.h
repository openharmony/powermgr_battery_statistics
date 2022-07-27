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

#ifndef BATTERY_STATS_PARSER_H
#define BATTERY_STATS_PARSER_H

#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <cstdint>
#include <iosfwd>

#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsParser {
public:
    explicit BatteryStatsParser()
    {
        STATS_HILOGI(COMP_SVC, "BatteryStatsParser instance is created");
    }
    ~BatteryStatsParser() = default;
    double GetAveragePowerMa(std::string type);
    double GetAveragePowerMa(std::string type, uint16_t level);
    uint16_t GetClusterNum();
    uint16_t GetSpeedNum(uint16_t cluster);
    bool Init();
private:
    bool LoadAveragePowerFromFile(const std::string& path);
    std::map<std::string, double> averageMap_;
    std::map<std::string, std::vector<double>> averageVecMap_;
    uint16_t clusterNum_ = 0;
    std::vector<uint16_t> speedNum_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_PARSER_H