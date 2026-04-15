/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

/* This files contains faultlog fuzzer test modules. */

#define FUZZ_PROJECT_NAME "utils_fuzzer"

#include "ibattery_stats.h"
#include "batterystats_fuzzer.h"
#include "stats_helper.h"

using namespace OHOS::PowerMgr;

namespace {
BatteryStatsFuzzerTest g_serviceTest;

// 辅助函数：从数据中提取值
template<typename T>
T ExtractValue(const uint8_t* data, size_t size, size_t& offset)
{
    if (offset + sizeof(T) > size) {
        return T{};
    }
    T value;
    memcpy(&value, data + offset, sizeof(T));
    offset += sizeof(T);
    return value;
}

// 测试StatsHelper静态方法
void TestStatsHelperStaticMethods(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(bool)) {
        return;
    }

    size_t offset = 0;
    bool onBattery = ExtractValue<bool>(data, size, offset);
    StatsHelper::SetOnBattery(onBattery);
    
    if (offset < size) {
        bool screenOff = ExtractValue<bool>(data, size, offset);
        StatsHelper::SetScreenOff(screenOff);
    }
    
    // 调用所有getter方法
    StatsHelper::GetOnBatteryBootTimeMs();
    StatsHelper::GetOnBatteryUpTimeMs();
    StatsHelper::IsOnBattery();
    StatsHelper::IsOnBatteryScreenOff();
    StatsHelper::GetBootTimeMs();
    StatsHelper::GetUpTimeMs();
}

// 测试ActiveTimer
void TestActiveTimer(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 1) {
        return;
    }

    StatsHelper::ActiveTimer timer;
    size_t offset = 0;

    while (offset < size) {
        uint8_t operation = ExtractValue<uint8_t>(data, size, offset);
        
        switch (operation % 5) {
            case 0:
                timer.StartRunning();
                break;
            case 1:
                timer.StopRunning();
                break;
            case 2:
                timer.GetRunningTimeMs();
                break;
            case 3:
                if (offset + sizeof(int64_t) <= size) {
                    int64_t activeTime = ExtractValue<int64_t>(data, size, offset);
                    timer.AddRunningTimeMs(activeTime);
                }
                break;
            case 4:
                timer.Reset();
                break;
        }
    }
}

// 测试Counter
void TestCounter(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 1) {
        return;
    }

    StatsHelper::Counter counter;
    size_t offset = 0;

    // 设置电池状态
    if (offset < size) {
        bool onBattery = ExtractValue<bool>(data, size, offset);
        StatsHelper::SetOnBattery(onBattery);
    }

    while (offset < size) {
        uint8_t operation = ExtractValue<uint8_t>(data, size, offset);
        
        switch (operation % 3) {
            case 0:
                if (offset + sizeof(int64_t) <= size) {
                    int64_t count = ExtractValue<int64_t>(data, size, offset);
                    counter.AddCount(count);
                }
                break;
            case 1:
                counter.GetCount();
                break;
            case 2:
                counter.Reset();
                break;
        }
    }
}
} // namespace

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 4) {
        return 0;
    }

    // 使用第一个字节决定测试哪个部分
    uint8_t testSelector = data[0];
    const uint8_t* testData = data + 1;
    size_t testSize = size - 1;

    switch (testSelector % 6) {
        case 0:
            // 测试RESET IPC
            g_serviceTest.TestStatsServiceStub(
                static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_RESET_IPC), 
                testData, testSize);
            break;
        case 1:
            // 测试SHELL_DUMP IPC
            g_serviceTest.TestStatsServiceStub(
                static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_SHELL_DUMP_IPC), 
                testData, testSize);
            break;
        case 2:
            // 测试StatsHelper静态方法
            TestStatsHelperStaticMethods(testData, testSize);
            break;
        case 3:
            // 测试ActiveTimer
            TestActiveTimer(testData, testSize);
            break;
        case 4:
            // 测试Counter
            TestCounter(testData, testSize);
            break;
        case 5:
            // 综合测试：所有功能
            if (testSize >= 15) {
                size_t part = testSize / 5;
                g_serviceTest.TestStatsServiceStub(
                    static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_RESET_IPC), 
                    testData, part);
                g_serviceTest.TestStatsServiceStub(
                    static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_SHELL_DUMP_IPC), 
                    testData + part, part);
                TestStatsHelperStaticMethods(testData + 2*part, part);
                TestActiveTimer(testData + 3*part, part);
                TestCounter(testData + 4*part, testSize - 4*part);
            }
            break;
    }
    
    return 0;
}