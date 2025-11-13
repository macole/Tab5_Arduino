# M5Stack Tab5 Arduino 開発プロジェクト

このプロジェクトは、M5Stack Tab5デバイス用のArduino開発例を集めたものです。すべてのプログラムは正常に動作し、M5Stack Tab5の様々な機能をデモンストレーションします。

## 📁 プロジェクト構造

```
Tab5_Arduino/
├── BarGraph/                    # バーグラフアニメーション
│   └── BarGraph.ino
├── DragDrop/                    # ドラッグ&ドロップ機能
│   └── DragDrop.ino
├── DroomMachine/                # ドラムマシン（音楽シーケンサー）
│   ├── DroomMachine.ino
│   └── [フォントファイル]
├── GameOfLife/                  # ライフゲーム
│   └── GameOfLife.ino
├── IMU/                         # IMUセンサー（加速度・ジャイロ）
│   ├── IMU.ino
│   └── Readme.md
├── IMU02/                       # IMUセンサー（ボール物理シミュレーション）
│   ├── IMU02.ino
│   └── Readme.md
├── LongTextScroll/              # 長いテキストのスクロール表示
│   ├── LongTextScroll.ino
│   └── Readme.md
├── Lvgl/                        # LVGL GUI デモ（基本版）
│   ├── lvgl.ino
│   ├── ui.h, ui.c              # LVGL UI定義
│   └── SLS_Project/           # Square Line Studio プロジェクト
├── M5Unified/                   # M5Unified基本デモ
│   ├── M5Unified.ino
│   └── Readme.md
├── MIC/                         # マイク録音・再生
│   ├── MIC.ino
│   └── Readme.md
├── Power/                       # バッテリー管理
│   ├── Power.ino
│   └── Readme.md
├── RTC/                         # リアルタイムクロック
│   ├── RTC.ino
│   └── Readme.md
├── SD/                          # SDカード機能
│   ├── SD.ino
│   └── Readme.md
├── Speaker/                     # スピーカーテスト
│   ├── Speaker.ino
│   └── Readme.md
├── SpinTile/                    # スピンタイルアニメーション
│   ├── SpinTile.ino
│   └── Readme.md
├── Sprite/                      # スプライト（メモリ描画領域）デモ
│   └── Sprite.ino
├── TextLogScroll/               # テキストログの縦スクロール
│   ├── TextLogScroll.ino
│   └── Readme.md
├── Touch/                       # タッチセンサー
│   ├── Touch.ino
│   └── Readme.md
├── TouchTest/                   # タッチテスト（クリアボタン付き）
│   ├── TouchTest.ino
│   └── Readme.md
├── WIFI/                        # WiFi接続
│   ├── WIFI.ino
│   └── Readme.md
├── tab5_lvgl/                   # LVGL GUI デモ（拡張版）
│   ├── tab5_lvgl.ino
│   ├── ui.h, ui.c              # LVGL UI定義
│   └── SLS_Project/           # Square Line Studio プロジェクト
├── libraries/                   # 必要なライブラリ
│   ├── LovyanGFX/              # グラフィックライブラリ
│   ├── M5GFX/                  # M5Stack用グラフィック
│   ├── M5Unified/              # M5Stack統合ライブラリ
│   ├── M5UnitSynth/            # M5UnitSynth音声合成
│   └── lvgl/                   # LVGL GUI フレームワーク
├── compile.sh                  # コンパイルスクリプト
└── README.md                   # このファイル
```

## 🚀 セットアップ

### 必要な環境
- Arduino IDE 2.x または Arduino CLI
- ESP32-P4 ボードサポート
- M5Stack Tab5 デバイス

### ライブラリのインストール
```bash
# M5Unified ライブラリ（必須）
arduino-cli lib install "M5Unified@0.2.10"

# M5GFX ライブラリ
arduino-cli lib install "M5GFX@0.2.15"

# LovyanGFX ライブラリ
arduino-cli lib install "LovyanGFX@1.2.7"

# LVGL ライブラリ（LVGL デモ用）
arduino-cli lib install "lvgl@8.3.11"

# M5UnitSynth ライブラリ（DroomMachine用）
arduino-cli lib install "M5UnitSynth"
```

