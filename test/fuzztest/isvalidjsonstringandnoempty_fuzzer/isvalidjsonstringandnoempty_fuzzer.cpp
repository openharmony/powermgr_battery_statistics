/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

#include <cJSON.h>

#include "stats_cjson_utils.h"

#define FUZZ_PROJECT_NAME "isvalidjsonstringandnoempty_fuzzer"

using namespace OHOS::PowerMgr::StatsJsonUtils;

namespace {
constexpr size_t MAX_INPUT_SIZE = 10240;
constexpr size_t MIN_INPUT_SIZE = 1;
constexpr size_t MAX_STRING_LEN = 4096;
constexpr size_t MIN_VAR_LENGTH_SIZE = 2;

/**
 * Test IsValidJsonStringAndNoEmpty with nullptr
 */
void TestIsValidJsonStringAndNoEmptyWithNull()
{
    (void)IsValidJsonStringAndNoEmpty(nullptr);
}

/**
 * Test IsValidJsonStringAndNoEmpty with empty string
 */
void TestIsValidJsonStringAndNoEmptyWithEmpty()
{
    cJSON* jsonStr = cJSON_CreateString("");
    if (jsonStr != nullptr) {
        (void)IsValidJsonStringAndNoEmpty(jsonStr);
        cJSON_Delete(jsonStr);
    }
}

/**
 * Test IsValidJsonStringAndNoEmpty with single character
 */
void TestIsValidJsonStringAndNoEmptyWithSingleChar(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    
    char singleChar[2] = {static_cast<char>(data[0]), '\0'};
    cJSON* jsonStr = cJSON_CreateString(singleChar);
    if (jsonStr != nullptr) {
        (void)IsValidJsonStringAndNoEmpty(jsonStr);
        cJSON_Delete(jsonStr);
    }
}

/**
 * Test IsValidJsonStringAndNoEmpty with whitespace-only strings
 */
void TestIsValidJsonStringAndNoEmptyWithWhitespace()
{
    const char* whitespaceStrings[] = {
        " ",
        "  ",
        "\t",
        "\n",
        "\r\n",
        "   \t\n   ",
        nullptr
    };
    
    for (size_t i = 0; whitespaceStrings[i] != nullptr; i++) {
        cJSON* jsonStr = cJSON_CreateString(whitespaceStrings[i]);
        if (jsonStr != nullptr) {
            (void)IsValidJsonStringAndNoEmpty(jsonStr);
            cJSON_Delete(jsonStr);
        }
    }
}

/**
 * Test IsValidJsonStringAndNoEmpty with valid non-empty strings
 */
void TestIsValidJsonStringAndNoEmptyWithValidString(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0 || size > MAX_STRING_LEN) {
        return;
    }
    
    // Ensure string is not empty by checking first byte
    if (data[0] == '\0') {
        return;
    }
    
    std::string testStr(reinterpret_cast<const char*>(data), size);
    
    // Remove any null terminators in the middle
    testStr.erase(std::remove(testStr.begin(), testStr.end(), '\0'), testStr.end());
    
    if (testStr.empty()) {
        testStr = "A";  // Ensure non-empty
    }
    
    cJSON* jsonStr = cJSON_CreateString(testStr.c_str());
    if (jsonStr != nullptr) {
        (void)IsValidJsonStringAndNoEmpty(jsonStr);
        cJSON_Delete(jsonStr);
    }
}

/**
 * Test IsValidJsonStringAndNoEmpty with various string lengths
 */
