/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import batteryStats from "@ohos.batteryStatistics"
import {describe, it, expect} from 'deccjsunit/index'

var ConsumptionType = {
    CONSUMPTION_TYPE_INVALID : -17,
    CONSUMPTION_TYPE_APP : -16,
    CONSUMPTION_TYPE_BLUETOOTH : -15,
    CONSUMPTION_TYPE_IDLE : -14,
    CONSUMPTION_TYPE_PHONE : -13,
    CONSUMPTION_TYPE_RADIO : -12,
    CONSUMPTION_TYPE_SCREEN : -11,
    CONSUMPTION_TYPE_USER : -10,
    CONSUMPTION_TYPE_WIFI : -9,
}

describe('appInfoTest', function () {
    console.log("*************batteryStats API Performance Test Begin*************");
    const MAXNUM = 1000;
    const MAXNUM_PROMISE = 80;
    const TIMEOUT = 70000;
    const MS_TO_US = 1000;

    /**
     * @tc.number batteryStats_performance_001
     * @tc.name batteryStats_performance_001
     * @tc.desc batteryStats getAppPowerValue Interface
     */
    it('batteryStats_performance_001', 0, function () {
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            batteryStats.getAppPowerValue(111);
        }
        let waitTime = new Date().getTime() - startTime;
        let avgTime = waitTime * MS_TO_US / MAXNUM; //us
        console.info(`batteryStats_performance_001: getAppPowerValue Wait Time : ${waitTime}`);
        expect(avgTime < TIMEOUT).assertTrue();
    })

    /**
     * @tc.number batteryStats_performance_002
     * @tc.name batteryStats_performance_002
     * @tc.desc batteryStats getAppPowerPercent Interface
     */
     it('batteryStats_performance_002', 0, function () {
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            batteryStats.getAppPowerPercent(111);
        }
        let waitTime = new Date().getTime() - startTime;
        let avgTime = waitTime * MS_TO_US / MAXNUM; //us
        console.info(`batteryStats_performance_002: getAppPowerPercent Wait Time : ${waitTime}`);
        expect(avgTime < TIMEOUT).assertTrue();
    })

    /**
     * @tc.number batteryStats_performance_003
     * @tc.name batteryStats_performance_003
     * @tc.desc batteryStats getHardwareUnitPowerValue Interface
     */
     it('batteryStats_performance_003', 0, function () {
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            batteryStats.getHardwareUnitPowerValue(ConsumptionType.CONSUMPTION_TYPE_IDLE);
        }
        let waitTime = new Date().getTime() - startTime;
        let avgTime = waitTime * MS_TO_US / MAXNUM; //us
        console.info(`batteryStats_performance_003: getHardwareUnitPowerValue Wait Time : ${waitTime}`);
        expect(avgTime < TIMEOUT).assertTrue();
    })

    /**
     * @tc.number batteryStats_performance_004
     * @tc.name batteryStats_performance_004
     * @tc.desc batteryStats getHardwareUnitPowerPercent Interface
     */
     it('batteryStats_performance_004', 0, function () {
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM; i++) {
            batteryStats.getHardwareUnitPowerPercent(ConsumptionType.CONSUMPTION_TYPE_IDLE);
        }
        let waitTime = new Date().getTime() - startTime;
        let avgTime = waitTime * MS_TO_US / MAXNUM; //us
        console.info(`batteryStats_performance_004: getHardwareUnitPowerPercent Wait Time : ${waitTime}`);
        expect(avgTime < TIMEOUT).assertTrue();
    })

    /**
    /* @tc.number batteryStats_performance_005
     * @tc.name batteryStats_performance_005
     * @tc.desc batteryStats getBatteryStats Interface
     */
    it('batteryStats_performance_005', 0, async function (done) {
        let startTime = new Date().getTime();
        for (let i = 0; i < MAXNUM_PROMISE; i++) {
            console.info("batteryStats_performance_005: call function before: " + i);
            let promise = batteryStats.getBatteryStats().then(function () {});
            await promise;
            console.info("batteryStats_performance_005: call function after: " + i);
        }
        done();
        let waitTime = new Date().getTime() - startTime;
        let avgTime = waitTime * MS_TO_US / MAXNUM_PROMISE; //us
        console.info(`batteryStats_performance_005: Promise: getBatteryStats Wait Time : ${avgTime}`);
        expect(avgTime < TIMEOUT).assertTrue();
    })
})
