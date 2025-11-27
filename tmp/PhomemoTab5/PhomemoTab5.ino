/*
 * Phomemo M02 Printer Library for Arduino (Tab5 compatible)
 * Based on: https://github.com/K0IN/Phomemo-M02-Web
 * Protocol analysis from: https://github.com/vivier/phomemo-tools
 * 
 * This library allows Arduino boards with Bluetooth capability to connect
 * and print to Phomemo M02 thermal printers.
 * 
 * Hardware Requirements:
 * - M5Stack Tab5
 * - Phomemo M02 printer
 * 
 * Features:
 * - Text printing with various font sizes
 * - Image printing (bitmap data)
 * - Printer control commands
 * - Connection management
 * - Display status on Tab5 screen
 */

#include <M5Unified.h>

#ifdef ESP32
  #include "BLEDevice.h"

// BLE Serial用のラッパークラス
class BLESerial : public Stream {
private:
  BLEServer* pServer;
  BLEService* pService;
  BLECharacteristic* pTxCharacteristic;
  BLECharacteristic* pRxCharacteristic;
  bool deviceConnected;
  bool oldDeviceConnected;
  String rxBuffer;
  
public:
  BLESerial() : pServer(nullptr), pService(nullptr), 
                pTxCharacteristic(nullptr), pRxCharacteristic(nullptr),
                deviceConnected(false), oldDeviceConnected(false) {}
  
  bool begin(const char* deviceName) {
    BLEDevice::init(deviceName);
    
    // BLEサーバー作成
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks(this));
    
    // サービス作成（UUID: 6E400001-B5A3-F393-E0A9-E50E24DCCA9E）
    pService = pServer->createService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    
    // TXキャラクタリスティック（送信用）
    pTxCharacteristic = pService->createCharacteristic(
      "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
      BLECharacteristic::PROPERTY_NOTIFY
    );
    pTxCharacteristic->addDescriptor(new BLE2902());
    
    // RXキャラクタリスティック（受信用）
    pRxCharacteristic = pService->createCharacteristic(
      "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
      BLECharacteristic::PROPERTY_WRITE
    );
    pRxCharacteristic->setCallbacks(new CharacteristicCallbacks(this));
    
    // サービス開始
    pService->start();
    
    // アドバタイズ開始
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();
    
    return true;
  }
  
  void update() {
    if (!deviceConnected && oldDeviceConnected) {
      delay(500);
      pServer->startAdvertising();
      oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
      oldDeviceConnected = deviceConnected;
    }
  }
  
  bool hasClient() {
    return deviceConnected;
  }
  
  virtual size_t write(uint8_t c) {
    if (deviceConnected && pTxCharacteristic) {
      uint8_t data[1] = {c};
      pTxCharacteristic->setValue(data, 1);
      pTxCharacteristic->notify();
      return 1;
    }
    return 0;
  }
  
  virtual size_t write(const uint8_t* buffer, size_t size) {
    if (deviceConnected && pTxCharacteristic) {
      pTxCharacteristic->setValue(buffer, size);
      pTxCharacteristic->notify();
      return size;
    }
    return 0;
  }
  
  virtual int available() {
    return rxBuffer.length();
  }
  
  virtual int read() {
    if (rxBuffer.length() > 0) {
      int c = rxBuffer.charAt(0);
      rxBuffer.remove(0, 1);
      return c;
    }
    return -1;
  }
  
  virtual int peek() {
    if (rxBuffer.length() > 0) {
      return rxBuffer.charAt(0);
    }
    return -1;
  }
  
  virtual void flush() {}
  
  void addToRxBuffer(const uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
      rxBuffer += (char)data[i];
    }
  }
  
  void setConnected(bool connected) {
    deviceConnected = connected;
  }
  