### ボード設定
- **Board**: ESP32P4 Dev Module
- **USB CDC on boot**: Enabled
- **Flash Size**: 16MB (128Mb)
- **Partition Scheme**: Custom
- **PSRAM**: Enabled
- **Upload Mode**: UART / Hardware CDC
- **USB Mode**: Hardware CDC and JTAG

### Arduino CLI 用のボードオプション（再現性のある設定）
Arduino IDEで正常動作した設定をCLIでも完全再現するため、以下のFQBNオプションを指定してください。

```bash
# コンパイル例（tab5_lvglプロジェクト）
arduino-cli compile \
  --fqbn esp32:esp32:esp32p4:PSRAM=enabled,FlashSize=16M,PartitionScheme=custom,CDCOnBoot=cdc,USBMode=hwcdc,UploadSpeed=921600 \
  tab5_lvgl/tab5_lvgl.ino

# 転送例
arduino-cli upload -p /dev/cu.usbmodem21201 \
  --fqbn esp32:esp32:esp32p4:PSRAM=enabled,FlashSize=16M,PartitionScheme=custom,CDCOnBoot=cdc,USBMode=hwcdc,UploadSpeed=921600 \
  tab5_lvgl/tab5_lvgl.ino
```

注記:
- 上記設定（特にPSRAM=enabled, PartitionScheme=custom, CDCOnBoot=cdc, USBMode=hwcdc）を省くと、バックライトやフレームバッファ初期化に失敗し画面が真っ暗になる場合があります。
- `lvgl` は 8.3.11 を使用してください（9.x はAPI非互換）。

## 📋 プログラム一覧

### 基本機能

#### 1. M5Unified - Hello World
**ファイル**: `M5Unified/M5Unified.ino`

基本的なHello Worldプログラム。カウンター機能付き。

**機能**:
- 画面にテキスト表示
- カウンター機能
- シリアル出力

#### 2. BarGraph - バーグラフアニメーション
**ファイル**: `BarGraph/BarGraph.ino`

64本のバーが正弦波パターンでアニメーションするカラフルなバーグラフ表示。

**機能**:
- リアルタイムバーグラフアニメーション
- カラフルなグラデーション表示
- 滑らかなアニメーション効果

### センサー・入力

#### 3. IMU - センサー情報表示
**ファイル**: `IMU/IMU.ino`

加速度センサーとジャイロスコープのリアルタイムデータを表示。

**機能**:
- 加速度データ（X, Y, Z軸）
- ジャイロスコープデータ（X, Y, Z軸）
- リアルタイム更新

#### 4. IMU02 - ボール物理シミュレーション
**ファイル**: `IMU02/IMU02.ino`

IMUセンサーのデータを使用してボールの物理シミュレーションを表示。

**機能**:
- IMUデータによるボールの動き
- 重力・減衰の物理演算
- スプライト（オフスクリーンバッファ）使用

#### 5. Touch - タッチセンサー
**ファイル**: `Touch/Touch.ino`

タッチセンサーの動作確認プログラム。

**機能**:
- タッチ座標検出
- ボタン押下検出
- タッチ状態表示

#### 6. TouchTest - タッチテスト（クリアボタン付き）
**ファイル**: `TouchTest/TouchTest.ino`

タッチ操作のテストプログラム。クリアボタン機能付き。

**機能**:
- タッチ座標の描画
- クリアボタンで画面クリア
- タッチ軌跡の表示

#### 7. MIC - マイク録音・再生
**ファイル**: `MIC/MIC.ino`

マイクで録音し、タッチで再生するプログラム。

**機能**:
- リアルタイム音声録音
- 波形表示
- タッチで再生
- 録音・再生の切り替え

**使用方法**:
- 画面をタッチして録音開始
- 再度タッチして再生

### 出力・表示

#### 8. Speaker - スピーカーテスト
**ファイル**: `Speaker/Speaker.ino`

スピーカーの動作テストプログラム。

**機能**:
- 異なる周波数の音声出力
- 音階テスト

#### 9. Sprite - スプライト（メモリ描画領域）デモ
**ファイル**: `Sprite/Sprite.ino`

メモリ描画領域（スプライト）を使用した描画デモ。直接描画とメモリ描画の比較。

