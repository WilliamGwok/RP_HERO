# RP_Hero

## 前書き

<div align="center">
<img src="https://github.com/WilliamGwok/RP_HERO/blob/main/Figure_and_Video/Figure/Fig_1.jpg" width="710px">
</div>
<br>
<p align="center">本プロジェクトは、2022年から2023年にかけて深セン大学のRobotPilots研究室で完成しました。研究室の新しいメンバーが学ぶための参考用として提供されています。誤りがありましたら、ぜひご指摘ください。</p>

## HeroMaster メインコントロールコード

[メインコントロールコード](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application)

| ファイル名 | 機能                                                  | ファイルリンク                                               |
| ---------- | ----------------------------------------------------- | ------------------------------------------------------------ |
| Configs    | CANやシリアルポートなどのハードウェアモジュールの設定 | [Con](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Configs) |
| Devices    | IMUやリモコンに関連するコード                         | [Dev](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Devices) |
| Drivers    | ハードウェアドライバーコード                          | [Dri](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Drivers) |
| Hardwares  | ロボットのシャーシ、ジンバル、発射機構のモーター設定  | [Har](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Hardwares) |
| Modules    | ロボットの各モジュールの制御コード                    | [Mod](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Modules) |
| Protocols  | 通信プロトコル                                        | [Pro](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Protocols) |
| Supports   | アルゴリズム機能パッケージ                            | [Sup](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Supports) |
| Tasks      | 全車両タスク                                          | [Tas](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroMaster/Application/Tasks) |

## HeroSlave サブコントロールコード

[サブコントロールコード](https://github.com/WilliamGwok/RP_HERO/tree/main/HeroSlave/User)

サブコントローラーの主なタスクは、審判システムのデータを受信および転送し、UI情報を送信することです。以下はサブコントローラー特有の内容を記載しています。

| ファイル名 | 機能                                 | ファイルリンク                                               |
| ---------- | ------------------------------------ | ------------------------------------------------------------ |
| rpui       | UIのグラフィック出力や内容更新を担当 | [UI](https://github.com/WilliamGwok/RP_HERO/blob/main/HeroSlave/User/rpui/UI.c) |

私のGitHubホームページで**Priority_UI**プロジェクトのフォークを確認できます。これは研究室が2024年に更新したUIコードで、より優れた優先度メカニズムを備えています。

## Outpost - フィールド小道具と前哨基地コード

このセクションは簡単なコードで構成されているため、詳細な説明は省略します。

## シーズン総括

デバッグ中に遭遇した経験的な問題と、このロボットの最適化方向についてまとめました。  
[PDF総括](https://github.com/WilliamGwok/RP_HERO/blob/main/2023%E5%B9%B4%E8%8B%B1%E9%9B%84%E8%B5%9B%E5%AD%A3%E6%80%BB%E7%BB%93.pdf)

## テスト動画

<p align="center">シャーシ機能テスト。</p>

https://github.com/user-attachments/assets/981f2a2e-17d2-4cb0-a21d-21c682b7ae14

<p align="center">出力制限下でのロボットの坂道テスト。</p>

https://github.com/user-attachments/assets/3c661f7d-0a61-4ac3-bc00-6929a2dadf31

<p align="center">視覚認識および追尾テスト。</p>

https://github.com/user-attachments/assets/26ab51b6-4f9d-45a8-b156-4b01cc7bc2f1
