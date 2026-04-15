/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

/* This file contains StatsCjsonUtils fuzzer test modules. */

#define FUZZ_PROJECT_NAME "statscjsonutils_fuzzer"

#include "stats_cjson_utils.h"
#include <cJSON.h>
#include <cstring>
#include <string>

using namespace OHOS::PowerMgr;

namespace {
constexpr size_t OBJ_FIELD_MIN_SIZE = 10;
constexpr size_t OBJ_KEY_LEN = 5;
constexpr uint8_t BOOL_MODULO = 2;
constexpr size_t MIN_TYPE_DATA_SIZE = 2;
constexpr size_t MIN_MIXED_DATA_SIZE = 10;
constexpr size_t MIN_ENTRY_DATA_SIZE = 3;
constexpr size_t MIN_COMBINED_TEST_SIZE = 20;
constexpr size_t COMBINED_PART_COUNT = 7;
constexpr size_t QUARTER_DIVISOR = 4;
constexpr size_t HALF_DIVISOR = 2;

enum class JsonType : uint8_t {
    STRING = 0,
    EMPTY_STRING,
    NUMBER,
    OBJECT,
    ARRAY,
    BOOL_VALUE,
    NULL_VALUE,
    PARSED_JSON,
    TYPE_COUNT
};

enum class TestFunc : uint8_t {
    VALID_STRING = 0,
    VALID_STRING_NO_EMPTY,
    VALID_NUMBER,
    VALID_OBJECT,
    VALID_ARRAY,
    VALID_BOOL,
    VALID_OBJ_OR_ARRAY,
    MIXED_TYPES,
    COMBINED,
    FUNC_COUNT
};

// 从fuzzer数据中安全提取字符串
std::string ExtractString(const uint8_t* data, size_t size, size_t maxLen = 1024)
{
    if (data == nullptr || size == 0) {
        return "";
    }
    size_t len = size < maxLen ? size : maxLen;
    std::string str(reinterpret_cast<const char*>(data), len);
    return str;
}

// 创建数字类型的cJSON对象
static cJSON* CreateJsonNumber(const uint8_t* data, size_t size)
{
    double num = 0.0;
    if (size >= sizeof(double)) {
        if (memcpy_s(&num, sizeof(double), data, sizeof(double)) != 0) {
            num = 0.0;
        }
    }
    return cJSON_CreateNumber(num);
}

// 根据fuzzer数据创建各种类型的cJSON对象
cJSON* CreateJsonFromData(const uint8_t* data, size_t size, uint8_t jsonType)
{
    if (data == nullptr || size == 0) {
        return nullptr;
    }

    switch (static_cast<JsonType>(jsonType % static_cast<uint8_t>(JsonType::TYPE_COUNT))) {
        case JsonType::STRING: {
            // 创建字符串
            std::string str = ExtractString(data, size);
            return cJSON_CreateString(str.c_str());
        }
        case JsonType::EMPTY_STRING: {
            // 创建空字符串
            return cJSON_CreateString("");
        }
        case JsonType::NUMBER:
            // 创建数字
            return CreateJsonNumber(data, size);
        case JsonType::OBJECT: {
            // 创建对象
            cJSON* obj = cJSON_CreateObject();
            if (size >= OBJ_FIELD_MIN_SIZE) {
                std::string key = ExtractString(data, OBJ_KEY_LEN);
                std::string value = ExtractString(data + OBJ_KEY_LEN, size - OBJ_KEY_LEN);
                cJSON_AddStringToObject(obj, key.c_str(), value.c_str());
            }
            return obj;
        }
        case JsonType::ARRAY: {
            // 创建数组
            cJSON* array = cJSON_CreateArray();
            if (size >= sizeof(int32_t)) {
                int32_t value = 0;
                if (memcpy_s(&value, sizeof(int32_t), data, sizeof(int32_t)) == 0) {
                    cJSON_AddItemToArray(array, cJSON_CreateNumber(value));
                }
            }
            return array;
        }
        case JsonType::BOOL_VALUE: {
            // 创建布尔值
            bool value = (size > 0) ? (data[0] % BOOL_MODULO == 0) : false;
            return cJSON_CreateBool(value);
        }
        case JsonType::NULL_VALUE: {
            // 创建null
            return cJSON_CreateNull();
        }
        case JsonType::PARSED_JSON: {
            // 尝试解析JSON字符串
            std::string jsonStr = ExtractString(data, size);
            return cJSON_Parse(jsonStr.c_str());
        }
        default:
            return nullptr;
    }
}

// 测试 IsValidJsonString
void TestIsValidJsonString(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_TYPE_DATA_SIZE) {
        return;
    }

    uint8_t jsonType = data[0];
    cJSON* jsonValue = CreateJsonFromData(data + 1, size - 1, jsonType);

    StatsJsonUtils::IsValidJsonString(jsonValue);

