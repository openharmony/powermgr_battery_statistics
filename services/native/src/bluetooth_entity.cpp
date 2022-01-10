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

#include "bluetooth_entity.h"

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

BluetoothEntity::BluetoothEntity()
{
    SetType(BatteryStatsInfo::STATS_TYPE_BLUETOOTH);
}

void BluetoothEntity::Calculate()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    // Calculate Bluetooth on power
    auto parser = statsService->GetBatteryStatsParser();
    auto core = statsService->GetBatteryStatsCore();
    auto bluetoothOnAverage = parser->GetAveragePower(BatteryStatsUtils::TYPE_BLUETOOTH_ON);
    auto bluetoothOnTime = core->GetTotalTimeMs(BatteryStatsUtils::TYPE_BLUETOOTH_ON);
    auto bluetoothOnPower = bluetoothOnAverage * bluetoothOnTime;

    // Sum Bluetooth power(mAms)
    auto bluetoothPower = bluetoothOnPower + GetBluetoothUidPower();
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth on time: %{public}ld", bluetoothOnTime);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth on average: %{public}lf", bluetoothOnAverage);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth on power: %{public}lf", bluetoothOnPower);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Calculate bluetooth total power: %{public}lf", bluetoothPower);
    SetPower(bluetoothPower);
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

double BluetoothEntity::GetBluetoothUidPower()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    double bluetoothUidPower = BatteryStatsUtils::DEFAULT_VALUE;
    auto core = statsService->GetBatteryStatsCore();
    auto bundleObj =
    DelayedSingleton<AppExecFwk::SysMrgClient>::GetInstance()->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleObj == nullptr) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "failed to get bundle manager service, return default power");
        return bluetoothUidPower;
    }
    sptr<AppExecFwk::IBundleMgr> bmgr = iface_cast<AppExecFwk::IBundleMgr>(bundleObj);
    std::string bundleName = "com.ohos.bluetooth";
    int32_t bluetoothUid = bmgr->GetUidByBundleName(bundleName, AppExecFwk::Constants::DEFAULT_USERID);

    auto entity = core->GetEntity(bluetoothUid);
    if (entity != nullptr) {
        bluetoothUidPower = entity->GetTotalPower();
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Get bluetooth uid power: %{public}lf", bluetoothUidPower);
    return bluetoothUidPower;
}
} // namespace PowerMgr
} // namespace OHOS
