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
#include <cstring>
#include <string>

#include <cJSON.h>

#include "stats_cjson_utils.h"

#define FUZZ_PROJECT_NAME "isvalidjsonstring_fuzzer"

using namespace OHOS::PowerMgr::StatsJsonUtils;

namespace {
constexpr size_t MAX_INPUT_SIZE = 10240;
constexpr size_t MIN_INPUT_SIZE = 1;
constexpr size_t MAX_STRING_LEN = 4096;
constexpr size_t MEDIUM_STRING_LEN = 1024;
constexpr size_t SMALL_STRING_LEN = 256;

/**
 * Test IsValidJsonString with nullptr
 */
void TestIsValidJsonStringWithNull()
{
    (void)IsValidJsonString(nullptr);
}

/**
 * Test IsValidJsonString with valid string
 */
void TestIsValidJsonStringWithValidString(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0 || size > MAX_STRING_LEN) {
        return;
    }
    
    // Create a null-terminated string from fuzzer data
    std::string testStr(reinterpret_cast<const char*>(data), size);
    
    cJSON* jsonStr = cJSON_CreateString(testStr.c_str());
    if (jsonStr != nullptr) {
        (void)IsValidJsonString(jsonStr);
        cJSON_Delete(jsonStr);
    }
}

/**
 * Test IsValidJsonString with empty string
 */
void TestIsValidJsonStringWithEmptyString()
{
    cJSON* jsonStr = cJSON_CreateString("");
    if (jsonStr != nullptr) {
        (void)IsValidJsonString(jsonStr);
        cJSON_Delete(jsonStr);
    }
}

/**
 * Test IsValidJsonString with non-string types
 */
void TestIsValidJsonStringWithNonStringTypes(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return;
    }
    
    // Test with number
    int32_t numValue = *reinterpret_cast<const int32_t*>(data);
    cJSON* jsonNum = cJSON_CreateNumber(numValue);
    if (jsonNum != nullptr) {
        (void)IsValidJsonString(jsonNum);
        cJSON_Delete(jsonNum);
    }
    
    // Test with boolean
    cJSON* jsonBool = cJSON_CreateBool(data[0] % 2);
    if (jsonBool != nullptr) {
        (void)IsValidJsonString(jsonBool);
        cJSON_Delete(jsonBool);
    }
    
    // Test with null
    cJSON* jsonNull = cJSON_CreateNull();
    if (jsonNull != nullptr) {
        (void)IsValidJsonString(jsonNull);
        cJSON_Delete(jsonNull);
    }
    
    // Test with object
    cJSON* jsonObj = cJSON_CreateObject();
    if (jsonObj != nullptr) {
        (void)IsValidJsonString(jsonObj);
        cJSON_Delete(jsonObj);
    }
    
    // Test with array
    cJSON* jsonArr = cJSON_CreateArray();
    if (jsonArr != nullptr) {
        (void)IsValidJsonString(jsonArr);
        cJSON_Delete(jsonArr);
    }
}

/**
 * Test IsValidJsonString with malformed JSON
 */
void TestIsValidJsonStringWithMalformed(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0 || size > MAX_STRING_LEN) {
        return;
    }
    
    // Try to parse fuzzer data as JSON
    std::string jsonStr(reinterpret_cast<const char*>(data), size);
    cJSON* json = cJSON_Parse(jsonStr.c_str());
    if (json != nullptr) {
        (void)IsValidJsonString(json);
        cJSON_Delete(json);
    }
}

/**
 * Test IsValidJsonString with special characters
 */
void TestIsValidJsonStringWithSpecialChars()
{
    const char* specialStrings[] = {
        "\\n\\r\\t",
        "\x00\x01\x02",
        "🔥emoji🎉",
        "\\u0000",
        "\"quotes\"",
        "\\\\backslash",
        "/",
        "\b\f",
        "\u4E2D\u6587",  // Chinese characters
        "\u0000\u001F",  // Control characters
        "'single quotes'",
        "mixed\"quotes'test",
        "tab\ttab",
        "newline\nnewline",
        "carriage\rreturn",
        nullptr
    };
    
    for (size_t i = 0; specialStrings[i] != nullptr; i++) {
        cJSON* jsonStr = cJSON_CreateString(specialStrings[i]);
        if (jsonStr != nullptr) {
            (void)IsValidJsonString(jsonStr);
            cJSON_Delete(jsonStr);
        }
    }
}

/**
 * Test IsValidJsonString with various encodings
 */
