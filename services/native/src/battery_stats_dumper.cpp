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

#include "battery_stats_dumper.h"

#include "battery_stats_service.h"
#include "stats_common.h"

namespace OHOS {
namespace PowerMgr {
namespace {
constexpr const char* ARGS_HELP = "-h";
constexpr const char* ARGS_STATS = "-batterystats";
}

bool BatteryStatsDumper::Dump(const std::vector<std::string>& args, std::string& result)
{
    result.clear();
    auto argc = args.size();
    if ((argc == 0) || (args[0] == ARGS_HELP)) {
        ShowUsage(result);
        return true;
    }
    auto bss = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
    if (bss == nullptr) {
        return true;
    }
    for (auto it = args.begin(); it != args.end(); it++) {
        if (*it == ARGS_STATS) {
            auto core = bss->GetBatteryStatsCore();
            if (core == nullptr) {
                continue;
            }
            core->DumpInfo(result);
        }
    }
    return true;
}

void BatteryStatsDumper::ShowUsage(std::string& result)
{
    std::string HELP_COMMAND_MSG =
        "usage: statistics <command> [<options>]\n"
        "command list:\n"
        "  -h              :    Show this help menu. \n"
        "  -batterystats   :    Show all the information of battery stats.\n";
    result.append(HELP_COMMAND_MSG);
}
} // namespace PowerMgr
} // namespace OHOS
