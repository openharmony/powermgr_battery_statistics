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

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "battery_stats_client.h"

#define FUZZ_PROJECT_NAME "dump_fuzzer"

using namespace OHOS::PowerMgr;

namespace {
constexpr size_t MAX_INPUT_SIZE = 10240;        // Maximum input size to prevent excessive memory allocation
constexpr size_t MAX_STRING_LEN = 1024;         // Maximum length for a single string parameter
constexpr size_t MAX_PARAM_COUNT = 10;          // Maximum number of parameters
constexpr size_t MIN_PARAM_SIZE = sizeof(uint32_t);  // Minimum size for parameter length field

// Valid commands supported by BatteryStatsDumper
const std::vector<std::string> VALID_COMMANDS = {
    "-h",
    "-batterystats",
    "-poweraverage"
};

/**
 * * Parse fuzzer input data into a vector of string arguments
 * Format: [len1][data1][len2][data2]...
 * Each len is a uint32_t indicating the length of the following data
 */
std::vector<std::string> ParseArgsFromData(const uint8_t* data, size_t size)
{
    std::vector<std::string> args;
    
    // Check input validity
    if (data == nullptr || size == 0 || size > MAX_INPUT_SIZE) {
        return args;
    }
    
    size_t offset = 0;
    while (offset + MIN_PARAM_SIZE <= size && args.size() < MAX_PARAM_COUNT) {
        // Read string length
        uint32_t strLen = *reinterpret_cast<const uint32_t*>(data + offset);
        offset += sizeof(uint32_t);
        
        // Validate string length to prevent integer overflow and excessive allocation
        if (strLen == 0 || strLen > MAX_STRING_LEN || offset + strLen > size) {
            break;
        }
        
        // Construct string safely
        std::string arg(reinterpret_cast<const char*>(data + offset), strLen);
        args.emplace_back(arg);
        offset += strLen;
    }
    
    return args;
}

/**
 * Test BatteryStatsClient::Dump with structured input
 */
void TestDumpWithStructuredInput(const uint8_t* data, size_t size)
{
    std::vector<std::string> args = ParseArgsFromData(data, size);
    auto& client = BatteryStatsClient::GetInstance();
    std::string result = client.Dump(args);
}

/**
 * Test BatteryStatsClient::Dump with raw string input
 */
void TestDumpWithRawString(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0 || size > MAX_STRING_LEN) {
        return;
    }
    
    std::vector<std::string> args;
    std::string arg(reinterpret_cast<const char*>(data), size);
    args.emplace_back(arg);
    
    auto& client = BatteryStatsClient::GetInstance();
    std::string result = client.Dump(args);
}

/**
 * Test BatteryStatsClient::Dump with valid commands
 */
void TestDumpWithValidCommands(const uint8_t* data, size_t size)
{
    auto& client = BatteryStatsClient::GetInstance();
    
    // Test all valid commands individually
    for (const auto& cmd : VALID_COMMANDS) {
        std::vector<std::string> args = {cmd};
        std::string result = client.Dump(args);
    }
    
    // Test command combinations if we have enough data
    if (size > 0) {
        std::vector<std::string> combinedArgs;
        // Use fuzzer input to select which commands to combine
        for (size_t i = 0; i < size && i < VALID_COMMANDS.size(); i++) {
            size_t cmdIndex = data[i] % VALID_COMMANDS.size();
            combinedArgs.emplace_back(VALID_COMMANDS[cmdIndex]);
        }
        if (!combinedArgs.empty()) {
            std::string result = client.Dump(combinedArgs);
        }
    }
}

/**
 * Test BatteryStatsClient::Dump with mixed valid and invalid commands
 */
void TestDumpWithMixedCommands(const uint8_t* data, size_t size)
{
    const size_t MIN_SIZE = 2;
    if (size < MIN_SIZE) {
        return;
    }
    
    auto& client = BatteryStatsClient::GetInstance();
    std::vector<std::string> args;
    
    // Add a valid command
    size_t validIdx = data[0] % VALID_COMMANDS.size();
    args.emplace_back(VALID_COMMANDS[validIdx]);
    
    // Add an invalid command from fuzzer data
    const size_t MIN_ARGS_SIZE = 2;
    const size_t INVALID_CMD_OFFSET = 2;
    size_t invalidLen = std::min(static_cast<size_t>(data[1]), MAX_STRING_LEN);
    if (invalidLen > 0 && size >= MIN_ARGS_SIZE + invalidLen) {
        std::string invalidCmd(reinterpret_cast<const char*>(data + INVALID_CMD_OFFSET), invalidLen);
        args.emplace_back(invalidCmd);
    }
    
    std::string result = client.Dump(args);
}

/**
 * Test BatteryStatsClient::Dump with boundary conditions
 */
void TestDumpBoundary(const uint8_t* data, size_t size)
{
    auto& client = BatteryStatsClient::GetInstance();
    
    // Test empty arguments
    std::vector<std::string> emptyArgs;
    std::string result1 = client.Dump(emptyArgs);
    
    // Test single empty string
    if (size > 0) {
        std::vector<std::string> singleEmpty;
        singleEmpty.emplace_back("");
        std::string result2 = client.Dump(singleEmpty);
    }
    
    // Test maximum number of parameters with valid commands
    if (size >= MAX_PARAM_COUNT) {
        std::vector<std::string> maxArgs;
        for (size_t i = 0; i < MAX_PARAM_COUNT; i++) {
            // Mix valid commands and fuzzer data
            if (i < VALID_COMMANDS.size()) {
                maxArgs.emplace_back(VALID_COMMANDS[i]);
            } else {
                maxArgs.emplace_back(std::string(1, static_cast<char>(data[i % size])));
            }
        }
        std::string result3 = client.Dump(maxArgs);
    }
}

} // namespace

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Validate input parameters
    if (data == nullptr || size == 0) {
        return 0;
    }
    
    // Prevent excessive memory allocation
    if (size > MAX_INPUT_SIZE) {
        return 0;
    }
    
    // Test with structured input (primary test)
    TestDumpWithStructuredInput(data, size);
    
    // Test with raw string input (alternative test)
    TestDumpWithRawString(data, size);
    
    // Test with valid commands to improve coverage
    TestDumpWithValidCommands(data, size);
    
    // Test with mixed valid and invalid commands
    TestDumpWithMixedCommands(data, size);
    
    // Test boundary conditions
    TestDumpBoundary(data, size);
    
    return 0;
}