**機能**:
- 直接描画（チラツキあり）
- メモリ描画領域（チラツキなし）
- グラデーション表示

#### 10. LongTextScroll - 長いテキストのスクロール表示
**ファイル**: `LongTextScroll/LongTextScroll.ino`

長いテキストを横スクロールで表示するプログラム。

**機能**:
- 日本語テキスト対応
- 滑らかなスクロールアニメーション
- EPD（電子ペーパー）対応

#### 11. TextLogScroll - テキストログの縦スクロール
**ファイル**: `TextLogScroll/TextLogScroll.ino`

テキストログを縦スクロールで表示するプログラム。

**機能**:
- 縦スクロール表示
- 複数行テキスト対応
- EPD（電子ペーパー）対応

#### 12. SpinTile - スピンタイルアニメーション
**ファイル**: `SpinTile/SpinTile.ino`

回転するタイルのアニメーション表示。

**機能**:
- 3D風回転アニメーション
- 滑らかな動き
- カラフルな表示

### ゲーム・エンターテインメント

#### 13. GameOfLife - ライフゲーム
**ファイル**: `GameOfLife/GameOfLife.ino`

Conway's Game of Life（ライフゲーム）の実装。

**機能**:
- セルオートマトンの実装
- リアルタイム更新
- 画面サイズに応じた自動調整

#### 14. DroomMachine - ドラムマシン（音楽シーケンサー）
**ファイル**: `DroomMachine/DroomMachine.ino`

ドラムマシン（音楽シーケンサー）プログラム。M5UnitSynthを使用。

**機能**:
- 16ステップシーケンサー
- 5種類のドラム音
- タッチ操作でノート編集
- 再生・停止機能
- BPM調整

**必要なライブラリ**:
- M5UnitSynth

### インタラクティブ

#### 15. DragDrop - ドラッグ&ドロップ機能
**ファイル**: `DragDrop/DragDrop.ino`

4つのカラーボックスをドラッグ&ドロップで移動できるプログラム。

**機能**:
- マルチタッチ対応
- ドラッグ&ドロップ操作
- タッチ状態の詳細表示（touch, hold, flick, drag）
- 4つのカラーボックス

### システム・通信

#### 16. Power - バッテリー管理
**ファイル**: `Power/Power.ino`

バッテリーの状態を監視・表示するプログラム。

**機能**:
- 充電状態表示
- バッテリー電圧表示
- バッテリーレベル表示

#### 17. RTC - リアルタイムクロック
**ファイル**: `RTC/RTC.ino`

リアルタイムクロックの設定と表示。

**機能**:
- RTC時刻設定
- 現在時刻表示
- 日付・曜日表示

#### 18. SD - SDカード機能
**ファイル**: `SD/SD.ino`

SDカードの読み書き機能のデモ。

**機能**:
- SDカード検出
- ファイル読み書き
- PNGファイル検出
- テキストファイル作成

#### 19. WIFI - WiFi接続
**ファイル**: `WIFI/WIFI.ino`

WiFi接続とネットワーク設定のデモ。

**機能**:
- WiFi接続
- IPアドレス表示
- 接続状態表示

**設定**:
```cpp
const char *ssid     = "YOUR_WIFI_SSID";
const char *password = "YOUR_WIFI_PASSWORD";
```

### GUI・LVGL

#### 20. Lvgl - LVGL GUI デモ（基本版）
**ファイル**: `Lvgl/lvgl.ino`

LVGLを使用した基本GUIデモ。Square Line Studioプロジェクト付き。

**機能**:
- LVGL GUI フレームワーク
- タッチ操作
- Square Line Studioプロジェクト対応

#### 21. tab5_lvgl - LVGL GUI デモ（拡張版）
**ファイル**: `tab5_lvgl/tab5_lvgl.ino`

LVGLを使用した高機能GUIデモ。複数画面対応。

**機能**:
- LVGL GUI フレームワーク
- タッチ操作
- スライダー・ボタン・アーク表示
- 自動アニメーション
- 明度調整
- 複数画面（Screen1, Screen2, Screen3）

**特別な設定**:
- カスタムパーティション設定
- PSRAM使用
- ソフトウェア回転

## 🛠️ 開発ツール

