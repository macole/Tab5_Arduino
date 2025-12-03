/*
 * M5Stack Tab5 BLE サーバーアプリ
 * Bluetooth Low Energyを使用してセンサーデータを送信するアプリケーション
 * 
 * 機能:
 * - BLEサーバーとして動作
 * - IMUデータ（加速度・ジャイロ）を送信
 * - タッチ情報を送信
 * - バッテリー情報を送信
 */

#include <Arduino.h>
#include <M5GFX.h>
#include <M5Unified.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// BLE UUID定義
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_IMU "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_TOUCH "beb5483f-36e1-4688-b7f5-ea07361b26a9"
#define CHARACTERISTIC_UUID_BATTERY "beb54840-36e1-4688-b7f5-ea07361b26aa"

// BLEオブジェクト
BLEServer* pServer = nullptr;
BLECharacteristic* pCharacteristicIMU = nullptr;
BLECharacteristic* pCharacteristicTouch = nullptr;
BLECharacteristic* pCharacteristicBattery = nullptr;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// データ送信間隔
unsigned long lastIMUSend = 0;
unsigned long lastTouchSend = 0;
unsigned long lastBatterySend = 0;
const unsigned long IMU_INTERVAL = 100;      // 100msごと
const unsigned long TOUCH_INTERVAL = 50;     // 50msごと
const unsigned long BATTERY_INTERVAL = 1000; // 1秒ごと

// BLEコールバッククラス
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("BLE Client Connected");
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("BLE Client Disconnected");
    }
};

void setup() {
    M5.begin();
    Serial.begin(115200);
    delay(1000);

    M5.Display.setRotation(3); // 横向き
    M5.Display.setFont(&fonts::lgfxJapanMinchoP_16);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(0, 0);
    M5.Display.println("BLE Server");
    M5.Display.println("Initializing...");

    // BLEデバイスの初期化
    BLEDevice::init("M5Stack Tab5");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // BLEサービスの作成
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // IMUデータ用のキャラクタリスティック
    pCharacteristicIMU = pService->createCharacteristic(
        CHARACTERISTIC_UUID_IMU,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristicIMU->addDescriptor(new BLE2902());

    // タッチデータ用のキャラクタリスティック
    pCharacteristicTouch = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TOUCH,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristicTouch->addDescriptor(new BLE2902());

    // バッテリーデータ用のキャラクタリスティック
    pCharacteristicBattery = pService->createCharacteristic(
        CHARACTERISTIC_UUID_BATTERY,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristicBattery->addDescriptor(new BLE2902());

    // サービスの開始
    pService->start();

    // アドバタイジングの開始
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();

    M5.Display.fillScreen(BLACK);
    M5.Display.setCursor(0, 0);
    M5.Display.println("BLE Server Ready!");
    M5.Display.println("Device Name:");
    M5.Display.println("M5Stack Tab5");
    M5.Display.println("");
    M5.Display.println("Waiting for");
    M5.Display.println("connection...");

    Serial.println("BLE Server Ready!");
    Serial.println("Waiting for client connection...");
}

void loop() {
    M5.update();

    // デバイス接続状態の管理
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // クライアントが切断処理を完了するのを待つ
        pServer->startAdvertising(); // 再接続を開始
        Serial.println("Start advertising");
        oldDeviceConnected = deviceConnected;
        
        M5.Display.fillScreen(BLACK);
        M5.Display.setCursor(0, 0);
        M5.Display.println("BLE Server Ready!");
        M5.Display.println("Waiting for");
        M5.Display.println("connection...");
    }

    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
        
        M5.Display.fillScreen(BLACK);
        M5.Display.setCursor(0, 0);
        M5.Display.setTextColor(GREEN, BLACK);
        M5.Display.println("BLE Connected!");
        M5.Display.setTextColor(WHITE, BLACK);
    }

    // IMUデータの送信
    if (deviceConnected && (millis() - lastIMUSend > IMU_INTERVAL)) {
        sendIMUData();
        lastIMUSend = millis();
    }

    // タッチデータの送信
    if (deviceConnected && (millis() - lastTouchSend > TOUCH_INTERVAL)) {
        sendTouchData();
        lastTouchSend = millis();
    }

    // バッテリーデータの送信
    if (deviceConnected && (millis() - lastBatterySend > BATTERY_INTERVAL)) {
        sendBatteryData();
        lastBatterySend = millis();
    }

    // 画面表示の更新
    displayStatus();

    delay(10);
}

