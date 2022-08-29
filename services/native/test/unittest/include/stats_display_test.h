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

#ifndef STATS_DISPLAY_TEST_H
#define STATS_DISPLAY_TEST_H

#include <gtest/gtest.h>

namespace OHOS {
namespace PowerMgr {
constexpr int32_t SECOND_PER_HOUR = 3600;
constexpr int32_t WAIT_TIME = 1;
class StatsDisplayTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_DISPLAY_TEST_H