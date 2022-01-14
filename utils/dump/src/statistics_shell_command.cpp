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