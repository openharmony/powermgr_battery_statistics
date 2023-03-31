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

#ifndef BATTERY_STATS_CLIENT_H
#define BATTERY_STATS_CLIENT_H

#include <memory>
#include <mutex>
#include <singleton.h>
#include <string>

#include "iremote_object.h"

#include "battery_stats_errors.h"
#include "battery_stats_info.h"
#include "ibattery_stats.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsClient final : public DelayedRefSingleton<BatteryStatsClient> {
    DECLARE_DELAYED_REF_SINGLETON(BatteryStatsClient);

public:
    DISALLOW_COPY_AND_MOVE(BatteryStatsClient);
    BatteryStatsInfoList GetBatteryStats();
    void SetOnBattery(bool isOnBattery);
    double GetAppStatsMah(const int32_t& uid);
    double GetAppStatsPercent(const int32_t& uid);
    double GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& type);
    double GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& type);
    uint64_t GetTotalTimeSecond(const StatsUtils::StatsType& statsType, const int32_t& uid = StatsUtils::INVALID_VALUE);
    uint64_t GetTotalDataBytes(const StatsUtils::StatsType& statsType, const int32_t& uid = StatsUtils::INVALID_VALUE);
    void Reset();
    std::string Dump(const std::vector<std::string>& args);
    StatsError GetLastError();

#ifndef STATS_SERVICE_DEATH_UT
private:
#endif
    class BatteryStatsDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        BatteryStatsDeathRecipient() = default;
        ~BatteryStatsDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject>& remote);
    private:
        DISALLOW_COPY_AND_MOVE(BatteryStatsDeathRecipient);
    };

    ErrCode Connect();
    StatsError lastError_ {StatsError::ERR_OK};
    sptr<IBatteryStats> proxy_ {nullptr};
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ {nullptr};
    void ResetProxy(const wptr<IRemoteObject>& remote);
    std::mutex mutex_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_CLIENT_H