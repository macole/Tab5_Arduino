# Serial - シリアル通信 + LED制御 (M5Unified 対応)

M5Stack Tab5 上で、シリアルモニタとディスプレイの両方に LED のオン / オフ状態を表示するサンプルプログラムです。

## 📋 概要

このプログラム **Serial** は、

- GPIO ピン（3番ピン）で LED を点滅させ
- シリアルモニタに「LED ON / LED OFF」を出力し
- 同じ内容を M5Stack Tab5 のディスプレイにも表示します。

シリアル通信と画面表示の両方を同時に確認できる、基本的なデバッグ用サンプルです。

## 🔧 必要なハードウェア

- M5Stack Tab5
- （任意）GPIO 3番ピンに接続した LED + 抵抗

※ Tab5 本体に直接は LED がついていないため、LED 出力は任意です。  
　シリアルモニタと画面の表示だけでも動作確認できます。

## 📚 必要なライブラリ

- **M5Unified**: M5Stack 統合ライブラリ

```bash
arduino-cli lib install "M5Unified@0.2.10"
```

## 🚀 セットアップ

1. **ボード設定**
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

2. **プログラムのアップロード**
   - スケッチ: `serial/serial.ino`
   - Arduino IDE または Arduino CLI でアップロード

3. **シリアルモニタ**
   - ボーレート: **115200bps**
   - 起動直後に `Serial - Ready`、その後 `LED ON / LED OFF` が1秒ごとに表示されます。

## 💻 使用方法

1. プログラムを Tab5 にアップロードします。
2. シリアルモニタ（115200bps）を開きます。
3. 画面とシリアルモニタの両方に以下が表示されます。
   - 起動時: `Serial - Ready`
   - 1秒ごとに:
     - `LED ON` / `Status: LED ON`
     - `LED OFF` / `Status: LED OFF`

## 📊 機能

- **シリアル出力**: `Serial.println()` で状態を出力
- **ディスプレイ表示**: `M5Unified` の `M5.Display` 経由でテキスト表示
- **LED 制御**: `pinMode(3, OUTPUT)` + `digitalWrite()` による ON/OFF 制御

## 🔍 プログラムの動作

1. **初期化**
   - `M5.config()` → `M5.begin(cfg)` で M5Unified を初期化
   - ディスプレイの回転・テキストサイズの設定
   - シリアル通信を 115200bps で開始

2. **ループ処理**
   - 3番ピンを HIGH にして「LED ON」にし、シリアルと画面に状態を表示
   - 1秒待機
   - 3番ピンを LOW にして「LED OFF」にし、シリアルと画面に状態を表示
   - 1秒待機

## 📝 ライセンス

このプログラムは MIT ライセンスの下で公開されています。  
詳細はプロジェクトルートの `LICENSE` ファイルを参照してください。

---

**作成日**: 2025年12月6日  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み（想定）


