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

#include "battery_stats_proxy.h"

#include "battery_stats_ipc_interface_code.h"
#include <message_parcel.h>
#include "errors.h"
#include "message_option.h"
#include "stats_common.h"
#include "stats_log.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
constexpr int32_t PARAM_MAX_NUM = 2000;
BatteryStatsInfoList BatteryStatsProxy::GetBatteryStats()
{
    STATS_HILOGD(COMP_FWK, "Enter");
    BatteryStatsInfoList infoList;
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, infoList);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(COMP_FWK, "Write descriptor failed");
        return infoList;
    }

    int ret = remote->SendRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GET),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(COMP_FWK, "Send request is failed, error code: %{public}d", ret);
        return infoList;
    }
    int32_t size = reply.ReadInt32();
    if (size < 0 || size > PARAM_MAX_NUM) {
        STATS_HILOGE(COMP_FWK, "size exceed limit, size=%{public}d", size);
        return infoList;
    }
    for (int32_t i = 0; i < size; ++i) {
        std::shared_ptr<BatteryStatsInfo> info = std::make_shared<BatteryStatsInfo>();
        info->ReadFromParcel(reply);
        infoList.emplace_back(info);
    }
    int32_t error;
    STATS_RETURN_IF_READ_PARCEL_FAILED_WITH_RET(COMP_FWK, reply, Int32, error, infoList);
    lastError_ = static_cast<StatsError>(error);
    return infoList;
}

uint64_t BatteryStatsProxy::GetTotalTimeSecond(const StatsUtils::StatsType& statsType, const int32_t& uid)
{
    STATS_HILOGD(COMP_FWK, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(COMP_FWK, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    uint64_t time = StatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(COMP_FWK, data, Int32, static_cast<int32_t>(statsType),
        StatsUtils::DEFAULT_VALUE);
    STATS_RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(COMP_FWK, data, Int32, uid, StatsUtils::DEFAULT_VALUE);

    int ret = remote->SendRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETTIME),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(COMP_FWK, "Transact is failed, error code: %{public}d", ret);
    }

    STATS_RETURN_IF_READ_PARCEL_FAILED_WITH_RET(COMP_FWK, reply, Uint64, time, StatsUtils::DEFAULT_VALUE);
    return time;
}

uint64_t BatteryStatsProxy::GetTotalDataBytes(const StatsUtils::StatsType& statsType, const int32_t& uid)
{
    STATS_HILOGD(COMP_FWK, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(COMP_FWK, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    uint64_t count = StatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(COMP_FWK, data, Int32, static_cast<int32_t>(statsType),
        StatsUtils::DEFAULT_VALUE);
    STATS_RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(COMP_FWK, data, Int32, uid, StatsUtils::DEFAULT_VALUE);

    int ret = remote->SendRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETDATA),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(COMP_FWK, "Transact is failed, error code: %{public}d", ret);
    }

    STATS_RETURN_IF_READ_PARCEL_FAILED_WITH_RET(COMP_FWK, reply, Uint64, count, StatsUtils::DEFAULT_VALUE);
    return count;
}

double BatteryStatsProxy::GetAppStatsMah(const int32_t& uid)
{
    STATS_HILOGD(COMP_FWK, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(COMP_FWK, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(uid);

    int ret = remote->SendRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETAPPMAH),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(COMP_FWK, "Transact is failed, error code: %{public}d", ret);
    }

    double appStatsMah = reply.ReadDouble();
    STATS_HILOGD(COMP_FWK, "Get stats mah: %{public}lf for uid: %{public}d", appStatsMah, uid);
    int32_t error;
    STATS_RETURN_IF_READ_PARCEL_FAILED_WITH_RET(COMP_FWK, reply, Int32, error, appStatsMah);
    lastError_ = static_cast<StatsError>(error);
    return appStatsMah;
}

