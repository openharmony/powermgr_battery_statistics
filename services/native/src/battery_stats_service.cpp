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

#include "battery_stats_service.h"

#include <file_ex.h>
#include <cmath>
#include <ipc_skeleton.h>

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_subscribe_info.h"
#include "common_event_support.h"
#include "hisysevent.h"
#include "hisysevent_manager.h"
#include "iservice_registry.h"
#include "if_system_ability_manager.h"
#include "permission.h"
#include "system_ability_definition.h"
#include "watchdog.h"

#include "battery_stats_dumper.h"
#include "battery_stats_listener.h"
#include "battery_stats_subscriber.h"
#include "stats_common.h"

namespace OHOS {
namespace PowerMgr {
namespace {
auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
const bool G_REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(g_statsService.GetRefPtr());
}

BatteryStatsService::BatteryStatsService() : SystemAbility(POWER_MANAGER_BATT_STATS_SERVICE_ID, true) {}

BatteryStatsService::~BatteryStatsService() {}

void BatteryStatsService::OnStart()
{
    if (ready_) {
        STATS_HILOGI(COMP_SVC, "OnStart is ready, nothing to do");
        return;
    }
    if (!(Init())) {
        STATS_HILOGE(COMP_SVC, "Call init failed");
        return;
    }
    AddSystemAbilityListener(DFX_SYS_EVENT_SERVICE_ABILITY_ID);
    AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
    if (!Publish(DelayedStatsSpSingleton<BatteryStatsService>::GetInstance())) {
        STATS_HILOGE(COMP_SVC, "OnStart register to system ability manager failed");
        return;
    }

    ready_ = true;
}

void BatteryStatsService::OnStop()
{
    if (!ready_) {
        STATS_HILOGI(COMP_SVC, "OnStop is not ready, nothing to do");
        return;
    }
    ready_ = false;
    RemoveSystemAbilityListener(DFX_SYS_EVENT_SERVICE_ABILITY_ID);
    RemoveSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
    HiviewDFX::HiSysEventManager::RemoveListener(listenerPtr_);
    if (!OHOS::EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriberPtr_)) {
        STATS_HILOGE(COMP_SVC, "OnStart unregister to commonevent manager failed");
    }
}

void BatteryStatsService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    STATS_HILOGI(COMP_SVC, "systemAbilityId=%{public}d, deviceId=%{private}s", systemAbilityId,
                 deviceId.c_str());
    if (systemAbilityId == DFX_SYS_EVENT_SERVICE_ABILITY_ID) {
        AddHiSysEventListener();
    }
    if (systemAbilityId == COMMON_EVENT_SERVICE_ID) {
        SubscribeCommonEvent();
    }
}

bool BatteryStatsService::Init()
{
    if (parser_ == nullptr) {
        parser_ = std::make_shared<BatteryStatsParser>();
        if (!parser_->Init()) {
            STATS_HILOGE(COMP_SVC, "Battery stats parser initialization failed");
            return false;
        }
    }

    if (core_ == nullptr) {
        core_ = std::make_shared<BatteryStatsCore>();
        if (!core_->Init()) {
            STATS_HILOGE(COMP_SVC, "Battery stats core initialization failed");
            return false;
        }
    }

    if (detector_ == nullptr) {
        detector_ = std::make_shared<BatteryStatsDetector>();
    }

    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create("BatteryStatsEventRunner");
        if (runner_ == nullptr) {
            STATS_HILOGE(COMP_SVC, "Create EventRunner failed");
            return false;
        }
    }

    if (!handler_) {
        handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner_);
        if (handler_ == nullptr) {
            STATS_HILOGE(COMP_SVC, "Create EventHandler failed");
            return false;
        }
        HiviewDFX::Watchdog::GetInstance().AddThread("BatteryStatsEventHandler", handler_);
    }

    return true;
}

bool BatteryStatsService::SubscribeCommonEvent()
{
    bool result = false;
    OHOS::EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_SHUTDOWN);
    matchingSkills.AddEvent(OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_BOOT_COMPLETED);
    matchingSkills.AddEvent(OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_BATTERY_CHANGED);
    OHOS::EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    if (!subscriberPtr_) {
        subscriberPtr_ = std::make_shared<BatteryStatsSubscriber>(subscribeInfo);
    }
    result = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(subscriberPtr_);
    if (!result) {
        STATS_HILOGE(COMP_SVC, "Subscribe CommonEvent failed");
    }
    return result;
}