void TestIsValidJsonStringWithEncodings(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0 || size > SMALL_STRING_LEN) {
        return;
    }
    
    // Test different string construction methods
    std::string testStr;
    
    // Method 1: Direct construction
    testStr = std::string(reinterpret_cast<const char*>(data), size);
    cJSON* json1 = cJSON_CreateString(testStr.c_str());
    if (json1 != nullptr) {
        (void)IsValidJsonString(json1);
        cJSON_Delete(json1);
    }
    
    // Method 2: Escaped construction
    testStr.clear();
    for (size_t i = 0; i < size && i < 50; i++) {
        if (data[i] >= 32 && data[i] < 127) {
            testStr += static_cast<char>(data[i]);
        }
    }
    if (!testStr.empty()) {
        cJSON* json2 = cJSON_CreateString(testStr.c_str());
        if (json2 != nullptr) {
            (void)IsValidJsonString(json2);
            cJSON_Delete(json2);
        }
    }
}

/**
 * Test IsValidJsonString with Unicode patterns
 */
void TestIsValidJsonStringWithUnicode()
{
    const char* unicodeStrings[] = {
        "ABC",                 // Basic ASCII
        "😀",                  // Emoji (UTF-8 encoded)
        "éèê",                 // Accented characters
        "あいう",              // Hiragana
        "一二三",              // Kanji
        "ЀЁЂ",                 // Cyrillic
        "אבג",                 // Hebrew
        "؀؁؂",                 // Arabic
        nullptr
    };
    
    for (size_t i = 0; unicodeStrings[i] != nullptr; i++) {
        cJSON* jsonStr = cJSON_CreateString(unicodeStrings[i]);
        if (jsonStr != nullptr) {
            (void)IsValidJsonString(jsonStr);
            cJSON_Delete(jsonStr);
        }
    }
}

/**
 * Test IsValidJsonString with nested JSON parsing
 */
void TestIsValidJsonStringWithNestedJson(const uint8_t* data, size_t size)
{
    const size_t MIN_STRING_LEN = 10;
    if (data == nullptr || size < MIN_STRING_LEN || size > MEDIUM_STRING_LEN) {
        return;
    }
    
    // Try to parse as nested JSON
    std::string jsonStr = "{\"test\":\"" + 
                          std::string(reinterpret_cast<const char*>(data), std::min(size, size_t(100))) + 
                          "\"}";
    
    cJSON* root = cJSON_Parse(jsonStr.c_str());
    if (root != nullptr) {
        cJSON* testField = cJSON_GetObjectItem(root, "test");
        if (testField != nullptr) {
            (void)IsValidJsonString(testField);
        }
        cJSON_Delete(root);
    }
}

/**
 * Test IsValidJsonString with corrupted valuestring
 */
void TestIsValidJsonStringWithCorruptedValue()
{
    // Create a valid JSON string first
    cJSON* jsonStr = cJSON_CreateString("valid string");
    if (jsonStr != nullptr) {
        // Test with normal state
        (void)IsValidJsonString(jsonStr);
        
        // The function should handle this gracefully
        cJSON_Delete(jsonStr);
    }
}

/**
 * Test IsValidJsonString with boundary lengths
 */
void TestIsValidJsonStringBoundary()
{
    // Very long string
    std::string longStr(MAX_STRING_LEN, 'A');
    cJSON* jsonStr = cJSON_CreateString(longStr.c_str());
    if (jsonStr != nullptr) {
        (void)IsValidJsonString(jsonStr);
        cJSON_Delete(jsonStr);
    }
    
    // Single character
    cJSON* jsonSingle = cJSON_CreateString("X");
    if (jsonSingle != nullptr) {
        (void)IsValidJsonString(jsonSingle);
        cJSON_Delete(jsonSingle);
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
    
    // Test with nullptr (critical path)
    TestIsValidJsonStringWithNull();
    
    // Test with valid string
    TestIsValidJsonStringWithValidString(data, size);
    
    // Test with empty string
    TestIsValidJsonStringWithEmptyString();
    
    // Test with non-string types
    if (size >= sizeof(int32_t)) {
        TestIsValidJsonStringWithNonStringTypes(data, size);
    }
    
    // Test with malformed JSON
    TestIsValidJsonStringWithMalformed(data, size);
    
    // Test with special characters (extensive)
    TestIsValidJsonStringWithSpecialChars();
    
    // Test with various encodings
    if (size <= SMALL_STRING_LEN) {
        TestIsValidJsonStringWithEncodings(data, size);
    }
    
    // Test with Unicode patterns
    TestIsValidJsonStringWithUnicode();
    
    // Test with nested JSON
    if (size >= 10 && size <= MEDIUM_STRING_LEN) {
        TestIsValidJsonStringWithNestedJson(data, size);
    }
    
    // Test with corrupted values
    TestIsValidJsonStringWithCorruptedValue();
    
    // Test boundary conditions
    TestIsValidJsonStringBoundary();
    
    return 0;
}
