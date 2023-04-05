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

#ifndef STATS_SERVICE_WRITE_EVENT_H
#define STATS_SERVICE_WRITE_EVENT_H

#include <string>

#include "battery_stats_service.h"
#include "hisysevent_operation.h"
#include "hisysevent_record.h"

namespace OHOS {
namespace PowerMgr {
template<typename... Types>
static void StatsWriteHiSysEvent(const sptr<BatteryStatsService>& service, const std::string &domain,
    const std::string &eventName, int type, Types... keyValues)
{
    std::string eventDetail = HiSysEventOperation::CombineHiSysEvent(domain, eventName, type, keyValues...);
    std::shared_ptr<HiviewDFX::HiSysEventRecord> sysEvent = std::make_shared<HiviewDFX::HiSysEventRecord>(eventDetail);
    if (service->listenerPtr_ != nullptr) {
        service->listenerPtr_->OnEvent(sysEvent);
    }
}
} // namespace PowerMgr
} // namespace OHOS
#endif // STATS_SERVICE_WRITE_EVENT_H