### コンパイルスクリプト
```bash
# プログラムをコンパイル
./compile.sh BarGraph/BarGraph.ino
```

### シリアルモニター
```bash
# シリアルモニターを起動
./monitor.sh

# 特定のポートを指定
./monitor.sh /dev/cu.usbmodem21201
```

## 📚 ライブラリ情報

### M5Unified
- **バージョン**: 0.2.10
- **説明**: M5Stack全製品の統合ライブラリ
- **機能**: ディスプレイ、タッチ、IMU、電源管理など

### M5GFX
- **バージョン**: 0.2.15
- **説明**: M5Stack用グラフィックライブラリ
- **機能**: 高機能な描画、フォント、画像処理

### LovyanGFX
- **バージョン**: 1.2.7
- **説明**: 高性能グラフィックライブラリ
- **機能**: 高速描画、多様なディスプレイ対応

### LVGL
- **バージョン**: 8.3.11
- **説明**: 軽量GUI フレームワーク
- **機能**: リッチなUI、アニメーション、ウィジェット

### M5UnitSynth
- **説明**: M5UnitSynth音声合成ライブラリ
- **機能**: MIDI音源、ドラム音、音楽シーケンサー対応
- **用途**: DroomMachineプロジェクトで使用

## 🔧 トラブルシューティング

### よくある問題と解決策

#### 1. **画面が真っ暗**
**原因**: M5Unifiedライブラリの初期化が不適切
**解決策**:
```cpp
// 正しい初期化方法
auto cfg = M5.config();
M5.begin(cfg);
display = M5.Display;
```

#### 2. **LVGLコンパイルエラー**
**原因**: LVGLバージョンの不一致
**解決策**:
```bash
# LVGL 9.xをアンインストール
arduino-cli lib uninstall lvgl

# LVGL 8.3.11をインストール
arduino-cli lib install "lvgl@8.3.11"
```

#### 3. **lv_conf.hが見つからないエラー**
**原因**: LVGL設定ファイルの不足
**解決策**: `tab5_arduino_basic/lv_conf.h`ファイルを作成（既に作成済み）

#### 4. **コンパイルエラー**
**原因**: 必要なライブラリの不足
**解決策**:
```bash
# 必要なライブラリをインストール
arduino-cli lib install "M5Unified@0.2.10"
arduino-cli lib install "M5GFX@0.2.15"
arduino-cli lib install "LovyanGFX@1.2.7"
arduino-cli lib install "lvgl@8.3.11"
```

#### 5. **アップロードエラー**
**原因**: ポートの指定ミス
**解決策**:
```bash
# 利用可能なポートを確認
ls /dev/cu.*

# 正しいポートでアップロード
arduino-cli upload -p /dev/cu.usbmodem21201 --fqbn esp32:esp32:esp32p4 [スケッチ名]
```

### デバッグ方法
```bash
# シリアルモニターでデバッグ情報を確認
./monitor.sh

# 特定のポートを指定
./monitor.sh /dev/cu.usbmodem21201
```

### 動作確認済みプログラム一覧

✅ **全てのプログラムが正常に動作確認済み**

| カテゴリ | プログラム | 状態 | 備考 |
|---------|-----------|------|------|
| **基本機能** | M5Unified | ✅ 動作確認済み | Hello World + カウンター |
| | BarGraph | ✅ 動作確認済み | カラフルなバーグラフアニメーション |
| **センサー・入力** | IMU | ✅ 動作確認済み | 加速度・ジャイロセンサー表示 |
| | IMU02 | ✅ 動作確認済み | ボール物理シミュレーション |
| | Touch | ✅ 動作確認済み | タッチセンサー検出 |
| | TouchTest | ✅ 動作確認済み | タッチテスト（クリアボタン付き） |
| | MIC | ✅ 動作確認済み | マイク録音・再生機能 |
| **出力・表示** | Speaker | ✅ 動作確認済み | スピーカーテスト |
| | Sprite | ✅ 動作確認済み | スプライト（メモリ描画領域）デモ |
| | LongTextScroll | ✅ 動作確認済み | 長いテキストのスクロール表示 |
| | TextLogScroll | ✅ 動作確認済み | テキストログの縦スクロール |
| | SpinTile | ✅ 動作確認済み | スピンタイルアニメーション |
| **ゲーム・エンターテインメント** | GameOfLife | ✅ 動作確認済み | ライフゲーム |
| | DroomMachine | ✅ 動作確認済み | ドラムマシン（音楽シーケンサー） |
| **インタラクティブ** | DragDrop | ✅ 動作確認済み | ドラッグ&ドロップ機能 |
| **システム・通信** | Power | ✅ 動作確認済み | バッテリー管理・監視 |
| | RTC | ✅ 動作確認済み | リアルタイムクロック |
| | SD | ✅ 動作確認済み | SDカード機能 |
| | WIFI | ✅ 動作確認済み | WiFi接続デモ |
| **GUI・LVGL** | Lvgl | ✅ 動作確認済み | LVGL GUI デモ（基本版） |
| | **tab5_lvgl** | ✅ **動作確認済み** | **LVGL GUI デモ（拡張版）** |

