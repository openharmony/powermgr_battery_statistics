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

#include "battery_stats_listener.h"

#include <string>
#include <strstream>

#include "json/json.h"

#include "stats_hilog_wrapper.h"
#include "stats_utils.h"
#include "battery_stats_service.h"

namespace OHOS {
namespace PowerMgr {
void BatteryStatsListener::OnHandle(const std::string& domain, const std::string& eventName,
    const int eventType, const std::string& eventDetail)
{
    STATS_HILOGD(STATS_MODULE_SERVICE, "EventDetail: %{public}s", eventDetail.c_str());
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
    auto detector = statsService->GetBatteryStatsDetector();
    StatsUtils::StatsData data;
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    std::istrstream is(eventDetail.c_str());
    if(parseFromStream(reader, is, &root, &errors)){
        StatsUtils::StatsType type = StatsUtils::StatsType(stoi(root["TYPE"].asString()));
        StatsUtils::StatsState state = StatsUtils::StatsState(stoi(root["STATE"].asString()));
        data.type = type;
        data.state = state;
        if (!root["LEVEL"].asString().empty()) {
            data.level = stoi(root["LEVEL"].asString());
        }
        if (!root["UID"].asString().empty()) {
            data.uid = stoi(root["UID"].asString());
        }
        if (!root["TIME"].asString().empty()) {
            data.time = stoi(root["TIME"].asString());
        }
        if (!root["TRAFFIC"].asString().empty()) {
            data.traffic = stoi(root["TRAFFIC"].asString());
        }
        detector->HandleStatsChangedEvent(data);
    } else{
        STATS_HILOGE(STATS_MODULE_SERVICE, "Parse hisysevent data failed");
    }
}

void BatteryStatsListener::OnServiceDied()
{
    STATS_HILOGE(STATS_MODULE_SERVICE, "Service disconnected");
    exit(0);
}
} // namespace PowerMgr
} // namespace OHOS