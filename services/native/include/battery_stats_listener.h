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

#ifndef BATTERY_STATS_LISTENER_H
#define BATTERY_STATS_LISTENER_H

#include "hisysevent_subscribe_callback.h"
#include "json/json.h"

#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsListener : public HiviewDFX::HiSysEventSubscribeCallBack {
public:
    explicit BatteryStatsListener() : HiviewDFX::HiSysEventSubscribeCallBack() {}
    virtual ~BatteryStatsListener() {}
    void OnHandle(const std::string& domain, const std::string& eventName, const int eventType,
            const std::string& eventDetail) override;
    void OnServiceDied() override;
private:
    void ProcessHiSysEvent(const Json::Value& root);
    void ProcessPhoneEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessWakelockEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessDispalyEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessBatteryEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessThermalEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessWorkschedulerEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessFlashlightEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessCameraEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessAudioEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessSensorEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessRadioEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessGpsEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessBluetoothEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessWifiEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessDistributedSchedulerEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessAlarmEvent(StatsUtils::StatsData& data, const Json::Value& root);
    void ProcessDispalyDebugInfo(StatsUtils::StatsData& data, const Json::Value& root);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS 4_LISTENER_H