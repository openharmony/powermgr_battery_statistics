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

/* This file contains BatteryStatsInfo fuzzer test modules. */

#define FUZZ_PROJECT_NAME "batterystatsinfo_fuzzer"

#include "battery_stats_info.h"
#include <parcel.h>
#include <cstring>
#include <cfloat>
#include <memory>

using namespace OHOS::PowerMgr;
using namespace OHOS;

namespace {
// 从fuzzer数据中提取值
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

// 测试 BatteryStatsInfo 的 setter 和 getter 方法
void TestSettersAndGetters(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 20) {
        return;
    }

    auto info = std::make_shared<BatteryStatsInfo>();
    size_t offset = 0;

    // 从fuzzer数据提取值并设置
    int32_t uid = ExtractValue<int32_t>(data, size, offset);
    info->SetUid(uid);

    int32_t userId = ExtractValue<int32_t>(data, size, offset);
    info->SetUserId(userId);

    // 提取ConsumptionType（使用模运算确保在有效范围内）
    int32_t typeValue = ExtractValue<int32_t>(data, size, offset);
    // ConsumptionType范围从-17到CONSUMPTION_TYPE_ALARM
    BatteryStatsInfo::ConsumptionType type = 
        static_cast<BatteryStatsInfo::ConsumptionType>(typeValue % 17);
    info->SetConsumptioType(type);

    double power = ExtractValue<double>(data, size, offset);
    info->SetPower(power);

    // 调用getter方法验证
    info->GetUid();
    info->GetUserId();
    info->GetConsumptionType();
    info->GetPower();
}

// 测试 Marshalling/Unmarshalling
void TestMarshalling(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 20) {
        return;
    }

    auto info = std::make_shared<BatteryStatsInfo>();
    size_t offset = 0;

    // 设置数据
    int32_t uid = ExtractValue<int32_t>(data, size, offset);
    info->SetUid(uid);

    int32_t userId = ExtractValue<int32_t>(data, size, offset);
    info->SetUserId(userId);

    int32_t typeValue = ExtractValue<int32_t>(data, size, offset);
    BatteryStatsInfo::ConsumptionType type = 
        static_cast<BatteryStatsInfo::ConsumptionType>(typeValue % 17);
    info->SetConsumptioType(type);

    double power = ExtractValue<double>(data, size, offset);
    info->SetPower(power);

    // 测试序列化
    Parcel parcel;
    info->Marshalling(parcel);

    // 测试反序列化
    auto unmarshalled = BatteryStatsInfo::Unmarshalling(parcel);
    if (unmarshalled != nullptr) {
        unmarshalled->GetUid();
        unmarshalled->GetUserId();
        unmarshalled->GetConsumptionType();
        unmarshalled->GetPower();
    }
}

// 测试 ReadFromParcel
void TestReadFromParcel(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 4) {
        return;
    }

    Parcel parcel;
    // 将fuzzer数据写入parcel
    parcel.WriteBuffer(data, size);
    parcel.RewindRead(0);

    auto info = std::make_shared<BatteryStatsInfo>();
    info->ReadFromParcel(parcel);

    // 验证读取后的数据
    info->GetUid();
    info->GetUserId();
    info->GetConsumptionType();
    info->GetPower();
}

// 测试 ConvertConsumptionType
void TestConvertConsumptionType(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return;
    }

    size_t offset = 0;
    int32_t typeValue = ExtractValue<int32_t>(data, size, offset);

    // 测试所有可能的ConsumptionType值
    for (int i = -17; i <= 16; i++) {
        BatteryStatsInfo::ConsumptionType type = 
            static_cast<BatteryStatsInfo::ConsumptionType>(i);
        BatteryStatsInfo::ConvertConsumptionType(type);
    }

    // 测试fuzzer提供的随机值
    BatteryStatsInfo::ConsumptionType randomType = 
        static_cast<BatteryStatsInfo::ConsumptionType>(typeValue);
    BatteryStatsInfo::ConvertConsumptionType(randomType);
}

// 测试 ParcelableBatteryStatsList
void TestParcelableBatteryStatsList(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 24) {
        return;
    }

    ParcelableBatteryStatsList statsList;
    size_t offset = 0;

    // 创建多个BatteryStatsInfo并添加到列表
    // 使用第一个字节决定创建多少个对象（最多10个）
    uint8_t count = data[offset++] % 10 + 1;

    for (uint8_t i = 0; i < count && offset + 20 <= size; i++) {
        auto info = std::make_shared<BatteryStatsInfo>();

        int32_t uid = ExtractValue<int32_t>(data, size, offset);
        info->SetUid(uid);

        int32_t userId = ExtractValue<int32_t>(data, size, offset);
        info->SetUserId(userId);

        int32_t typeValue = ExtractValue<int32_t>(data, size, offset);
        BatteryStatsInfo::ConsumptionType type = 
            static_cast<BatteryStatsInfo::ConsumptionType>(typeValue % 17);
        info->SetConsumptioType(type);

        double power = ExtractValue<double>(data, size, offset);
        info->SetPower(power);

        statsList.statsList_.push_back(info);
    }

    // 测试序列化
    Parcel parcel;
    statsList.Marshalling(parcel);

    // 测试反序列化
    auto unmarshalled = ParcelableBatteryStatsList::Unmarshalling(parcel);
    if (unmarshalled != nullptr) {
        // 遍历反序列化后的列表
        for (const auto& item : unmarshalled->statsList_) {
            if (item != nullptr) {
                item->GetUid();
                item->GetUserId();
                item->GetConsumptionType();
                item->GetPower();
            }
        }
        delete unmarshalled;
    }
}

