/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "stats_service_test_proxy.h"

#include <message_parcel.h>

#include "ibattery_stats.h"
#include "battery_stats_proxy.h"
#include "errors.h"
#include "message_option.h"
#include "stats_common.h"
#include "hilog/log.h"
#include "stats_utils.h"

using OHOS::HiviewDFX::HiLog;

namespace OHOS {
namespace PowerMgr {
StatsServiceTestProxy::StatsServiceTestProxy(const sptr<BatteryStatsService>& service)
{
    if (service != nullptr) {
        stub_ = static_cast<sptr<BatteryStatsStub>>(service);
    }
}

ErrCode StatsServiceTestProxy::GetBatteryStatsIpc(
    ParcelableBatteryStatsList& batteryStats,
    int32_t& tempError)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "Write interface token failed!");
        return ERR_INVALID_VALUE;
    }

    int32_t result = stub_->SendRequest(
        static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_BATTERY_STATS_IPC), data, reply, option);
    if (FAILED(result)) {
        HiLog::Error(LABEL, "Send request failed!");
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        HiLog::Error(LABEL, "Read result failed, code is: %{public}d.",
            static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_BATTERY_STATS_IPC));
        return errCode;
    }

    std::unique_ptr<ParcelableBatteryStatsList> batteryStatsInfo(reply.ReadParcelable<ParcelableBatteryStatsList>());
    if (batteryStatsInfo != nullptr) {
        batteryStats = *batteryStatsInfo;
    }

    tempError = reply.ReadInt32();
    return ERR_OK;
}

ErrCode StatsServiceTestProxy::SetOnBatteryIpc(
    bool isOnBattery)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "Write interface token failed!");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInt32(isOnBattery ? 1 : 0)) {
        HiLog::Error(LABEL, "Write [isOnBattery] failed!");
        return ERR_INVALID_DATA;
    }

    int32_t result = stub_->SendRequest(
        static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_SET_ON_BATTERY_IPC), data, reply, option);
    if (FAILED(result)) {
        HiLog::Error(LABEL, "Send request failed!");
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        HiLog::Error(LABEL, "Read result failed, code is: %{public}d.",
            static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_SET_ON_BATTERY_IPC));
        return errCode;
    }

    return ERR_OK;
}

ErrCode StatsServiceTestProxy::GetAppStatsMahIpc(
    int32_t uid,
    double& appStatsMah,
    int32_t& tempError)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "Write interface token failed!");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInt32(uid)) {
        HiLog::Error(LABEL, "Write [uid] failed!");
        return ERR_INVALID_DATA;
    }

    int32_t result = stub_->SendRequest(
        static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_APP_STATS_MAH_IPC), data, reply, option);
    if (FAILED(result)) {
        HiLog::Error(LABEL, "Send request failed!");
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        HiLog::Error(LABEL, "Read result failed, code is: %{public}d.",
            static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_APP_STATS_MAH_IPC));
        return errCode;
    }

    appStatsMah = reply.ReadDouble();
    tempError = reply.ReadInt32();
    return ERR_OK;
}

ErrCode StatsServiceTestProxy::GetAppStatsPercentIpc(
    int32_t uid,
    double& appStatsPercent,
    int32_t& tempError)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "Write interface token failed!");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInt32(uid)) {
        HiLog::Error(LABEL, "Write [uid] failed!");
        return ERR_INVALID_DATA;
    }

    int32_t result = stub_->SendRequest(
        static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_APP_STATS_PERCENT_IPC), data, reply, option);
    if (FAILED(result)) {
        HiLog::Error(LABEL, "Send request failed!");
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        HiLog::Error(LABEL, "Read result failed, code is: %{public}d.",
            static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_APP_STATS_PERCENT_IPC));
        return errCode;
    }

    appStatsPercent = reply.ReadDouble();
    tempError = reply.ReadInt32();
    return ERR_OK;
}

ErrCode StatsServiceTestProxy::GetPartStatsMahIpc(
    int32_t type,
    double& partStatsMah,
    int32_t& tempError)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "Write interface token failed!");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInt32(type)) {
        HiLog::Error(LABEL, "Write [type] failed!");
        return ERR_INVALID_DATA;
    }

    int32_t result = stub_->SendRequest(
        static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_PART_STATS_MAH_IPC), data, reply, option);
    if (FAILED(result)) {
        HiLog::Error(LABEL, "Send request failed!");
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        HiLog::Error(LABEL, "Read result failed, code is: %{public}d.",
            static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_PART_STATS_MAH_IPC));
        return errCode;
    }

    partStatsMah = reply.ReadDouble();
    tempError = reply.ReadInt32();
    return ERR_OK;
}