bool BatteryStatsService::AddHiSysEventListener()
{
    if (!listenerPtr_) {
        OHOS::EventFwk::CommonEventSubscribeInfo info;
        listenerPtr_ = std::make_shared<BatteryStatsListener>();
    }
    OHOS::HiviewDFX::ListenerRule statsRule("PowerStats");
    OHOS::HiviewDFX::ListenerRule distSchedRule("DISTSCHEDULE", "START_REMOTE_ABILITY");
    std::vector<OHOS::HiviewDFX::ListenerRule> sysRules;
    sysRules.push_back(statsRule);
    sysRules.push_back(distSchedRule);
    auto res = HiviewDFX::HiSysEventManager::AddListener(listenerPtr_, sysRules);
    if (res != 0) {
        STATS_HILOGE(COMP_SVC, "Listener added failed");
    }
    return res;
}

bool BatteryStatsService::IsServiceReady() const
{
    return ready_;
}

BatteryStatsInfoList BatteryStatsService::GetBatteryStats()
{
    BatteryStatsInfoList statsInfoList = {};
    if (!Permission::IsSystem()) {
        return statsInfoList;
    }
    core_->ComputePower();
    statsInfoList = core_->GetBatteryStats();
    return statsInfoList;
}

int32_t BatteryStatsService::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    std::lock_guard lock(mutex_);
    std::vector<std::string> argsInStr;
    std::transform(args.begin(), args.end(), std::back_inserter(argsInStr),
        [](const std::u16string &arg) {
        std::string ret = Str16ToStr8(arg);
        STATS_HILOGD(COMP_SVC, "arg: %{public}s", ret.c_str());
        return ret;
    });
    std::string result;
    BatteryStatsDumper::Dump(argsInStr, result);
    if (!SaveStringToFd(fd, result)) {
        STATS_HILOGE(COMP_SVC, "Dump save to fd failed, %{public}s", result.c_str());
        return ERR_OK;
    }
    return ERR_OK;
}

double BatteryStatsService::GetAppStatsMah(const int32_t& uid)
{
    if (!Permission::IsSystem()) {
        return StatsUtils::DEFAULT_VALUE;
    }
    core_->ComputePower();
    return core_->GetAppStatsMah(uid);
}

double BatteryStatsService::GetAppStatsPercent(const int32_t& uid)
{
    if (!Permission::IsSystem()) {
        return StatsUtils::DEFAULT_VALUE;
    }
    core_->ComputePower();
    return core_->GetAppStatsPercent(uid);
}

double BatteryStatsService::GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& type)
{
    if (!Permission::IsSystem()) {
        return StatsUtils::DEFAULT_VALUE;
    }
    core_->ComputePower();
    return core_->GetPartStatsMah(type);
}

double BatteryStatsService::GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& type)
{
    if (!Permission::IsSystem()) {
        return StatsUtils::DEFAULT_VALUE;
    }
    core_->ComputePower();
    return core_->GetPartStatsPercent(type);
}

uint64_t BatteryStatsService::GetTotalTimeSecond(const StatsUtils::StatsType& statsType, const int32_t& uid)
{
    STATS_HILOGD(COMP_SVC, "statsType: %{public}d, uid: %{public}d", statsType, uid);
    uint64_t timeSecond;
    if (uid > StatsUtils::INVALID_VALUE) {
        double timeMs = static_cast<double>(core_->GetTotalTimeMs(uid, statsType));
        timeSecond = round(timeMs / StatsUtils::MS_IN_SECOND);
    } else {
        double timeMs = static_cast<double>(core_->GetTotalTimeMs(statsType));
        timeSecond = round(timeMs / StatsUtils::MS_IN_SECOND);
    }
    return timeSecond;
}

uint64_t BatteryStatsService::GetTotalDataBytes(const StatsUtils::StatsType& statsType, const int32_t& uid)
{
    return core_->GetTotalDataCount(statsType, uid);
}

void BatteryStatsService::Reset()
{
    if (!Permission::IsSystem()) {
        return;
    }
    core_->Reset();
}

std::shared_ptr<BatteryStatsCore> BatteryStatsService::GetBatteryStatsCore() const
{
    return core_;
}

std::shared_ptr<BatteryStatsParser> BatteryStatsService::GetBatteryStatsParser() const
{
    return parser_;
}

std::shared_ptr<BatteryStatsDetector> BatteryStatsService::GetBatteryStatsDetector() const
{
    return detector_;
}

void BatteryStatsService::SetOnBattery(bool isOnBattery)
{
    if (!Permission::IsSystem()) {
        return;
    }
    StatsHelper::SetOnBattery(isOnBattery);
}

std::string BatteryStatsService::ShellDump(const std::vector<std::string>& args, uint32_t argc)
{
    if (!Permission::IsSystem()) {
        return "";
    }
    std::lock_guard lock(mutex_);
    pid_t pid = IPCSkeleton::GetCallingPid();
    std::string result;
    bool ret = BatteryStatsDumper::Dump(args, result);
    STATS_HILOGI(COMP_SVC, "PID: %{public}d, Dump result :%{public}d", pid, ret);
    return result;
}
} // namespace PowerMgr
} // namespace OHOS
