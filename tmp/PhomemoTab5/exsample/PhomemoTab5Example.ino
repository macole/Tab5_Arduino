/*
 * Phomemo M02 for Tab5 - 使用例とセットアップガイド
 * 
 * このサンプルコードはTab5環境でPhomemo M02プリンターを使用する
 * 完全なサンプルプログラムです。
 */

#include "PhomemoAdvanced.h"

// ESP32使用時
#ifdef ESP32
  #include <BluetoothSerial.h>
  BluetoothSerial SerialBT;
#endif

// プリンターインスタンス
PhomemoAdvanced printer;

// 設定パラメータ
struct PrinterConfig {
  uint8_t density = 3;           // 印刷濃度 (1-5)
  PhomemoAlignment alignment = ALIGN_LEFT;  // 位置合わせ
  PhomemoFontSize font_size = FONT_SMALL;   // フォントサイズ
  bool debug_mode = true;        // デバッグモード
} config;

void setup() {
  Serial.begin(115200);
  Serial.println("=== Phomemo M02 for Tab5 ===");
  Serial.println("Starting printer initialization...");
  
  // デバッグモード設定
  printer.enableDebug(config.debug_mode);
  
#ifdef ESP32
  setupESP32Bluetooth();
#else
  setupSerialConnection();
#endif
  
  // プリンター設定適用
  applyPrinterConfig();
  
  // 接続確認
  if (printer.isConnected()) {
    Serial.println("Printer connected successfully!");
    
    // ステータス表示
    printer.printStatus();
    
    // 初期テスト印刷
    printWelcomeMessage();
    
    // メニュー表示
    showMenu();
  } else {
    Serial.println("Failed to connect to printer!");
  }
}

void loop() {
  // シリアルコマンド処理
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    processUserCommand(input);
  }
  
  delay(100);
}

#ifdef ESP32
void setupESP32Bluetooth() {
  Serial.println("Initializing ESP32 Bluetooth...");
  
  // Bluetooth初期化
  if (!SerialBT.begin("Tab5_PhomemoM02")) {
    Serial.println("Bluetooth initialization failed!");
    return;
  }
  
  Serial.println("Bluetooth ready. Device name: Tab5_PhomemoM02");
  Serial.println("Please pair your Phomemo M02 printer with this device.");
  
  // 接続待機
  Serial.println("Waiting for printer connection...");
  unsigned long start_time = millis();
  
  while (!SerialBT.hasClient() && (millis() - start_time < 30000)) {
    delay(500);
    Serial.print(".");
  }
  
  if (SerialBT.hasClient()) {
    Serial.println("\nBluetooth connection established!");
    
    // プリンター初期化
    if (printer.begin(&SerialBT)) {
      Serial.println("Printer initialized successfully!");
    } else {
      Serial.println("Printer initialization failed!");
    }
  } else {
    Serial.println("\nBluetooth connection timeout!");
  }
}
#endif

#ifndef ESP32
void setupSerialConnection() {
  Serial.println("Initializing Serial connection...");
  
  // シリアル接続セットアップ（ボード固有の設定）
  Serial1.begin(9600);
  delay(1000);
  
  if (printer.begin(&Serial1)) {
    Serial.println("Serial printer connection established!");
  } else {
    Serial.println("Serial printer connection failed!");
  }
}
#endif

void applyPrinterConfig() {
  if (!printer.isConnected()) return;
  
  Serial.println("Applying printer configuration...");
  
  printer.setDensity(config.density);
  printer.setAlignment(config.alignment);
  printer.setFontSize(config.font_size);
  
  Serial.printf("Configuration applied - Density: %d, Alignment: %d, Font: %d\n", 
                config.density, config.alignment, config.font_size);
}

void printWelcomeMessage() {
  if (!printer.isConnected()) return;
  
  Serial.println("Printing welcome message...");
  
  printer.setAlignment(ALIGN_CENTER);
  printer.setFontSize(FONT_LARGE);
  printer.println("=== Tab5 ===");
  printer.println("Phomemo M02");
  
  printer.setFontSize(FONT_MEDIUM);
  printer.println("Connection Test");
  
  printer.setAlignment(ALIGN_LEFT);
  printer.setFontSize(FONT_SMALL);
  printer.println("");
  printer.printf("Date: %s\n", __DATE__);
  printer.printf("Time: %s\n", __TIME__);
  printer.println("");
  
  // 簡単なグラフィックテスト
  printer.println("Graphics Test:");
  printer.printLine('=', 32);
  printer.printLine('-', 32);
  printer.printLine('*', 16);
  
  printer.feedAndCut();
  
  Serial.println("Welcome message printed!");
}

