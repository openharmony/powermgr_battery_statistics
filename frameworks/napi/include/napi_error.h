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

#ifndef BATTERY_STATS_ERROR_H
#define BATTERY_STATS_ERROR_H

#include <map>
#include <string>

#include "napi/native_node_api.h"
#include "battery_stats_errors.h"

namespace OHOS {
namespace PowerMgr {
class NapiError {
public:
    NapiError() {}
    explicit NapiError(StatsError code) : code_(code) {}
    napi_value GetNapiError(napi_env& env) const;
    napi_value ThrowError(napi_env& env, StatsError code = StatsError::ERR_OK);
    inline void Error(StatsError code)
    {
        code_ = (code != StatsError::ERR_OK) ? code : code_;
    }
    inline bool IsError() const
    {
        return code_ != StatsError::ERR_OK;
    }

private:
    StatsError code_ {StatsError::ERR_OK};
    static std::map<StatsError, std::string> errorTable_;
};
} // namespace PowerMgr
} // namespace OHOS

#endif // BATTERY_STATS_ERROR_H
