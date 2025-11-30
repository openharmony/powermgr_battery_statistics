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
#include <cmath>
#include <limits>
#include <string>

#include <cJSON.h>

#include "stats_cjson_utils.h"

#define FUZZ_PROJECT_NAME "isvalidjsonnumber_fuzzer"

using namespace OHOS::PowerMgr::StatsJsonUtils;

namespace {
constexpr size_t MAX_INPUT_SIZE = 1024;
constexpr size_t MIN_INPUT_SIZE = 1;

/**
 * Test IsValidJsonNumber with nullptr
 */
void TestIsValidJsonNumberWithNull()
{
    (void)IsValidJsonNumber(nullptr);
}

/**
 * Test IsValidJsonNumber with integer values
 */
void TestIsValidJsonNumberWithIntegers(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return;
    }
    
    // Test with int32_t
    int32_t int32Value = *reinterpret_cast<const int32_t*>(data);
    cJSON* jsonInt32 = cJSON_CreateNumber(int32Value);
    if (jsonInt32 != nullptr) {
        (void)IsValidJsonNumber(jsonInt32);
        cJSON_Delete(jsonInt32);
    }
    
    // Test with int64_t if size permits
    if (size >= sizeof(int64_t)) {
        int64_t int64Value = *reinterpret_cast<const int64_t*>(data);
        cJSON* jsonInt64 = cJSON_CreateNumber(static_cast<double>(int64Value));
        if (jsonInt64 != nullptr) {
            (void)IsValidJsonNumber(jsonInt64);
            cJSON_Delete(jsonInt64);
        }
    }
}

/**
 * Test IsValidJsonNumber with floating point values
 */
void TestIsValidJsonNumberWithFloats(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(double)) {
        return;
    }
    
    // Test with double
    double doubleValue = *reinterpret_cast<const double*>(data);
    cJSON* jsonDouble = cJSON_CreateNumber(doubleValue);
    if (jsonDouble != nullptr) {
        (void)IsValidJsonNumber(jsonDouble);
        cJSON_Delete(jsonDouble);
    }
}

/**
 * Test IsValidJsonNumber with special numeric values
 */
void TestIsValidJsonNumberWithSpecialValues()
{
    // Test with zero
    cJSON* jsonZero = cJSON_CreateNumber(0.0);
    if (jsonZero != nullptr) {
        (void)IsValidJsonNumber(jsonZero);
        cJSON_Delete(jsonZero);
    }
    
    // Test with negative zero
    cJSON* jsonNegZero = cJSON_CreateNumber(-0.0);
    if (jsonNegZero != nullptr) {
        (void)IsValidJsonNumber(jsonNegZero);
        cJSON_Delete(jsonNegZero);
    }
    
    // Test with infinity
    cJSON* jsonInf = cJSON_CreateNumber(std::numeric_limits<double>::infinity());
    if (jsonInf != nullptr) {
        (void)IsValidJsonNumber(jsonInf);
        cJSON_Delete(jsonInf);
    }
    
    // Test with negative infinity
    cJSON* jsonNegInf = cJSON_CreateNumber(-std::numeric_limits<double>::infinity());
    if (jsonNegInf != nullptr) {
        (void)IsValidJsonNumber(jsonNegInf);
        cJSON_Delete(jsonNegInf);
    }
    
    // Test with NaN
    cJSON* jsonNaN = cJSON_CreateNumber(std::numeric_limits<double>::quiet_NaN());
    if (jsonNaN != nullptr) {
        (void)IsValidJsonNumber(jsonNaN);
        cJSON_Delete(jsonNaN);
    }
    
    // Test with max values
    cJSON* jsonMax = cJSON_CreateNumber(std::numeric_limits<double>::max());
    if (jsonMax != nullptr) {
        (void)IsValidJsonNumber(jsonMax);
        cJSON_Delete(jsonMax);
    }
    
    // Test with min values
    cJSON* jsonMin = cJSON_CreateNumber(std::numeric_limits<double>::lowest());
    if (jsonMin != nullptr) {
        (void)IsValidJsonNumber(jsonMin);
        cJSON_Delete(jsonMin);
    }
    
    // Test with epsilon
    cJSON* jsonEpsilon = cJSON_CreateNumber(std::numeric_limits<double>::epsilon());
    if (jsonEpsilon != nullptr) {
        (void)IsValidJsonNumber(jsonEpsilon);
        cJSON_Delete(jsonEpsilon);
    }
}

