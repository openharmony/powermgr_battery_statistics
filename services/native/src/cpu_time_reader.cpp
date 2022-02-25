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

#include "cpu_time_reader.h"

#include <fstream>
#include "string_ex.h"

#include "battery_stats_service.h"
#include "stats_helper.h"
#include "stats_hilog_wrapper.h"
#include "stats_utils.h"

namespace OHOS {
namespace PowerMgr {
namespace {
static const std::string UID_CPU_ACTIVE_TIME_FILE = "/proc/uid_concurrent_active_time";
static const std::string UID_CPU_CLUSTER_TIME_FILE = "/proc/uid_concurrent_policy_time";
static const std::string UID_CPU_FREQ_TIME_FILE = "/proc/uid_time_in_state";
static const std::string UID_CPU_TIME_FILE = "/proc/uid_cputime/show_uid_stat";
auto g_statsService = DelayedStatsSpSingleton<BatteryStatsService>::GetInstance();
} // namespace
bool CpuTimeReader::Init()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");

    if (!UpdateCpuTime()) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Update cpu time failed");
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

int64_t CpuTimeReader::GetUidCpuActiveTimeMs(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    int64_t cpuActiveTime = 0;
    auto iter = activeTimeMap_.find(uid);
    if (iter != activeTimeMap_.end()) {
        cpuActiveTime = iter->second;
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got cpu active time: %{public}s for uid: %{public}d",
            std::to_string(cpuActiveTime).c_str(), uid);
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "No cpu active time found for uid: %{public}d, return 0", uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return cpuActiveTime;
}

void CpuTimeReader::DumpInfo(std::string& result, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto uidIter = lastUidTimeMap_.find(uid);
    if (uidIter == lastUidTimeMap_.end()) {
        STATS_HILOGI(STATS_MODULE_SERVICE, "No related CPU info for uid: %{public}d", uid);
        return;
    }
    std::string freqTime = "";
    auto freqIter = lastFreqTimeMap_.find(uid);
    if (freqIter != lastFreqTimeMap_.end()) {
        for (auto timeIter = freqIter->second.begin(); timeIter != freqIter->second.end(); timeIter++) {
            for (uint32_t i = 0; i < timeIter->second.size(); i++) {
                freqTime.append(ToString(timeIter->second[i]))
                    .append(" ");
            }
        }
    }
    result.append("Total cpu time: userSpaceTime=")
        .append(ToString(uidIter->second[0]))
        .append("ms, systemSpaceTime=")
        .append(ToString(uidIter->second[1]))
        .append("ms\n")
        .append("Total cpu time per freq: ")
        .append(freqTime)
        .append("\n");
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

int64_t CpuTimeReader::GetUidCpuClusterTimeMs(int32_t uid, uint32_t cluster)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    int64_t cpuClusterTime = 0;
    auto iter = clusterTimeMap_.find(uid);
    if (iter != clusterTimeMap_.end()) {
        auto cpuClusterTimeVector = iter->second;
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got cpu cluster time vector for uid: %{public}d, size: %{public}d", uid,
            (int)cpuClusterTimeVector.size());
        if (cluster < cpuClusterTimeVector.size()) {
            cpuClusterTime = cpuClusterTimeVector[cluster];
            STATS_HILOGD(STATS_MODULE_SERVICE, "Got cpu cluster time: %{public}s of cluster: %{public}d",
                std::to_string(cpuClusterTime).c_str(), cluster);
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No cpu cluster time of cluster: %{public}d found, return 0", cluster);
        }
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "No cpu cluster time vector found for uid: %{public}d, return 0", uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return cpuClusterTime;
}

int64_t CpuTimeReader::GetUidCpuFreqTimeMs(int32_t uid, uint32_t cluster, uint32_t speed)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    int64_t cpuFreqTime = 0;
    auto uidIter = freqTimeMap_.find(uid);
    if (uidIter != freqTimeMap_.end()) {
        auto cpuFreqTimeMap = uidIter->second;
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got uid cpu freq time map for uid: %{public}d, size: %{public}d", uid,
            (int)cpuFreqTimeMap.size());
        auto clusterIter = cpuFreqTimeMap.find(cluster);
        if (clusterIter != cpuFreqTimeMap.end()) {
            auto cpuFreqTimeVector = clusterIter->second;
            STATS_HILOGD(STATS_MODULE_SERVICE, "Got cpu freq time vector of cluster: %{public}d, size: %{public}d",
                cluster, (int)cpuFreqTimeVector.size());
            if (speed < cpuFreqTimeVector.size()) {
                cpuFreqTime = cpuFreqTimeVector[speed];
                STATS_HILOGD(STATS_MODULE_SERVICE, "Got cpu freq time: %{public}s of speed: %{public}d",
                    std::to_string(cpuFreqTime).c_str(), speed);
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE, "No cpu freq time of speed: %{public}d found, return 0", speed);
            }
        } else {
            STATS_HILOGE(STATS_MODULE_SERVICE, "No cluster cpu freq time vector of cluster: %{public}d found, return 0",
                cluster);
        }
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "No uid cpu freq time map found for uid: %{public}d, return 0", uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return cpuFreqTime;
}

