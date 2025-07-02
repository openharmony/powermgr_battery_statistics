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

#include "battery_stats_client.h"

#include "errors.h"
#include "refbase.h"
#include "if_system_ability_manager.h"
#include "iremote_broker.h"
#include "iservice_registry.h"

#include "stats_common.h"
#include "stats_errors.h"
#include "stats_log.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace PowerMgr {
BatteryStatsClient::BatteryStatsClient() {}
BatteryStatsClient::~BatteryStatsClient() {}
constexpr int32_t INIT_VALUE = -1;
constexpr int32_t PARAM_MAX_NUM = 10;

ErrCode BatteryStatsClient::Connect()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (proxy_ != nullptr) {
        return ERR_OK;
    }
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        STATS_HILOGE(COMP_FWK, "Fail to get registry");
        return E_STATS_GET_SYSTEM_ABILITY_MANAGER_FAILED;
    }
    sptr<IRemoteObject> remoteObject_ = sam->GetSystemAbility(POWER_MANAGER_BATT_STATS_SERVICE_ID);
    if (remoteObject_ == nullptr) {
        STATS_HILOGE(COMP_FWK, "Get batterystats service failed");
        return E_STATS_GET_SERVICE_FAILED;
    }
    proxy_ = iface_cast<IBatteryStats>(remoteObject_);
    return ERR_OK;
}

void BatteryStatsClient::ResetProxy(const wptr<IRemoteObject>& remote)
{
    std::lock_guard<std::mutex> lock(mutex_);
    STATS_RETURN_IF(proxy_ == nullptr);
    auto serviceRemote = proxy_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        proxy_ = nullptr;
    }
}

void BatteryStatsClient::BatteryStatsDeathRecipient::OnRemoteDied(const wptr<IRemoteObject>& remote)
{
    if (remote == nullptr) {
        STATS_HILOGE(COMP_FWK, "OnRemoteDied failed, remote is nullptr");
        return;
    }
    BatteryStatsClient::GetInstance().ResetProxy(remote);
    STATS_HILOGI(COMP_FWK, "Receive death notification");
}

BatteryStatsInfoList BatteryStatsClient::GetBatteryStats()
{
    STATS_HILOGD(COMP_FWK, "Call GetBatteryStats");
    BatteryStatsInfoList entityList;
    if (Connect() != ERR_OK) {
        lastError_ = StatsError::ERR_CONNECTION_FAIL;
        return entityList;
    }

    ParcelableBatteryStatsList parcelableEntityList;
    int32_t tempError = INIT_VALUE;
    proxy_->GetBatteryStatsIpc(parcelableEntityList, tempError);
    tempError_ = static_cast<StatsError>(tempError);
    return parcelableEntityList.statsList_;
}

double BatteryStatsClient::GetAppStatsMah(const int32_t& uid)
{
    STATS_HILOGD(COMP_FWK, "Call GetAppStatsMah");
    double appStatsMah = StatsUtils::DEFAULT_VALUE;
    if (Connect() != ERR_OK) {
        lastError_ = StatsError::ERR_CONNECTION_FAIL;
        return appStatsMah;
    }
    int32_t tempError = INIT_VALUE;
    proxy_->GetAppStatsMahIpc(uid, appStatsMah, tempError);
    tempError_ = static_cast<StatsError>(tempError);
    return appStatsMah;
}

void BatteryStatsClient::SetOnBattery(bool isOnBattery)
{
    STATS_HILOGD(COMP_FWK, "Call SetOnBattery");
    STATS_RETURN_IF(Connect() != ERR_OK);
    proxy_->SetOnBatteryIpc(isOnBattery);
}

double BatteryStatsClient::GetAppStatsPercent(const int32_t& uid)
{
    STATS_HILOGD(COMP_FWK, "Call GetAppStatsPercent");
    double appStatsPercent = StatsUtils::DEFAULT_VALUE;
    if (Connect() != ERR_OK) {
        lastError_ = StatsError::ERR_CONNECTION_FAIL;
        return appStatsPercent;
    }
    int32_t tempError = INIT_VALUE;
    proxy_->GetAppStatsPercentIpc(uid, appStatsPercent, tempError);
    tempError_ = static_cast<StatsError>(tempError);
    return appStatsPercent;
}

double BatteryStatsClient::GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& type)
{
    STATS_HILOGD(COMP_FWK, "Call GetPartStatsMah");
    double partStatsMah = StatsUtils::DEFAULT_VALUE;
    if (Connect() != ERR_OK) {
        lastError_ = StatsError::ERR_CONNECTION_FAIL;
        return partStatsMah;
    }
    int32_t tempError = INIT_VALUE;
    proxy_->GetPartStatsMahIpc(static_cast<int32_t>(type), partStatsMah, tempError);
    tempError_ = static_cast<StatsError>(tempError);
    return partStatsMah;
}

double BatteryStatsClient::GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& type)
{
    STATS_HILOGD(COMP_FWK, "Call GetPartStatsPercent");
    double partStatsPercent = StatsUtils::DEFAULT_VALUE;
    if (Connect() != ERR_OK) {
        lastError_ = StatsError::ERR_CONNECTION_FAIL;
        return partStatsPercent;
    }
    int32_t tempError = INIT_VALUE;
    proxy_->GetPartStatsPercentIpc(static_cast<int32_t>(type), partStatsPercent, tempError);
    tempError_ = static_cast<StatsError>(tempError);
    return partStatsPercent;
}

void BatteryStatsClient::Reset()
{
    STATS_HILOGD(COMP_FWK, "Call Reset");
    STATS_RETURN_IF(Connect() != ERR_OK);
    proxy_->ResetIpc();
}

uint64_t BatteryStatsClient::GetTotalTimeSecond(const StatsUtils::StatsType& statsType, const int32_t& uid)
{
    STATS_HILOGD(COMP_FWK, "Call GetTotalTimeSecond");
    uint64_t time = StatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WITH_RET(Connect() != ERR_OK, time);
    proxy_->GetTotalTimeSecondIpc(static_cast<int32_t>(statsType), uid, time);
    return time;
}

uint64_t BatteryStatsClient::GetTotalDataBytes(const StatsUtils::StatsType& statsType, const int32_t& uid)
{
    STATS_HILOGD(COMP_FWK, "Call GetTotalDataBytes");
    uint64_t count = StatsUtils::DEFAULT_VALUE;
    STATS_RETURN_IF_WITH_RET(Connect() != ERR_OK, count);
    proxy_->GetTotalDataBytesIpc(static_cast<int32_t>(statsType), uid, count);
    return count;
}

std::string BatteryStatsClient::Dump(const std::vector<std::string>& args)
{
    STATS_HILOGD(COMP_FWK, "Call Dump");
    std::string error = "can't connect service";
    STATS_RETURN_IF_WITH_RET(Connect() != ERR_OK, error);
    std::string dumpshell = "remote error";
    int32_t argc = args.size();
    if (argc >= PARAM_MAX_NUM) {
        STATS_HILOGE(COMP_FWK, "params exceed limit, argc=%{public}d", argc);
        return dumpshell;
    }
    proxy_->ShellDumpIpc(args, argc, dumpshell);
    return dumpshell;
}

StatsError BatteryStatsClient::GetLastError()
{
    if (lastError_ != StatsError::ERR_OK) {
        StatsError tmpError = lastError_;
        lastError_ = StatsError::ERR_OK;
        return tmpError;
    }
    STATS_RETURN_IF_WITH_RET(Connect() != ERR_OK, StatsError::ERR_CONNECTION_FAIL);
    return tempError_;
}
}  // namespace PowerMgr
}  // namespace OHOS
