/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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
#include "stats_log.h"

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
    STATS_HILOGI(LABEL_TEST, "StatsServiceTest_001 start");
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(sam != nullptr) << "StatsServiceTest_001 fail to get GetSystemAbilityManager";
    sptr<IRemoteObject> remoteObject_ = sam->CheckSystemAbility(POWER_MANAGER_BATT_STATS_SERVICE_ID);
    ASSERT_TRUE(remoteObject_ != nullptr) << "GetSystemAbility failed.";
    STATS_HILOGI(LABEL_TEST, "StatsServiceTest_001 end");
}

/**
 * @tc.name: StatsServiceTest_002
 * @tc.desc: test OnStart
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceTest, StatsServiceTest_002, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceTest_002 start");
    auto statsService = BatteryStatsService::GetInstance();
    EXPECT_TRUE(statsService != nullptr);

    statsService->OnStart();
    statsService->OnStart();
    bool ret = statsService->IsServiceReady();
    EXPECT_FALSE(ret);
    STATS_HILOGI(LABEL_TEST, "StatsServiceTest_002 end");
}

/**
 * @tc.name: StatsServiceTest_003
 * @tc.desc: test OnStop
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceTest, StatsServiceTest_003, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceTest_003 start");
    auto statsService = BatteryStatsService::GetInstance();
    EXPECT_TRUE(statsService != nullptr);

    statsService->OnAddSystemAbility(DFX_SYS_EVENT_SERVICE_ABILITY_ID, "");
    statsService->OnAddSystemAbility(COMMON_EVENT_SERVICE_ID, "");
    statsService->OnStart();
    statsService->OnStop();
    bool ret = statsService->IsServiceReady();
    EXPECT_FALSE(ret);
    STATS_HILOGI(LABEL_TEST, "StatsServiceTest_003 end");
}

/**
 * @tc.name: StatsServiceTest_004
 * @tc.desc: test Dump
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceTest, StatsServiceTest_004, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsServiceTest_004 start");
    auto statsService = BatteryStatsService::GetInstance();
    EXPECT_TRUE(statsService != nullptr);

    int32_t fd = 1;
    std::vector<std::u16string> vec;
    statsService->isBootCompleted_ = true;
    int32_t ret = statsService->Dump(fd, vec);
    EXPECT_EQ(ret, OHOS::ERR_OK);
    STATS_HILOGI(LABEL_TEST, "StatsServiceTest_004 end");
}

/**
 * @tc.name: StatsParserTest_001
 * @tc.desc: test Init
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceTest, StatsParserTest_001, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsParserTest_001 start");
    auto parser = std::make_shared<BatteryStatsParser>();
    EXPECT_TRUE(parser != nullptr);

    bool ret = parser->Init();
    EXPECT_TRUE(ret);
    STATS_HILOGI(LABEL_TEST, "StatsParserTest_001 end");
}

/**
 * @tc.name: StatsParserTest_002
 * @tc.desc: test DumpInfo
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceTest, StatsParserTest_002, TestSize.Level0)
{
    STATS_HILOGI(LABEL_TEST, "StatsParserTest_002 start");
    auto parser = std::make_shared<BatteryStatsParser>();
    EXPECT_TRUE(parser != nullptr);

    parser->Init();
    std::string result = "";
    parser->DumpInfo(result);
    EXPECT_TRUE(result != "");
    STATS_HILOGI(LABEL_TEST, "StatsParserTest_002 end");
}
}