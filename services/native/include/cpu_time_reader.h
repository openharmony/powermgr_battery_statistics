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

#ifndef CPU_TIME_READER
#define CPU_TIME_READER

#include <map>
#include <vector>

namespace OHOS {
namespace PowerMgr {
class CpuTimeReader {
public:
    CpuTimeReader() = default;
    ~CpuTimeReader() = default;
    bool Init();
    long GetUidCpuActiveTimeMs(int32_t uid);
    long GetUidCpuClusterTimeMs(int32_t uid, uint32_t cluster);
    long GetUidCpuFreqTimeMs(int32_t uid, uint32_t cluster, uint32_t speed);
    bool UpdateCpuTime();
    std::vector<long> GetUidCpuTimeMs(int32_t uid);
    void DumpInfo(std::string& result, int32_t uid);

private:
    uint32_t wakelockCounts_ = 0;
    std::map<int32_t, long> activeTimeMap_;
    std::map<int32_t, std::vector<long>> clusterTimeMap_;
    std::map<int32_t, std::map<uint32_t, std::vector<long>>> freqTimeMap_;
    std::map<int32_t, std::vector<long>> uidTimeMap_;
    std::map<int32_t, long> lastActiveTimeMap_;
    std::map<int32_t, std::vector<long>> lastClusterTimeMap_;
    std::map<int32_t, std::map<uint32_t, std::vector<long>>> lastFreqTimeMap_;
    std::map<int32_t, std::vector<long>> lastUidTimeMap_;
    std::map<uint16_t, uint16_t> clustersMap_;
    bool ReadUidCpuActiveTime();
    bool ReadUidCpuActiveTimeImpl(std::string& line, int32_t uid);
    bool ReadUidCpuClusterTime();
    void ReadPolicy(std::vector<uint16_t>& clusters, std::string& line);
    bool ReadClusterTimeIncrement(std::vector<long>& clusterTime, std::vector<long>& increments, int32_t uid,
        std::vector<uint16_t>& clusters, std::string& timeLine);
    bool ReadUidCpuFreqTime();
    bool ReadFreqTimeIncrement(std::map<uint32_t, std::vector<long>>& speedTime,
        std::map<uint32_t, std::vector<long>>& increments, int32_t uid, std::vector<std::string>& splitedTime);
    bool ProcessFreqTime(std::map<uint32_t, std::vector<long>>& map, std::map<uint32_t, std::vector<long>>& increments,
        std::map<uint32_t, std::vector<long>>& speedTime, int32_t index, int32_t uid);
    void DistributeFreqTime(std::map<uint32_t, std::vector<long>>& uidIncrements,
        std::map<uint32_t, std::vector<long>>& increments);
    void AddFreqTimeToUid(std::map<uint32_t, std::vector<long>>& uidIncrements, int32_t uid);
    bool ReadUidCpuTime();
    bool ReadUidTimeIncrement(std::vector<long>& clusterTime, std::vector<long>& uidIncrements, int32_t uid,
        std::string& timeLine);
    void Split(std::string &origin, char delimiter, std::vector<std::string> &splited);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // CPU_TIME_READER