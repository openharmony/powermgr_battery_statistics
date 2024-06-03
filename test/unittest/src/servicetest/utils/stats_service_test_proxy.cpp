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

#include "battery_stats_ipc_interface_code.h"
#include "battery_stats_proxy.h"
#include "errors.h"
#include "message_option.h"
#include "stats_common.h"
#include "stats_log.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
StatsServiceTestProxy::StatsServiceTestProxy(const sptr<BatteryStatsService>& service)
{
    if (service != nullptr) {
        stub_ = static_cast<sptr<BatteryStatsStub>>(service);
    }
}

BatteryStatsInfoList StatsServiceTestProxy::GetBatteryStats()
{
    BatteryStatsInfoList infoList;
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, infoList);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(LABEL_TEST, "Write descriptor failed");
        return infoList;
    }

    int ret = stub_->OnRemoteRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GET),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(LABEL_TEST, "Send request is failed, error code: %{public}d", ret);
        return infoList;
    }
    int32_t size = reply.ReadInt32();
    for (int32_t i = 0; i < size; ++i) {
        std::shared_ptr<BatteryStatsInfo> info = std::make_shared<BatteryStatsInfo>();
        info->ReadFromParcel(reply);
        infoList.emplace_back(info);
    }
    return infoList;
}

uint64_t StatsServiceTestProxy::GetTotalTimeSecond(const StatsUtils::StatsType& statsType, const int32_t& uid)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(LABEL_TEST, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    uint64_t time = StatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(LABEL_TEST, data, Int32, static_cast<int32_t>(statsType),
        StatsUtils::DEFAULT_VALUE);
    STATS_RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(LABEL_TEST, data, Int32, uid, StatsUtils::DEFAULT_VALUE);

    int ret = stub_->OnRemoteRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETTIME),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(LABEL_TEST, "Transact is failed, error code: %{public}d", ret);
    }

    STATS_RETURN_IF_READ_PARCEL_FAILED_WITH_RET(LABEL_TEST, reply, Uint64, time, StatsUtils::DEFAULT_VALUE);
    return time;
}

uint64_t StatsServiceTestProxy::GetTotalDataBytes(const StatsUtils::StatsType& statsType, const int32_t& uid)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(LABEL_TEST, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    uint64_t count = StatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(LABEL_TEST, data, Int32, static_cast<int32_t>(statsType),
        StatsUtils::DEFAULT_VALUE);
    STATS_RETURN_IF_WRITE_PARCEL_FAILED_WITH_RET(LABEL_TEST, data, Int32, uid, StatsUtils::DEFAULT_VALUE);

    int ret = stub_->OnRemoteRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETDATA),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(LABEL_TEST, "Transact is failed, error code: %{public}d", ret);
    }

    STATS_RETURN_IF_READ_PARCEL_FAILED_WITH_RET(LABEL_TEST, reply, Uint64, count, StatsUtils::DEFAULT_VALUE);
    return count;
}

double StatsServiceTestProxy::GetAppStatsMah(const int32_t& uid)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(LABEL_TEST, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(uid);

    int ret = stub_->OnRemoteRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETAPPMAH),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(LABEL_TEST, "Transact is failed, error code: %{public}d", ret);
    }

    double appStatsMah = reply.ReadDouble();
    STATS_HILOGD(LABEL_TEST, "Get stats mah: %{public}lf for uid: %{public}d", appStatsMah, uid);
    return appStatsMah;
}

bool StatsServiceTestProxy::SetOnBattery(bool isOnBattery)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, false);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(LABEL_TEST, "Write descriptor failed");
        return false;
    }

    data.WriteBool(isOnBattery);

    int ret = stub_->OnRemoteRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_SETONBATT),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(LABEL_TEST, "Transact is failed, error code: %{public}d", ret);
        return false;
    }
    return true;
}

double StatsServiceTestProxy::GetAppStatsPercent(const int32_t& uid)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(LABEL_TEST, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(uid);

    int ret = stub_->OnRemoteRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETAPPPER),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(LABEL_TEST, "Transact is failed, error code: %{public}d", ret);
    }

    double appStatsPercent = reply.ReadDouble();
    STATS_HILOGD(LABEL_TEST, "Get stats percent: %{public}lf for uid: %{public}d", appStatsPercent, uid);
    return appStatsPercent;
}

double StatsServiceTestProxy::GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& type)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(LABEL_TEST, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(type);

    int ret = stub_->OnRemoteRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETPARTMAH),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(LABEL_TEST, "Transact is failed, error code: %{public}d", ret);
    }

    double partStatsMah = reply.ReadDouble();
    STATS_HILOGD(LABEL_TEST, "Get stats mah: %{public}lf for type: %{public}d", partStatsMah, type);
    return partStatsMah;
}

double StatsServiceTestProxy::GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& type)
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, StatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(LABEL_TEST, "Write descriptor failed");
        return StatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(type);

    int ret = stub_->OnRemoteRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_GETPARTPER),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(LABEL_TEST, "Transact is failed, error code: %{public}d", ret);
    }

    double partStatsPercent = reply.ReadDouble();
    STATS_HILOGD(LABEL_TEST, "Get stats percent: %{public}lf for type: %{public}d", partStatsPercent, type);
    return partStatsPercent;
}

bool StatsServiceTestProxy::Reset()
{
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, false);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(LABEL_TEST, "Write descriptor failed");
        return false;
    }

    int ret = stub_->OnRemoteRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_RESET),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(LABEL_TEST, "Transact is failed, error code: %{public}d", ret);
        return false;
    }
    return true;
}

std::string StatsServiceTestProxy::ShellDump(const std::vector<std::string>& args, uint32_t argc)
{
    std::string result = "remote error";
    STATS_RETURN_IF_WITH_RET(stub_ == nullptr, result);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(LABEL_TEST, "Write descriptor failed");
        return result;
    }

    data.WriteUint32(argc);
    for (uint32_t i = 0; i < argc; i++) {
        data.WriteString(args[i]);
    }
    int ret = stub_->OnRemoteRequest(
        static_cast<uint32_t>(PowerMgr::BatteryStatsInterfaceCode::BATTERY_STATS_DUMP),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(LABEL_TEST, "SendRequest is failed, error code: %{public}d", ret);
        return result;
    }
    result = reply.ReadString();

    return result;
}
} // namespace PowerMgr
} // namespace OHOS