# RP_Hero

## 前言
<div align=center>
<img src="https://github.com/WilliamGwok/RP_HERO/blob/main/Figure_and_Video/Figure/Fig_1.jpg" width="710px">
</div>
<br>
<p align="center">本项目为本人于2022~2023年在深圳大学RobotPilots实验室完成，供实验室新队员学习参考用。若有错误之处，欢迎指出。</p>

## HeroMater 上主控实车代码

[上主控主要代码](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application)

| 文件名 | 功能 | 文件链接 |
|---|---|---|
| Configs | 一些硬件模块的设置，如CAN，串口等 | [Con](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Configs) |
| Devices | Imu,遥控器相关代码 | [Dev](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Devices) |
| Drivers | 硬件驱动代码 | [Dri](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Drivers) |
| Hardwares | 机器人底盘，云台，发射机构的电机设置 | [Har](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Hardwares) |
| Modules | 机器人各模块控制代码 | [Mod](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Modules) |
| Protocols | 通信协议 | [Pro](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Protocols) |
| Supports | 算法功能包 | [Sup](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Supports) |
| Tasks | 整车任务 | [Tas](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Tasks) |

## Heroslave 下主控实车代码

[下主控主要代码](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroSlave/User)

下主控的主要任务是收取于转发裁判系统数据，并向其发送UI信息。以下只列出区别于上主控部分的内容。

| 文件名 | 功能 | 文件链接 |
|---|---|---|
| rpui | UI的图像输出，内容更新等 | [UI](https://github.com/WilliamGwok/RP_HERO/blob/main/HeroSlave/User/rpui/UI.c) |

可以在我的主页查看我fork的Priority_UI项目，这是实验室24年更新后的UI代码，拥有更优秀的优先级机制。

## Outpost 场地道具_前哨站代码

该部分代码较为简单，不做说明。

## 赛季总结

总结了一些调试过程中遇到的经验性问题和对该机器人的优化方向
 [pdf](https://github.com/WilliamGwok/RP_HERO/blob/main/2023%E5%B9%B4%E8%8B%B1%E9%9B%84%E8%B5%9B%E5%AD%A3%E6%80%BB%E7%BB%93.pdf)

## 测试视频

<p align="center">底盘功能测试。</p>

https://github.com/user-attachments/assets/981f2a2e-17d2-4cb0-a21d-21c682b7ae14

<p align="center">功率限制下机器人飞坡测试。</p>

https://github.com/user-attachments/assets/3c661f7d-0a61-4ac3-bc00-6929a2dadf31

<p align="center">视觉识别，跟随。</p>

https://github.com/user-attachments/assets/26ab51b6-4f9d-45a8-b156-4b01cc7bc2f1

