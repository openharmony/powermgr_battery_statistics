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

#include "battery_stats_client.h"
#include "battery_stats_proxy.h"
#include "string_ex.h"
#include "datetime_ex.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "system_ability_definition.h"
#include "stats_common.h"

namespace OHOS {
namespace PowerMgr {
BatteryStatsClient::BatteryStatsClient() {}
BatteryStatsClient::~BatteryStatsClient() {}

ErrCode BatteryStatsClient::Connect()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    std::lock_guard<std::mutex> lock(mutex_);
    if (proxy_ != nullptr) {
        return ERR_OK;
    }
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "Fail to get Registry");
        return E_STATS_GET_SYSTEM_ABILITY_MANAGER_FAILED;
    }
    sptr<IRemoteObject> remoteObject_ = sam->CheckSystemAbility(POWER_MANAGER_BATT_STATS_SERVICE_ID);
    if (remoteObject_ == nullptr) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "GetSystemAbility failed.");
        return E_STATS_GET_SERVICE_FAILED;
    }
    proxy_ = iface_cast<IBatteryStats>(remoteObject_);
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Connect BatteryStatsService ok.");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
    return ERR_OK;
}

void BatteryStatsClient::ResetProxy(const wptr<IRemoteObject>& remote)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    std::lock_guard<std::mutex> lock(mutex_);
    STATS_RETURN_IF(proxy_ == nullptr);
    auto serviceRemote = proxy_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        proxy_ = nullptr;
    }
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

void BatteryStatsClient::BatteryStatsDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    if (remote == nullptr) {
        STATS_HILOGE(STATS_MODULE_INNERKIT, "BatteryStatsDeathRecipient::OnRemoteDied failed, remote is nullptr.");
        return;
    }
    BatteryStatsClient::GetInstance().ResetProxy(remote);
    STATS_HILOGI(STATS_MODULE_INNERKIT, "BatteryStatsDeathRecipient::Recv death notice.");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

BatteryStatsInfoList BatteryStatsClient::GetBatteryStats()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    BatteryStatsInfoList entityList;
    STATS_RETURN_IF_WITH_RET(Connect() != ERR_OK, entityList);
    entityList = proxy_->GetBatteryStats();
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Calling GetBatteryStats Success!");
    return entityList;
}

double BatteryStatsClient::GetAppStatsMah(const int32_t& uid)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    double appStatsMah = BatteryStatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WITH_RET(Connect() != ERR_OK, appStatsMah);
    appStatsMah = proxy_->GetAppStatsMah(uid);
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Calling GetAppStatsMah Success!");
    return appStatsMah;
}

double BatteryStatsClient::GetAppStatsPercent(const int32_t& uid)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    double appStatsPercent = BatteryStatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WITH_RET(Connect() != ERR_OK, appStatsPercent);
    appStatsPercent = proxy_->GetAppStatsPercent(uid);
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Calling GetAppStatsPercent Success!");
    return appStatsPercent;
}

double BatteryStatsClient::GetPartStatsMah(const BatteryStatsInfo::BatteryStatsType& type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    double partStatsMah = BatteryStatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WITH_RET(Connect() != ERR_OK, partStatsMah);
    partStatsMah = proxy_->GetPartStatsMah(type);
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Calling GetPartStatsMah Success!");
    return partStatsMah;
}

double BatteryStatsClient::GetPartStatsPercent(const BatteryStatsInfo::BatteryStatsType& type)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    double partStatsPercent = BatteryStatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WITH_RET(Connect() != ERR_OK, partStatsPercent);
    partStatsPercent = proxy_->GetPartStatsPercent(type);
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Calling GetPartStatsPercent Success!");
    return partStatsPercent;
}

void BatteryStatsClient::Reset()
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    STATS_RETURN_IF(Connect() != ERR_OK);
    proxy_->Reset();
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Calling Reset Success!");
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Exit");
}

uint64_t BatteryStatsClient::GetTotalTimeSecond(const std::string& hwId, const int32_t& uid)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    uint64_t time = BatteryStatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WITH_RET(Connect() != ERR_OK, time);
    time = proxy_->GetTotalTimeSecond(hwId, uid);
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Calling GetTotalTimeSecond Success!");
    return time;
}

uint64_t BatteryStatsClient::GetTotalDataBytes(const std::string& hwId, const int32_t& uid)
{
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Enter");
    uint64_t count = BatteryStatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WITH_RET(Connect() != ERR_OK, count);
    count = proxy_->GetTotalDataBytes(hwId, uid);
    STATS_HILOGI(STATS_MODULE_INNERKIT, "Calling GetTotalDataBytes Success!");
    return count;
}
}  // namespace PowerMgr
}  // namespace OHOS
