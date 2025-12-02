# IMU_BallBox - ボール物理シミュレーション

M5Stack Tab5のIMUセンサー（加速度・ジャイロ）を使用して、ボールの物理シミュレーションを行うプログラムです。

## 📋 概要

このプログラムは、M5Stack Tab5のIMUセンサーのデータを使用して、画面内のボールが物理法則に従って動くシミュレーションを実現します。デバイスを傾けると、ボールが重力と慣性に従って動きます。

## 🔧 必要なハードウェア

- M5Stack Tab5

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```
- **M5GFX**: M5Stack用グラフィックライブラリ
  ```bash
  arduino-cli lib install "M5GFX@0.2.15"
  ```

## 🚀 セットアップ

1. **ボード設定**
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

2. **プログラムのアップロード**
   - Arduino IDEまたはArduino CLIでプログラムをアップロード

## 💻 使用方法

1. プログラムをアップロード
2. デバイスを傾けると、画面内のボールが動きます
3. ボールは画面の端で跳ね返ります
4. 加速度とジャイロのデータが画面に表示されます

## 📊 機能

- **物理シミュレーション**: 重力と慣性に基づいたボールの動き
- **IMUセンサー連動**: デバイスの傾きに応じてボールが動く
- **跳ね返り**: 画面の端でボールが跳ね返る
- **減衰**: 摩擦による減衰効果
- **リアルタイム表示**: 加速度とジャイロのデータを表示

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - スプライト（オフスクリーンバッファ）の作成
   - ボールを画面中央に配置

2. **メインループ**
   - IMUデータを取得
   - 加速度から速度を計算
   - 速度から位置を更新
   - 画面端で跳ね返り処理
   - 減衰を適用
   - スプライトに描画
   - 画面に表示

## 📝 コードの主要なポイント

### 物理パラメータ
```cpp
const float GRAVITY = 9.8;   // 重力加速度（m/s²）
const float DAMPING = 0.95;  // 減衰係数（摩擦）
```

### IMUデータの取得
```cpp
M5.Imu.update();
imuData = M5.Imu.getImuData();
```

### 速度の更新
```cpp
velX += imuData.accel.x * 0.1;  // X軸加速度から速度を計算
velY -= imuData.accel.y * 0.1;  // Y軸加速度から速度を計算
```

### 跳ね返り処理
```cpp
if (ballX < BALL_RADIUS || ballX > width - BALL_RADIUS) {
    velX = -velX * 0.8;  // X軸で跳ね返り
}
if (ballY < BALL_RADIUS || ballY > height - BALL_RADIUS) {
    velY = -velY * 0.8;  // Y軸で跳ね返り
}
```

## 🎮 操作方法

- **デバイスを傾ける**: ボールが傾きの方向に動きます
- **デバイスを振る**: ボールが加速します
- **デバイスを静止**: ボールは減衰して停止します

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み

