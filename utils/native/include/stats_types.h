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

#ifndef STATS_TYPES_H
#define STATS_TYPES_H

namespace OHOS {
namespace PowerMgr {
/**
 * Audio State
 */
enum class AudioState : int32_t {
    AUDIO_STATE_INVALID = -1,
    AUDIO_STATE_NEW,
    AUDIO_STATE_PREPARED,
    AUDIO_STATE_RUNNING,
    AUDIO_STATE_STOPPED,
    AUDIO_STATE_RELEASED,
    AUDIO_STATE_PAUSED
};
} // namespace PowerMgr
} // namespace OHOS

#endif // STATS_TYPES_H