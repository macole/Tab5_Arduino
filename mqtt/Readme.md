# MQTT - MQTT通信

M5Stack Tab5のWiFi機能を使用して、MQTTプロトコルでIoTデバイス間通信を行うプログラムです。

## 📋 概要

このプログラムは、M5Stack Tab5のWiFi機能を使用して、MQTTブローカーに接続し、トピックの購読（Subscribe）と発行（Publish）を行うデモプログラムです。IoTデバイス間の通信に使用できます。

## 🔧 必要なハードウェア

- M5Stack Tab5

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```
- **WiFi**: ESP32標準ライブラリ（Arduino IDEに含まれています）
- **PubSubClient**: MQTTクライアントライブラリ
  ```bash
  arduino-cli lib install "PubSubClient"
  ```

## 🚀 セットアップ

1. **ボード設定**
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

2. **WiFi設定とMQTTブローカー設定**
   - `secrets.h.example`をコピーして`secrets.h`を作成してください：
   ```bash
   cp secrets.h.example secrets.h
   ```
   - `secrets.h`ファイル内の`ssid`、`password`、`mqtt_server`を実際の値に編集してください：
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   const char* mqtt_server = "mqtt.m5stack.com";
   ```
   - **重要**: `secrets.h`は`.gitignore`に追加されているため、Gitにコミットされません。認証情報が漏れる心配はありません。

4. **プログラムのアップロード**
   - Arduino IDEまたはArduino CLIでプログラムをアップロード

## 💻 使用方法

1. WiFiのSSIDとパスワードを設定
2. MQTTブローカーのアドレスを設定
3. プログラムをアップロード
4. デバイスがWiFiネットワークに接続し、MQTTブローカーに接続します
5. トピックの購読と発行が可能になります

## 📊 機能

- **MQTT接続**: MQTTブローカーへの接続
- **トピック購読**: 指定したトピックのメッセージを受信
- **トピック発行**: 指定したトピックにメッセージを送信
- **自動再接続**: 接続が切れた場合の自動再接続機能
- **コールバック処理**: 受信したメッセージの処理

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - WiFi接続
   - MQTTクライアントの設定

2. **MQTT接続**
   - MQTTブローカーに接続
   - トピックを購読
   - コールバック関数を設定

3. **メッセージ処理**
   - 受信したメッセージを処理
   - 画面とシリアルモニターに表示
   - 必要に応じてメッセージを発行

## 📝 コードの主要なポイント

### MQTTクライアントの初期化
```cpp
WiFiClient espClient;
PubSubClient client(espClient);
client.setServer(mqtt_server, 1883);
client.setCallback(callback);
```

### コールバック関数
```cpp
void callback(char* topic, byte* payload, unsigned int length) {
    // 受信したメッセージを処理
}
```

### トピックの購読
```cpp
client.subscribe("topic_name");
```

### メッセージの発行
```cpp
client.publish("topic_name", "message");
```

## 🔧 トラブルシューティング

### 接続できない場合
- WiFiのSSIDとパスワードが正しいか確認してください
- MQTTブローカーのアドレスが正しいか確認してください
- ポート番号（デフォルト: 1883）が正しいか確認してください
- シリアルモニターでエラーメッセージを確認してください

### メッセージが受信できない場合
- トピック名が正しいか確認してください
- MQTTブローカーが正常に動作しているか確認してください
- 購読設定が正しいか確認してください

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み

