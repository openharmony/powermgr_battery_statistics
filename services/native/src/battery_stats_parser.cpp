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

#include "battery_stats_parser.h"

#include <fstream>
#include "ios"
#include "json/reader.h"
#include "json/value.h"

#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
namespace {
static const std::string VENDOR_POWER_AVERAGE_FILE = "/vendor/etc/profile/power_average.json";
static const std::string SYSTEM_POWER_AVERAGE_FILE = "/system/etc/profile/power_average.json";
} // namespace
bool BatteryStatsParser::Init()
{
    if (!LoadAveragePowerFromFile(VENDOR_POWER_AVERAGE_FILE)) {
        STATS_HILOGE(COMP_SVC, "Failed to load vendor average power file");
        if (!LoadAveragePowerFromFile(SYSTEM_POWER_AVERAGE_FILE)) {
            STATS_HILOGE(COMP_SVC, "Failed to load system average power file");
            return false;
        }
    }
    return true;
}

uint16_t BatteryStatsParser::GetSpeedNum(uint16_t cluster)
{
    for (uint16_t i = 0; i < speedNum_.size(); i++) {
        if (cluster == i) {
            STATS_HILOGD(COMP_SVC, "Get speed num: %{public}d, for cluster: %{public}d", speedNum_[i],
                cluster);
            return speedNum_[i];
        }
    }
    STATS_HILOGW(COMP_SVC, "No related speed number, return 0");
    return StatsUtils::DEFAULT_VALUE;
}

bool BatteryStatsParser::LoadAveragePowerFromFile(const std::string& path)
{
    Json::CharReaderBuilder reader;
    Json::Value root;
    std::string errors;
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs.is_open()) {
        STATS_HILOGE(COMP_SVC, "Json file doesn't exist");
        return false;
    }
    if (!parseFromStream(reader, ifs, &root, &errors)) {
        STATS_HILOGE(COMP_SVC, "Failed to parse the JSON file");
        ifs.close();
        return false;
    }
    ifs.close();

    Json::Value::Members members = root.getMemberNames();
    for (auto iter = members.begin(); iter != members.end(); iter++) {
        std::string type = *iter;
        Json::Value value = root[type];

        if (type == StatsUtils::CURRENT_CPU_CLUSTER) {
            clusterNum_ = value.size();
            STATS_HILOGD(COMP_SVC, "Read cluster num: %{public}d", clusterNum_);
        }

        if (type.find(StatsUtils::CURRENT_CPU_SPEED) != std::string::npos) {
            STATS_HILOGD(COMP_SVC, "Read speed num: %{public}d", static_cast<int32_t>(value.size()));
            speedNum_.push_back(value.size());
        }

        if (value.isArray()) {
            std::vector<double> listValues;
            for (uint16_t i = 0; i < value.size(); i++) {
                listValues.push_back(value[i].asDouble());
                STATS_HILOGD(COMP_SVC, "Read list value: %{public}lf of %{public}s", value[i].asDouble(),
                    type.c_str());
            }
            averageVecMap_.insert(std::pair<std::string, std::vector<double>>(type, listValues));
        } else {
            double singleValue = value.asDouble();
            averageMap_.insert(std::pair<std::string, double>(type, singleValue));
            STATS_HILOGD(COMP_SVC, "Read single value: %{public}lf of %{public}s", singleValue,
                type.c_str());
        }
    }
    return true;
}

double BatteryStatsParser::GetAveragePowerMa(std::string type)
{
    double average = 0.0;
    auto iter = averageMap_.find(type);
    if (iter != averageMap_.end()) {
        average = iter->second;
    }
    STATS_HILOGD(COMP_SVC, "Get average power: %{public}lfma of %{public}s", average, type.c_str());
    return average;
}

double BatteryStatsParser::GetAveragePowerMa(std::string type, uint16_t level)
{
    double average = 0.0;
    auto iter = averageVecMap_.find(type);
    if (iter != averageVecMap_.end()) {
        if (level < iter->second.size()) {
            average = iter->second[level];
        }
    }
    STATS_HILOGD(COMP_SVC, "Get average power: %{public}lf of %{public}s, level: %{public}d",
        average, type.c_str(), level);
    return average;
}

uint16_t BatteryStatsParser::GetClusterNum()
{
    return clusterNum_;
}
} // namespace PowerMgr
} // namespace OHOS
