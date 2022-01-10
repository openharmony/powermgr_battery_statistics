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

#include "battery_stats_proxy.h"

#include <ipc_types.h>
#include <message_parcel.h>
#include <string_ex.h>

#include "battery_stats_utils.h"
#include "stats_common.h"

namespace OHOS {
namespace PowerMgr {
BatteryStatsInfoList BatteryStatsProxy::GetBatteryStats()
{
    STATS_HILOGD(STATS_MODULE_INNERKIT, "%{public}s.", __func__);
    BatteryStatsInfoList infoList;
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, infoList);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "%{public}s write descriptor failed!", __func__);
        return infoList;
    }

    int ret = remote->SendRequest(static_cast<int>(IBatteryStats::BATTERY_STATS_GET),
        data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "%{public}s SendRequest is failed, error code: %{public}d", __func__, ret);
        return infoList;
    }
    int32_t size = reply.ReadInt32();
    for (int i = 0; i < size; ++i) {
        std::shared_ptr<BatteryStatsInfo> info = std::make_shared<BatteryStatsInfo>();
        info->ReadFromParcel(reply);
        infoList.emplace_back(info);
    }
    return infoList;
}

uint64_t BatteryStatsProxy::GetTotalTimeSecond(const std::string& hwId, const int32_t& uid)
{
    STATS_HILOGD(STATS_MODULE_INNERKIT, "%{public}s.", __func__);
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, BatteryStatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "%{public}s - write descriptor failed!", __func__);
        return BatteryStatsUtils::DEFAULT_VALUE;
    }

    data.WriteCString(hwId.c_str());
    data.WriteInt32(uid);

    int ret = remote->SendRequest(static_cast<int>(IBatteryStats::BATTERY_STATS_GETTIME), data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "%{public}s - Transact is failed, error code: %{public}d", __func__, ret);
    }
    uint64_t time = BatteryStatsUtils::DEFAULT_VALUE;
    time = reply.ReadUint64();
    return time;
}

uint64_t BatteryStatsProxy::GetTotalDataBytes(const std::string& hwId, const int32_t& uid)
{
    STATS_HILOGD(STATS_MODULE_INNERKIT, "%{public}s.", __func__);
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, BatteryStatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "%{public}s - write descriptor failed!", __func__);
        return BatteryStatsUtils::DEFAULT_VALUE;
    }

    data.WriteCString(hwId.c_str());
    data.WriteInt32(uid);

    int ret = remote->SendRequest(static_cast<int>(IBatteryStats::BATTERY_STATS_GETDATA), data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "%{public}s - Transact is failed, error code: %{public}d", __func__, ret);
    }
    uint64_t count = BatteryStatsUtils::DEFAULT_VALUE;
    count = reply.ReadUint64();
    return count;
}

double BatteryStatsProxy::GetAppStatsMah(const int32_t& uid)
{
    STATS_HILOGD(STATS_MODULE_INNERKIT, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, BatteryStatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Write descriptor failed!");
        return BatteryStatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(uid);

    int ret = remote->SendRequest(static_cast<int>(IBatteryStats::BATTERY_STATS_GETAPPMAH), data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Transact is failed, error code: %{public}d", ret);
    }

    double appStatsMah = BatteryStatsUtils::DEFAULT_VALUE;
    appStatsMah = reply.ReadFloat();
    STATS_HILOGD(STATS_MODULE_INNERKIT, "Got stats mah: %{public}lf for uid: %{public}d", appStatsMah, uid);
    return appStatsMah;
}

double BatteryStatsProxy::GetAppStatsPercent(const int32_t& uid)
{
    STATS_HILOGD(STATS_MODULE_INNERKIT, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, BatteryStatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Write descriptor failed!");
        return BatteryStatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(uid);

    int ret = remote->SendRequest(static_cast<int>(IBatteryStats::BATTERY_STATS_GETAPPPER), data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Transact is failed, error code: %{public}d", ret);
    }

    double appStatsPercent = BatteryStatsUtils::DEFAULT_VALUE;
    appStatsPercent = reply.ReadFloat();
    STATS_HILOGD(STATS_MODULE_INNERKIT, "Got stats percent: %{public}lf for uid: %{public}d", appStatsPercent, uid);
    return appStatsPercent;
}

double BatteryStatsProxy::GetPartStatsMah(const BatteryStatsInfo::BatteryStatsType& type)
{
    STATS_HILOGD(STATS_MODULE_INNERKIT, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, BatteryStatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Write descriptor failed!");
        return BatteryStatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(type);

    int ret = remote->SendRequest(static_cast<int>(IBatteryStats::BATTERY_STATS_GETPARTMAH), data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Transact is failed, error code: %{public}d", ret);
    }

    double partStatsMah = BatteryStatsUtils::DEFAULT_VALUE;
    partStatsMah = reply.ReadFloat();
    STATS_HILOGD(STATS_MODULE_INNERKIT, "Got stats mah: %{public}lf for type: %{public}d", partStatsMah, type);
    return partStatsMah;
}

double BatteryStatsProxy::GetPartStatsPercent(const BatteryStatsInfo::BatteryStatsType& type)
{
    STATS_HILOGD(STATS_MODULE_INNERKIT, "Enter");
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF_WITH_RET(remote == nullptr, BatteryStatsUtils::DEFAULT_VALUE);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Write descriptor failed!");
        return BatteryStatsUtils::DEFAULT_VALUE;
    }

    data.WriteInt32(type);

    int ret = remote->SendRequest(static_cast<int>(IBatteryStats::BATTERY_STATS_GETPARTPER), data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Transact is failed, error code: %{public}d", ret);
    }

    double partStatsPercent = BatteryStatsUtils::DEFAULT_VALUE;
    partStatsPercent = reply.ReadFloat();
    STATS_HILOGD(STATS_MODULE_INNERKIT, "Got stats percent: %{public}lf for type: %{public}d", partStatsPercent, type);
    return partStatsPercent;
}

void BatteryStatsProxy::Reset()
{
    STATS_HILOGD(STATS_MODULE_INNERKIT, "%{public}s.", __func__);
    sptr<IRemoteObject> remote = Remote();
    STATS_RETURN_IF(remote == nullptr);

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(BatteryStatsProxy::GetDescriptor())) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "%{public}s - write descriptor failed!", __func__);
        return;
    }

    int ret = remote->SendRequest(static_cast<int>(IBatteryStats::BATTERY_STATS_RESET), data, reply, option);
    if (ret != ERR_OK) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "%{public}s - Transact is failed, error code: %{public}d", __func__, ret);
    }
}
} // namespace PowerMgr
} // namespace OHOS