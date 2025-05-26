/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { describe, it, expect, TestType, Size, Level, beforeAll } from "../../index";
import hilog from '@ohos.hilog'
import batteryStats from '@ohos.batteryStatistics'
import { BusinessError } from '@ohos.base'

let domain: number = 0x0000;
let tag: string = 'BatteryStatisticsTest';

export default function batteryStatisticsTest() {

  describe("BatteryStatisticsTest", (): void => {
    hilog.info(domain, tag, '%{public}s', 'BatteryStatisticsTest start');
    it("BatteryStatisticsTest0100", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest0100';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let consumptionType = batteryStats.ConsumptionType.CONSUMPTION_TYPE_INVALID;
          hilog.info(domain, tag, `${TAG} consumptionType type: ${typeof consumptionType}`);
          expect(new Number(consumptionType)).assertEqual(-17);
          done();
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect().assertFail();
          done();
        }
      })
    it("BatteryStatisticsTest0200", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest0200';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let consumptionType = batteryStats.ConsumptionType.CONSUMPTION_TYPE_APP;
          hilog.info(domain, tag, `${TAG} consumptionType type: ${typeof consumptionType}`);
          expect(new Number(consumptionType)).assertEqual(-16);
          done();
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect().assertFail();
          done();
        }
      })
    it("BatteryStatisticsTest0300", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest0300';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let consumptionType = batteryStats.ConsumptionType.CONSUMPTION_TYPE_BLUETOOTH;
          hilog.info(domain, tag, `${TAG} consumptionType type: ${typeof consumptionType}`);
          expect(new Number(consumptionType)).assertEqual(-15);
          done();
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect().assertFail();
          done();
        }
      })
    it("BatteryStatisticsTest0400", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest0400';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let consumptionType = batteryStats.ConsumptionType.CONSUMPTION_TYPE_IDLE;
          hilog.info(domain, tag, `${TAG} consumptionType type: ${typeof consumptionType}`);
          expect(new Number(consumptionType)).assertEqual(-14);
          done();
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect().assertFail();
          done();
        }
      })
    it("BatteryStatisticsTest0500", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest0500';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let consumptionType = batteryStats.ConsumptionType.CONSUMPTION_TYPE_PHONE;
          hilog.info(domain, tag, `${TAG} consumptionType type: ${typeof consumptionType}`);
          expect(new Number(consumptionType)).assertEqual(-13);
          done();
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect().assertFail();
          done();
        }
      })
    it("BatteryStatisticsTest0600", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest0600';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let consumptionType = batteryStats.ConsumptionType.CONSUMPTION_TYPE_RADIO;
          hilog.info(domain, tag, `${TAG} consumptionType type: ${typeof consumptionType}`);
          expect(new Number(consumptionType)).assertEqual(-12);
          done();
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect().assertFail();
          done();
        }
      })
    it("BatteryStatisticsTest0700", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest0700';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let consumptionType = batteryStats.ConsumptionType.CONSUMPTION_TYPE_SCREEN;
          hilog.info(domain, tag, `${TAG} consumptionType type: ${typeof consumptionType}`);
          expect(new Number(consumptionType)).assertEqual(-11);
          done();
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect().assertFail();
          done();
        }
      })
    it("BatteryStatisticsTest0800", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest0800';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let consumptionType = batteryStats.ConsumptionType.CONSUMPTION_TYPE_USER;
          hilog.info(domain, tag, `${TAG} consumptionType type: ${typeof consumptionType}`);
          expect(new Number(consumptionType)).assertEqual(-10);
          done();
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect().assertFail();
          done();
        }
      })
    it("BatteryStatisticsTest0900", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest0900';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let consumptionType = batteryStats.ConsumptionType.CONSUMPTION_TYPE_WIFI;
          hilog.info(domain, tag, `${TAG} consumptionType type: ${typeof consumptionType}`);
          expect(new Number(consumptionType)).assertEqual(-9);
          done();
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect().assertFail();
          done();
        }
      })
    it("BatteryStatisticsTest1000", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest1000';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          batteryStats.getBatteryStats()
            .then(() => {
              hilog.error(domain, tag, `${TAG} getBatteryStats Failure`);
              expect().assertFail();
              done;
            })
            .catch((error: Error): void => {
              hilog.error(domain, tag, `${TAG} error: ${JSON.stringify(error)}`);
              expect(error.message).assertContain('System permission is denied');
              done();
            });
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect(e.code).assertEqual(202);
          done();
        }
      })
    it("BatteryStatisticsTest1100", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest1100';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          batteryStats.getBatteryStats((error: BusinessError, data?: Array<batteryStats.BatteryStatsInfo>) => {
            if (error) {
              hilog.error(domain, tag, `${TAG} error: ${JSON.stringify(error)}`);
              expect(error.message).assertContain('System permission is denied');
              done();
            } else {
              hilog.error(domain, tag, `${TAG} getBatteryStats Failure`);
              expect().assertFail();
              done;
            }
          })
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect(e.code).assertEqual(202);
          done();
        }
      })
    it("BatteryStatisticsTest1200", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest1200';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let value = batteryStats.getAppPowerValue(10021);
          hilog.info(domain, tag, `${TAG} value: ${value}`);
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect(e.code).assertEqual(202);
          done();
        }
      })
    it("BatteryStatisticsTest1300", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest1300';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let percent = batteryStats.getAppPowerPercent(10021);
          hilog.info(domain, tag, `${TAG} percent: ${percent}`);
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect(e.code).assertEqual(202);
          done();
        }
      })
    it("BatteryStatisticsTest1400", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest1400';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let value = batteryStats.getHardwareUnitPowerValue(batteryStats.ConsumptionType.CONSUMPTION_TYPE_USER);
          hilog.info(domain, tag, `${TAG} value: ${value}`);
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect(e.code).assertEqual(202);
          done();
        }
      })
    it("BatteryStatisticsTest1500", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3,
      async (done: () => void): Promise<void> => {
        let TAG: string = 'BatteryStatisticsTest1500';
        hilog.info(domain, tag, `${TAG} start`);
        try {
          let percent = batteryStats.getHardwareUnitPowerPercent(batteryStats.ConsumptionType.CONSUMPTION_TYPE_WIFI);
          hilog.info(domain, tag, `${TAG} percent: ${percent}`);
        } catch (e: BusinessError) {
          hilog.error(domain, tag, `${TAG} e: ${JSON.stringify(e)}`);
          expect(e.code).assertEqual(202);
          done();
        }
      })
  })
  hilog.info(domain, tag, '%{public}s', 'BatteryInfoTest end');
}