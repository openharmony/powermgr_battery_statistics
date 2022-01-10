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

#include "wifi_entity.h"
#include "battery_stats_parser.h"
#include "battery_stats_service.h"

#include "stats_hilog_wrapper.h"
#include "bundle_mgr_interface.h"
#include "sys_mgr_client.h"
#include "system_ability_definition.h"
#include "bundle_constants.h"

namespace OHOS {
namespace PowerMgr {
namespace {
    auto statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
}

WifiEntity::WifiEntity()
{
    SetType(BatteryStatsInfo::STATS_TYPE_WIFI);
}

void WifiEntity::Calculate()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Calculate WIFI on power
    auto parser = statsService->GetBatteryStatsParser();
    auto core = statsService->GetBatteryStatsCore();
    auto wifiOnAverage = parser->GetAveragePower(BatteryStatsUtils::TYPE_WIFI_ON);
    auto wifiOnTime = core->GetTotalTimeMs(BatteryStatsUtils::TYPE_WIFI_ON);
    double wifiOnPower = wifiOnAverage * wifiOnTime;
    auto wifiPower = wifiOnPower + GetWifiUidPower();
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi on time: %{public}ld", wifiOnTime);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi on average: %{public}lf", wifiOnAverage);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi on power: %{public}lf", wifiOnPower);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate wifi on total power: %{public}lf", wifiPower);
    SetPower(wifiPower);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double WifiEntity::GetWifiUidPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double wifiUidPower = BatteryStatsUtils::DEFAULT_VALUE;
    auto core = statsService->GetBatteryStatsCore();
    auto bundleObj =
    DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "failed to get bundle manager service, return default power");
        return wifiUidPower;
    }
    sptr<AppExecFwk::IBundleMgr> bmgr = iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
    std::string bundleName = "com.ohos.wifi";
    int32_t wifiUid = bmgr->GetUidByBundleName(bundleName, AppExecFwk::Constants::DEFAULT_USERID);

    auto entity = statsService->GetBatteryStatsCore()->GetEntity(wifiUid);
    if (entity != nullptr) {
        wifiUidPower = entity->GetTotalPower();
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Get wifi uid power: %{public}lf", wifiUidPower);
    return wifiUidPower;
}
} // namespace PowerMgr
} // namespace OHOS
