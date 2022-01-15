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

#include "battery_stats_stub.h"

#include <string_ex.h>

#include "battery_stats_info.h"
#include "stats_common.h"

namespace OHOS {
namespace PowerMgr {
int BatteryStatsStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    STATS_HILOGD(STATS_MODULE_SERVICE, "BatteryStatsStub::OnRemoteRequest, cmd = %{public}d, flags = %{public}d",
        code, option.GetFlags());
    std::u16string descriptor = BatteryStatsStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "BatteryStatsStub::OnRemoteRequest failed, descriptor is not matched!");
        return E_STATS_GET_SERVICE_FAILED;
    }

    switch (code) {
        case static_cast<int>(IBatteryStats::BATTERY_STATS_GET): {
            return GetBatteryStatsStub(reply);
        }
        case static_cast<int>(IBatteryStats::BATTERY_STATS_GETTIME): {
            return GetTotalTimeSecondStub(data, reply);
        }
        case static_cast<int>(IBatteryStats::BATTERY_STATS_GETDATA): {
            return GetTotalDataBytesStub(data, reply);
        }
        case static_cast<int>(IBatteryStats::BATTERY_STATS_GETAPPMAH): {
            return GetAppStatsMahStub(data, reply);
        }
        case static_cast<int>(IBatteryStats::BATTERY_STATS_GETAPPPER): {
            return GetAppStatsPercentStub(data, reply);
        }
        case static_cast<int>(IBatteryStats::BATTERY_STATS_GETPARTMAH): {
            return GetPartStatsMahStub(data, reply);
        }
        case static_cast<int>(IBatteryStats::BATTERY_STATS_GETPARTPER): {
            return GetPartStatsPercentStub(data, reply);
        }
        case static_cast<int>(IBatteryStats::BATTERY_STATS_RESET): {
            return ResetStub();
        }
        case static_cast<int>(IBatteryStats::BATTERY_STATS_SETONBATT): {
            return SetOnBatteryStub(data);
        }
        default: {
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
    }
    return ERR_OK;
}

int32_t BatteryStatsStub::GetBatteryStatsStub(MessageParcel& reply)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "%{public}s.", __func__);
    BatteryStatsInfoList ret = GetBatteryStats();

    uint32_t size = static_cast<uint32_t>(ret.size());
    if (!reply.WriteUint32(size)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "%{public}s - Write size failed.", __func__);
        return false;
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "%{public}s - Write size: %{public}u", __func__, size);
    for (const auto& templateVal : ret) {
        if (templateVal == nullptr) {
            continue;
        }
        templateVal->Marshalling(reply);
    }
    return ERR_OK;
}

int32_t BatteryStatsStub::GetTotalTimeSecondStub(MessageParcel &data, MessageParcel& reply)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "%{public}s.", __func__);
    int32_t type = data.ReadInt32();
    StatsUtils::StatsType statsType = StatsUtils::StatsType(type);
    int32_t uid = data.ReadInt32();
    uint64_t ret = GetTotalTimeSecond(statsType, uid);
    if (!reply.WriteUint64(ret)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "%{public}s - Write ret failed.", __func__);
        return false;
    }
    return ERR_OK;
}

int32_t BatteryStatsStub::GetTotalDataBytesStub(MessageParcel &data, MessageParcel& reply)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "%{public}s.", __func__);
    int32_t type = data.ReadInt32();
    StatsUtils::StatsType statsType = StatsUtils::StatsType(type);
    int32_t uid = data.ReadInt32();
    uint64_t ret = GetTotalDataBytes(statsType, uid);
    if (!reply.WriteUint64(ret)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "%{public}s - Write ret failed.", __func__);
        return false;
    }
    return ERR_OK;
}

int32_t BatteryStatsStub::GetAppStatsMahStub(MessageParcel &data, MessageParcel& reply)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    int32_t uid = data.ReadInt32();
    double ret = GetAppStatsMah(uid);
    if (!reply.WriteDouble(ret)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Write ret failed.");
        return false;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return ERR_OK;
}

int32_t BatteryStatsStub::GetAppStatsPercentStub(MessageParcel &data, MessageParcel& reply)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    int32_t uid = data.ReadInt32();
    double ret = GetAppStatsPercent(uid);
    if (!reply.WriteDouble(ret)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Write ret failed.");
        return false;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return ERR_OK;
}

int32_t BatteryStatsStub::GetPartStatsMahStub(MessageParcel &data, MessageParcel& reply)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    int32_t typeProxy = data.ReadInt32();
    BatteryStatsInfo::ConsumptionType type = BatteryStatsInfo::ConsumptionType(typeProxy);
    double ret = GetPartStatsMah(type);
    if (!reply.WriteDouble(ret)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Write ret failed.");
        return false;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return ERR_OK;
}

int32_t BatteryStatsStub::GetPartStatsPercentStub(MessageParcel &data, MessageParcel& reply)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    int32_t typeProxy = data.ReadInt32();
    BatteryStatsInfo::ConsumptionType type = BatteryStatsInfo::ConsumptionType(typeProxy);
    double ret = GetPartStatsPercent(type);
    if (!reply.WriteDouble(ret)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Write ret failed.");
        return false;
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return ERR_OK;
}

int32_t BatteryStatsStub::ResetStub()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "%{public}s.", __func__);
    Reset();
    return ERR_OK;
}

int32_t BatteryStatsStub::SetOnBatteryStub(MessageParcel& data)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "%{public}s.", __func__);
    bool isOnBattery = data.ReadBool();
    SetOnBattery(isOnBattery);
    return ERR_OK;
}
} // namespace PowerMgr
} // namespace OHOS
