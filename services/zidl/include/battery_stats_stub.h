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

#ifndef BATTERY_STATS_STUB_H
#define BATTERY_STATS_STUB_H

#include <cstdint>
#include "message_option.h"
#include "message_parcel.h"
#include "refbase.h"
#include "iremote_stub.h"
#include "nocopyable.h"

#include "ibattery_stats.h"

namespace OHOS {
namespace PowerMgr {
class BatteryStatsStub : public IRemoteStub<IBatteryStats> {
public:
    DISALLOW_COPY_AND_MOVE(BatteryStatsStub);

    BatteryStatsStub() = default;

    virtual ~BatteryStatsStub() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
private:
    int32_t GetBatteryStatsStub(MessageParcel& reply);
    int32_t SetOnBatteryStub(MessageParcel& data);
    int32_t GetAppStatsMahStub(MessageParcel& data, MessageParcel& reply);
    int32_t GetAppStatsPercentStub(MessageParcel& data, MessageParcel& reply);
    int32_t GetPartStatsMahStub(MessageParcel& data, MessageParcel& reply);
    int32_t GetPartStatsPercentStub(MessageParcel& data, MessageParcel& reply);
    int32_t GetTotalTimeSecondStub(MessageParcel& data, MessageParcel& reply);
    int32_t GetTotalDataBytesStub(MessageParcel& data, MessageParcel& reply);
    int32_t ResetStub();
    int32_t ShellDumpStub(MessageParcel& data, MessageParcel& reply);
    int32_t ChooseCodeStub(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // BATTERY_STATS_STUB_H
