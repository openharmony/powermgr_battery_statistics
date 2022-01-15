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

#include "battery_stats_service_test.h"

#include <csignal>
#include <iostream>

#include <if_system_ability_manager.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <string_ex.h>
#include <system_ability_definition.h>

#include "battery_stats_client.h"
#include "battery_stats_service.h"
#include "stats_common.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

void BatteryStatsServiceTest::SetUpTestCase(void)
{
}

void BatteryStatsServiceTest::TearDownTestCase(void)
{
}

void BatteryStatsServiceTest::SetUp(void)
{
}

void BatteryStatsServiceTest::TearDown(void)
{
}

/**
 * @tc.name: BatteryStatsServiceTest_001
 * @tc.desc: Test BatteryStatsService service ready.
 * @tc.type: FUNC
 */
HWTEST_F (BatteryStatsServiceTest, BatteryStatsServiceTest_001, TestSize.Level0)
{
    GTEST_LOG_(INFO) << " BatteryStatsServiceTest_001: test start";
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    ASSERT_TRUE(sam != nullptr) << "BatteryStatsServiceTest_001 fail to get GetSystemAbilityManager";
    sptr<IRemoteObject> remoteObject_ = sam->CheckSystemAbility(POWER_MANAGER_BATT_STATS_SERVICE_ID);
    ASSERT_TRUE(remoteObject_ != nullptr) << "GetSystemAbility failed.";
    GTEST_LOG_(INFO) << " BatteryStatsServiceTest_001: test end";
}