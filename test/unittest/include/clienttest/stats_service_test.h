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

#ifndef STATS_SERVICE_TEST_H
#define STATS_SERVICE_TEST_H

#include <gtest/gtest.h>
#include "stats_xcollie.h"

namespace OHOS {
namespace PowerMgr {
class StatsServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

class StatsXCollieTestable : public StatsXCollie {
public:
    StatsXCollieTestable(const std::string &logTag, bool isRecovery)
        : StatsXCollie(logTag, isRecovery) {}

    void CallCancel()
    {
        CancelStatsXCollie();
    }
};
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_SERVICE_TEST_H