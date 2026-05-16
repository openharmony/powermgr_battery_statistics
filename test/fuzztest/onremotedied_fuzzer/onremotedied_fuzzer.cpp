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

#include <cstddef>
#include <cstdint>
#include <memory>
#include <thread>
#include <vector>

#include "battery_stats_client.h"
#include "iremote_object.h"
#include "refbase.h"

#define FUZZ_PROJECT_NAME "onremotedied_fuzzer"

using namespace OHOS;
using namespace OHOS::PowerMgr;

namespace {
constexpr size_t MAX_INPUT_SIZE = 1024;
constexpr size_t MIN_INPUT_SIZE = 1;

/**
 * Mock IRemoteObject for testing
 */
class MockRemoteObject : public IRemoteObject {
public:
    MockRemoteObject() : IRemoteObject(u"MockRemoteObject") {}
    ~MockRemoteObject() {}
    
    int32_t GetObjectRefCount() override
    {
        return 1;
    }
    
    int SendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        return 0;
    }
    
    bool IsProxyObject() const override
    {
        return false;
    }
    
    bool CheckObjectLegality() const override
    {
        return true;
    }
    
    bool AddDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        return true;
    }
    
    bool RemoveDeathRecipient(const sptr<DeathRecipient>& recipient) override
    {
        return true;
    }
    
    bool Marshalling(Parcel& parcel) const override
    {
        return true;
    }
    
    sptr<IRemoteBroker> AsInterface() override
    {
        return nullptr;
    }
    
    int Dump(int fd, const std::vector<std::u16string>& args) override
    {
        return 0;
    }
};

/**
 * Test OnRemoteDied with valid remote object
 */
void TestOnRemoteDiedWithValidObject(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    
    sptr<IRemoteObject> remoteObject = new (std::nothrow) MockRemoteObject();
    if (remoteObject == nullptr) {
        return;
    }
    
    wptr<IRemoteObject> weakRemote(remoteObject);
    
    BatteryStatsClient::BatteryStatsDeathRecipient recipient;
    recipient.OnRemoteDied(weakRemote);
    
    // Test again with same object to ensure idempotency
    recipient.OnRemoteDied(weakRemote);
}

/**
 * Test OnRemoteDied with nullptr
 */
void TestOnRemoteDiedWithNull()
{
    wptr<IRemoteObject> nullRemote;
    
    BatteryStatsClient::BatteryStatsDeathRecipient recipient;
    recipient.OnRemoteDied(nullRemote);
    
    // Test multiple times to ensure robustness
    recipient.OnRemoteDied(nullRemote);
    recipient.OnRemoteDied(nullRemote);
}

/**
 * Test OnRemoteDied with expired weak pointer
 */
void TestOnRemoteDiedWithExpiredPointer()
{
    wptr<IRemoteObject> weakRemote;
    {
        sptr<IRemoteObject> remoteObject = new (std::nothrow) MockRemoteObject();
        if (remoteObject != nullptr) {
            weakRemote = remoteObject;
        }
    }
    
    BatteryStatsClient::BatteryStatsDeathRecipient recipient;
    recipient.OnRemoteDied(weakRemote);
    
    // Test again after expiration
    recipient.OnRemoteDied(weakRemote);
}

/**
 * Test OnRemoteDied with multiple recipients
 */
void TestOnRemoteDiedWithMultipleRecipients(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    
    sptr<IRemoteObject> remoteObject = new (std::nothrow) MockRemoteObject();
    if (remoteObject == nullptr) {
        return;
    }
    
    wptr<IRemoteObject> weakRemote(remoteObject);
    
    // Create multiple recipients
    BatteryStatsClient::BatteryStatsDeathRecipient recipient1;
    BatteryStatsClient::BatteryStatsDeathRecipient recipient2;
    BatteryStatsClient::BatteryStatsDeathRecipient recipient3;
    
    // Test with all recipients
    recipient1.OnRemoteDied(weakRemote);
    recipient2.OnRemoteDied(weakRemote);
    recipient3.OnRemoteDied(weakRemote);
}

/**
 * Test OnRemoteDied with concurrent calls
 */
void TestOnRemoteDiedConcurrent(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    
    sptr<IRemoteObject> remoteObject = new (std::nothrow) MockRemoteObject();
    if (remoteObject == nullptr) {
        return;
    }
    
    wptr<IRemoteObject> weakRemote(remoteObject);
    
    BatteryStatsClient::BatteryStatsDeathRecipient recipient;
    
    // Simulate concurrent OnRemoteDied calls
    std::vector<std::thread> threads;
    uint8_t threadCount = (data[0] % 4) + 1;  // 1-4 threads
    
    for (uint8_t i = 0; i < threadCount; i++) {
        threads.emplace_back([&recipient, weakRemote]() {
            recipient.OnRemoteDied(weakRemote);
        });
    }
    
    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

/**
 * Test OnRemoteDied with rapid creation and destruction
 */
void TestOnRemoteDiedRapidLifecycle()
{
    const int rapidLifecycleCount = 10;
    for (int i = 0; i < rapidLifecycleCount; i++) {
        sptr<IRemoteObject> remoteObject = new (std::nothrow) MockRemoteObject();
        if (remoteObject != nullptr) {
            wptr<IRemoteObject> weakRemote(remoteObject);
            BatteryStatsClient::BatteryStatsDeathRecipient recipient;
            recipient.OnRemoteDied(weakRemote);
        }
    }
}

/**
 * Test OnRemoteDied with ResetProxy integration
 */
void TestOnRemoteDiedWithResetProxy(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    
    sptr<IRemoteObject> remoteObject = new (std::nothrow) MockRemoteObject();
    if (remoteObject == nullptr) {
        return;
    }
    
    wptr<IRemoteObject> weakRemote(remoteObject);
    
    // Test OnRemoteDied which internally calls ResetProxy
    BatteryStatsClient::BatteryStatsDeathRecipient recipient;
    recipient.OnRemoteDied(weakRemote);
    
    // Verify the client state after death notification
    (void)BatteryStatsClient::GetInstance();
    // The proxy should be reset after OnRemoteDied
}

} // namespace

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Validate input parameters
    if (data == nullptr || size < MIN_INPUT_SIZE || size > MAX_INPUT_SIZE) {
        return 0;
    }
    
    // Test with valid remote object
    TestOnRemoteDiedWithValidObject(data, size);
    
    // Test with nullptr (critical path)
    TestOnRemoteDiedWithNull();
    
    // Test with expired weak pointer
    TestOnRemoteDiedWithExpiredPointer();
    
    // Test with multiple recipients
    if (size >= 1) {
        TestOnRemoteDiedWithMultipleRecipients(data, size);
    }
    
    // Test concurrent calls
    if (size >= 1) {
        TestOnRemoteDiedConcurrent(data, size);
    }
    
    // Test rapid lifecycle
    TestOnRemoteDiedRapidLifecycle();
    
    // Test with ResetProxy integration
    if (size >= 1) {
        TestOnRemoteDiedWithResetProxy(data, size);
    }
    
    return 0;
}
