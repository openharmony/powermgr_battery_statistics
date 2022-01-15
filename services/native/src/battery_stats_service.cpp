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

#include "battery_stats_service.h"

#include <file_ex.h>
#include <cmath>

#include "common_event_data.h"
#include "common_event_manager.h"
#include "common_event_subscribe_info.h"
#include "common_event_support.h"
#include "hisysevent.h"
#include "hisysevent_manager.h"
#include "system_ability_definition.h"

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
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    if (ready_) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "OnStart is ready, nothing to do");
        return;
    }
    if (!(Init())) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "OnStart call init fail");
        return;
    }
    if (!Publish(DelayedStatsSpSingleton<BatteryStatsService>::GetInstance())) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "OnStart register to system ability manager failed.");
        return;
    }
    if (!SubscribeCommonEvent()) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "OnStart register to commonevent manager failed.");
        return;
    }

    if (!AddListener()) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "OnStart add listener to hisysevent manager failed.");
        return;
    }

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
    HiviewDFX::HiSysEventManager::RemoveListener(listenerPtr_);
    if (!OHOS::EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriberPtr_)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "OnStart unregister to commonevent manager failed.");
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

    STATS_HILOGI(STATS_MODULE_SERVICE, "Battery stats service initialization success");
    return true;
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
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return result;
}

bool BatteryStatsService::AddListener()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    bool result = false;
    if (!listenerPtr_) {
        OHOS::EventFwk::CommonEventSubscribeInfo info;
        listenerPtr_ = std::make_shared<BatteryStatsListener>();
    }
    struct HiviewDFX::ListenerRule statsRule;
    statsRule.ruleType = HiviewDFX::HiSysEvent::EventType::STATISTIC;
    statsRule.domain = HiviewDFX::HiSysEvent::Domain::POWERMGR;
    std::vector<struct HiviewDFX::ListenerRule> sysRules;
    sysRules.push_back(statsRule);
    int res = HiviewDFX::HiSysEventManager::AddEventListener(listenerPtr_, sysRules);

    switch (res) {
        case 0:
            STATS_HILOGD(STATS_MODULE_SERVICE, "Listener has already been added, res code: %{public}d", res);
            result = true;
            break;
        case 1:
            STATS_HILOGD(STATS_MODULE_SERVICE, "Listener is added successfully, res code: %{public}d", res);
            result = true;
            break;
        default:
            STATS_HILOGE(STATS_MODULE_SERVICE, "Add Hisys event failed, res code: %{public}d", res);
            break;
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return result;
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
    STATS_HILOGI(STATS_MODULE_SERVICE, "Get time: %{public}" PRId64 " seconds", timeSecond);
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
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter service reset");
    StatsHelper::SetOnBattery(isOnBattery);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit service reset");
}
} // namespace PowerMgr
} // namespace OHOS
