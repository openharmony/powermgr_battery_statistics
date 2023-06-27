/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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
#include "battery_stats_client.h"
#include "battery_stats_service.h"
#include "message_parcel.h"
#include "securec.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::PowerMgr;

namespace {
auto& g_batterystatsClient = BatteryStatsClient::GetInstance();
sptr<BatteryStatsService> g_service = nullptr;
const int32_t REWIND_READ_DATA = 0;
}

static int32_t GetInt32(const uint8_t* data, size_t size)
{
    int32_t value = 0;
    if (size < sizeof(value)) {
        return value;
    }
    if (memcpy_s(&value, sizeof(value), data, sizeof(value)) != EOK) {
        return value;
    }
    return value;
}

static void GetBatteryStats([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterystatsClient.GetBatteryStats();
}

static void SetOnBattery(const uint8_t* data, size_t size)
{
    int32_t onBattery = GetInt32(data, size);
    g_batterystatsClient.SetOnBattery(onBattery);
}

static void GetAppStatsMah(const uint8_t* data, size_t size)
{
    int32_t uid = GetInt32(data, size);
    g_batterystatsClient.GetAppStatsMah(uid);
}

static void GetAppStatsPercent(const uint8_t* data, size_t size)
{
    int32_t uid = GetInt32(data, size);
    g_batterystatsClient.GetAppStatsPercent(uid);
}

static void GetPartStatsMah(const uint8_t* data, size_t size)
{
    int32_t type = GetInt32(data, size);
    g_batterystatsClient.GetPartStatsMah(static_cast<BatteryStatsInfo::ConsumptionType>(type));
}

static void GetPartStatsPercent(const uint8_t* data, size_t size)
{
    int32_t type = GetInt32(data, size);
    g_batterystatsClient.GetPartStatsPercent(static_cast<BatteryStatsInfo::ConsumptionType>(type));
}

static void GetTotalTimeSecond(const uint8_t* data, size_t size)
{
    int32_t number = GetInt32(data, size);
    g_batterystatsClient.GetTotalTimeSecond(static_cast<OHOS::PowerMgr::StatsUtils::StatsType>(number), number);
}

static void GetTotalDataBytes(const uint8_t* data, size_t size)
{
    int32_t number = GetInt32(data, size);
    g_batterystatsClient.GetTotalDataBytes(static_cast<OHOS::PowerMgr::StatsUtils::StatsType>(number), number);
}

static void Reset([[maybe_unused]] const uint8_t* data, [[maybe_unused]] size_t size)
{
    g_batterystatsClient.Reset();
}

static void BatteryStatsServiceStub(const uint8_t* data, size_t size)
{
    uint32_t code;
    if (size < sizeof(code)) {
        return;
    }
    if (memcpy_s(&code, sizeof(code), data, sizeof(code)) != EOK) {
        return;
    }

    MessageParcel datas;
    datas.WriteInterfaceToken(BatteryStatsService::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(REWIND_READ_DATA);
    MessageParcel reply;
    MessageOption option;
    if (g_service == nullptr) {
        g_service = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
        g_service->OnStart();
    }
    g_service->OnRemoteRequest(code, datas, reply, option);
}

static std::vector<std::function<void(const uint8_t*, size_t)>> fuzzFunc = {
    &GetBatteryStats,
    &SetOnBattery,
    &GetAppStatsMah,
    &GetAppStatsPercent,
    &GetPartStatsMah,
    &GetPartStatsPercent,
    &GetTotalTimeSecond,
    &GetTotalDataBytes,
    &Reset
};

namespace OHOS {
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int32_t> randomNum(0, fuzzFunc.size() - 1);
    int32_t number = randomNum(engine);
    fuzzFunc[number](data, size);
    BatteryStatsServiceStub(data, size);
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

