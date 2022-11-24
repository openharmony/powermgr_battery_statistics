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

#include "batterystats_fuzzer.h"

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <ctime>
#include "securec.h"
#include "battery_stats_client.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::PowerMgr;

namespace {
auto& g_batterystatsClient = BatteryStatsClient::GetInstance();
constexpr size_t DATANUM = 4;
constexpr int32_t INDEX_0 = 0;
}

static void GetBatteryStats(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_batterystatsClient.GetBatteryStats();
}

static void SetOnBattery(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_batterystatsClient.SetOnBattery(type[0]);
}

static void GetAppStatsMah(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_batterystatsClient.GetAppStatsMah(type[0]);
}

static void GetAppStatsPercent(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_batterystatsClient.GetAppStatsPercent(type[0]);
}

static void GetPartStatsMah(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_batterystatsClient.GetPartStatsMah(static_cast <BatteryStatsInfo::ConsumptionType>(type[0]));
}

static void GetPartStatsPercent(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_batterystatsClient.GetPartStatsPercent(static_cast <BatteryStatsInfo::ConsumptionType>(type[0]));
}

static void GetTotalTimeSecond(const uint8_t* data, size_t size)
{
    int32_t type[1];
    size_t idSize = 4;
    int32_t uid[1];
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }
    if (size <= (idSize + DATANUM) || (memcpy_s(uid, sizeof(uid), (data + DATANUM), idSize) != EOK)) {
        uid[INDEX_0] = type[INDEX_0];
    }

    g_batterystatsClient.GetTotalTimeSecond(static_cast <OHOS::PowerMgr::StatsUtils::StatsType>(type[0]), uid[0]);
}

static void GetTotalDataBytes(const uint8_t* data, size_t size)
{
    int32_t type[1];
    size_t idSize = 4;
    int32_t uid[1];
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }
    if (size <= (idSize + DATANUM) || (memcpy_s(uid, sizeof(uid), (data + DATANUM), idSize) != EOK)) {
        uid[INDEX_0] = type[INDEX_0];
    }

    g_batterystatsClient.GetTotalDataBytes(static_cast <OHOS::PowerMgr::StatsUtils::StatsType>(type[0]), uid[0]);
}

static void Reset(const uint8_t* data)
{
    int32_t type[1];
    int32_t idSize = 4;
    if ((memcpy_s(type, sizeof(type), data, idSize)) != EOK) {
        return;
    }

    g_batterystatsClient.Reset();
}

namespace OHOS {
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    int32_t idSize = 4;
    int32_t cond[1];
    if (static_cast<int32_t>(size) > idSize) {
        if ((memcpy_s(cond, sizeof(cond), data, idSize)) != EOK) {
            return false;
        }
        std::random_device rd;
        std::default_random_engine engine(rd());
        std::uniform_int_distribution<int32_t> randomNum(static_cast<int32_t>(ApiNumber::NUM_ZERO),
            static_cast<int32_t>(ApiNumber::NUM_END) - 1);
        int32_t number = randomNum(engine);

        switch (static_cast<ApiNumber>(number)) {
            case ApiNumber::NUM_ZERO:
                GetBatteryStats(data);
                break;
            case ApiNumber::NUM_ONE:
                SetOnBattery(data);
                break;
            case ApiNumber::NUM_TWO:
                GetAppStatsMah(data);
                break;
            case ApiNumber::NUM_THREE:
                GetAppStatsPercent(data);
                break;
            case ApiNumber::NUM_FOUR:
                GetPartStatsMah(data);
                break;
            case ApiNumber::NUM_FIVE:
                GetPartStatsPercent(data);
                break;
            case ApiNumber::NUM_SIX:
                GetTotalTimeSecond(data, size);
                break;
            case ApiNumber::NUM_SEVEN:
                GetTotalDataBytes(data, size);
                break;
            case ApiNumber::NUM_EIGHT:
                Reset(data);
                break;
            case ApiNumber::NUM_NINE:
                g_batterystatsClient.GetLastError();
                break;
            default:
                break;
        }
    }

    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

