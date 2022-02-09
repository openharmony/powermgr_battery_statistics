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

#include <string_ex.h>

#include "stats_errors.h"
#include "stats_hilog_wrapper.h"
#include "battery_stats_client.h"

namespace OHOS {
namespace PowerMgr {
namespace {
static constexpr uint8_t ARG_SIZE_TWO = 2;
static constexpr uint8_t ARG_SIZE_THREE = 3;
static constexpr uint8_t ARG_INDEX_TWO = 2;
static const std::string HELP_COMMAND_MSG =
    "usage: statistics <command> [<options>]\n"
    "command list:\n"
    "  dump            :    Dump battery statistics info. \n"
    "  help            :    Show this help menu. \n"
    "\n"
    "options list:\n"
    "  -batterystats   :    Show the all information of battery stats.\n";
static const std::string HELP_DUMP_OPTION_MSG =
    "usage: statistics dump <options>\n"
    "options list:\n"
    "  -batterystats   :    Show the all information of battery stats.\n";
static const std::string HELP_ERROR_OPTION_NEED_MSG = "error: needs a option\n";
static const std::string HELP_ERROR_OPTION_WRONG_MSG = "error: wrong option\n";
} // namespace

StatisticsShellCommand::StatisticsShellCommand(int argc, char *argv[]) : ShellCommand(argc, argv, "Statistics")
{
    STATS_HILOGI(STATS_MODULE_COMMON, "Enter");
    for (int i = 0; i < argc_; i++) {
        STATS_HILOGI(STATS_MODULE_COMMON, "argv_[%{public}d]: %{public}s", i, argv_[i]);
    }
    STATS_HILOGI(STATS_MODULE_COMMON, "Exit");
}

ErrCode StatisticsShellCommand::CreateCommandMap()
{
    STATS_HILOGI(STATS_MODULE_COMMON, "Enter");
    commandMap_ = {
        {"help", std::bind(&StatisticsShellCommand::CommandHelp, this)},
        {"dump", std::bind(&StatisticsShellCommand::CommandDump, this)},
    };
    STATS_HILOGI(STATS_MODULE_COMMON, "Exit");
    return ERR_OK;
}

ErrCode StatisticsShellCommand::CreateMessageMap()
{
    STATS_HILOGI(STATS_MODULE_COMMON, "Enter");
    messageMap_ = {};
    STATS_HILOGI(STATS_MODULE_COMMON, "Exit");
    return ERR_OK;
}

ErrCode StatisticsShellCommand::init()
{
    STATS_HILOGI(STATS_MODULE_COMMON, "Enter");
    STATS_HILOGI(STATS_MODULE_COMMON, "Exit");
    return ERR_OK;
}

ErrCode StatisticsShellCommand::CheckParameter(void)
{
    STATS_HILOGI(STATS_MODULE_COMMON, "Enter");
    switch (argc_) {
        case ARG_SIZE_TWO:
            resultReceiver_.clear();
            resultReceiver_.append(HELP_ERROR_OPTION_NEED_MSG);
            OptionHelp();
            STATS_HILOGE(STATS_MODULE_COMMON, "No option parameter");
            return ERR_INVALID_VALUE;
        case ARG_SIZE_THREE: {
            if ((strcmp(argv_[ARG_INDEX_TWO], "-batterystats") == 0)) {
                break;
            } else {
                resultReceiver_.clear();
                resultReceiver_.append(HELP_ERROR_OPTION_WRONG_MSG);
                OptionHelp();
                STATS_HILOGE(STATS_MODULE_COMMON, "Wrong option parameter");
                return ERR_INVALID_VALUE;
            }
        }
        default:
            resultReceiver_.clear();
            resultReceiver_.append(HELP_ERROR_OPTION_WRONG_MSG);
            OptionHelp();
            STATS_HILOGE(STATS_MODULE_COMMON, "Wrong option parameter");
            return ERR_INVALID_VALUE;
    }
    STATS_HILOGI(STATS_MODULE_COMMON, "Exit");
    return ERR_OK;
}

ErrCode StatisticsShellCommand::CommandDump(void)
{
    STATS_HILOGI(STATS_MODULE_COMMON, "Enter");
    auto res = CheckParameter();
    if (res != ERR_OK) {
        return res;
    }
    resultReceiver_.clear();
    BatteryStatsClient &client = BatteryStatsClient::GetInstance();
    std::string ret = client.Dump(argList_);
    resultReceiver_.append("Battery Statistics Dump result: \n");
    resultReceiver_.append(ret);
    STATS_HILOGI(STATS_MODULE_COMMON, "Exit");
    return ERR_OK;
}

ErrCode StatisticsShellCommand::CommandHelp(void)
{
    STATS_HILOGI(STATS_MODULE_COMMON, "Enter");
    resultReceiver_.append(HELP_COMMAND_MSG);
    STATS_HILOGI(STATS_MODULE_COMMON, "Exit");
    return ERR_OK;
}

ErrCode StatisticsShellCommand::OptionHelp(void)
{
    STATS_HILOGI(STATS_MODULE_COMMON, "Enter");
    resultReceiver_.append(HELP_DUMP_OPTION_MSG);
    STATS_HILOGI(STATS_MODULE_COMMON, "Exit");
    return ERR_OK;
}
} // namespace PowerMgr
} // namespace OHOS