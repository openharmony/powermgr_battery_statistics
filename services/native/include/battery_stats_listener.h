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

#include <memory>

#include <cJSON.h>
#include "hisysevent_listener.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsListener : public HiviewDFX::HiSysEventListener {
public:
    explicit BatteryStatsListener() : HiviewDFX::HiSysEventListener() {}
    virtual ~BatteryStatsListener() {}
    void OnEvent(std::shared_ptr<HiviewDFX::HiSysEventRecord> sysEvent) override;
    void OnServiceDied() override;
private:
    void ProcessHiSysEventInternal(StatsUtils::StatsData& data, const std::string& eventName, const cJSON* root);
    void ProcessHiSysEvent(const std::string& eventName, const cJSON* root);
    void ProcessPhoneEvent(StatsUtils::StatsData& data, const cJSON* root, const std::string& eventName);
    void ProcessWakelockEvent(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessWakelockEventInternal(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessDisplayEvent(StatsUtils::StatsData& data, const cJSON* root, const std::string& eventName);
    void ProcessBatteryEvent(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessThermalEvent(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessThermalEventInternal(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessPowerWorkschedulerEvent(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessOthersWorkschedulerEventInternal(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessOthersWorkschedulerEvent(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessWorkschedulerEvent(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessFlashlightEvent(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessCameraEvent(StatsUtils::StatsData& data, const cJSON* root, const std::string& eventName);
    void ProcessAudioEvent(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessSensorEvent(StatsUtils::StatsData& data, const cJSON* root, const std::string& eventName);
    void ProcessGnssEvent(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessBluetoothBrEvent(StatsUtils::StatsData& data, const cJSON* root, const std::string& eventName);
    void ProcessBluetoothBleEvent(StatsUtils::StatsData& data, const cJSON* root, const std::string& eventName);
    void ProcessBluetoothEvent(StatsUtils::StatsData& data, const cJSON* root, const std::string& eventName);
    void ProcessWifiEvent(StatsUtils::StatsData& data, const cJSON* root, const std::string& eventName);
    void ProcessDistributedSchedulerEventInternal(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessDistributedSchedulerEvent(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessAlarmEvent(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessDisplayDebugInfo(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessDisplayDebugInfoInternal(StatsUtils::StatsData& data, const cJSON* root);
    void ProcessPhoneDebugInfo(StatsUtils::StatsData& data, const cJSON* root);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS 4_LISTENER_H