    if (jsonValue != nullptr) {
        cJSON_Delete(jsonValue);
    }

    // 测试 nullptr
    StatsJsonUtils::IsValidJsonString(nullptr);
}

// 测试 IsValidJsonStringAndNoEmpty
void TestIsValidJsonStringAndNoEmpty(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_TYPE_DATA_SIZE) {
        return;
    }

    uint8_t jsonType = data[0];
    cJSON* jsonValue = CreateJsonFromData(data + 1, size - 1, jsonType);

    StatsJsonUtils::IsValidJsonStringAndNoEmpty(jsonValue);

    if (jsonValue != nullptr) {
        cJSON_Delete(jsonValue);
    }

    // 测试边界情况
    StatsJsonUtils::IsValidJsonStringAndNoEmpty(nullptr);

    cJSON* emptyStr = cJSON_CreateString("");
    StatsJsonUtils::IsValidJsonStringAndNoEmpty(emptyStr);
    cJSON_Delete(emptyStr);
}

// 测试 IsValidJsonNumber
void TestIsValidJsonNumber(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_TYPE_DATA_SIZE) {
        return;
    }

    uint8_t jsonType = data[0];
    cJSON* jsonValue = CreateJsonFromData(data + 1, size - 1, jsonType);

    StatsJsonUtils::IsValidJsonNumber(jsonValue);

    if (jsonValue != nullptr) {
        cJSON_Delete(jsonValue);
    }

    // 测试特殊数字
    cJSON* zero = cJSON_CreateNumber(0);
    StatsJsonUtils::IsValidJsonNumber(zero);
    cJSON_Delete(zero);

    cJSON* negative = cJSON_CreateNumber(-123.456);
    StatsJsonUtils::IsValidJsonNumber(negative);
    cJSON_Delete(negative);
}

// 测试 IsValidJsonObject
void TestIsValidJsonObject(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_TYPE_DATA_SIZE) {
        return;
    }

    uint8_t jsonType = data[0];
    cJSON* jsonValue = CreateJsonFromData(data + 1, size - 1, jsonType);

    StatsJsonUtils::IsValidJsonObject(jsonValue);

    if (jsonValue != nullptr) {
        cJSON_Delete(jsonValue);
    }

    // 测试空对象
    cJSON* emptyObj = cJSON_CreateObject();
    StatsJsonUtils::IsValidJsonObject(emptyObj);
    cJSON_Delete(emptyObj);
}

// 测试 IsValidJsonArray
void TestIsValidJsonArray(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_TYPE_DATA_SIZE) {
        return;
    }

    uint8_t jsonType = data[0];
    cJSON* jsonValue = CreateJsonFromData(data + 1, size - 1, jsonType);

    StatsJsonUtils::IsValidJsonArray(jsonValue);

    if (jsonValue != nullptr) {
        cJSON_Delete(jsonValue);
    }

    // 测试空数组
    cJSON* emptyArray = cJSON_CreateArray();
    StatsJsonUtils::IsValidJsonArray(emptyArray);
    cJSON_Delete(emptyArray);
}

// 测试 IsValidJsonBool
void TestIsValidJsonBool(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_TYPE_DATA_SIZE) {
        return;
    }

    uint8_t jsonType = data[0];
    cJSON* jsonValue = CreateJsonFromData(data + 1, size - 1, jsonType);

    StatsJsonUtils::IsValidJsonBool(jsonValue);

    if (jsonValue != nullptr) {
        cJSON_Delete(jsonValue);
    }

    // 测试 true 和 false
    cJSON* trueValue = cJSON_CreateBool(true);
    StatsJsonUtils::IsValidJsonBool(trueValue);
    cJSON_Delete(trueValue);

    cJSON* falseValue = cJSON_CreateBool(false);
    StatsJsonUtils::IsValidJsonBool(falseValue);
    cJSON_Delete(falseValue);
}

// 测试 IsValidJsonObjectOrJsonArray
void TestIsValidJsonObjectOrJsonArray(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_TYPE_DATA_SIZE) {
        return;
    }

    uint8_t jsonType = data[0];
    cJSON* jsonValue = CreateJsonFromData(data + 1, size - 1, jsonType);

    StatsJsonUtils::IsValidJsonObjectOrJsonArray(jsonValue);

    if (jsonValue != nullptr) {
        cJSON_Delete(jsonValue);
    }

    // 测试对象
    cJSON* obj = cJSON_CreateObject();
    StatsJsonUtils::IsValidJsonObjectOrJsonArray(obj);
    cJSON_Delete(obj);

    // 测试数组
    cJSON* array = cJSON_CreateArray();
    StatsJsonUtils::IsValidJsonObjectOrJsonArray(array);
    cJSON_Delete(array);
}

