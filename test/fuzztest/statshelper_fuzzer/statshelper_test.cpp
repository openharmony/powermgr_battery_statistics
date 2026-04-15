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

constexpr size_t MIN_INPUT_SIZE = 4;
constexpr size_t MIN_COMBINED_TEST_SIZE = 15;
constexpr size_t COMBINED_SECTION_COUNT = 5;

enum class ActiveTimerOp : uint8_t {
    START = 0,
    STOP,
    GET_TIME,
    ADD_TIME,
    RESET,
    OP_COUNT
};

enum class CounterOp : uint8_t {
    ADD_COUNT = 0,
    GET_COUNT,
    RESET,
    OP_COUNT
};

enum class TestCase : uint8_t {
    RESET_IPC = 0,
    SHELL_DUMP_IPC,
    STATIC_METHODS,
    ACTIVE_TIMER,
    COUNTER,
    COMBINED,
    CASE_COUNT
};

// 辅助函数：从数据中提取值
template<typename T>
T ExtractValue(const uint8_t* data, size_t size, size_t& offset)
{
    if (offset + sizeof(T) > size) {
        return T{};
    }
    T value;
    memcpy_s(&value, sizeof(T), data + offset, sizeof(T));
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

        switch (static_cast<ActiveTimerOp>(operation % static_cast<uint8_t>(ActiveTimerOp::OP_COUNT))) {
            case ActiveTimerOp::START:
                timer.StartRunning();
                break;
            case ActiveTimerOp::STOP:
                timer.StopRunning();
                break;
            case ActiveTimerOp::GET_TIME:
                timer.GetRunningTimeMs();
                break;
            case ActiveTimerOp::ADD_TIME:
                if (offset + sizeof(int64_t) <= size) {
                    int64_t activeTime = ExtractValue<int64_t>(data, size, offset);
                    timer.AddRunningTimeMs(activeTime);
                }
                break;
            case ActiveTimerOp::RESET:
                timer.Reset();
                break;
            default:
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

        switch (static_cast<CounterOp>(operation % static_cast<uint8_t>(CounterOp::OP_COUNT))) {
            case CounterOp::ADD_COUNT:
                if (offset + sizeof(int64_t) <= size) {
                    int64_t count = ExtractValue<int64_t>(data, size, offset);
                    counter.AddCount(count);
                }
                break;
            case CounterOp::GET_COUNT:
                counter.GetCount();
                break;
            case CounterOp::RESET:
                counter.Reset();
                break;
            default:
                break;
        }
    }
}
} // namespace

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_INPUT_SIZE) {
        return 0;
    }

    // 使用第一个字节决定测试哪个部分
    uint8_t testSelector = data[0];
    const uint8_t* testData = data + 1;
    size_t testSize = size - 1;

    switch (static_cast<TestCase>(testSelector % static_cast<uint8_t>(TestCase::CASE_COUNT))) {
        case TestCase::RESET_IPC:
            // 测试RESET IPC
            g_serviceTest.TestStatsServiceStub(
                static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_RESET_IPC),
                testData, testSize);
            break;
        case TestCase::SHELL_DUMP_IPC:
            // 测试SHELL_DUMP IPC
            g_serviceTest.TestStatsServiceStub(
                static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_SHELL_DUMP_IPC),
                testData, testSize);
            break;
        case TestCase::STATIC_METHODS:
            // 测试StatsHelper静态方法
            TestStatsHelperStaticMethods(testData, testSize);
            break;
        case TestCase::ACTIVE_TIMER:
            // 测试ActiveTimer
            TestActiveTimer(testData, testSize);
            break;
        case TestCase::COUNTER:
            // 测试Counter
            TestCounter(testData, testSize);
            break;
        case TestCase::COMBINED:
            // 综合测试：所有功能
            if (testSize >= MIN_COMBINED_TEST_SIZE) {
                size_t part = testSize / COMBINED_SECTION_COUNT;
                g_serviceTest.TestStatsServiceStub(
                    static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_RESET_IPC),
                    testData, part);
                g_serviceTest.TestStatsServiceStub(
                    static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_SHELL_DUMP_IPC),
                    testData + part, part);
                TestStatsHelperStaticMethods(
                    testData + static_cast<size_t>(TestCase::STATIC_METHODS) * part, part);
                TestActiveTimer(
                    testData + static_cast<size_t>(TestCase::ACTIVE_TIMER) * part, part);
                TestCounter(
                    testData + static_cast<size_t>(TestCase::COUNTER) * part,
                    testSize - static_cast<size_t>(TestCase::COUNTER) * part);
            }
            break;
        default:
            break;
    }

    return 0;
}
