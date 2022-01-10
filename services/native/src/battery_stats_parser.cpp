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

#include "battery_stats_parser.h"

#include "json/json.h"

#include "stats_hilog_wrapper.h"
#include "battery_stats_utils.h"

namespace OHOS {
namespace PowerMgr {
namespace {
static const std::string POWER_AVERAGE_FILE = "/system/etc/profile/power_average.json";
} // namespace

bool BatteryStatsParser::Init()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto pmsptr = bss_.promote();
    if (pmsptr == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Initialization failed: promoting failure");
        return false;
    }

    if (!LoadAveragePowerFromFile()) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Initialization failed: loading average power file failure");
        return false;
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Initialization succeeded");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

uint16_t BatteryStatsParser::GetSpeedNum(uint16_t cluster)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    for (uint16_t i = 0; i < speedNum_.size(); i++) {
        if (cluster == i) {
            STATS_HILOGI(STATS_MODULE_SERVICE, "Got speed num: %{public}d, for cluster: %{public}d", speedNum_[i],
                cluster);
            return speedNum_[i];
        }
    }
    STATS_HILOGE(STATS_MODULE_SERVICE, "No related speed number, return 0");
    return BatteryStatsUtils::DEFAULT_VALUE;
}

bool BatteryStatsParser::LoadAveragePowerFromFile()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    Json::CharReaderBuilder reader;
    Json::Value root;
    std::string errors;
    std::ifstream ifs(POWER_AVERAGE_FILE, std::ios::binary);
    if (!ifs.is_open()) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Json file doesn't exist");
        return false;
    }
    if (!parseFromStream(reader, ifs, &root, &errors)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Parsing json file failed");
        return false;
    }
    ifs.close();

    Json::Value::Members members = root.getMemberNames();
    for (auto iter = members.begin(); iter != members.end(); iter++) {
        std::string type = *iter;
        Json::Value value = root[type];

        if (type == BatteryStatsUtils::TYPE_CPU_CLUSTER) {
            clusterNum_ = value.size();
            STATS_HILOGD(STATS_MODULE_SERVICE, "Read cluster num: %{public}d", clusterNum_);
        }

        if (type.find(BatteryStatsUtils::TYPE_CPU_SPEED) != std::string::npos) {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Read speed num: %{public}d", value.size());
            speedNum_.push_back(value.size());
        }

        if (value.isArray()) {
            std::vector<double> listValues;
            for (uint16_t i = 0; i < value.size(); i++) {
                listValues.push_back(value[i].asDouble());
                STATS_HILOGD(STATS_MODULE_SERVICE, "Read list value: %{public}lf of %{public}s", value[i].asDouble(),
                    type.c_str());
            }
            averageVecMap_.insert(std::pair<std::string, std::vector<double>>(type, listValues));
        } else {
            double singleValue = value.asDouble();
            averageMap_.insert(std::pair<std::string, double>(type, singleValue));
            STATS_HILOGD(STATS_MODULE_SERVICE, "Read single value: %{public}lf of %{public}s", singleValue,
                type.c_str());
        }
    }
    STATS_HILOGE(STATS_MODULE_SERVICE, "Load power average json file complete");
    return true;
}

double BatteryStatsParser::GetAveragePower(std::string type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double average = 0.0;
    auto iter = averageMap_.find(type);
    if (iter != averageMap_.end()) {
        average = iter->second;
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got average power: %{public}lf of %{public}s", average, type.c_str());
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "No average power of %{public}s found, return 0", type.c_str());
    }
    return average;
}

double BatteryStatsParser::GetAveragePower(std::string type, uint16_t level)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double average = 0.0;
    auto iter = averageVecMap_.find(type);
    if (iter != averageVecMap_.end()) {
        if (level < iter->second.size()) {
            average = iter->second[level];
        }
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got average power: %{public}lf of %{public}s", average, type.c_str());
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "No average power of %{public}s found, return 0", type.c_str());
    }
    return average;
}

uint16_t BatteryStatsParser::GetClusterNum()
{
    STATS_HILOGD(STATS_MODULE_SERVICE, "Got cluster number: %{public}d", clusterNum_);
    return clusterNum_;
}
} // namespace PowerMgr
} // namespace OHOS