private:
  class ServerCallbacks : public BLEServerCallbacks {
    BLESerial* serial;
  public:
    ServerCallbacks(BLESerial* s) : serial(s) {}
    void onConnect(BLEServer* pServer) {
      serial->setConnected(true);
    }
    void onDisconnect(BLEServer* pServer) {
      serial->setConnected(false);
    }
  };
  
  class CharacteristicCallbacks : public BLECharacteristicCallbacks {
    BLESerial* serial;
  public:
    CharacteristicCallbacks(BLESerial* s) : serial(s) {}
    void onWrite(BLECharacteristic* pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      if (value.length() > 0) {
        serial->addToRxBuffer((uint8_t*)value.data(), value.length());
      }
    }
  };
};

BLESerial SerialBT;
#endif


// Phomemo M02 プロトコル定数
#define PHOMEMO_ESC 0x1B
#define PHOMEMO_GS  0x1D
#define PHOMEMO_US  0x1F

// プリンター設定
#define PRINT_WIDTH_MM     48    // 印刷幅 48mm
#define PRINT_WIDTH_PIXELS 384   // 384ピクセル (8ピクセル/mm)
#define PRINT_WIDTH_BYTES  48    // 48バイト (384/8)

class PhomemoPrinter {
private:
  Stream* _stream;
  bool _connected;
  
public:
  PhomemoPrinter() : _stream(nullptr), _connected(false) {}
  
  // 初期化
  bool begin(Stream* stream) {
    _stream = stream;
    if (_stream) {
      _connected = true;
      initialize();
      return true;
    }
    return false;
  }
  
  // プリンター初期化
  void initialize() {
    if (!_connected) return;
    
    // ESC @ : プリンター初期化
    uint8_t init_cmd[] = {PHOMEMO_ESC, 0x40};
    _stream->write(init_cmd, sizeof(init_cmd));
    
    // ESC a : 左寄せ
    uint8_t align_cmd[] = {PHOMEMO_ESC, 0x61, 0x00};
    _stream->write(align_cmd, sizeof(align_cmd));
    
    // 初期化コマンド
    uint8_t init_seq[] = {PHOMEMO_US, 0x11, 0x02, 0x04};
    _stream->write(init_seq, sizeof(init_seq));
    
    delay(100);
  }
  
  // 濃度設定 (1-5)
  void setDensity(uint8_t density) {
    if (!_connected || density < 1 || density > 5) return;
    
    uint8_t cmd[] = {PHOMEMO_ESC, 0x4E, 0x04, density};
    _stream->write(cmd, sizeof(cmd));
  }
  
  // テキスト印刷
  void printText(const char* text) {
    if (!_connected || !text) return;
    
    // テキストをそのまま送信
    _stream->print(text);
    
    // 改行
    uint8_t lf = 0x0A;
    _stream->write(&lf, 1);
  }
  
  // テキスト印刷（フォントサイズ指定）
  void printText(const char* text, uint8_t fontSize) {
    if (!_connected || !text) return;
    
    // フォントサイズ設定 (1=通常, 2=大きめ)
    if (fontSize == 2) {
      // 大きいフォント設定
      uint8_t large_font[] = {PHOMEMO_ESC, 0x21, 0x30};
      _stream->write(large_font, sizeof(large_font));
    } else {
      // 通常フォント設定
      uint8_t normal_font[] = {PHOMEMO_ESC, 0x21, 0x00};
      _stream->write(normal_font, sizeof(normal_font));
    }
    
    printText(text);
  }
  
  // 位置合わせ設定
  void setAlignment(uint8_t align) {
    if (!_connected) return;
    
    // 0: 左寄せ, 1: 中央, 2: 右寄せ
    if (align > 2) align = 0;
    
    uint8_t cmd[] = {PHOMEMO_ESC, 0x61, align};
    _stream->write(cmd, sizeof(cmd));
  }
  