void showMenu() {
  Serial.println("\n=== Available Commands ===");
  Serial.println("Text Commands:");
  Serial.println("  text:<message>      - Print text");
  Serial.println("  large:<message>     - Print large text");
  Serial.println("  center:<message>    - Print centered text");
  Serial.println("  right:<message>     - Print right-aligned text");
  Serial.println("  bold:<message>      - Print bold text");
  Serial.println("  line:<char><num>    - Print line (e.g., line:-30)");
  Serial.println("");
  Serial.println("Settings:");
  Serial.println("  density:<1-5>       - Set print density");
  Serial.println("  align:<0-2>         - Set alignment (0=left,1=center,2=right)");
  Serial.println("  font:<0-3>          - Set font size (0=small,1=med,2=large,3=xl)");
  Serial.println("");
  Serial.println("Special Functions:");
  Serial.println("  qr:<data>           - Print QR code");
  Serial.println("  status              - Show printer status");
  Serial.println("  test                - Print test page");
  Serial.println("  receipt             - Print sample receipt");
  Serial.println("  label               - Print sample label");
  Serial.println("");
  Serial.println("Control:");
  Serial.println("  feed                - Feed paper");
  Serial.println("  cut                 - Feed and cut");
  Serial.println("  beep                - Beep printer");
  Serial.println("  reset               - Reset printer");
  Serial.println("  menu                - Show this menu");
  Serial.println("========================");
}

void processUserCommand(const String& command) {
  Serial.println("Processing: " + command);
  
  if (command.startsWith("text:")) {
    String text = command.substring(5);
    printer.println(text.c_str());
    Serial.println("✓ Text printed");
    
  } else if (command.startsWith("large:")) {
    String text = command.substring(6);
    printer.setFontSize(FONT_LARGE);
    printer.println(text.c_str());
    printer.setFontSize(config.font_size);
    Serial.println("✓ Large text printed");
    
  } else if (command.startsWith("center:")) {
    String text = command.substring(7);
    printer.printCentered(text.c_str());
    Serial.println("✓ Centered text printed");
    
  } else if (command.startsWith("right:")) {
    String text = command.substring(6);
    printer.printRight(text.c_str());
    Serial.println("✓ Right-aligned text printed");
    
  } else if (command.startsWith("bold:")) {
    String text = command.substring(5);
    printer.printBold(text.c_str());
    printer.println("");
    Serial.println("✓ Bold text printed");
    
  } else if (command.startsWith("line:")) {
    String params = command.substring(5);
    if (params.length() >= 2) {
      char lineChar = params.charAt(0);
      int count = params.substring(1).toInt();
      if (count > 0 && count <= 48) {
        printer.printLine(lineChar, count);
        Serial.println("✓ Line printed");
      } else {
        Serial.println("✗ Invalid line count (1-48)");
      }
    } else {
      Serial.println("✗ Invalid line format (use: line:<char><num>)");
    }
    
  } else if (command.startsWith("density:")) {
    int density = command.substring(8).toInt();
    if (density >= 1 && density <= 5) {
      config.density = density;
      printer.setDensity(density);
      Serial.println("✓ Density set to " + String(density));
    } else {
      Serial.println("✗ Invalid density (1-5)");
    }
    
  } else if (command.startsWith("align:")) {
    int align = command.substring(6).toInt();
    if (align >= 0 && align <= 2) {
      config.alignment = (PhomemoAlignment)align;
      printer.setAlignment(config.alignment);
      String alignText[] = {"left", "center", "right"};
      Serial.println("✓ Alignment set to " + alignText[align]);
    } else {
      Serial.println("✗ Invalid alignment (0-2)");
    }
    
  } else if (command.startsWith("font:")) {
    int font = command.substring(5).toInt();
    if (font >= 0 && font <= 3) {
      config.font_size = (PhomemoFontSize)font;
      printer.setFontSize(config.font_size);
      String fontText[] = {"small", "medium", "large", "extra-large"};
      Serial.println("✓ Font size set to " + fontText[font]);
    } else {
      Serial.println("✗ Invalid font size (0-3)");
    }
    
  } else if (command.startsWith("qr:")) {
    String data = command.substring(3);
    printer.println("QR Code:");
    printer.printQRCode(data.c_str(), 3);
    printer.println("Data: " + data);
    Serial.println("✓ QR code printed");
    
  } else if (command == "status") {
    printer.printStatus();
    Serial.println("✓ Status displayed");
    
  } else if (command == "test") {
    printTestPage();
    Serial.println("✓ Test page printed");
    
  } else if (command == "receipt") {
    printSampleReceipt();
    Serial.println("✓ Sample receipt printed");
    
  } else if (command == "label") {
    printSampleLabel();
    Serial.println("✓ Sample label printed");
    
  } else if (command == "feed") {
    printer.feed(3);
    Serial.println("✓ Paper fed");
    
  } else if (command == "cut") {
    printer.feedAndCut();
    Serial.println("✓ Paper cut");
    
  } else if (command == "beep") {
    printer.beep(2);
    Serial.println("✓ Beep sent");
    
  } else if (command == "reset") {
    printer.reset();
    applyPrinterConfig();
    Serial.println("✓ Printer reset");
    
  } else if (command == "menu") {
    showMenu();
    
  } else if (command.length() == 0) {
    // 空コマンド - 何もしない
    
  } else {
    Serial.println("✗ Unknown command: " + command);
    Serial.println("Type 'menu' for available commands");
  }
}

