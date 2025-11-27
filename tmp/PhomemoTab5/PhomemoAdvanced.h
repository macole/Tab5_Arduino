/*
 * Phomemo M02 Advanced Library Header
 * Tab5対応版 - より詳細な機能を含む拡張ライブラリ
 */

#ifndef PHOMEMO_ADVANCED_H
#define PHOMEMO_ADVANCED_H

#include <Arduino.h>

// プロトコル定数
#define PHOMEMO_ESC 0x1B
#define PHOMEMO_GS  0x1D
#define PHOMEMO_US  0x1F
#define PHOMEMO_NAK 0x15

// プリンター仕様
#define PHOMEMO_M02_WIDTH_PIXELS 384
#define PHOMEMO_M02_WIDTH_BYTES  48
#define PHOMEMO_M02_MAX_LINES    255

// フォントサイズ
enum PhomemoFontSize {
  FONT_SMALL = 0,
  FONT_MEDIUM = 1,
  FONT_LARGE = 2,
  FONT_EXTRA_LARGE = 3
};

// 位置合わせ
enum PhomemoAlignment {
  ALIGN_LEFT = 0,
  ALIGN_CENTER = 1,
  ALIGN_RIGHT = 2
};

// プリンター状態
enum PhomemoPrinterState {
  STATE_UNKNOWN = 0,
  STATE_READY = 1,
  STATE_PAPER_EMPTY = 2,
  STATE_ERROR = 3
};

class PhomemoAdvanced {
private:
  Stream* _stream;
  bool _connected;
  uint8_t _current_density;
  uint8_t _current_alignment;
  PhomemoFontSize _current_font_size;
  
  // 内部メソッド
  void sendCommand(const uint8_t* cmd, size_t len);
  void sendESCCommand(uint8_t cmd, uint8_t param = 0);
  void sendGSCommand(uint8_t cmd, uint8_t param = 0);
  void sendUSCommand(uint8_t cmd, uint8_t param = 0);
  uint8_t calculateChecksum(const uint8_t* data, size_t len);
  
public:
  PhomemoAdvanced();
  
  // 基本操作
  bool begin(Stream* stream);
  void end();
  bool isConnected();
  
  // 初期化・設定
  void initialize();
  void reset();
  
  // プリンター設定
  void setDensity(uint8_t density);        // 1-5
  void setAlignment(PhomemoAlignment align);
  void setFontSize(PhomemoFontSize size);
  void setLineSpacing(uint8_t spacing);
  
  // プリンター情報取得
  uint32_t getSerialNumber();
  String getFirmwareVersion();
  uint16_t getBatteryLevel();
  PhomemoPrinterState getPrinterState();
  
  // テキスト印刷
  void print(const char* text);
  void println(const char* text = "");
  void printf(const char* format, ...);
  
  // フォーマット印刷
  void printCentered(const char* text);
  void printRight(const char* text);
  void printBold(const char* text);
  void printUnderline(const char* text);
  
  // 特殊印刷
  void printLine(char character = '-', uint8_t length = 32);
  void printQRCode(const char* data, uint8_t size = 3);
  void printBarcode(const char* data, uint8_t type = 0);
  
  // ビットマップ印刷
  void printBitmap(const uint8_t* bitmap, uint16_t width, uint16_t height);
  void printBitmapFromSD(const char* filename);
  
  // 紙送り・カット
  void feed(uint8_t lines = 1);
  void feedAndCut();
  void feedMillimeters(uint8_t mm);
  
  // 特殊機能
  void beep(uint8_t times = 1);
  void setAutoSleep(uint16_t minutes);
  void wakeup();
  
  // デバッグ
  void enableDebug(bool enable = true);
  void printStatus();
};

// 日本語文字列変換ヘルパー
class PhomemoJapanese {
public:
  static String utf8ToShiftJIS(const String& utf8_str);
  static void printJapanese(PhomemoAdvanced& printer, const String& japanese_text);
  static void createJapaneseCharBitmap(uint16_t unicode, uint8_t* bitmap);
};

// ビットマップ操作ヘルパー
class PhomemoBitmap {
public:
  static void createTextBitmap(const char* text, uint8_t* bitmap, uint16_t* width, uint16_t* height);
  static void invertBitmap(uint8_t* bitmap, uint16_t width, uint16_t height);
  static void rotateBitmap(uint8_t* src, uint8_t* dst, uint16_t width, uint16_t height, int angle);
  static void resizeBitmap(uint8_t* src, uint8_t* dst, uint16_t src_w, uint16_t src_h, uint16_t dst_w, uint16_t dst_h);
};

// QRコード生成ヘルパー
class PhomemoQR {
private:
  static const uint8_t QR_VERSION_1_SIZE = 21;
  static const uint8_t QR_VERSION_2_SIZE = 25;
  
public:
  static bool generateQR(const char* data, uint8_t* qr_bitmap, uint8_t* size, uint8_t version = 1);
  static void printQR(PhomemoAdvanced& printer, const char* data, uint8_t scale = 3);
};

// プリセット印刷テンプレート
class PhomemoTemplates {
public:
  static void printReceipt(PhomemoAdvanced& printer, const char* shop_name, const char* items[], float prices[], int item_count);
  static void printLabel(PhomemoAdvanced& printer, const char* title, const char* content);
  static void printBusinessCard(PhomemoAdvanced& printer, const char* name, const char* company, const char* phone);
  static void printBarcode(PhomemoAdvanced& printer, const char* code, const char* description);
};

#endif // PHOMEMO_ADVANCED_H