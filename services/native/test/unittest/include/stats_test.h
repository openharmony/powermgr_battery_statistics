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

#ifndef STATS_TEST_H
#define STATS_TEST_H

#include <gtest/gtest.h>

#include "battery_stats_parser.h"

namespace OHOS {
namespace PowerMgr {
class StatsTest {
public:
static constexpr int64_t US_PER_HOUR = 3600000000;
static constexpr int32_t MS_PER_HOUR = 3600000;
static constexpr int32_t US_PER_SECOND = 1000000;
static constexpr int32_t US_PER_MS = 1000;
static constexpr int32_t POWER_CONSUMPTION_TRIGGERED_US = 20000;
static constexpr int32_t POWER_CONSUMPTION_DURATION_US = 300000;
static constexpr double DEVIATION_PERCENT_THRESHOLD = 0.05;
static constexpr int64_t TIMER_DURATION_MS = 50;
static constexpr int32_t DEVIATION_TIMER_THRESHOLD = 5;
static std::shared_ptr<BatteryStatsParser> g_statsParser;

static void ParserAveragePowerFile()
{
    if (g_statsParser == nullptr) {
        g_statsParser = std::make_shared<BatteryStatsParser>();
        if (!g_statsParser->Init()) {
            GTEST_LOG_(INFO) << __func__ << ": Battery stats parser initialization failed";
        }
    }
}
};
std::shared_ptr<BatteryStatsParser> StatsTest::g_statsParser = nullptr;
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_TEST_H