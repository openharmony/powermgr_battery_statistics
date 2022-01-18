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

#ifndef BATTERY_STATS_LISTENER_H
#define BATTERY_STATS_LISTENER_H

#include "hisysevent_subscribe_callback_native.h"
#include "json/json.h"

#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsListener : public HiviewDFX::HiSysEventSubscribeCallBackNative {
public:
    explicit BatteryStatsListener() : HiviewDFX::HiSysEventSubscribeCallBackNative() {}
    virtual ~BatteryStatsListener() {}
    void OnHandle(const std::string& domain, const std::string& eventName, const int eventType,
            const std::string& eventDetail) override;
    void OnServiceDied() override;
private:
    void processPhoneEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void processWakelockEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void processDispalyEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void processBatteryEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void processThermalEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void processWorkschedulerEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void processFlashlightEvent(StatsUtils::StatsData& data, const Json::Value& root);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS 4_LISTENER_H