ErrCode StatsServiceTestProxy::GetPartStatsPercentIpc(
    int32_t type,
    double& partStatsPercent,
    int32_t& tempError)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "Write interface token failed!");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInt32(type)) {
        HiLog::Error(LABEL, "Write [type] failed!");
        return ERR_INVALID_DATA;
    }

    int32_t result = stub_->SendRequest(
        static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_PART_STATS_PERCENT_IPC), data, reply, option);
    if (FAILED(result)) {
        HiLog::Error(LABEL, "Send request failed!");
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        HiLog::Error(LABEL, "Read result failed, code is: %{public}d.",
            static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_PART_STATS_PERCENT_IPC));
        return errCode;
    }

    partStatsPercent = reply.ReadDouble();
    tempError = reply.ReadInt32();
    return ERR_OK;
}

ErrCode StatsServiceTestProxy::GetTotalTimeSecondIpc(
    int32_t statsType,
    int32_t uid,
    uint64_t& totalTimeSecond)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "Write interface token failed!");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInt32(statsType)) {
        HiLog::Error(LABEL, "Write [statsType] failed!");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteInt32(uid)) {
        HiLog::Error(LABEL, "Write [uid] failed!");
        return ERR_INVALID_DATA;
    }

    int32_t result = stub_->SendRequest(
        static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_TOTAL_TIME_SECOND_IPC), data, reply, option);
    if (FAILED(result)) {
        HiLog::Error(LABEL, "Send request failed!");
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        HiLog::Error(LABEL, "Read result failed, code is: %{public}d.",
            static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_TOTAL_TIME_SECOND_IPC));
        return errCode;
    }

    totalTimeSecond = reply.ReadUint64();
    return ERR_OK;
}

ErrCode StatsServiceTestProxy::GetTotalDataBytesIpc(
    int32_t statsType,
    int32_t uid,
    uint64_t& totalDataBytes)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "Write interface token failed!");
        return ERR_INVALID_VALUE;
    }

    if (!data.WriteInt32(statsType)) {
        HiLog::Error(LABEL, "Write [statsType] failed!");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteInt32(uid)) {
        HiLog::Error(LABEL, "Write [uid] failed!");
        return ERR_INVALID_DATA;
    }

    int32_t result = stub_->SendRequest(
        static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_TOTAL_DATA_BYTES_IPC), data, reply, option);
    if (FAILED(result)) {
        HiLog::Error(LABEL, "Send request failed!");
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        HiLog::Error(LABEL, "Read result failed, code is: %{public}d.",
            static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_GET_TOTAL_DATA_BYTES_IPC));
        return errCode;
    }

    totalDataBytes = reply.ReadUint64();
    return ERR_OK;
}

ErrCode StatsServiceTestProxy::ResetIpc()
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "Write interface token failed!");
        return ERR_INVALID_VALUE;
    }

    int32_t result = stub_->SendRequest(
        static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_RESET_IPC), data, reply, option);
    if (FAILED(result)) {
        HiLog::Error(LABEL, "Send request failed!");
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        HiLog::Error(LABEL, "Read result failed, code is: %{public}d.",
            static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_RESET_IPC));
        return errCode;
    }

    return ERR_OK;
}

ErrCode StatsServiceTestProxy::ShellDumpIpc(
    const std::vector<std::string>& args,
    uint32_t argc,
    std::string& dumpShell)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        HiLog::Error(LABEL, "Write interface token failed!");
        return ERR_INVALID_VALUE;
    }

    if (args.size() > static_cast<size_t>(VECTOR_MAX_SIZE)) {
        HiLog::Error(LABEL, "The vector/array size exceeds the security limit!");
        return ERR_INVALID_DATA;
    }
    data.WriteInt32(args.size());
    for (auto it1 = args.begin(); it1 != args.end(); ++it1) {
        if (!data.WriteString16(Str8ToStr16((*it1)))) {
            HiLog::Error(LABEL, "Write [(*it1)] failed!");
            return ERR_INVALID_DATA;
        }
    }
    if (!data.WriteUint32(argc)) {
        HiLog::Error(LABEL, "Write [argc] failed!");
        return ERR_INVALID_DATA;
    }

    int32_t result = stub_->SendRequest(
        static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_SHELL_DUMP_IPC), data, reply, option);
    if (FAILED(result)) {
        HiLog::Error(LABEL, "Send request failed!");
        return result;
    }

    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        HiLog::Error(LABEL, "Read result failed, code is: %{public}d.",
            static_cast<uint32_t>(IBatteryStatsIpcCode::COMMAND_SHELL_DUMP_IPC));
        return errCode;
    }

    dumpShell = Str16ToStr8(reply.ReadString16());
    return ERR_OK;
}
} // namespace PowerMgr
} // namespace OHOS