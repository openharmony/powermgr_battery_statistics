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

#include "batterystats_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "message_parcel.h"
#include "securec.h"

using namespace OHOS::PowerMgr;
using namespace OHOS;

namespace {
const int32_t REWIND_READ_DATA = 0;
} // namespace

BatteryStatsFuzzerTest::BatteryStatsFuzzerTest()
{
    service_ = BatteryStatsService::GetInstance();
    service_->OnStart();
}

BatteryStatsFuzzerTest::~BatteryStatsFuzzerTest()
{
    if (service_ != nullptr) {
        service_->OnStop();
    }
    service_ = nullptr;
}

void BatteryStatsFuzzerTest::TestStatsServiceStub(const uint32_t code, const uint8_t* data, size_t size)
{
    MessageParcel datas;
    datas.WriteInterfaceToken(BatteryStatsService::GetDescriptor());
    datas.WriteBuffer(data, size);
    datas.RewindRead(REWIND_READ_DATA);
    MessageParcel reply;
    MessageOption option;
    service_->OnRemoteRequest(code, datas, reply, option);
}
