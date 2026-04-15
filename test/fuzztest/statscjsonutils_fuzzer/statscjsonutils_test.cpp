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

// 根据fuzzer数据创建各种类型的cJSON对象
cJSON* CreateJsonFromData(const uint8_t* data, size_t size, uint8_t jsonType)
{
    if (data == nullptr || size == 0) {
        return nullptr;
    }
    
    switch (jsonType % 8) {
        case 0: {
            // 创建字符串
            std::string str = ExtractString(data, size);
            return cJSON_CreateString(str.c_str());
        }
        case 1: {
            // 创建空字符串
            return cJSON_CreateString("");
        }
        case 2: {
            // 创建数字
            double num = 0.0;
            if (size >= sizeof(double)) {
                memcpy(&num, data, sizeof(double));
            }
            return cJSON_CreateNumber(num);
        }
        case 3: {
            // 创建对象
            cJSON* obj = cJSON_CreateObject();
            if (size >= 10) {
                std::string key = ExtractString(data, 5);
                std::string value = ExtractString(data + 5, size - 5);
                cJSON_AddStringToObject(obj, key.c_str(), value.c_str());
            }
            return obj;
        }
        case 4: {
            // 创建数组
            cJSON* array = cJSON_CreateArray();
            if (size >= sizeof(int32_t)) {
                int32_t value = 0;
                memcpy(&value, data, sizeof(int32_t));
                cJSON_AddItemToArray(array, cJSON_CreateNumber(value));
            }
            return array;
        }
        case 5: {
            // 创建布尔值
            bool value = (size > 0) ? (data[0] % 2 == 0) : false;
            return cJSON_CreateBool(value);
        }
        case 6: {
            // 创建null
            return cJSON_CreateNull();
        }
        case 7: {
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
    if (data == nullptr || size < 2) {
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
    if (data == nullptr || size < 2) {
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
    if (data == nullptr || size < 2) {
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
    if (data == nullptr || size < 2) {
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
    if (data == nullptr || size < 2) {
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
    if (data == nullptr || size < 2) {
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
    if (data == nullptr || size < 2) {
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
    if (data == nullptr || size < 10) {
        return;
    }

    // 创建复杂的JSON结构
    cJSON* root = cJSON_CreateObject();
    
    // 添加字符串
    std::string str1 = ExtractString(data, size / 4);
    cJSON_AddStringToObject(root, "stringField", str1.c_str());
    
    // 添加数字
    if (size >= sizeof(double) + size / 4) {
        double num = 0.0;
        memcpy(&num, data + size / 4, sizeof(double));
        cJSON_AddNumberToObject(root, "numberField", num);
    }
    
    // 添加布尔值
    bool boolVal = (size > size / 2) ? (data[size / 2] % 2 == 0) : false;
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

} // namespace

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 3) {
        return 0;
    }

    // 使用第一个字节决定测试哪个函数
    uint8_t testSelector = data[0];
    const uint8_t* testData = data + 1;
    size_t testSize = size - 1;

    switch (testSelector % 9) {
        case 0:
            TestIsValidJsonString(testData, testSize);
            break;
        case 1:
            TestIsValidJsonStringAndNoEmpty(testData, testSize);
            break;
        case 2:
            TestIsValidJsonNumber(testData, testSize);
            break;
        case 3:
            TestIsValidJsonObject(testData, testSize);
            break;
        case 4:
            TestIsValidJsonArray(testData, testSize);
            break;
        case 5:
            TestIsValidJsonBool(testData, testSize);
            break;
        case 6:
            TestIsValidJsonObjectOrJsonArray(testData, testSize);
            break;
        case 7:
            TestMixedJsonTypes(testData, testSize);
            break;
        case 8:
            // 综合测试：调用所有函数
            if (testSize >= 20) {
                size_t part = testSize / 7;
                TestIsValidJsonString(testData, part);
                TestIsValidJsonStringAndNoEmpty(testData + part, part);
                TestIsValidJsonNumber(testData + 2 * part, part);
                TestIsValidJsonObject(testData + 3 * part, part);
                TestIsValidJsonArray(testData + 4 * part, part);
                TestIsValidJsonBool(testData + 5 * part, part);
                TestIsValidJsonObjectOrJsonArray(testData + 6 * part, testSize - 6 * part);
            }
            break;
    }
    
    return 0;
}