std::vector<int64_t> CpuTimeReader::GetUidCpuTimeMs(int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::vector<int64_t> cpuTimeVec;
    auto iter = uidTimeMap_.find(uid);
    if (iter != uidTimeMap_.end()) {
        cpuTimeVec = iter->second;
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got uid cpu time vector for uid: %{public}d, size: %{public}d", uid,
            (int)cpuTimeVec.size());
    } else {
        STATS_HILOGE(STATS_MODULE_SERVICE, "No uid cpu time vector found for uid: %{public}d, return null", uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return cpuTimeVec;
}

bool CpuTimeReader::UpdateCpuTime()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    bool result = true;
    if (ReadUidCpuClusterTime()) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Read uid cpu cluster time successfully");
    } else {
        result = false;
        STATS_HILOGE(STATS_MODULE_SERVICE, "Read uid cpu cluster time failed");
    }

    if (ReadUidCpuTime()) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Read uid cpu time successfully");
    } else {
        result = false;
        STATS_HILOGE(STATS_MODULE_SERVICE, "Read uid cpu time failed");
    }

    if (ReadUidCpuActiveTime()) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Read uid cpu active time successfully");
    } else {
        result = false;
        STATS_HILOGE(STATS_MODULE_SERVICE, "Read uid cpu active time failed");
    }

    if (ReadUidCpuFreqTime()) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Read uid cpu freq time successfully");
    } else {
        result = false;
        STATS_HILOGE(STATS_MODULE_SERVICE, "Read uid cpu freq time failed");
    }

    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return result;
}

bool CpuTimeReader::ReadUidCpuActiveTimeImpl(std::string& line, int32_t uid)
{
    int64_t timeMs = 0;
    std::vector<std::string> splitedTime;
    Split(line, ' ', splitedTime);
    for (uint16_t i = 0; i < splitedTime.size(); i++) {
        timeMs += stoll(splitedTime[i]) * 10; // Unit is 10ms
    }

    int64_t increment = 0;
    if (timeMs > 0) {
        auto iterLast = lastActiveTimeMap_.find(uid);
        if (iterLast != lastActiveTimeMap_.end()) {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Cpu last active time: %{public}s ms",
                std::to_string(iterLast->second).c_str());
            increment = timeMs - iterLast->second;
            if (increment >= 0) {
                iterLast->second = timeMs;
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE, "Negative cpu active time increment got");
                return false;
            }
        } else {
            lastActiveTimeMap_.insert(std::pair<int32_t, int64_t>(uid, timeMs));
            increment = timeMs;
        }
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got cpu active time increment: %{public}s ms",
            std::to_string(increment).c_str());
        STATS_HILOGD(STATS_MODULE_SERVICE, "Update last cpu active time: %{public}s ms, uid: %{public}d",
            std::to_string(timeMs).c_str(), uid);
    }

    if (StatsHelper::IsOnBattery()) {
        auto iter = activeTimeMap_.find(uid);
        if (iter != activeTimeMap_.end()) {
            iter->second += increment;
            STATS_HILOGD(STATS_MODULE_SERVICE, "Update active time: %{public}sms, uid: %{public}d",
                std::to_string(iter->second).c_str(), uid);
        } else {
            activeTimeMap_.insert(std::pair<int32_t, int64_t>(uid, increment));
            STATS_HILOGD(STATS_MODULE_SERVICE, "Add active time: %{public}sms, uid: %{public}d",
                std::to_string(increment).c_str(), uid);
        }
    } else {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Power supply is connected, don't add the increment");
    }
    return true;
}

