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

#ifndef STATS_COMMON_H
#define STATS_COMMON_H

#include <cstdint>
#include <type_traits>

#include "stats_errors.h"
#include "stats_log.h"

namespace OHOS {
namespace PowerMgr {
#define STATS_RETURN_IF_WITH_RET(cond, retval) if (cond) { return (retval); }
#define STATS_RETURN_IF(cond) if (cond) { return; }
#define STATS_RETURN_IF_WITH_LOG(loglabel, cond, loginfo)                                   \
    do {                                                                                    \
        if (cond) {                                                                         \
            STATS_HILOGE(loglabel, ""#loginfo"");                                           \
            return;                                                                         \
        }                                                                                   \
    } while (0)                                                                             \

#define STATS_READ_PARCEL_NO_RET(loglabel, parcel, type, out)                               \
    do {                                                                                    \
        if (!(parcel).Read##type(out)) {                                                    \
            STATS_HILOGE(loglabel, "Read "#out" failed");                                   \
            return;                                                                         \
        }                                                                                   \
    } while (0)                                                                             \

#define STATS_WRITE_PARCEL_NO_RET(loglabel, parcel, type, data)                             \
    do {                                                                                    \
        if (!(parcel).Write##type(data)) {                                                  \
            STATS_HILOGE(loglabel, "Write "#data" failed");                                 \
            return;                                                                         \
        }                                                                                   \
    } while (0)                                                                             \

#define STATS_READ_PARCEL_WITH_RET(loglabel, parcel, type, out, retval)                     \
    do {                                                                                    \
        if (!(parcel).Read##type(out)) {                                                    \
            STATS_HILOGE(loglabel, "Read "#out" failed");                                   \
            return (retval);                                                                \
        }                                                                                   \
    } while (0)                                                                             \

#define STATS_WRITE_PARCEL_WITH_RET(loglabel, parcel, type, data, retval)                   \
    do {                                                                                    \
        if (!(parcel).Write##type(data)) {                                                  \
            STATS_HILOGE(loglabel, "Write "#data" failed");                                 \
            return (retval);                                                                \
        }                                                                                   \
    } while (0)

template<typename E>
constexpr auto StatsToUnderlying(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}
} // namespace PowerMgr
} // namespace OHOS

#endif // STATS_COMMON_H
