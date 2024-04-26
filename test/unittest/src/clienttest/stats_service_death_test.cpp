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

#include "stats_service_death_test.h"

#include "battery_stats_client.h"
#include "battery_stats_parser.h"
#include "battery_stats_service.h"
#include "config_policy_utils.h"

using namespace testing::ext;
using namespace OHOS::PowerMgr;
using namespace OHOS;
using namespace std;

char* GetOneCfgFile(const char *pathSuffix, char *buf, unsigned int bufLength)
{
    std::string path = " ";
    char* ret = (char*)path.c_str();
    return ret;
}

namespace {
/**
 * @tc.name: StatsServiceDeathTest_001
 * @tc.desc: test OnRemoteDied function(Input remoteObj is nullptr, function don't reset proxy)
 * @tc.type: FUNC
 * @tc.require: issueI63PI8
 */
HWTEST_F (StatsServiceDeathTest, StatsServiceDeathTest_001, TestSize.Level0)
{
    auto& statsClient = BatteryStatsClient::GetInstance();
    EXPECT_EQ(statsClient.Connect(), ERR_OK);

    std::shared_ptr<IRemoteObject::DeathRecipient> deathRecipient =
        std::make_shared<BatteryStatsClient::BatteryStatsDeathRecipient>();
    wptr<IRemoteObject> remoteObj = nullptr;
    EXPECT_NE(deathRecipient, nullptr);
    deathRecipient->OnRemoteDied(remoteObj);
    EXPECT_NE(statsClient.proxy_, nullptr);
}

/**
 * @tc.name: StatsParserTest_001
 * @tc.desc: test Init
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceDeathTest, StatsParserTest_001, TestSize.Level0)
{
    auto parser = std::make_shared<BatteryStatsParser>();
    bool ret = parser->Init();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: StatsServiceTest_001
 * @tc.desc: test OnStart
 * @tc.type: FUNC
 */
HWTEST_F (StatsServiceDeathTest, StatsServiceTest_001, TestSize.Level0)
{
    auto statsService = BatteryStatsService::GetInstance();
    statsService->OnStart();
    bool ret = statsService->IsServiceReady();
    EXPECT_FALSE(ret);
}
}