void BatteryStatsProxy::SetOnBattery(bool isOnBattery)
{
    STATS_HILOGD(COMP_FWK, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF(remote == nullptr);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(COMP_FWK, "Write descriptor failed");
        return;
    }

    data.WriteBool(isOnBattery);

    int ret = remote->SendRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_SETONBATT),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(COMP_FWK, "Transact is failed, error code: %{public}d", ret);
    }
}

double BatteryStatsProxy::GetAppStatsPercent(const int32_t& uid)
{
    STATS_HILOGD(COMP_FWK, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(COMP_FWK, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(uid);

    int ret = remote->SendRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETAPPPER),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(COMP_FWK, "Transact is failed, error code: %{public}d", ret);
    }

    double appStatsPercent = reply.ReadDouble();
    STATS_HILOGD(COMP_FWK, "Get stats percent: %{public}lf for uid: %{public}d", appStatsPercent, uid);
    int32_t error;
    STATS_RETURN_IF_READ_PARCEL_FAILED_WITH_RET(COMP_FWK, reply, Int32, error, appStatsPercent);
    lastError_ = static_cast<StatsError>(error);
    return appStatsPercent;
}

double BatteryStatsProxy::GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& type)
{
    STATS_HILOGD(COMP_FWK, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(COMP_FWK, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(type);

    int ret = remote->SendRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETPARTMAH),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(COMP_FWK, "Transact is failed, error code: %{public}d", ret);
    }

    double partStatsMah = reply.ReadDouble();
    STATS_HILOGD(COMP_FWK, "Get stats mah: %{public}lf for type: %{public}d", partStatsMah, type);
    int32_t error;
    STATS_RETURN_IF_READ_PARCEL_FAILED_WITH_RET(COMP_FWK, reply, Int32, error, partStatsMah);
    lastError_ = static_cast<StatsError>(error);
    return partStatsMah;
}

double BatteryStatsProxy::GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& type)
{
    STATS_HILOGD(COMP_FWK, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(COMP_FWK, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(type);

    int ret = remote->SendRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETPARTPER),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(COMP_FWK, "Transact is failed, error code: %{public}d", ret);
    }

    double partStatsPercent = reply.ReadDouble();
    STATS_HILOGD(COMP_FWK, "Get stats percent: %{public}lf for type: %{public}d", partStatsPercent, type);
    int32_t error;
    STATS_RETURN_IF_READ_PARCEL_FAILED_WITH_RET(COMP_FWK, reply, Int32, error, partStatsPercent);
    lastError_ = static_cast<StatsError>(error);
    return partStatsPercent;
}

void BatteryStatsProxy::Reset()
{
    STATS_HILOGD(COMP_FWK, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF(remote == nullptr);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(COMP_FWK, "Write descriptor failed");
        return;
    }

    int ret = remote->SendRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_RESET),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(COMP_FWK, "Transact is failed, error code: %{public}d", ret);
    }
}

std::string BatteryStatsProxy::ShellDump(const std::vector<std::string>& args, uint32_t argc)
{
    sptr<IRemoteObject> remote = Remote();
    std::string result = "remote error";
    STATS_RETURN_IF_WITH_RET(remote == nullptr, result);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(COMP_FWK, "Write descriptor failed");
        return result;
    }
    if (argc > args.size()) {
        STATS_HILOGE(COMP_FWK, "argc is greater than args size!");
        return result;
    }

    data.WriteUint32(argc);
    for (uint32_t i = 0; i < argc; i++) {
        data.WriteString(args[i]);
    }
    int ret = remote->SendRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_DUMP),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(COMP_FWK, "SendRequest is failed, error code: %{public}d", ret);
        return result;
    }
    result = reply.ReadString();

    return result;
}

StatsError BatteryStatsProxy::GetLastError()
{
    StatsError tmpError = lastError_;
    lastError_ = StatsError::ERR_OK;
    return tmpError;
}
} // namespace PowerMgr
} // namespace OHOS
