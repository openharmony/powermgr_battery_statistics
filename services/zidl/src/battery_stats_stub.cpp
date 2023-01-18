/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "errors.h"
#include "battery_stats_info.h"
#include "ipc_object_stub.h"
#include "stats_errors.h"
#include "stats_log.h"
#include "stats_utils.h"
#include "xcollie.h"
#include "xcollie_define.h"

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr uint32_t PARAM_MAX_NUM = 10;
}
int BatteryStatsStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    STATS_HILOGD(COMP_SVC, "Remote request, cmd = %{public}d, flags = %{public}d",
        code, option.GetFlags());
    const uint32_t DFX_DELAY_MS = 10000;
    int id = HiviewDFX::XCollie::GetInstance().SetTimer("BatteryStatsStub", DFX_DELAY_MS, nullptr, nullptr,
        HiviewDFX::XCOLLIE_FLAG_NOOP);

    std::u16string descriptor = BatteryStatsStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        STATS_HILOGE(COMP_SVC, "Remote request failed, descriptor is not matched");
        return E_STATS_GET_SERVICE_FAILED;
    }
    int ret = ChooseCodeStub(code, data, reply, option);
    HiviewDFX::XCollie::GetInstance().CancelTimer(id);
    return ret;
}

int32_t BatteryStatsStub::ChooseCodeStub(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    int32_t ret;
    switch (code) {
        case static_cast<uint32_t>(IBatteryStats::BATTERY_STATS_GET): {
            ret = GetBatteryStatsStub(reply);
            break;
        }
        case static_cast<uint32_t>(IBatteryStats::BATTERY_STATS_GETTIME): {
            ret = GetTotalTimeSecondStub(data, reply);
            break;
        }
        case static_cast<uint32_t>(IBatteryStats::BATTERY_STATS_GETDATA): {
            ret = GetTotalDataBytesStub(data, reply);
            break;
        }
        case static_cast<uint32_t>(IBatteryStats::BATTERY_STATS_GETAPPMAH): {
            ret = GetAppStatsMahStub(data, reply);
            break;
        }
        case static_cast<uint32_t>(IBatteryStats::BATTERY_STATS_GETAPPPER): {
            ret = GetAppStatsPercentStub(data, reply);
            break;
        }
        case static_cast<uint32_t>(IBatteryStats::BATTERY_STATS_GETPARTMAH): {
            ret = GetPartStatsMahStub(data, reply);
            break;
        }
        case static_cast<uint32_t>(IBatteryStats::BATTERY_STATS_GETPARTPER): {
            ret = GetPartStatsPercentStub(data, reply);
            break;
        }
        case static_cast<uint32_t>(IBatteryStats::BATTERY_STATS_RESET): {
            ret = ResetStub();
            break;
        }
        case static_cast<uint32_t>(IBatteryStats::BATTERY_STATS_SETONBATT): {
            ret = SetOnBatteryStub(data);
            break;
        }
        case static_cast<uint32_t>(IBatteryStats::BATTERY_STATS_DUMP): {
            ret = ShellDumpStub(data, reply);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

int32_t BatteryStatsStub::GetBatteryStatsStub(MessageParcel& reply)
{
    STATS_HILOGD(COMP_SVC, "Enter");
    BatteryStatsInfoList ret = GetBatteryStats();

    uint32_t size = static_cast<uint32_t>(ret.size());
    if (!reply.WriteUint32(size)) {
        STATS_HILOGE(COMP_SVC, "Write size failed");
        return false;
    }
    STATS_HILOGD(COMP_SVC, "Write size: %{public}u", size);
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
    STATS_HILOGD(COMP_SVC, "Enter");
    int32_t type = data.ReadInt32();
    StatsUtils::StatsType statsType = StatsUtils::StatsType(type);
    int32_t uid = data.ReadInt32();
    uint64_t ret = GetTotalTimeSecond(statsType, uid);
    if (!reply.WriteUint64(ret)) {
        STATS_HILOGE(COMP_SVC, "Write ret failed");
        return false;
    }
    return ERR_OK;
}

int32_t BatteryStatsStub::GetTotalDataBytesStub(MessageParcel &data, MessageParcel& reply)
{
    STATS_HILOGD(COMP_SVC, "Enter");
    int32_t type = data.ReadInt32();
    StatsUtils::StatsType statsType = StatsUtils::StatsType(type);
    int32_t uid = data.ReadInt32();
    uint64_t ret = GetTotalDataBytes(statsType, uid);
    if (!reply.WriteUint64(ret)) {
        STATS_HILOGE(COMP_SVC, "Write ret failed");
        return false;
    }
    return ERR_OK;
}

int32_t BatteryStatsStub::GetAppStatsMahStub(MessageParcel &data, MessageParcel& reply)
{
    STATS_HILOGD(COMP_SVC, "Enter");
    int32_t uid = data.ReadInt32();
    double ret = GetAppStatsMah(uid);
    if (!reply.WriteDouble(ret)) {
        STATS_HILOGE(COMP_SVC, "Write ret failed");
        return false;
    }
    return ERR_OK;
}

int32_t BatteryStatsStub::GetAppStatsPercentStub(MessageParcel &data, MessageParcel& reply)
{
    STATS_HILOGD(COMP_SVC, "Enter");
    int32_t uid = data.ReadInt32();
    double ret = GetAppStatsPercent(uid);
    if (!reply.WriteDouble(ret)) {
        STATS_HILOGE(COMP_SVC, "Write ret failed");
        return false;
    }
    return ERR_OK;
}

int32_t BatteryStatsStub::GetPartStatsMahStub(MessageParcel &data, MessageParcel& reply)
{
    STATS_HILOGD(COMP_SVC, "Enter");
    int32_t typeProxy = data.ReadInt32();
    BatteryStatsInfo::ConsumptionType type = BatteryStatsInfo::ConsumptionType(typeProxy);
    double ret = GetPartStatsMah(type);
    if (!reply.WriteDouble(ret)) {
        STATS_HILOGE(COMP_SVC, "Write ret failed");
        return false;
    }
    return ERR_OK;
}

int32_t BatteryStatsStub::GetPartStatsPercentStub(MessageParcel &data, MessageParcel& reply)
{
    STATS_HILOGD(COMP_SVC, "Enter");
    int32_t typeProxy = data.ReadInt32();
    BatteryStatsInfo::ConsumptionType type = BatteryStatsInfo::ConsumptionType(typeProxy);
    double ret = GetPartStatsPercent(type);
    if (!reply.WriteDouble(ret)) {
        STATS_HILOGE(COMP_SVC, "Write ret failed");
        return false;
    }
    return ERR_OK;
}

int32_t BatteryStatsStub::ResetStub()
{
    STATS_HILOGD(COMP_SVC, "Enter");
    Reset();
    return ERR_OK;
}

int32_t BatteryStatsStub::SetOnBatteryStub(MessageParcel& data)
{
    STATS_HILOGD(COMP_SVC, "Enter");
    bool isOnBattery = data.ReadBool();
    SetOnBattery(isOnBattery);
    return ERR_OK;
}

int32_t BatteryStatsStub::ShellDumpStub(MessageParcel& data, MessageParcel& reply)
{
    uint32_t argc;
    std::vector<std::string> args;

    if (!data.ReadUint32(argc)) {
        STATS_HILOGE(COMP_SVC, "Readback fail");
        return E_STATS_READ_PARCEL_ERROR;
    }

    if (argc >= PARAM_MAX_NUM) {
        STATS_HILOGW(COMP_SVC, "params exceed limit, argc=%{public}d", argc);
        return E_STATS_EXCEED_PARAM_LIMIT;
    }

    for (uint32_t i = 0; i < argc; i++) {
        std::string arg = data.ReadString();
        if (arg.empty()) {
            STATS_HILOGW(COMP_SVC, "read value fail=%{public}d", i);
            return E_STATS_READ_PARCEL_ERROR;
        }
        args.push_back(arg);
    }

    std::string ret = ShellDump(args, argc);
    if (!reply.WriteString(ret)) {
        STATS_HILOGE(COMP_SVC, "Dump Writeback Fail");
        return E_STATS_WRITE_PARCEL_ERROR;
    }
    return ERR_OK;
}
} // namespace PowerMgr
} // namespace OHOS