// 测试边界值和特殊值
void TestBoundaryValues(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return;
    }

    auto info = std::make_shared<BatteryStatsInfo>();

    // 测试极限值
    info->SetUid(INT32_MAX);
    info->SetUid(INT32_MIN);
    info->SetUid(0);
    info->SetUid(-1);

    info->SetUserId(INT32_MAX);
    info->SetUserId(INT32_MIN);
    info->SetUserId(0);
    info->SetUserId(-1);

    info->SetPower(0.0);
    info->SetPower(-1.0);
    info->SetPower(DBL_MAX);
    info->SetPower(DBL_MIN);
    info->SetPower(INFINITY);
    info->SetPower(-INFINITY);

    // 测试所有ConsumptionType枚举值
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_INVALID);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_BLUETOOTH);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_IDLE);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_PHONE);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_RADIO);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_SCREEN);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_USER);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_WIFI);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_CAMERA);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_FLASHLIGHT);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_AUDIO);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_SENSOR);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_GNSS);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_CPU);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_WAKELOCK);
    info->SetConsumptioType(BatteryStatsInfo::CONSUMPTION_TYPE_ALARM);

    // 调用getter验证
    info->GetUid();
    info->GetUserId();
    info->GetConsumptionType();
    info->GetPower();
}

// 测试空列表和单项列表
void TestEmptyAndSingleItemList(const uint8_t* data, size_t size)
{
    // 测试空列表
    ParcelableBatteryStatsList emptyList;
    Parcel parcel1;
    emptyList.Marshalling(parcel1);
    auto unmarshalled1 = ParcelableBatteryStatsList::Unmarshalling(parcel1);
    if (unmarshalled1 != nullptr) {
        delete unmarshalled1;
    }

    // 测试单项列表
    if (data != nullptr && size >= 20) {
        ParcelableBatteryStatsList singleList;
        auto info = std::make_shared<BatteryStatsInfo>();
        
        size_t offset = 0;
        info->SetUid(ExtractValue<int32_t>(data, size, offset));
        info->SetUserId(ExtractValue<int32_t>(data, size, offset));
        
        int32_t typeValue = ExtractValue<int32_t>(data, size, offset);
        info->SetConsumptioType(
            static_cast<BatteryStatsInfo::ConsumptionType>(typeValue % 17));
        
        info->SetPower(ExtractValue<double>(data, size, offset));
        
        singleList.statsList_.push_back(info);
        
        Parcel parcel2;
        singleList.Marshalling(parcel2);
        auto unmarshalled2 = ParcelableBatteryStatsList::Unmarshalling(parcel2);
        if (unmarshalled2 != nullptr) {
            delete unmarshalled2;
        }
    }
}

// 测试重复设置相同的值
void TestRepeatedSets(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 20) {
        return;
    }

    auto info = std::make_shared<BatteryStatsInfo>();
    size_t offset = 0;

    int32_t uid = ExtractValue<int32_t>(data, size, offset);
    int32_t userId = ExtractValue<int32_t>(data, size, offset);
    int32_t typeValue = ExtractValue<int32_t>(data, size, offset);
    double power = ExtractValue<double>(data, size, offset);

    // 重复设置多次
    for (int i = 0; i < 10; i++) {
        info->SetUid(uid);
        info->SetUserId(userId);
        info->SetConsumptioType(
            static_cast<BatteryStatsInfo::ConsumptionType>(typeValue % 17));
        info->SetPower(power);
    }

    // 验证最终值
    info->GetUid();
    info->GetUserId();
    info->GetConsumptionType();
    info->GetPower();
}

} // namespace

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < 3) {
        return 0;
    }

    // 使用第一个字节决定测试哪个功能
    uint8_t testSelector = data[0];
    const uint8_t* testData = data + 1;
    size_t testSize = size - 1;

    switch (testSelector % 8) {
        case 0:
            // 测试 setter/getter
            TestSettersAndGetters(testData, testSize);
            break;
        case 1:
            // 测试 Marshalling/Unmarshalling
            TestMarshalling(testData, testSize);
            break;
        case 2:
            // 测试 ReadFromParcel
            TestReadFromParcel(testData, testSize);
            break;
        case 3:
            // 测试 ConvertConsumptionType
            TestConvertConsumptionType(testData, testSize);
            break;
        case 4:
            // 测试 ParcelableBatteryStatsList
            TestParcelableBatteryStatsList(testData, testSize);
            break;
        case 5:
            // 测试边界值
            TestBoundaryValues(testData, testSize);
            break;
        case 6:
            // 测试空列表和单项列表
            TestEmptyAndSingleItemList(testData, testSize);
            break;
        case 7:
            // 测试重复设置
            TestRepeatedSets(testData, testSize);
            break;
    }

    return 0;
}