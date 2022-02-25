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
    int64_t GetUidCpuActiveTimeMs(int32_t uid);
    int64_t GetUidCpuClusterTimeMs(int32_t uid, uint32_t cluster);
    int64_t GetUidCpuFreqTimeMs(int32_t uid, uint32_t cluster, uint32_t speed);
    bool UpdateCpuTime();
    std::vector<int64_t> GetUidCpuTimeMs(int32_t uid);
    void DumpInfo(std::string& result, int32_t uid);

private:
    uint32_t wakelockCounts_ = 0;
    std::map<int32_t, int64_t> activeTimeMap_;
    std::map<int32_t, std::vector<int64_t>> clusterTimeMap_;
    std::map<int32_t, std::map<uint32_t, std::vector<int64_t>>> freqTimeMap_;
    std::map<int32_t, std::vector<int64_t>> uidTimeMap_;
    std::map<int32_t, int64_t> lastActiveTimeMap_;
    std::map<int32_t, std::vector<int64_t>> lastClusterTimeMap_;
    std::map<int32_t, std::map<uint32_t, std::vector<int64_t>>> lastFreqTimeMap_;
    std::map<int32_t, std::vector<int64_t>> lastUidTimeMap_;
    std::map<uint16_t, uint16_t> clustersMap_;
    bool ReadUidCpuActiveTime();
    bool ReadUidCpuActiveTimeImpl(std::string& line, int32_t uid);
    bool ReadUidCpuClusterTime();
    void ReadPolicy(std::vector<uint16_t>& clusters, std::string& line);
    bool ReadClusterTimeIncrement(std::vector<int64_t>& clusterTime, std::vector<int64_t>& increments, int32_t uid,
        std::vector<uint16_t>& clusters, std::string& timeLine);
    bool ReadUidCpuFreqTime();
    bool ReadFreqTimeIncrement(std::map<uint32_t, std::vector<int64_t>>& speedTime,
        std::map<uint32_t, std::vector<int64_t>>& increments, int32_t uid, std::vector<std::string>& splitedTime);
    bool ProcessFreqTime(std::map<uint32_t, std::vector<int64_t>>& map, std::map<uint32_t,
        std::vector<int64_t>>& increments, std::map<uint32_t, std::vector<int64_t>>& speedTime, int32_t index,
        int32_t uid);
    void DistributeFreqTime(std::map<uint32_t, std::vector<int64_t>>& uidIncrements,
        std::map<uint32_t, std::vector<int64_t>>& increments);
    void AddFreqTimeToUid(std::map<uint32_t, std::vector<int64_t>>& uidIncrements, int32_t uid);
    bool ReadUidCpuTime();
    bool ReadUidTimeIncrement(std::vector<int64_t>& clusterTime, std::vector<int64_t>& uidIncrements, int32_t uid,
        std::string& timeLine);
    void Split(std::string &origin, char delimiter, std::vector<std::string> &splited);
};
} // namespace PowerMgr
} // namespace OHOS
#endif // CPU_TIME_READER