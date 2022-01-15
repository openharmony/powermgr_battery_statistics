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

#include "statistics_shell_command.h"

#include "string_ex.h"

#include "battery_stats_service.h"
#include "stats_hilog_wrapper.h"

namespace OHOS {
namespace PowerMgr {
StatisticsShellCommand::StatisticsShellCommand(int argc, char *argv[])
    : ShellCommand(argc, argv, "Statistics")
{
}

ErrCode StatisticsShellCommand::CreateCommandMap()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "StatisticsShellCommand...");
    auto bss = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
    std::vector<std::u16string> argsInStr;
    std::transform(argList_.begin(), argList_.end(), std::back_inserter(argsInStr),
        [](const std::string &arg) {
        std::u16string ret = Str8ToStr16(arg);
        STATS_HILOGI(STATS_MODULE_SERVICE, "StatisticsShellCommand %{public}s", arg.c_str());
        return ret;
    });
    int32_t fd = 1;
    commandMap_ = {
        {"dump", std::bind(&BatteryStatsService::Dump, bss, fd, argsInStr)}
    };
    return OHOS::ERR_OK;
}  

ErrCode StatisticsShellCommand::CreateMessageMap()
{
    messageMap_ = {};
    return OHOS::ERR_OK;
}

ErrCode StatisticsShellCommand::init()
{
    return OHOS::ERR_OK;
}
}
}