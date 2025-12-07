# BLE - Bluetooth Low Energy サーバーアプリ

M5Stack Tab5のBLE機能を使用して、センサーデータを送信するサーバーアプリケーションです。

## 📋 概要

このプログラムは、M5Stack Tab5をBLEサーバー（Peripheral）として動作させ、接続したクライアントデバイスに以下のデータを送信します：

- **IMUデータ**: 加速度センサーとジャイロセンサーのデータ
- **タッチ情報**: タッチスクリーンの座標と状態
- **バッテリー情報**: バッテリー電圧、充電レベル、充電状態

## 🔧 必要なハードウェア

- M5Stack Tab5

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```

- **BLEライブラリ**: ESP32標準ライブラリ（Arduino IDEに含まれています）
  - `BLEDevice.h`
  - `BLEServer.h`
  - `BLEUtils.h`
  - `BLE2902.h`

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

### 1. プログラムの起動

1. プログラムをアップロードすると、自動的にBLEサーバーが起動します
2. デバイス名「M5Stack Tab5」でアドバタイジングを開始します
3. 画面に「Waiting for connection...」と表示されます

### 2. クライアントデバイスからの接続

スマートフォンやPCなどのBLEクライアントから接続できます：

#### iOS/Androidアプリ
- **LightBlue** (iOS/Android): BLEデバイスのスキャンと接続が可能
- **nRF Connect** (iOS/Android): Nordic Semiconductor製のBLEツール
- **BLE Scanner** (Android): BLEデバイスのスキャンと接続が可能

#### PCアプリ
- **Bluetooth LE Explorer** (Windows 10/11): Windows標準ツール
- **Bluetooth Explorer** (macOS): macOS標準ツール
- **GATT Tool** (Linux): BlueZパッケージに含まれるコマンドラインツール

### 3. データの受信

接続後、以下の3つのキャラクタリスティックからデータを受信できます：

| UUID | 説明 | データ形式 |
|------|------|-----------|
| `beb5483e-36e1-4688-b7f5-ea07361b26a8` | IMUデータ | JSON形式 |
| `beb5483f-36e1-4688-b7f5-ea07361b26a9` | タッチ情報 | JSON形式 |
| `beb54840-36e1-4688-b7f5-ea07361b26aa` | バッテリー情報 | JSON形式 |

## 📊 機能

- **BLEサーバー**: Peripheralモードで動作
- **IMUデータ送信**: 加速度・ジャイロデータを100msごとに送信
- **タッチ情報送信**: タッチイベントを50msごとに送信
- **バッテリー情報送信**: バッテリー状態を1秒ごとに送信
- **自動再接続**: クライアント切断後、自動的にアドバタイジングを再開

## 🔍 プログラムの動作

### 1. 初期化
- M5Stackの初期化
- BLEデバイスの初期化
- BLEサービスの作成
- キャラクタリスティックの作成
- アドバタイジングの開始

### 2. メインループ
- クライアント接続状態の監視
- IMUデータの送信（100ms間隔）
- タッチ情報の送信（50ms間隔）
- バッテリー情報の送信（1秒間隔）
- 画面表示の更新

### 3. データ送信
- JSON形式でデータを送信
- BLE Notifyを使用してリアルタイムにデータを送信

## 📝 データ形式

### IMUデータ
```json
{
  "accel": {
    "x": 0.12,
    "y": -0.05,
    "z": 0.98
  },
  "gyro": {
    "x": 0.01,
    "y": -0.02,
    "z": 0.00
  }
}
```

### タッチ情報
```json
{
  "x": 320,
  "y": 240,
  "pressed": 1,
  "released": 0
}
```

### バッテリー情報
```json
{
  "voltage": 4.15,
  "level": 85,
  "charging": 0
}
```

## 📝 コードの主要なポイント

### BLEデバイスの初期化
```cpp
BLEDevice::init("M5Stack Tab5");
pServer = BLEDevice::createServer();
pServer->setCallbacks(new MyServerCallbacks());
```

### サービスの作成
```cpp
BLEService *pService = pServer->createService(SERVICE_UUID);
```

### キャラクタリスティックの作成
```cpp
pCharacteristicIMU = pService->createCharacteristic(
    CHARACTERISTIC_UUID_IMU,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
);
pCharacteristicIMU->addDescriptor(new BLE2902());
```

### データの送信
```cpp
pCharacteristicIMU->setValue(buffer);
pCharacteristicIMU->notify();
```

### 接続状態の管理
```cpp
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    }
    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};
