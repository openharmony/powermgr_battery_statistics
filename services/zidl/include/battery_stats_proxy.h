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

#ifndef BATTERY_STATS_PROXY_H
#define BATTERY_STATS_PROXY_H

#include <cstdint>
#include <functional>
#include <iosfwd>
#include <vector>
#include <iremote_proxy.h>
#include <nocopyable.h>
#include "iremote_broker.h"
#include "iremote_object.h"
#include "refbase.h"
#include "ibattery_stats.h"
#include "battery_stats_info.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsProxy : public IRemoteProxy<IBatteryStats> {
public:
    explicit BatteryStatsProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IBatteryStats>(impl) {}
    ~BatteryStatsProxy() = default;
    DISALLOW_COPY_AND_MOVE(BatteryStatsProxy);

    virtual BatteryStatsInfoList GetBatteryStats() override;
    virtual void SetOnBattery(bool isOnBattery) override;
    virtual double GetAppStatsMah(const int32_t& uid) override;
    virtual double GetAppStatsPercent(const int32_t& uid) override;
    virtual double GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& type) override;
    virtual double GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& type) override;
    virtual uint64_t GetTotalTimeSecond(const StatsUtils::StatsType& statsType, const int32_t& uid) override;
    virtual uint64_t GetTotalDataBytes(const StatsUtils::StatsType& statsType, const int32_t& uid) override;
    virtual void Reset() override;
    virtual std::string ShellDump(const std::vector<std::string>& args, uint32_t argc) override;
private:
    static inline BrokerDelegator<BatteryStatsProxy> delegator_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_PROXY_H