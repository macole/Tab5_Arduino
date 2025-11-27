// 参考
// https://github.com/espressif/arduino-esp32/blob/master/libraries/BLE/examples/BLE_server/BLE_server.ino
 
#include <M5Unified.h>

// ESP32環境でのみBluetoothSerialを使用
#if defined(ESP32) || defined(ESP_PLATFORM)
  #include <BluetoothSerial.h>
  BluetoothSerial bts;
#else
  #error "This code requires ESP32 platform. Please select ESP32 board in Arduino IDE."
#endif

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial.begin(115200);
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE, BLACK); // Display text in white on black background
  M5.Display.setTextSize(1);
  M5.Display.setTextDatum(4);

#ifdef ESP32
  if (!bts.begin("MSR IoT Device")) {
    Serial.println("Bluetooth initialization failed!");
    M5.Display.println("BT Init Failed!");
  } else {
    Serial.println("Bluetooth initialized: MSR IoT Device");
    M5.Display.println("BT Ready");
  }
#else
  Serial.println("ESP32 required for Bluetooth!");
  M5.Display.println("ESP32 required!");
#endif
}

void loop() {
  int value = random(0, 1000);
  M5.Display.fillScreen(BLACK);
  M5.Display.drawNumber(value, 160, 120, 8);
  
#ifdef ESP32
  if (bts.hasClient()) {
    bts.println(value);
  }
#endif
  
  delay(1000);
}