// 综合测试：混合各种JSON类型
void TestMixedJsonTypes(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_MIXED_DATA_SIZE) {
        return;
    }

    // 创建复杂的JSON结构
    cJSON* root = cJSON_CreateObject();

    // 添加字符串
    std::string str1 = ExtractString(data, size / QUARTER_DIVISOR);
    cJSON_AddStringToObject(root, "stringField", str1.c_str());

    // 添加数字
    if (size >= sizeof(double) + size / QUARTER_DIVISOR) {
        double num = 0.0;
        if (memcpy_s(&num, sizeof(double), data + size / QUARTER_DIVISOR, sizeof(double)) == 0) {
            cJSON_AddNumberToObject(root, "numberField", num);
        }
    }

    // 添加布尔值
    bool boolVal = (size > size / HALF_DIVISOR) ? (data[size / HALF_DIVISOR] % BOOL_MODULO == 0) : false;
    cJSON_AddBoolToObject(root, "boolField", boolVal);

    // 添加嵌套对象
    cJSON* nested = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "nestedObject", nested);

    // 添加数组
    cJSON* array = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "arrayField", array);

    // 测试所有验证函数
    cJSON* stringField = cJSON_GetObjectItem(root, "stringField");
    StatsJsonUtils::IsValidJsonString(stringField);
    StatsJsonUtils::IsValidJsonStringAndNoEmpty(stringField);

    cJSON* numberField = cJSON_GetObjectItem(root, "numberField");
    StatsJsonUtils::IsValidJsonNumber(numberField);

    cJSON* boolField = cJSON_GetObjectItem(root, "boolField");
    StatsJsonUtils::IsValidJsonBool(boolField);

    cJSON* nestedObj = cJSON_GetObjectItem(root, "nestedObject");
    StatsJsonUtils::IsValidJsonObject(nestedObj);
    StatsJsonUtils::IsValidJsonObjectOrJsonArray(nestedObj);

    cJSON* arrayField = cJSON_GetObjectItem(root, "arrayField");
    StatsJsonUtils::IsValidJsonArray(arrayField);
    StatsJsonUtils::IsValidJsonObjectOrJsonArray(arrayField);

    // 清理
    cJSON_Delete(root);
}

// 综合测试：依次调用所有验证函数
static void RunCombinedTest(const uint8_t* testData, size_t testSize)
{
    if (testSize < MIN_COMBINED_TEST_SIZE) {
        return;
    }
    size_t part = testSize / COMBINED_PART_COUNT;
    TestIsValidJsonString(testData, part);
    TestIsValidJsonStringAndNoEmpty(testData + part, part);
    TestIsValidJsonNumber(
        testData + static_cast<size_t>(TestFunc::VALID_NUMBER) * part, part);
    TestIsValidJsonObject(
        testData + static_cast<size_t>(TestFunc::VALID_OBJECT) * part, part);
    TestIsValidJsonArray(
        testData + static_cast<size_t>(TestFunc::VALID_ARRAY) * part, part);
    TestIsValidJsonBool(
        testData + static_cast<size_t>(TestFunc::VALID_BOOL) * part, part);
    TestIsValidJsonObjectOrJsonArray(
        testData + static_cast<size_t>(TestFunc::VALID_OBJ_OR_ARRAY) * part,
        testSize - static_cast<size_t>(TestFunc::VALID_OBJ_OR_ARRAY) * part);
}

} // namespace

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < MIN_ENTRY_DATA_SIZE) {
        return 0;
    }

    // 使用第一个字节决定测试哪个函数
    uint8_t testSelector = data[0];
    const uint8_t* testData = data + 1;
    size_t testSize = size - 1;

    switch (static_cast<TestFunc>(testSelector % static_cast<uint8_t>(TestFunc::FUNC_COUNT))) {
        case TestFunc::VALID_STRING:
            TestIsValidJsonString(testData, testSize);
            break;
        case TestFunc::VALID_STRING_NO_EMPTY:
            TestIsValidJsonStringAndNoEmpty(testData, testSize);
            break;
        case TestFunc::VALID_NUMBER:
            TestIsValidJsonNumber(testData, testSize);
            break;
        case TestFunc::VALID_OBJECT:
            TestIsValidJsonObject(testData, testSize);
            break;
        case TestFunc::VALID_ARRAY:
            TestIsValidJsonArray(testData, testSize);
            break;
        case TestFunc::VALID_BOOL:
            TestIsValidJsonBool(testData, testSize);
            break;
        case TestFunc::VALID_OBJ_OR_ARRAY:
            TestIsValidJsonObjectOrJsonArray(testData, testSize);
            break;
        case TestFunc::MIXED_TYPES:
            TestMixedJsonTypes(testData, testSize);
            break;
        case TestFunc::COMBINED:
            // 综合测试：调用所有函数
            RunCombinedTest(testData, testSize);
            break;
        default:
            break;
    }

    return 0;
}