  // ビットマップ画像印刷
  void printBitmap(const uint8_t* bitmap, uint16_t width, uint16_t height) {
    if (!_connected || !bitmap || width == 0 || height == 0) return;
    
    uint16_t bytes_per_line = (width + 7) / 8;
    
    // 画像を255行ずつに分割して送信（プロトコル制限）
    for (uint16_t start_line = 0; start_line < height; start_line += 255) {
      uint16_t lines_to_send = min(255, height - start_line);
      
      // ラスタービットマップ印刷コマンド
      uint8_t raster_cmd[] = {
        PHOMEMO_GS, 0x76, 0x30,      // GS v 0 コマンド
        0x00,                         // モード (通常)
        (uint8_t)(bytes_per_line & 0xFF),      // バイト数/行 (下位)
        (uint8_t)((bytes_per_line >> 8) & 0xFF), // バイト数/行 (上位)
        (uint8_t)(lines_to_send & 0xFF),       // 行数 (下位)
        (uint8_t)((lines_to_send >> 8) & 0xFF)  // 行数 (上位)
      };
      
      _stream->write(raster_cmd, sizeof(raster_cmd));
      
      // ビットマップデータ送信
      for (uint16_t line = 0; line < lines_to_send; line++) {
        uint16_t src_line = start_line + line;
        _stream->write(&bitmap[src_line * bytes_per_line], bytes_per_line);
      }
    }
  }
  
  // 改行送り
  void feedLines(uint8_t lines) {
    if (!_connected) return;
    
    uint8_t cmd[] = {PHOMEMO_ESC, 0x64, lines};
    _stream->write(cmd, sizeof(cmd));
  }
  
  // 紙送り
  void feedPaper() {
    if (!_connected) return;
    
    feedLines(3); // 3行送り
    
    // フッターコマンド
    uint8_t footer_cmds[] = {
      PHOMEMO_US, 0x11, 0x08,
      PHOMEMO_US, 0x11, 0x0E,
      PHOMEMO_US, 0x11, 0x07,
      PHOMEMO_US, 0x11, 0x09
    };
    
    _stream->write(footer_cmds, sizeof(footer_cmds));
  }
  
  // 接続状態確認
  bool isConnected() {
    return _connected;
  }
  
  // 切断
  void disconnect() {
    _connected = false;
    _stream = nullptr;
  }
};

// グローバルインスタンス
PhomemoPrinter printer;

// ディスプレイ表示用関数
void displayStatus(const char* message, uint16_t color = TFT_WHITE) {
  M5.Display.setTextColor(color);
  M5.Display.println(message);
  Serial.println(message);
}

// String型を受け取るオーバーロード
void displayStatus(const String& message, uint16_t color = TFT_WHITE) {
  displayStatus(message.c_str(), color);
}

void displayClear() {
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextColor(TFT_WHITE);
  M5.Display.setTextSize(2);
}

