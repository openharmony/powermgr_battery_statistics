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

#include "stats_hisysevent.h"

namespace OHOS {
namespace PowerMgr {
namespace {
const uint32_t COMPARE_SUCCESS = 0;
}
bool StatsHiSysEvent::CheckHiSysEvent(const std::string& eventName)
{
    for (int32_t i = 0; i < HISYSEVENT_TYPE_END; i++) {
        if (eventName.compare(HISYSEVENT_LIST[i]) == COMPARE_SUCCESS) {
            return true;
        }
    }
    return false;
}
} // namespace PowerMgr
} // namespace OHOS