/**
 * Test IsValidJsonNumber with non-number types
 */
void TestIsValidJsonNumberWithNonNumericTypes(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    
    // Test with string
    std::string testStr(reinterpret_cast<const char*>(data), std::min(size, size_t(100)));
    cJSON* jsonStr = cJSON_CreateString(testStr.c_str());
    if (jsonStr != nullptr) {
        (void)IsValidJsonNumber(jsonStr);
        cJSON_Delete(jsonStr);
    }
    
    // Test with boolean
    cJSON* jsonBool = cJSON_CreateBool(data[0] % 2);
    if (jsonBool != nullptr) {
        (void)IsValidJsonNumber(jsonBool);
        cJSON_Delete(jsonBool);
    }
    
    // Test with null
    cJSON* jsonNull = cJSON_CreateNull();
    if (jsonNull != nullptr) {
        (void)IsValidJsonNumber(jsonNull);
        cJSON_Delete(jsonNull);
    }
    
    // Test with object
    cJSON* jsonObj = cJSON_CreateObject();
    if (jsonObj != nullptr) {
        (void)IsValidJsonNumber(jsonObj);
        cJSON_Delete(jsonObj);
    }
    
    // Test with array
    cJSON* jsonArr = cJSON_CreateArray();
    if (jsonArr != nullptr) {
        (void)IsValidJsonNumber(jsonArr);
        cJSON_Delete(jsonArr);
    }
}

/**
 * Test IsValidJsonNumber with boundary values
 */
void TestIsValidJsonNumberBoundary(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    
    // Generate boundary values based on fuzzer input
    uint8_t selector = data[0] % 10;
    
    double testValue = 0.0;
    switch (selector) {
        case 0:
            testValue = 1.0;
            break;
        case 1:
            testValue = -1.0;
            break;
        case 2:
            testValue = 0.1;
            break;
        case 3:
            testValue = -0.1;
            break;
        case 4:
            testValue = 1e10;
            break;
        case 5:
            testValue = -1e10;
            break;
        case 6:
            testValue = 1e-10;
            break;
        case 7:
            testValue = -1e-10;
            break;
        case 8:
            testValue = 3.14159265358979323846;
            break;
        case 9:
            testValue = 2.71828182845904523536;
            break;
        default:
            testValue = 0.0;
            break;
    }
    
    cJSON* jsonNum = cJSON_CreateNumber(testValue);
    if (jsonNum != nullptr) {
        (void)IsValidJsonNumber(jsonNum);
        cJSON_Delete(jsonNum);
    }
}

/**
 * Test IsValidJsonNumber with parsed JSON
 */
void TestIsValidJsonNumberWithParsedJson(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0 || size > 512) {
        return;
    }
    
    // Try to parse fuzzer data as JSON
    std::string jsonStr(reinterpret_cast<const char*>(data), size);
    cJSON* json = cJSON_Parse(jsonStr.c_str());
    if (json != nullptr) {
        (void)IsValidJsonNumber(json);
        cJSON_Delete(json);
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
    TestIsValidJsonNumberWithNull();
    
    // Test with integer values
    if (size >= sizeof(int32_t)) {
        TestIsValidJsonNumberWithIntegers(data, size);
    }
    
    // Test with floating point values
    if (size >= sizeof(double)) {
        TestIsValidJsonNumberWithFloats(data, size);
    }
    
    // Test with special numeric values
    TestIsValidJsonNumberWithSpecialValues();
    
    // Test with non-numeric types
    TestIsValidJsonNumberWithNonNumericTypes(data, size);
    
    // Test boundary values
    TestIsValidJsonNumberBoundary(data, size);
    
    // Test with parsed JSON
    TestIsValidJsonNumberWithParsedJson(data, size);
    
    return 0;
}