void setup() {
  // M5Unified初期化
  auto cfg = M5.config();
  M5.begin(cfg);
  
  // ディスプレイ初期化
  displayClear();
  displayStatus("Phomemo M02 Printer");
  displayStatus("for Tab5");
  displayStatus("");
  
  Serial.begin(115200);
  delay(500);
  
  displayStatus("Initializing...", TFT_YELLOW);
  
#ifdef ESP32
  // BLE初期化
  if (!SerialBT.begin("Tab5_Printer")) {
    displayStatus("BLE Init Failed!", TFT_RED);
    Serial.println("BLE initialization failed!");
    return;
  }
  
  displayStatus("BLE Ready", TFT_GREEN);
  displayStatus("Device: Tab5_Printer", TFT_CYAN);
  displayStatus("Waiting for connection...", TFT_YELLOW);
  
  Serial.println("BLE initialized. Please connect with 'Tab5_Printer'");
  
  // 接続待機（最大30秒）
  unsigned long startTime = millis();
  bool connected = false;
  while (!SerialBT.hasClient() && (millis() - startTime < 30000)) {
    SerialBT.update();
    delay(500);
    Serial.print(".");
    M5.Display.print(".");
  }
  
  if (SerialBT.hasClient()) {
    displayStatus("");
    displayStatus("Connected!", TFT_GREEN);
    Serial.println("\nBLE connected!");
    
    // プリンター初期化（BLE Serial使用）
    if (printer.begin(&SerialBT)) {
      displayStatus("Printer Ready", TFT_GREEN);
      Serial.println("Printer initialized successfully!");
    } else {
      displayStatus("Printer Init Failed", TFT_RED);
      Serial.println("Failed to initialize printer");
    }
  } else {
    displayStatus("");
    displayStatus("Connection Timeout", TFT_RED);
    Serial.println("\nBLE connection timeout!");
  }
#else
  displayStatus("ESP32 required!", TFT_RED);
  Serial.println("This code requires ESP32!");
  return;
#endif
  
  // 初期設定
  if (printer.isConnected()) {
    printer.setDensity(3);  // 濃度レベル3
    printer.setAlignment(0); // 左寄せ
    displayStatus("Density: 3", TFT_CYAN);
    displayStatus("Alignment: Left", TFT_CYAN);
  }
  
  delay(1000);
  
  // テスト印刷
  if (printer.isConnected()) {
    displayStatus("");
    displayStatus("Starting test print...", TFT_YELLOW);
    testPrint();
  }
  
  displayStatus("");
  displayStatus("Ready for commands", TFT_GREEN);
  displayStatus("Type 'help' for commands", TFT_CYAN);
}

void loop() {
  M5.update(); // M5Unifiedの更新処理
  
#ifdef ESP32
  // BLE接続状態の更新
  SerialBT.update();
  
  // BLE接続状態の確認と表示
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 5000) { // 5秒ごとに確認
    lastCheck = millis();
    bool bleConnected = SerialBT.hasClient();
    if (bleConnected != printer.isConnected()) {
      if (bleConnected) {
        displayStatus("BLE Reconnected", TFT_GREEN);
        printer.begin(&SerialBT);
      } else {
        displayStatus("BLE Disconnected", TFT_RED);
        printer.disconnect();
      }
    }
  }
#endif
  
  // シリアルコマンド処理
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    processCommand(command);
  }
  
  delay(100);
}

void testPrint() {
  Serial.println("Printing test page...");
  
  printer.setAlignment(1); // 中央寄せ
  printer.printText("=== Test Print ===", 2);
  
  printer.setAlignment(0); // 左寄せ
  printer.printText("Tab5 Phomemo M02 Test");
  printer.printText("Date: 2024/11/15");
  printer.printText("");
  printer.printText("Normal font test");
  printer.printText("Large font test", 2);
  printer.printText("");
  
  // 簡単なビットマップパターンテスト
  uint8_t test_pattern[24] = {
    0xFF, 0x81, 0x81,  // ■□□...□■
    0x81, 0x81, 0x81,
    0x81, 0x81, 0x81,
    0x81, 0x81, 0x81,
    0x81, 0x81, 0x81,
    0x81, 0x81, 0x81,
    0x81, 0x81, 0x81,
    0xFF, 0xFF, 0xFF   // ■■■...■■
  };
  
  printer.printText("Bitmap test:");
  printer.printBitmap(test_pattern, 24, 8);
  
  printer.feedPaper();
  
  Serial.println("Test print completed!");
}