void TestIsValidJsonStringAndNoEmptyWithVariousLengths(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_VAR_LENGTH_SIZE) {
        return;
    }
    
    // Determine string length based on fuzzer input
    size_t strLen = (data[0] % 100) + 1;  // 1-100 characters
    if (strLen > size - 1) {
        strLen = size - 1;
    }
    
    std::string testStr(strLen, 'X');
    
    // Use fuzzer data to populate string
    for (size_t i = 0; i < strLen && i < size - 1; i++) {
        testStr[i] = static_cast<char>(data[i + 1]);
        if (testStr[i] == '\0') {
            testStr[i] = 'A';  // Replace null with valid char
        }
    }
    
    cJSON* jsonStr = cJSON_CreateString(testStr.c_str());
    if (jsonStr != nullptr) {
        (void)IsValidJsonStringAndNoEmpty(jsonStr);
        cJSON_Delete(jsonStr);
    }
}

/**
 * Test IsValidJsonStringAndNoEmpty with non-string types
 */
void TestIsValidJsonStringAndNoEmptyWithNonStringTypes(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return;
    }
    
    // Test with number
    int32_t numValue = *reinterpret_cast<const int32_t*>(data);
    cJSON* jsonNum = cJSON_CreateNumber(numValue);
    if (jsonNum != nullptr) {
        (void)IsValidJsonStringAndNoEmpty(jsonNum);
        cJSON_Delete(jsonNum);
    }
    
    // Test with boolean
    cJSON* jsonBool = cJSON_CreateBool(data[0] % 2);
    if (jsonBool != nullptr) {
        (void)IsValidJsonStringAndNoEmpty(jsonBool);
        cJSON_Delete(jsonBool);
    }
    
    // Test with object
    cJSON* jsonObj = cJSON_CreateObject();
    if (jsonObj != nullptr) {
        (void)IsValidJsonStringAndNoEmpty(jsonObj);
        cJSON_Delete(jsonObj);
    }
    
    // Test with array
    cJSON* jsonArr = cJSON_CreateArray();
    if (jsonArr != nullptr) {
        (void)IsValidJsonStringAndNoEmpty(jsonArr);
        cJSON_Delete(jsonArr);
    }
}

/**
 * Test IsValidJsonStringAndNoEmpty boundary conditions
 */
void TestIsValidJsonStringAndNoEmptyBoundary()
{
    // Maximum length string
    std::string maxStr(MAX_STRING_LEN, 'M');
    cJSON* jsonMax = cJSON_CreateString(maxStr.c_str());
    if (jsonMax != nullptr) {
        (void)IsValidJsonStringAndNoEmpty(jsonMax);
        cJSON_Delete(jsonMax);
    }
    
    // Null byte at different positions
    std::string nullByteStr = "test\0embedded";
    cJSON* jsonNullByte = cJSON_CreateString(nullByteStr.c_str());
    if (jsonNullByte != nullptr) {
        (void)IsValidJsonStringAndNoEmpty(jsonNullByte);
        cJSON_Delete(jsonNullByte);
    }
}

} // namespace

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Validate input parameters
    if (data == nullptr || size < MIN_INPUT_SIZE || size > MAX_INPUT_SIZE) {
        return 0;
    }
    
    // Test with nullptr
    TestIsValidJsonStringAndNoEmptyWithNull();
    
    // Test with empty string
    TestIsValidJsonStringAndNoEmptyWithEmpty();
    
    // Test with single character
    TestIsValidJsonStringAndNoEmptyWithSingleChar(data, size);
    
    // Test with whitespace-only strings
    TestIsValidJsonStringAndNoEmptyWithWhitespace();
    
    // Test with valid non-empty string
    TestIsValidJsonStringAndNoEmptyWithValidString(data, size);
    
    // Test with various lengths
    const size_t kMinimumValidLength = 2; // At least 1 byte for length + 1 byte for content
    if (size >= kMinimumValidLength) {
        TestIsValidJsonStringAndNoEmptyWithVariousLengths(data, size);
    }
    
    // Test with non-string types
    if (size >= sizeof(int32_t)) {
        TestIsValidJsonStringAndNoEmptyWithNonStringTypes(data, size);
    }
    
    // Test boundary conditions
    TestIsValidJsonStringAndNoEmptyBoundary();
    
    return 0;
}
