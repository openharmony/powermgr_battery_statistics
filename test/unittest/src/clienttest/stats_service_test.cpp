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

#include "stats_service_test.h"

#include <if_system_ability_manager.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>
#include "battery_stats_parser.h"
#define private public
#define protected public
#include "battery_stats_service.h"
#undef private
#undef protected
#include "config_policy_utils.h"
#include "gtest/gtest-message.h"
#include "gtest/gtest-test-part.h"
#include "gtest/gtest.h"
#include "gtest/hwext/gtest-ext.h"
#include "gtest/hwext/gtest-tag.h"
#include "iremote_object.h"
#include "ostream"
#include "refbase.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;

char* GetOneCfgFile(const char *pathSuffix, char *buf, unsigned int bufLength)
{
    return nullptr;
}

void StatsServiceTest::SetUpTestCase()
{
    GTEST_LOG_(INFO) << __func__;
}

void StatsServiceTest::TearDownTestCase()
{
    GTEST_LOG_(INFO) << __func__;
}

void StatsServiceTest::SetUp()
{
    GTEST_LOG_(INFO) << __func__;
}

void StatsServiceTest::TearDown()
{
    GTEST_LOG_(INFO) << __func__;
}

namespace {
/**
 * @tc.name: StatsServiceTest_001
 * @tc.desc: test BatteryStatsService service ready.
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceTest, StatsServiceTest_001, TestSize.Level0)
{
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(sam != nullptr) << "StatsServiceTest_001 fail to get GetSystemAbilityManager";
    sptr<IRemoteObject> remoteObject_ = sam->CheckSystemAbility(POWER_MANAGER_BATT_STATS_SERVICE_ID);
    ASSERT_TRUE(remoteObject_ != nullptr) << "GetSystemAbility failed.";
}

/**
 * @tc.name: StatsServiceTest_002
 * @tc.desc: test OnStart
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceTest, StatsServiceTest_002, TestSize.Level0)
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->OnStart();
    statsService->OnStart();
    bool ret = statsService->IsServiceReady();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: StatsServiceTest_003
 * @tc.desc: test OnStop
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceTest, StatsServiceTest_003, TestSize.Level0)
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->OnAddSystemAbility(DFX_SYS_EVENT_SERVICE_ABILITY_ID, "");
    statsService->OnAddSystemAbility(COMMON_EVENT_SERVICE_ID, "");
    statsService->OnStart();
    statsService->OnStop();
    bool ret = statsService->IsServiceReady();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: StatsServiceTest_004
 * @tc.desc: test Dump
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceTest, StatsServiceTest_004, TestSize.Level0)
{
    auto statsService = BatteryStatsService::GetInstance();
    int32_t fd = 1;
    std::vector<std::u16string> vec;
    statsService->isBootCompleted_ = true;
    int32_t ret = statsService->Dump(fd, vec);
    EXPECT_EQ(ret, OHOS::ERR_OK);
}

/**
 * @tc.name: StatsParserTest_001
 * @tc.desc: test Init
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceTest, StatsParserTest_001, TestSize.Level0)
{
    auto parser = std::make_shared<BatteryStatsParser>();
    bool ret = parser->Init();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: StatsParserTest_002
 * @tc.desc: test DumpInfo
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceTest, StatsParserTest_002, TestSize.Level0)
{
    auto parser = std::make_shared<BatteryStatsParser>();
    parser->Init();
    std::string result = "";
    parser->DumpInfo(result);
    EXPECT_TRUE(result != "");
}
}