void processCommand(const String& command) {
  if (command.startsWith("print:")) {
    String text = command.substring(6);
    if (printer.isConnected()) {
      printer.printText(text.c_str());
      displayStatus("Printed: " + text, TFT_GREEN);
      Serial.println("Printed: " + text);
    } else {
      displayStatus("Not connected!", TFT_RED);
      Serial.println("Printer not connected!");
    }
    
  } else if (command.startsWith("printlarge:")) {
    String text = command.substring(11);
    if (printer.isConnected()) {
      printer.printText(text.c_str(), 2);
      displayStatus("Printed large: " + text, TFT_GREEN);
      Serial.println("Printed large: " + text);
    } else {
      displayStatus("Not connected!", TFT_RED);
      Serial.println("Printer not connected!");
    }
    
  } else if (command.startsWith("align:")) {
    int align = command.substring(6).toInt();
    if (align >= 0 && align <= 2) {
      printer.setAlignment(align);
      const char* alignNames[] = {"Left", "Center", "Right"};
      displayStatus("Alignment: " + String(alignNames[align]), TFT_CYAN);
      Serial.println("Alignment set to: " + String(align));
    }
    
  } else if (command.startsWith("density:")) {
    int density = command.substring(8).toInt();
    if (density >= 1 && density <= 5) {
      printer.setDensity(density);
      displayStatus("Density: " + String(density), TFT_CYAN);
      Serial.println("Density set to: " + String(density));
    }
    
  } else if (command == "feed") {
    if (printer.isConnected()) {
      printer.feedPaper();
      displayStatus("Paper fed", TFT_GREEN);
      Serial.println("Paper fed");
    } else {
      displayStatus("Not connected!", TFT_RED);
      Serial.println("Printer not connected!");
    }
    
  } else if (command == "test") {
    if (printer.isConnected()) {
      displayStatus("Printing test...", TFT_YELLOW);
      testPrint();
    } else {
      displayStatus("Not connected!", TFT_RED);
      Serial.println("Printer not connected!");
    }
    
  } else if (command == "help") {
    displayClear();
    displayStatus("Available commands:", TFT_WHITE);
    displayStatus("  print:<text>", TFT_CYAN);
    displayStatus("  printlarge:<text>", TFT_CYAN);
    displayStatus("  align:<0-2>", TFT_CYAN);
    displayStatus("  density:<1-5>", TFT_CYAN);
    displayStatus("  feed", TFT_CYAN);
    displayStatus("  test", TFT_CYAN);
    displayStatus("  help", TFT_CYAN);
    
    Serial.println("Available commands:");
    Serial.println("  print:<text>      - Print text");
    Serial.println("  printlarge:<text> - Print large text");
    Serial.println("  align:<0-2>       - Set alignment (0=left,1=center,2=right)");
    Serial.println("  density:<1-5>     - Set print density");
    Serial.println("  feed              - Feed paper");
    Serial.println("  test              - Print test page");
    Serial.println("  help              - Show this help");
    
  } else {
    displayStatus("Unknown command", TFT_RED);
    displayStatus("Type 'help'", TFT_YELLOW);
    Serial.println("Unknown command. Type 'help' for available commands.");
  }
}

/* 
 * 使用方法:
 * 
 * 1. ハードウェアセットアップ:
 *    - M5Stack Tab5を使用
 *    - 内蔵Bluetoothを使用してPhomemo M02プリンターに接続
 * 
 * 2. プリンター接続:
 *    - Tab5起動後、"Tab5_Printer"でBLE検索
 *    - Phomemo M02プリンターと接続（BLE対応が必要）
 *    - 接続状態はディスプレイに表示されます
 * 
 * 3. シリアルコマンド使用例:
 *    print:Hello World     - テキスト印刷
 *    printlarge:Title      - 大きなフォントで印刷
 *    align:1               - 中央寄せ
 *    density:4             - 濃度レベル4
 *    feed                  - 紙送り
 *    test                  - テスト印刷
 *    help                  - コマンド一覧表示
 * 
 * 4. ディスプレイ表示:
 *    - 接続状態、コマンド実行結果が表示されます
 *    - 色分けで状態を視覚的に確認できます
 * 
 * 5. カスタマイズ:
 *    - processCommand()関数で独自コマンド追加
 *    - printBitmap()でオリジナル画像印刷
 *    - 日本語対応は文字コード変換が必要
 */