void printTestPage() {
  if (!printer.isConnected()) return;
  
  // テストページ印刷
  printer.setAlignment(ALIGN_CENTER);
  printer.setFontSize(FONT_LARGE);
  printer.println("=== TEST PAGE ===");
  
  printer.setAlignment(ALIGN_LEFT);
  printer.setFontSize(FONT_SMALL);
  printer.println("");
  
  // フォントサイズテスト
  printer.println("Font Size Test:");
  printer.setFontSize(FONT_SMALL);
  printer.println("Small font test");
  printer.setFontSize(FONT_MEDIUM);
  printer.println("Medium font test");
  printer.setFontSize(FONT_LARGE);
  printer.println("Large font test");
  
  printer.setFontSize(FONT_SMALL);
  printer.println("");
  
  // 位置合わせテスト
  printer.println("Alignment Test:");
  printer.setAlignment(ALIGN_LEFT);
  printer.println("Left aligned");
  printer.setAlignment(ALIGN_CENTER);
  printer.println("Center aligned");
  printer.setAlignment(ALIGN_RIGHT);
  printer.println("Right aligned");
  
  printer.setAlignment(ALIGN_LEFT);
  printer.println("");
  
  // 装飾テスト
  printer.println("Style Test:");
  printer.printBold("Bold text test");
  printer.println("");
  printer.printUnderline("Underline test");
  printer.println("");
  
  // ライン描画テスト
  printer.println("Line Test:");
  printer.printLine('=', 32);
  printer.printLine('-', 24);
  printer.printLine('*', 16);
  printer.printLine('.', 8);
  
  printer.feedAndCut();
}

void printSampleReceipt() {
  if (!printer.isConnected()) return;
  
  printer.setAlignment(ALIGN_CENTER);
  printer.setFontSize(FONT_MEDIUM);
  printer.println("TAB5 ELECTRONICS");
  printer.setFontSize(FONT_SMALL);
  printer.println("123-456-7890");
  printer.println("tab5@example.com");
  
  printer.printLine('=', 32);
  
  printer.setAlignment(ALIGN_LEFT);
  printer.println("Date: 2024/11/15 14:30");
  printer.println("Receipt #: R001234");
  printer.println("");
  
  // 商品リスト
  printer.printf("%-20s %8s\n", "Item", "Price");
  printer.printLine('-', 32);
  printer.printf("%-20s %8s\n", "Arduino Uno R3", "¥3,200");
  printer.printf("%-20s %8s\n", "Breadboard", "¥500");
  printer.printf("%-20s %8s\n", "Jumper Wires", "¥300");
  printer.printf("%-20s %8s\n", "LED Kit", "¥800");
  printer.printLine('-', 32);
  printer.printf("%-20s %8s\n", "Subtotal", "¥4,800");
  printer.printf("%-20s %8s\n", "Tax (10%)", "¥480");
  printer.printLine('=', 32);
  printer.setFontSize(FONT_MEDIUM);
  printer.printf("%-20s %8s\n", "Total", "¥5,280");
  
  printer.setFontSize(FONT_SMALL);
  printer.println("");
  printer.setAlignment(ALIGN_CENTER);
  printer.println("Thank you!");
  
  printer.feedAndCut();
}

void printSampleLabel() {
  if (!printer.isConnected()) return;
  
  printer.setAlignment(ALIGN_CENTER);
  printer.setFontSize(FONT_LARGE);
  printer.println("PRODUCT LABEL");
  
  printer.setFontSize(FONT_SMALL);
  printer.println("");
  
  printer.setAlignment(ALIGN_LEFT);
  printer.println("Product: Arduino Starter Kit");
  printer.println("SKU: ASK-001");
  printer.println("Price: ¥5,980");
  printer.println("");
  
  printer.println("Features:");
  printer.println("- Arduino Uno R3 included");
  printer.println("- 30+ components");
  printer.println("- Tutorial book");
  printer.println("- Beginner friendly");
  
  printer.println("");
  printer.printQRCode("ASK-001", 2);
  
  printer.feedAndCut();
}

/*
 * セットアップガイド:
 * 
 * 1. ハードウェア準備
 *    - ESP32開発ボード (推奨) または Arduino Nano 33 BLE
 *    - Phomemo M02プリンター
 *    - USB電源ケーブル
 * 
 * 2. Arduino IDE設定
 *    - ESP32ボードパッケージをインストール
 *    - このプログラムとライブラリファイルをコピー
 * 
 * 3. 接続手順
 *    a) ESP32をUSBでPCに接続
 *    b) プログラムをアップロード
 *    c) シリアルモニターを開く (115200 bps)
 *    d) Phomemo M02の電源をON
 *    e) スマホ/PCのBluetooth設定で "Tab5_PhomemoM02" を検索
 *    f) ペアリング実行
 *    g) 接続完了後、テストコマンド実行
 * 
 * 4. 使用方法
 *    - シリアルモニターからコマンド入力
 *    - 'menu' コマンドで全コマンド一覧表示
 *    - 'test' コマンドでテストページ印刷
 * 
 * 5. トラブルシューティング
 *    - 接続できない場合: プリンターを再起動してペアリング削除・再実行
 *    - 印刷が薄い場合: 'density:5' で濃度を上げる
 *    - 文字化けする場合: ASCII文字のみ使用する
 */