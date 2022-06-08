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
constexpr int32_t SA_ID_HISYSEVENT = 1203;
}

BatteryStatsService::BatteryStatsService() : SystemAbility(POWER_MANAGER_BATT_STATS_SERVICE_ID, true) {}

BatteryStatsService::~BatteryStatsService() {}

void BatteryStatsService::OnStart()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (ready_) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "OnStart is ready, nothing to do");
        return;
    }
    if (!(Init())) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "OnStart call init fail");
        return;
    }
    AddSystemAbilityListener(SA_ID_HISYSEVENT);
    if (!Publish(DelayedStatsSpSingleton<BatteryStatsService>::GetInstance())) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "OnStart register to system ability manager failed.");
        return;
    }
    InitDependency();

    ready_ = true;
    STATS_HILOGI(STATS_MODULE_SERVICE, "OnStart and add system ability success");
}

void BatteryStatsService::OnStop()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "stop service");
    if (!ready_) {
        return;
    }
    ready_ = false;
    RemoveSystemAbilityListener(SA_ID_HISYSEVENT);
    HiviewDFX::HiSysEventManager::RemoveListener(listenerPtr_);
    if (!OHOS::EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriberPtr_)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "OnStart unregister to commonevent manager failed.");
    }
}

void BatteryStatsService::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    STATS_HILOGD(STATS_MODULE_SERVICE, "systemAbilityId=%{public}d, deviceId=%{private}s", systemAbilityId,
                 deviceId.c_str());
    if (systemAbilityId == SA_ID_HISYSEVENT) {
        AddHiSysEventListener();
    }
}

bool BatteryStatsService::Init()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (parser_ == nullptr) {
        parser_ = std::make_shared<BatteryStatsParser>();
        if (!parser_->Init()) {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Battery stats parser initialization failed");
            return false;
        }
    }

    if (core_ == nullptr) {
        core_ = std::make_shared<BatteryStatsCore>();
        if (!core_->Init()) {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Battery stats core initialization failed");
            return false;
        }
    }

    if (detector_ == nullptr) {
        detector_ = std::make_shared<BatteryStatsDetector>();
    }

    if (!runner_) {
        runner_ = AppExecFwk::EventRunner::Create("BatteryStatsEventRunner");
        if (runner_ == nullptr) {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create EventRunner failed");
            return false;
        }
    }

    if (!handler_) {
        handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner_);
        if (handler_ == nullptr) {
            STATS_HILOGE(STATS_MODULE_SERVICE, "Create EventHandler failed");
            return false;
        }
        HiviewDFX::Watchdog::GetInstance().AddThread("BatteryStatsEventHandler", handler_, WATCH_DOG_DELAY_S);
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Battery stats service initialization success");
    return true;
}

void BatteryStatsService::InitDependency()
{
    sptr<ISystemAbilityManager> sysMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();

    if (!sysMgr || !sysMgr->CheckSystemAbility(COMMON_EVENT_SERVICE_ID)) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Dependency is not ready yet, re-check in 2s later");
        auto task = [this]() { this->InitDependency(); };
        handler_->PostTask(task, DEPENDENCY_CHECK_DELAY_MS);
        return;
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Dependency is ready now");
    if (!SubscribeCommonEvent()) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Register to commonevent manager failed.");
    } else {
        STATS_HILOGI(STATS_MODULE_SERVICE, "Register to commonevent manager successfuly.");
    }
}

bool BatteryStatsService::SubscribeCommonEvent()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
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
        STATS_HILOGE(STATS_MODULE_SERVICE, "Subscribe CommonEvent failed");
    }
    STATS_HILOGD(STATS_MODULE_SERVICE, "Subscribe CommonEvent successfully");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return result;
}

bool BatteryStatsService::AddHiSysEventListener()
{
    if (!listenerPtr_) {
        OHOS::EventFwk::CommonEventSubscribeInfo info;
        listenerPtr_ = std::make_shared<BatteryStatsListener>();
    }
    OHOS::HiviewDFX::ListenerRule statsRule("PowerStats");
    std::vector<OHOS::HiviewDFX::ListenerRule> sysRules;
    sysRules.push_back(statsRule);
    auto res = HiviewDFX::HiSysEventManager::AddEventListener(listenerPtr_, sysRules);
    if (res == 0) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Listener is added successfully");
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Listener is added failed");
    }
    return res;
}

bool BatteryStatsService::IsServiceReady() const
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    return ready_;
}

BatteryStatsInfoList BatteryStatsService::GetBatteryStats()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    core_->ComputePower();
    return core_->GetBatteryStats();
}

int32_t BatteryStatsService::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    std::lock_guard lock(mutex_);
    std::vector<std::string> argsInStr;
    std::transform(args.begin(), args.end(), std::back_inserter(argsInStr),
        [](const std::u16string &arg) {
        std::string ret = Str16ToStr8(arg);
        STATS_HILOGI(STATS_MODULE_SERVICE, "arg: %{public}s", ret.c_str());
        return ret;
    });
    std::string result;
    BatteryStatsDumper::Dump(argsInStr, result);
    if (!SaveStringToFd(fd, result)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Dump failed, save to fd failed.");
        STATS_HILOGE(STATS_MODULE_SERVICE, "Dump Info:\n");
        STATS_HILOGE(STATS_MODULE_SERVICE, "%{public}s", result.c_str());
        return ERR_OK;
    }
    return ERR_OK;
}

double BatteryStatsService::GetAppStatsMah(const int32_t& uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    core_->ComputePower();
    return core_->GetAppStatsMah(uid);
}

double BatteryStatsService::GetAppStatsPercent(const int32_t& uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    core_->ComputePower();
    return core_->GetAppStatsPercent(uid);
}

double BatteryStatsService::GetPartStatsMah(const BatteryStatsInfo::ConsumptionType& type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    core_->ComputePower();
    return core_->GetPartStatsMah(type);
}

double BatteryStatsService::GetPartStatsPercent(const BatteryStatsInfo::ConsumptionType& type)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    core_->ComputePower();
    return core_->GetPartStatsPercent(type);
}

uint64_t BatteryStatsService::GetTotalTimeSecond(const StatsUtils::StatsType& statsType, const int32_t& uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    uint64_t timeSecond = StatsUtils::DEFAULT_VALUE;
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
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    return core_->GetTotalDataCount(statsType, uid);
}

void BatteryStatsService::Reset()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter service reset");
    core_->Reset();
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit service reset");
}

std::shared_ptr<BatteryStatsCore> BatteryStatsService::GetBatteryStatsCore() const
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    return core_;
}

std::shared_ptr<BatteryStatsParser> BatteryStatsService::GetBatteryStatsParser() const
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    return parser_;
}

std::shared_ptr<BatteryStatsDetector> BatteryStatsService::GetBatteryStatsDetector() const
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    return detector_;
}

void BatteryStatsService::SetOnBattery(bool isOnBattery)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    STATS_HILOGI(STATS_MODULE_SERVICE, "isOnBattery: %{public}d", isOnBattery);
    StatsHelper::SetOnBattery(isOnBattery);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

std::string BatteryStatsService::ShellDump(const std::vector<std::string>& args, uint32_t argc)
{
    std::lock_guard lock(mutex_);
    pid_t pid = IPCSkeleton::GetCallingPid();
    STATS_HILOGI(STATS_MODULE_SERVICE, "PID: %{public}d Calls!", pid);

    std::string result;
    bool ret = BatteryStatsDumper::Dump(args, result);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calling Dump result :%{public}d", ret);
    return result;
}
} // namespace PowerMgr
} // namespace OHOS
