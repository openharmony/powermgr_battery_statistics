# 耗电统计服务组件<a name="ZH-CN_TOPIC_0000001115047353"></a>

-   [简介](#section11660541593)
-   [目录](#section19472752217)
-   [相关仓](#section63151229062)

## 简介<a name="section11660541593"></a>

耗电统计组件提供如下功能：

1.  软件耗电统计：统计每个应用或者软件的耗电情况，耗电量是以Uid为单位统计；软件功耗的包括不限于下面几项：cpu的耗电、持锁运行带来的耗电、移动无线的耗电 、wifi耗电 、gps耗电、传感器的耗电 、相机耗电、 闪光灯耗电等。
2.   硬件耗电统计：软件耗电之外的耗电都归属到硬件耗电，包括不限于如下几项：用户功耗 、通话功耗、屏幕功耗 、Wifi功耗 、蓝牙消耗等等。

**图 1**  耗电统计服务组件架构图<a name="fig106301571239"></a>  
![](figures/power-management-subsystem-architecture.png "电源管理子系统架构图")

## 目录<a name="section19472752217"></a>

```
/base/powermgr/battery_statistics
├── interfaces                   # 接口层
│   ├── innerkits                # 内部接口
│   └── kits                     # 外部接口
├── sa_profile                   # SA配置文件
└── services                     # 服务层
│   ├── native                   # native层
│   └── zidl                     # zidl层
└── utils                        # 工具和通用层
```



## 相关仓<a name="section63151229062"></a>

电源管理子系统

[powermgr_power_manager](https://gitee.com/openharmony/powermgr_power_manager)

[powermgr_display_manager](https://gitee.com/openharmony/powermgr_display_manager)

[powermgr_battery_manager](https://gitee.com/openharmony/powermgr_battery_manager)

[powermgr_thermal_manager](https://gitee.com/openharmony/powermgr_thermal_manager)

[**powermgr_battery_statistics**](https://gitee.com/openharmony/powermgr_battery_statistics)

[powermgr_battery_lite](https://gitee.com/openharmony/powermgr_battery_lite)

[powermgr_powermgr_lite](https://gitee.com/openharmony/powermgr_powermgr_lite)