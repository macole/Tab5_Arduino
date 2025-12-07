# SNTP - SNTP時刻同期

M5Stack Tab5のWiFi機能を使用して、SNTP（Simple Network Time Protocol）プロトコルで時刻を同期するプログラムです。

## 📋 概要

このプログラムは、M5Stack Tab5のWiFi機能を使用して、NTPサーバーから正確な時刻を取得し、デバイスの時刻を同期します。複数のNTPサーバーに対応し、タイムゾーン設定も可能です。

## 🔧 必要なハードウェア

- M5Stack Tab5

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```
- **WiFi**: ESP32標準ライブラリ（Arduino IDEに含まれています）
- **esp_sntp / sntp**: ESP32標準ライブラリ（Arduino IDEに含まれています）

### esp_sntp.h と sntp.h の違い

このスケッチでは、`SNTP/SNTP.ino` の先頭で次の2つのヘッダーファイルをインクルードしています。

```cpp
#include <esp_sntp.h>
#include <sntp.h>
```

それぞれの役割と、なぜ両方を使っているかは次の通りです。

- **esp_sntp.h**
  - ESP-IDF（ESP32の公式フレームワーク）側で提供されている、SNTP 用の**高レベルラッパーAPI**です。
  - このスケッチで使用している `configTzTime()` などの「タイムゾーンとNTPサーバーをまとめて設定する便利関数」は `esp_sntp.h` 側で宣言されています。
- **sntp.h**
  - ESP32内部で使われている lwIP スタックの **低レベルSNTPライブラリ**です。
  - SNTP同期状態を表す `sntp_sync_status_t` や、その状態を取得する `sntp_get_sync_status()` などの関数・定数がこちらで定義されています。

このスケッチでは、

- **NTPサーバーの設定とタイムゾーン設定**には `configTzTime()`（`esp_sntp.h`）を使用し、
- **「同期完了するまで待つ処理」には `sntp_get_sync_status()`（`sntp.h`）を使用**

しているため、**両方のヘッダーファイルをインクルードする必要があります。**  
片方だけでは、便利な高レベルAPIまたは同期状態の取得のいずれかが不足してしまうためです。

## 🚀 セットアップ

1. **ボード設定**
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

2. **WiFi設定**
   - `secrets.h.example`をコピーして`secrets.h`を作成してください：
   ```bash
   cp secrets.h.example secrets.h
   ```
   - `secrets.h`ファイル内の`ssid`と`password`を実際の値に編集してください：
   ```cpp
   const char *ssid = "YOUR_WIFI_SSID";
   const char *password = "YOUR_WIFI_PASSWORD";
   ```
   - **重要**: `secrets.h`は`.gitignore`に追加されているため、Gitにコミットされません。認証情報が漏れる心配はありません。

3. **タイムゾーン設定（オプション）**
   - `NTP_TIMEZONE`を編集してタイムゾーンを設定できます：
   ```cpp
   #define NTP_TIMEZONE "UTC-9"  // 日本時間の場合
   ```

4. **プログラムのアップロード**
   - Arduino IDEまたはArduino CLIでプログラムをアップロード

## 💻 使用方法

1. WiFiのSSIDとパスワードを設定
2. タイムゾーンを設定（オプション）
3. プログラムをアップロード
4. デバイスがWiFiネットワークに接続し、NTPサーバーから時刻を取得します
5. 同期された時刻が画面とシリアルモニターに表示されます

## 📊 機能

- **SNTP時刻同期**: NTPサーバーから正確な時刻を取得
- **複数サーバー対応**: 複数のNTPサーバーに対応（フォールバック機能）
- **タイムゾーン設定**: タイムゾーンを設定可能
- **自動同期**: 定期的に時刻を同期
- **時刻表示**: 同期された時刻を画面に表示

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - WiFi接続
   - SNTP設定（サーバー、タイムゾーン）

2. **時刻同期**
   - NTPサーバーに接続
   - 時刻を取得
   - デバイスの時刻を更新

3. **時刻表示**
   - 同期された時刻を画面に表示
   - シリアルモニターにも出力

## 📝 コードの主要なポイント

### NTPサーバーの設定
```cpp
#define NTP_SERVER1 "0.pool.ntp.org"
#define NTP_SERVER2 "1.pool.ntp.org"
#define NTP_SERVER3 "2.pool.ntp.org"
```

### タイムゾーンの設定
```cpp
#define NTP_TIMEZONE "UTC-9"  // 日本時間（UTC-9）
```

### SNTPの初期化
```cpp
sntp_set_time_sync_notification_cb(time_sync_notification_cb);
sntp_servermode_dhcp(1);
sntp_set_sync_mode(SNTP_SYNC_MODE_IMMED);
```

## 🌍 タイムゾーン設定例

- **日本時間**: `UTC-9`
- **UTC**: `UTC+0`
- **アメリカ東部時間**: `UTC+5`
- **ヨーロッパ中央時間**: `UTC-1`

## 🔧 トラブルシューティング

### 時刻が同期されない場合
- WiFiのSSIDとパスワードが正しいか確認してください
- NTPサーバーがアクセス可能か確認してください
- シリアルモニターでエラーメッセージを確認してください

### 時刻がずれている場合
- タイムゾーン設定が正しいか確認してください
- NTPサーバーの設定を確認してください

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年12月6日  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み
