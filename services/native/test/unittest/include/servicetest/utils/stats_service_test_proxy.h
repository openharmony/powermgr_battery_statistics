/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef STATS_SERVICE_TEST_PROXY_H
#define STATS_SERVICE_TEST_PROXY_H

#include <cstdint>
#include <vector>

#include "battery_stats_info.h"
#include "battery_stats_service.h"
#include "battery_stats_stub.h"

namespace OHOS {
namespace PowerMgr {
class StatsServiceTestProxy {
public:
    StatsServiceTestProxy(const sptr<BatteryStatsService>& service);
    ~StatsServiceTestProxy() = default;

    BatteryStatsInfoList GetBatteryStats();
    bool SetOnBattery(bool isOnBattery);
    double GetAppStatsMah(const int32_t& uid);
    double GetAppStatsPercent(const int32_t& uid);
    double GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& type);
    double GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& type);
    uint64_t GetTotalTimeSecond(const StatsUtils::StatsType& statsType, const int32_t& uid = StatsUtils::INVALID_VALUE);
    uint64_t GetTotalDataBytes(const StatsUtils::StatsType& statsType, const int32_t& uid = StatsUtils::INVALID_VALUE);
    bool Reset();
    std::string ShellDump(const std::vector<std::string>& args, uint32_t argc);

private:
    sptr<BatteryStatsStub> stub_ {nullptr};
};
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_SERVICE_TEST_PROXY_H