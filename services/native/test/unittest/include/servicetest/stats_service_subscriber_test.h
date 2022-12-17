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

#ifndef STATS_SERVICE_SUBSCRIBER_TEST_H
#define STATS_SERVICE_SUBSCRIBER_TEST_H

#include "battery_info.h"
#include "battery_stats_service.h"
#include "stats_test.h"

namespace OHOS {
namespace PowerMgr {
class StatsServiceSubscriberTest : public testing::Test, public StatsTest {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void PublishChangedEvent(const sptr<BatteryStatsService>& service, const std::string& action);
    void SetCapacity(const int32_t capacity)
    {
        capacity_ = capacity;
    }

    void SetPluggedType(const BatteryPluggedType pluggedType)
    {
        pluggedType_ = pluggedType;
    }

    void BatteryInfoReset()
    {
        capacity_ = 0;
        pluggedType_ = BatteryPluggedType::PLUGGED_TYPE_NONE;
    }
private:
    int32_t capacity_;
    BatteryPluggedType pluggedType_;
};
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_SERVICE_SUBSCRIBER_TEST_H