void sendIMUData() {
    auto imu_update = M5.Imu.update();
    if (imu_update) {
        auto imu_data = M5.Imu.getImuData();
        
        // JSON形式でデータを送信
        char buffer[256];
        snprintf(buffer, sizeof(buffer),
            "{\"accel\":{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f},"
            "\"gyro\":{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f}}",
            imu_data.accel.x, imu_data.accel.y, imu_data.accel.z,
            imu_data.gyro.x, imu_data.gyro.y, imu_data.gyro.z
        );
        
        pCharacteristicIMU->setValue(buffer);
        pCharacteristicIMU->notify();
    }
}

void sendTouchData() {
    auto t = M5.Touch.getDetail();
    if (t.wasPressed() || t.wasReleased()) {
        char buffer[128];
        snprintf(buffer, sizeof(buffer),
            "{\"x\":%d,\"y\":%d,\"pressed\":%d,\"released\":%d}",
            t.x, t.y,
            t.wasPressed() ? 1 : 0,
            t.wasReleased() ? 1 : 0
        );
        
        pCharacteristicTouch->setValue(buffer);
        pCharacteristicTouch->notify();
    }
}

void sendBatteryData() {
    float voltage = M5.Power.getBatteryVoltage();
    uint8_t level = M5.Power.getBatteryLevel();
    bool isCharging = M5.Power.isCharging();
    
    char buffer[128];
    snprintf(buffer, sizeof(buffer),
        "{\"voltage\":%.2f,\"level\":%d,\"charging\":%d}",
        voltage, level, isCharging ? 1 : 0
    );
    
    pCharacteristicBattery->setValue(buffer);
    pCharacteristicBattery->notify();
}

void displayStatus() {
    static unsigned long lastDisplayUpdate = 0;
    if (millis() - lastDisplayUpdate < 500) return; // 500msごとに更新
    lastDisplayUpdate = millis();

    int y = M5.Display.height() - 100;
    M5.Display.fillRect(0, y, M5.Display.width(), 100, BLACK);
    M5.Display.setFont(&fonts::lgfxJapanMinchoP_12);
    M5.Display.setCursor(10, y);

    if (deviceConnected) {
        M5.Display.setTextColor(GREEN, BLACK);
        M5.Display.println("Status: Connected");
        
        // IMUデータ表示
        auto imu_data = M5.Imu.getImuData();
        M5.Display.setTextColor(WHITE, BLACK);
        M5.Display.printf("Accel: %.1f,%.1f,%.1f\n",
            imu_data.accel.x, imu_data.accel.y, imu_data.accel.z);
        
        // バッテリー情報表示
        float voltage = M5.Power.getBatteryVoltage();
        uint8_t level = M5.Power.getBatteryLevel();
        M5.Display.printf("Battery: %d%% (%.2fV)\n", level, voltage);
        
        // タッチ情報表示
        auto t = M5.Touch.getDetail();
        if (t.isPressed()) {
            M5.Display.setTextColor(CYAN, BLACK);
            M5.Display.printf("Touch: (%d, %d)\n", t.x, t.y);
        }
    } else {
        M5.Display.setTextColor(YELLOW, BLACK);
        M5.Display.println("Status: Waiting...");
        M5.Display.setTextColor(WHITE, BLACK);
        M5.Display.println("Scan for:");
        M5.Display.println("M5Stack Tab5");
    }
}

