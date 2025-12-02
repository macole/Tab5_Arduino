# https_client - HTTPS クライアント

M5Stack Tab5のWiFi機能を使用して、HTTPS通信を行うクライアントプログラムです。

## 📋 概要

このプログラムは、M5Stack Tab5のWiFi機能を使用して、HTTPSサーバーに接続し、セキュアな通信を行うデモプログラムです。SSL/TLS証明書を使用した安全な通信を実現します。

## 🔧 必要なハードウェア

- M5Stack Tab5

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```
- **WiFi**: ESP32標準ライブラリ（Arduino IDEに含まれています）
- **HTTPClient**: ESP32標準ライブラリ（Arduino IDEに含まれています）
- **WiFiClientSecure**: ESP32標準ライブラリ（Arduino IDEに含まれています）

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
   - `secrets.h`ファイル内の`ssid`と`pass`を実際の値に編集してください：
   ```cpp
   const char *ssid = "YOUR_WIFI_SSID";
   const char *pass = "YOUR_WIFI_PASSWORD";
   ```
   - **重要**: `secrets.h`は`.gitignore`に追加されているため、Gitにコミットされません。認証情報が漏れる心配はありません。

3. **プログラムのアップロード**
   - Arduino IDEまたはArduino CLIでプログラムをアップロード

## 💻 使用方法

1. WiFiのSSIDとパスワードを設定
2. プログラムをアップロード
3. デバイスがWiFiネットワークに接続し、HTTPSサーバーに接続します
4. レスポンスが画面とシリアルモニターに表示されます

## 📊 機能

- **HTTPS接続**: SSL/TLSを使用したセキュアな通信
- **証明書検証**: ルートCA証明書によるサーバー証明書の検証
- **HTTPリクエスト**: GET/POSTリクエストの送信
- **レスポンス表示**: 取得したデータを画面とシリアルモニターに表示

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - WiFi接続
   - HTTPSクライアントの設定

2. **HTTPS通信**
   - ルートCA証明書の設定
   - HTTPSサーバーへの接続
   - HTTPリクエストの送信
   - レスポンスの受信と表示

## 📝 コードの主要なポイント

### ルートCA証明書の設定
```cpp
const char* rootCA = "-----BEGIN CERTIFICATE-----\n" \
                      "...証明書データ...\n" \
                      "-----END CERTIFICATE-----\n";
```

### HTTPSクライアントの初期化
```cpp
WiFiClientSecure client;
client.setCACert(rootCA);
```

### HTTPSリクエストの送信
```cpp
HTTPClient https;
https.begin(client, url);
int httpCode = https.GET();
```

## 🔒 セキュリティ

- SSL/TLS証明書によるサーバー認証
- 暗号化された通信チャネル
- ルートCA証明書による信頼性の確保

## 🔧 トラブルシューティング

### 接続できない場合
- WiFiのSSIDとパスワードが正しいか確認してください
- ルートCA証明書が正しいか確認してください
- シリアルモニターでエラーメッセージを確認してください

### 証明書エラーが発生する場合
- サーバーのルートCA証明書を正しく設定してください
- 証明書の有効期限を確認してください

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み
