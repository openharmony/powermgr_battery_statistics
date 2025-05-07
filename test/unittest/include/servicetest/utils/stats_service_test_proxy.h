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
#include "hilog/log.h"

namespace OHOS {
namespace PowerMgr {
class StatsServiceTestProxy {
public:
    StatsServiceTestProxy(const sptr<BatteryStatsService>& service);
    ~StatsServiceTestProxy() = default;

    int32_t GetBatteryStatsIpc(ParcelableBatteryStatsList& batteryStats, int32_t& tempError);
    int32_t SetOnBatteryIpc(bool isOnBattery);
    int32_t GetAppStatsMahIpc(int32_t uid, double& appStatsMah, int32_t& tempError);
    int32_t GetAppStatsPercentIpc(int32_t uid, double& appStatsPercent, int32_t& tempError);
    int32_t GetPartStatsMahIpc(int32_t type, double& partStatsMah, int32_t& tempError);
    int32_t GetPartStatsPercentIpc(int32_t type, double& partStatsPercent, int32_t& tempError);
    int32_t GetTotalTimeSecondIpc(int32_t statsType, int32_t uid, uint64_t& totalTimeSecond);
    int32_t GetTotalDataBytesIpc(int32_t statsType, int32_t uid, uint64_t& totalDataBytes);
    int32_t ResetIpc();
    int32_t ShellDumpIpc(const std::vector<std::string>& args, uint32_t argc, std::string& dumpShell);

private:
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0xD000F00, "StatsTest"};
    sptr<BatteryStatsStub> stub_ {nullptr};
    const int VECTOR_MAX_SIZE = 102400;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_SERVICE_TEST_PROXY_H