```

## ⚙️ カスタマイズ

### 送信間隔の変更
```cpp
const unsigned long IMU_INTERVAL = 100;      // IMUデータ送信間隔（ms）
const unsigned long TOUCH_INTERVAL = 50;     // タッチデータ送信間隔（ms）
const unsigned long BATTERY_INTERVAL = 1000; // バッテリーデータ送信間隔（ms）
```

### デバイス名の変更
```cpp
BLEDevice::init("Your Device Name");
```

### UUIDの変更
```cpp
#define SERVICE_UUID        "your-service-uuid"
#define CHARACTERISTIC_UUID_IMU "your-characteristic-uuid"
```

### 追加データの送信
新しいキャラクタリスティックを追加することで、他のセンサーデータも送信できます：

```cpp
// 新しいキャラクタリスティックの作成
BLECharacteristic* pCharacteristicNew = pService->createCharacteristic(
    "new-uuid",
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY
);
pCharacteristicNew->addDescriptor(new BLE2902());

// データの送信
pCharacteristicNew->setValue(data);
pCharacteristicNew->notify();
```

## 🔧 トラブルシューティング

### BLEデバイスが見つからない場合
- プログラムが正しくアップロードされているか確認してください
- シリアルモニターで「BLE Server Ready!」が表示されているか確認してください
- スマートフォンやPCのBluetoothが有効になっているか確認してください
- 他のBLEデバイスとの干渉がないか確認してください

### 接続できない場合
- デバイスがアドバタイジング中か確認してください（画面に「Waiting for connection...」と表示）
- BLEクライアントアプリでデバイスをスキャンしてください
- デバイス名「M5Stack Tab5」を探してください

### データが受信できない場合
- キャラクタリスティックのUUIDが正しいか確認してください
- Notifyを有効にしているか確認してください（LightBlueなどで設定可能）
- 送信間隔が適切か確認してください（短すぎると接続が不安定になる場合があります）

### 接続が頻繁に切断される場合
- 送信間隔を長くしてみてください
- 送信するデータサイズを小さくしてみてください
- 電波干渉がないか確認してください

## 💡 使用例

### 例1: スマートフォンアプリでの受信
LightBlueアプリを使用する場合：

1. LightBlueアプリを起動
2. 「M5Stack Tab5」をスキャン
3. 接続をタップ
4. サービス「4fafc201-1fb5-459e-8fcc-c5c9c331914b」を選択
5. 各キャラクタリスティックで「Listen for notifications」を有効化
6. データがリアルタイムで表示されます

### 例2: Pythonスクリプトでの受信
`bleak`ライブラリを使用する場合：

```python
import asyncio
from bleak import BleakClient

SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
CHAR_UUID_IMU = "beb5483e-36e1-4688-b7f5-ea07361b26a8"

def notification_handler(sender, data):
    print(f"IMU Data: {data.decode('utf-8')}")

async def main():
    async with BleakClient("M5Stack Tab5") as client:
        await client.start_notify(CHAR_UUID_IMU, notification_handler)
        await asyncio.sleep(60.0)
        await client.stop_notify(CHAR_UUID_IMU)

asyncio.run(main())
```

## 📖 参考資料

- [ESP32 BLE Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_ble.html)
- [Arduino BLE Library](https://github.com/nkolban/ESP32_BLE_Arduino)
- [BLE GATT Specification](https://www.bluetooth.com/specifications/specs/core-specification/)

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年12月6日  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み