bool CpuTimeReader::ReadUidCpuActiveTime()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::ifstream input(UID_CPU_ACTIVE_TIME_FILE);
    if (!input) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Opening file: %{public}s failed", UID_CPU_ACTIVE_TIME_FILE.c_str());
        return false;
    }

    std::string line;
    while (getline(input, line)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Got line: %{public}s", line.c_str());
        int32_t uid = StatsUtils::INVALID_VALUE;
        std::vector<std::string> splitedLine;
        Split(line, ':', splitedLine);
        if (splitedLine[0] == "cpus") {
            continue;
        } else {
            uid = stoi(splitedLine[0]);
        }

        if (uid > StatsUtils::INVALID_VALUE) {
            auto uidEntity =
                g_statsService->GetBatteryStatsCore()->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
            if (uidEntity) {
                uidEntity->UpdateUidMap(uid);
            }
        }

        if (ReadUidCpuActiveTimeImpl(splitedLine[1], uid)) {
            continue;
        } else {
            return false;
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

void CpuTimeReader::ReadPolicy(std::vector<uint16_t>& clusters, std::string& line)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::vector<std::string> splitedPolicy;
    Split(line, ' ', splitedPolicy);
    uint32_t step = 2;
    for (uint32_t i = 0; i < splitedPolicy.size(); i += step) {
        uint16_t coreNum = static_cast<uint16_t>(stoi(splitedPolicy[i + 1]));
        clusters.push_back(coreNum);
        clustersMap_.insert(std::pair<uint16_t, uint16_t>(i, coreNum));
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got cpu core num: %{public}d", coreNum);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

bool CpuTimeReader::ReadClusterTimeIncrement(std::vector<int64_t>& clusterTime, std::vector<int64_t>& increments,
    int32_t uid, std::vector<uint16_t>& clusters, std::string& timeLine)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::vector<std::string> splitedTime;
    Split(timeLine, ' ', splitedTime);
    uint16_t count = 0;
    for (uint16_t i = 0; i < clusters.size(); i++) {
        int64_t tempTimeMs = 0;
        for (int j = 0; j < clusters[i]; j++) {
            tempTimeMs += stoll(splitedTime[count++]) * 10; // Unit is 10ms
        }
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got cpu cluster time: %{public}s", std::to_string(tempTimeMs).c_str());
        clusterTime.push_back(tempTimeMs);
    }

    auto iterLast = lastClusterTimeMap_.find(uid);
    if (iterLast != lastClusterTimeMap_.end()) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Cpu last cluster time vec size: %{public}zu", iterLast->second.size());
        for (uint16_t i = 0; i < clusters.size(); i++) {
            int64_t increment = clusterTime[i] - iterLast->second[i];
            STATS_HILOGD(STATS_MODULE_SERVICE, "Cpu cluster time increment: %{public}s ms",
                std::to_string(increment).c_str());
            STATS_HILOGD(STATS_MODULE_SERVICE, "Cpu last cluster time increment: %{public}s ms",
                std::to_string(iterLast->second[i]).c_str());
            if (increment >= 0) {
                iterLast->second[i] = clusterTime[i];
                increments.push_back(increment);
                STATS_HILOGD(STATS_MODULE_SERVICE, "Update last cpu cluster time to: %{public}s ms, uid: %{public}d",
                    std::to_string(clusterTime[i]).c_str(), uid);
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE, "Negative cpu cluster time increment got");
                return false;
            }
        }
    } else {
        lastClusterTimeMap_.insert(std::pair<int32_t, std::vector<int64_t>>(uid, clusterTime));
        increments = clusterTime;
        STATS_HILOGD(STATS_MODULE_SERVICE, "Add last cpu cluster time for uid: %{public}d", uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

bool CpuTimeReader::ReadUidCpuClusterTime()
{
    std::ifstream input(UID_CPU_CLUSTER_TIME_FILE);
    if (!input) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Opening file: %{public}s failed", UID_CPU_CLUSTER_TIME_FILE.c_str());
        return false;
    }
    std::string line;
    int32_t uid = -1;
    std::vector<uint16_t> clusters;
    std::vector<int64_t> clusterTime;
    while (getline(input, line)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Got line: %{public}s", line.c_str());
        clusterTime.clear();
        if (line.find("policy") != line.npos) {
            ReadPolicy(clusters, line);
            continue;
        }

        std::vector<std::string> splitedLine;
        Split(line, ':', splitedLine);
        uid = stoi(splitedLine[0]);
        if (uid > StatsUtils::INVALID_VALUE) {
            auto uidEntity = g_statsService->GetBatteryStatsCore()->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
            if (uidEntity) {
                uidEntity->UpdateUidMap(uid);
            }
        }

        std::vector<int64_t> increments;
        if (!ReadClusterTimeIncrement(clusterTime, increments, uid, clusters, splitedLine[1])) {
            return false;
        }

        if (StatsHelper::IsOnBattery()) {
            auto iter = clusterTimeMap_.find(uid);
            if (iter != clusterTimeMap_.end()) {
                for (uint16_t i = 0; i < clusters.size(); i++) {
                    iter->second[i] += increments[i];
                    STATS_HILOGD(STATS_MODULE_SERVICE, "Update cpu cluster time to: %{public}s ms for uid: %{public}d",
                        std::to_string(iter->second[i]).c_str(), uid);
                }
            } else {
                clusterTimeMap_.insert(std::pair<int32_t, std::vector<int64_t>>(uid, increments));
                STATS_HILOGD(STATS_MODULE_SERVICE, "Add cpu cluster time for uid: %{public}d", uid);
            }
        } else {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Power supply is connected, don't add the increment");
        }
    }
    return true;
}

bool CpuTimeReader::ProcessFreqTime(std::map<uint32_t, std::vector<int64_t>>& map, std::map<uint32_t,
    std::vector<int64_t>>& increments, std::map<uint32_t, std::vector<int64_t>>& speedTime, int32_t index, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto iterLastTemp = map.find(index);
    if (iterLastTemp != map.end()) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Cpu last freq time vec size: %{public}zu", iterLastTemp->second.size());
        std::vector<int64_t> lastSpeedTimes = iterLastTemp->second;
        std::vector<int64_t> newIncrementTimes;
        newIncrementTimes.clear();
        for (uint16_t j = 0; j < lastSpeedTimes.size(); j++) {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Cpu last freq time: %{public}s ms",
                std::to_string(lastSpeedTimes[j]).c_str());
            int64_t increment = speedTime.at(index)[j] - lastSpeedTimes[j];
            if (increment >= 0) {
                newIncrementTimes.push_back(increment);
                increments.insert(std::pair<uint32_t, std::vector<int64_t>>(index, newIncrementTimes));
                STATS_HILOGD(STATS_MODULE_SERVICE, "Got cpu freq time increment: %{public}s ms, uid: %{public}d",
                    std::to_string(increment).c_str(), uid);
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE, "Negative cpu freq time increment got");
                return false;
            }
        }
        iterLastTemp->second = speedTime.at(index);
        STATS_HILOGD(STATS_MODULE_SERVICE, "Update last cpu freq time for uid: %{public}d", uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

bool CpuTimeReader::ReadFreqTimeIncrement(std::map<uint32_t, std::vector<int64_t>>& speedTime,
    std::map<uint32_t, std::vector<int64_t>>& increments, int32_t uid, std::vector<std::string>& splitedTime)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto parser = g_statsService->GetBatteryStatsParser();
    uint16_t clusterNum = parser->GetClusterNum();
    uint16_t count = 0;
    for (uint16_t i = 0; i < clusterNum; i++) {
        std::vector<int64_t> tempSpeedTimes;
        tempSpeedTimes.clear();
        for (int j = 0; j < parser->GetSpeedNum(i); j++) {
            int64_t tempTimeMs = stoll(splitedTime[count++]) * 10; // Unit is 10ms
            tempSpeedTimes.push_back(tempTimeMs);
            STATS_HILOGD(STATS_MODULE_SERVICE, "Got cpu freq time: %{public}s", std::to_string(tempTimeMs).c_str());
        }
        speedTime.insert(std::pair<uint32_t, std::vector<int64_t>>(i, tempSpeedTimes));
    }

    auto iterLast = lastFreqTimeMap_.find(uid);
    if (iterLast == lastFreqTimeMap_.end()) {
        lastFreqTimeMap_.insert(std::pair<int32_t, std::map<uint32_t, std::vector<int64_t>>>(uid, speedTime));
        increments = speedTime;
        STATS_HILOGD(STATS_MODULE_SERVICE, "Add last cpu freq time for uid: %{public}d", uid);
        return true;
    }
    for (uint16_t i = 0; i < lastFreqTimeMap_.size(); i++) {
        if (!ProcessFreqTime(iterLast->second, increments, speedTime, i, uid)) {
            return false;
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

void CpuTimeReader::DistributeFreqTime(std::map<uint32_t, std::vector<int64_t>>& uidIncrements,
    std::map<uint32_t, std::vector<int64_t>>& increments)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto parser = g_statsService->GetBatteryStatsParser();
    uint16_t clusterNum = parser->GetClusterNum();
    if (wakelockCounts_ > 0) {
        for (uint16_t i = 0; i < clusterNum; i++) {
            uint16_t speedNum = parser->GetSpeedNum(i);
            for (int j = 0; j < speedNum; j++) {
                int32_t step = 2;
                uidIncrements.at(i)[j] = increments.at(i)[j] / step;
            }
        }
        // TO-DO, distribute half of cpu freq time to wakelock holders
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

void CpuTimeReader::AddFreqTimeToUid(std::map<uint32_t, std::vector<int64_t>>& uidIncrements, int32_t uid)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    auto parser = g_statsService->GetBatteryStatsParser();
    uint16_t clusterNum = parser->GetClusterNum();
    auto iter = freqTimeMap_.find(uid);
    if (iter != freqTimeMap_.end()) {
        for (uint16_t i = 0; i < clusterNum; i++) {
            uint16_t speedNum = parser->GetSpeedNum(i);
            for (int j = 0; j < speedNum; j++) {
                iter->second.at(i)[j] += uidIncrements.at(i)[j];
                STATS_HILOGD(STATS_MODULE_SERVICE, "Update cpu freq time to: %{public}s ms for uid: %{public}d",
                    std::to_string(iter->second.at(i)[j]).c_str(), uid);
            }
        }
    } else {
        freqTimeMap_.insert(std::pair<int32_t, std::map<uint32_t, std::vector<int64_t>>>(uid, uidIncrements));
        STATS_HILOGD(STATS_MODULE_SERVICE, "Add cpu freq time for uid: %{public}d", uid);
        for (auto iter : uidIncrements) {
            STATS_HILOGD(STATS_MODULE_SERVICE, "uidIncrements cluster = %{public}u", iter.first);
            for (auto time : iter.second) {
                STATS_HILOGD(STATS_MODULE_SERVICE, "uidIncrements time = %{public}s", std::to_string(time).c_str());
            }
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}

bool CpuTimeReader::ReadUidCpuFreqTime()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::ifstream input(UID_CPU_FREQ_TIME_FILE);
    if (!input) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Opening file: %{public}s failed", UID_CPU_CLUSTER_TIME_FILE.c_str());
        return false;
    }
    std::string line;
    int32_t uid = -1;
    std::map<uint32_t, std::vector<int64_t>> speedTime;
    while (getline(input, line)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Got line: %{public}s", line.c_str());
        speedTime.clear();
        std::vector<std::string> splitedLine;
        Split(line, ':', splitedLine);
        if (splitedLine[0] == "uid") {
            continue;
        } else {
            uid = stoi(splitedLine[0]);
        }

        if (uid > StatsUtils::INVALID_VALUE) {
            auto uidEntity =
                g_statsService->GetBatteryStatsCore()->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
            if (uidEntity) {
                uidEntity->UpdateUidMap(uid);
            }
        }

        std::vector<std::string> splitedTime;
        Split(splitedLine[1], ' ', splitedTime);

        std::map<uint32_t, std::vector<int64_t>> increments;
        if (!ReadFreqTimeIncrement(speedTime, increments, uid, splitedTime)) {
            return false;
        }

        std::map<uint32_t, std::vector<int64_t>> uidIncrements = increments;
        DistributeFreqTime(uidIncrements, increments);

        if (!StatsHelper::IsOnBattery()) {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Power supply is connected, don't add the increment");
            continue;
        }
        AddFreqTimeToUid(uidIncrements, uid);
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

bool CpuTimeReader::ReadUidTimeIncrement(std::vector<int64_t>& cpuTime, std::vector<int64_t>& uidIncrements,
    int32_t uid, std::string& timeLine)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::vector<std::string> splitedTime;
    Split(timeLine, ' ', splitedTime);
    for (uint16_t i = 0; i < splitedTime.size(); i++) {
        int64_t tempTime = 0;
        tempTime = stoll(splitedTime[i]);
        cpuTime.push_back(tempTime);
        STATS_HILOGD(STATS_MODULE_SERVICE, "Got cpu time: %{public}s", std::to_string(tempTime).c_str());
    }

    std::vector<int64_t> increments;
    auto iterLast = lastUidTimeMap_.find(uid);
    if (iterLast != lastUidTimeMap_.end()) {
        STATS_HILOGD(STATS_MODULE_SERVICE, "Cpu last time vec size: %{public}zu", iterLast->second.size());
        for (uint16_t i = 0; i < splitedTime.size(); i++) {
            int64_t increment = 0;
            increment = cpuTime[i] - iterLast->second[i];
            STATS_HILOGD(STATS_MODULE_SERVICE, "Cpu last time: %{public}s ms",
                std::to_string(iterLast->second[i]).c_str());
            STATS_HILOGD(STATS_MODULE_SERVICE, "Cpu time increment: %{public}s ms, uid: %{public}d",
                std::to_string(increment).c_str(), uid);
            if (increment >= 0) {
                iterLast->second[i] = cpuTime[i];
                increments.push_back(increment);
                STATS_HILOGD(STATS_MODULE_SERVICE, "Update last cpu time to: %{public}s ms, uid: %{public}d",
                    std::to_string(cpuTime[i]).c_str(), uid);
            } else {
                STATS_HILOGE(STATS_MODULE_SERVICE, "Negative cpu time increment got");
                return false;
            }
        }
    } else {
        lastUidTimeMap_.insert(std::pair<int32_t, std::vector<int64_t>>(uid, cpuTime));
        increments = cpuTime;
        STATS_HILOGD(STATS_MODULE_SERVICE, "Add last cpu time for uid: %{public}d", uid);
    }

    uidIncrements = increments;

    if (wakelockCounts_ > 0) {
        double weight = 0.5;
        uidIncrements[0] = increments[0] / (StatsUtils::US_IN_MS * 1.0) * weight;
        uidIncrements[1] = increments[1] / (StatsUtils::US_IN_MS * 1.0) * weight;
        // TO-DO, distribute half of cpu time to wakelock holders
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

bool CpuTimeReader::ReadUidCpuTime()
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    std::ifstream input(UID_CPU_TIME_FILE);
    if (!input) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Opening file: %{public}s failed", UID_CPU_CLUSTER_TIME_FILE.c_str());
        return false;
    }
    std::string line;
    int32_t uid = -1;
    std::vector<int64_t> cpuTime;
    while (getline(input, line)) {
        STATS_HILOGE(STATS_MODULE_SERVICE, "Got line: %{public}s", line.c_str());
        cpuTime.clear();
        std::vector<std::string> splitedLine;
        Split(line, ':', splitedLine);
        uid = stoi(splitedLine[0]);
        if (uid > StatsUtils::INVALID_VALUE) {
            auto uidEntity =
                g_statsService->GetBatteryStatsCore()->GetEntity(BatteryStatsInfo::CONSUMPTION_TYPE_APP);
            if (uidEntity) {
                uidEntity->UpdateUidMap(uid);
            }
        }

        std::vector<int64_t> uidIncrements;
        if (!ReadUidTimeIncrement(cpuTime, uidIncrements, uid, splitedLine[1])) {
            return false;
        }

        if (StatsHelper::IsOnBattery()) {
            auto iter = uidTimeMap_.find(uid);
            if (iter != uidTimeMap_.end()) {
                for (uint16_t i = 0; i < uidIncrements.size(); i++) {
                    iter->second[i] = uidIncrements[i];
                    STATS_HILOGD(STATS_MODULE_SERVICE, "Update cpu time to: %{public}s ms for uid: %{public}d",
                        std::to_string(iter->second[i]).c_str(), uid);
                }
            } else {
                uidTimeMap_.insert(std::pair<int32_t, std::vector<int64_t>>(uid, uidIncrements));
                STATS_HILOGD(STATS_MODULE_SERVICE, "Add cpu time for uid: %{public}d", uid);
            }
        } else {
            STATS_HILOGD(STATS_MODULE_SERVICE, "Power supply is connected, don't add the increment");
        }
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
    return true;
}

void CpuTimeReader::Split(std::string &origin, char delimiter, std::vector<std::string> &splited)
{
    STATS_HILOGI(STATS_MODULE_SERVICE, "Enter");
    size_t start;
    size_t end = 0;

    while ((start = origin.find_first_not_of(delimiter, end)) != std::string::npos) {
        end = origin.find(delimiter, start);
        splited.push_back(origin.substr(start, end - start));
    }
    STATS_HILOGI(STATS_MODULE_SERVICE, "Exit");
}
} // namespace PowerMgr
} // namespace OHOS