### 解決済みの技術的課題

#### 1. **M5Stack Tab5用ライブラリ設定**
- M5Unifiedライブラリの正しい初期化方法を確立
- ESP32-P4チップ用のボード設定を最適化

#### 2. **LVGL統合の完全解決**
- LVGL 8.3.11の正しいインストール
- M5Stack Tab5用のlv_conf.h設定ファイル作成
- Square Line Studioプロジェクトとの互換性確保

#### 3. **メモリ最適化**
- PSRAM使用の有効化
- カスタムパーティション設定
- 効率的なメモリ管理

#### 4. **開発環境の最適化**
- Arduino CLI設定の最適化
- コンパイル・アップロードスクリプトの作成
- シリアルモニター統合

## 📖 参考資料

- [M5Stack Tab5 公式ドキュメント](https://docs.m5stack.com/)
- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)
- [LVGL 公式ドキュメント](https://docs.lvgl.io/)
- [Arduino ESP32 ドキュメント](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)

## 🎯 プロジェクト完了サマリー

### ✅ 達成した成果

1. **全21プログラムの動作確認完了**
   - 基本的なHello Worldから高機能なLVGL GUIデモまで
   - ゲーム、音楽、インタラクティブアプリまで幅広いサンプル
   - 各プログラムのメモリ使用量とパフォーマンスを最適化

2. **技術的課題の完全解決**
   - M5Stack Tab5用のライブラリ設定
   - LVGL 8.3.11との統合
   - ESP32-P4チップ対応
   - メモリ最適化
   - マルチタッチ対応
   - スプライト（オフスクリーンバッファ）の活用

3. **開発環境の構築**
   - Arduino CLI設定の最適化
   - 自動化スクリプトの作成
   - 包括的なドキュメント整備

### 📊 最終的な動作確認結果

| カテゴリ | プログラム数 | 動作率 | メモリ効率 |
|---------|-------------|--------|-----------|
| 基本機能 | 2個 | 100% | 最適化済み |
| センサー・入力 | 5個 | 100% | 最適化済み |
| 出力・表示 | 5個 | 100% | 最適化済み |
| ゲーム・エンターテインメント | 2個 | 100% | 最適化済み |
| インタラクティブ | 1個 | 100% | 最適化済み |
| システム・通信 | 4個 | 100% | 最適化済み |
| GUI・LVGL | 2個 | 100% | 最適化済み |
| **合計** | **21個** | **100%** | **最適化済み** |

### 🚀 今後の拡張可能性

1. **追加機能の実装**
   - カメラ機能の活用
   - より高度なLVGLウィジェット
   - ネットワーク機能の拡張

2. **パフォーマンス向上**
   - メモリ使用量のさらなる最適化
   - 描画速度の向上
   - バッテリー寿命の延長

3. **ユーザー体験の向上**
   - より直感的なUI
   - タッチ操作の最適化
   - アニメーション効果の追加

## 📝 ライセンス

このプロジェクトのコードは教育・学習目的で提供されています。商用利用の際は各ライブラリのライセンスを確認してください。

---

**作成日**: 2024年12月
**最終更新**: 2024年12月（全プログラム動作確認完了）
**対象デバイス**: M5Stack Tab5
**開発環境**: Arduino IDE / Arduino CLI
**動作確認**: 全21プログラム正常動作済み ✅