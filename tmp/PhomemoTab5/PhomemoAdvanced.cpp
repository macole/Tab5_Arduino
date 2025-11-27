/*
 * Phomemo M02 Advanced Library Implementation
 * Tab5対応版 - 拡張ライブラリの実装
 */

#include "PhomemoAdvanced.h"
#include <stdarg.h>

// デバッグフラグ
static bool debug_enabled = false;

PhomemoAdvanced::PhomemoAdvanced() 
  : _stream(nullptr), _connected(false), _current_density(3), 
    _current_alignment(ALIGN_LEFT), _current_font_size(FONT_SMALL) {
}

bool PhomemoAdvanced::begin(Stream* stream) {
  if (!stream) return false;
  
  _stream = stream;
  _connected = true;
  
  initialize();
  return true;
}

void PhomemoAdvanced::end() {
  _connected = false;
  _stream = nullptr;
}

bool PhomemoAdvanced::isConnected() {
  return _connected && _stream;
}

void PhomemoAdvanced::initialize() {
  if (!_connected) return;
  
  // プリンター初期化
  sendESCCommand(0x40); // ESC @
  delay(100);
  
  // 左寄せ設定
  sendESCCommand(0x61, 0x00); // ESC a 0
  
  // 初期化シーケンス
  uint8_t init_seq[] = {PHOMEMO_US, 0x11, 0x02, 0x04};
  sendCommand(init_seq, sizeof(init_seq));
  
  // デフォルト設定
  setDensity(3);
  setAlignment(ALIGN_LEFT);
  setFontSize(FONT_SMALL);
  
  delay(200);
  
  if (debug_enabled) {
    Serial.println("Phomemo printer initialized");
  }
}

void PhomemoAdvanced::reset() {
  if (!_connected) return;
  
  uint8_t reset_cmd[] = {PHOMEMO_ESC, 0x40, 0x02};
  sendCommand(reset_cmd, sizeof(reset_cmd));
  delay(500);
  initialize();
}

void PhomemoAdvanced::sendCommand(const uint8_t* cmd, size_t len) {
  if (!_connected || !cmd || len == 0) return;
  
  _stream->write(cmd, len);
  _stream->flush();
  
  if (debug_enabled) {
    Serial.print("Sent command: ");
    for (size_t i = 0; i < len; i++) {
      Serial.printf("0x%02X ", cmd[i]);
    }
    Serial.println();
  }
}

void PhomemoAdvanced::sendESCCommand(uint8_t cmd, uint8_t param) {
  uint8_t command[] = {PHOMEMO_ESC, cmd, param};
  sendCommand(command, param == 0 ? 2 : 3);
}

void PhomemoAdvanced::sendGSCommand(uint8_t cmd, uint8_t param) {
  uint8_t command[] = {PHOMEMO_GS, cmd, param};
  sendCommand(command, param == 0 ? 2 : 3);
}

void PhomemoAdvanced::sendUSCommand(uint8_t cmd, uint8_t param) {
  uint8_t command[] = {PHOMEMO_US, cmd, param};
  sendCommand(command, param == 0 ? 2 : 3);
}

void PhomemoAdvanced::setDensity(uint8_t density) {
  if (!_connected || density < 1 || density > 5) return;
  
  _current_density = density;
  uint8_t cmd[] = {PHOMEMO_ESC, 0x4E, 0x04, density};
  sendCommand(cmd, sizeof(cmd));
}

void PhomemoAdvanced::setAlignment(PhomemoAlignment align) {
  if (!_connected) return;
  
  _current_alignment = align;
  sendESCCommand(0x61, (uint8_t)align);
}

void PhomemoAdvanced::setFontSize(PhomemoFontSize size) {
  if (!_connected) return;
  
  _current_font_size = size;
  
  switch (size) {
    case FONT_SMALL:
      sendESCCommand(0x21, 0x00);
      break;
    case FONT_MEDIUM:
      sendESCCommand(0x21, 0x11);
      break;
    case FONT_LARGE:
      sendESCCommand(0x21, 0x30);
      break;
    case FONT_EXTRA_LARGE:
      sendESCCommand(0x21, 0x77);
      break;
  }
}

void PhomemoAdvanced::setLineSpacing(uint8_t spacing) {
  if (!_connected) return;
  
  uint8_t cmd[] = {PHOMEMO_ESC, 0x33, spacing};
  sendCommand(cmd, sizeof(cmd));
}

uint32_t PhomemoAdvanced::getSerialNumber() {
  if (!_connected) return 0;
  
  sendUSCommand(0x11, 0x13);
  delay(50);
  
  // 応答読み取り（実装要調整）
  if (_stream->available() >= 3) {
    uint8_t response[3];
    _stream->readBytes(response, 3);
    return (response[2] << 16) | (response[1] << 8) | response[0];
  }
  
  return 0;
}

String PhomemoAdvanced::getFirmwareVersion() {
  if (!_connected) return "";
  
  sendUSCommand(0x11, 0x07);
  delay(50);
  
  if (_stream->available() >= 5) {
    uint8_t response[5];
    _stream->readBytes(response, 5);
    return String(response[4]) + "." + String(response[3]) + "." + String(response[2]);
  }
  
  return "";
}

uint16_t PhomemoAdvanced::getBatteryLevel() {
  if (!_connected) return 0;
  
  sendUSCommand(0x11, 0x08);
  delay(50);
  
  if (_stream->available() >= 3) {
    uint8_t response[3];
    _stream->readBytes(response, 3);
    return (response[2] << 8) | response[1];
  }
  
  return 0;
}

PhomemoPrinterState PhomemoAdvanced::getPrinterState() {
  if (!_connected) return STATE_UNKNOWN;
  
  sendUSCommand(0x11, 0x11);
  delay(50);
  
  if (_stream->available() >= 3) {
    uint8_t response[3];
    _stream->readBytes(response, 3);
    
    uint8_t state = response[2];
    if (state == 0x00) return STATE_READY;
    if (state == 0x01) return STATE_PAPER_EMPTY;
    return STATE_ERROR;
  }
  
  return STATE_UNKNOWN;
}

void PhomemoAdvanced::print(const char* text) {
  if (!_connected || !text) return;
  
  _stream->print(text);
}

void PhomemoAdvanced::println(const char* text) {
  if (!_connected) return;
  
  if (text) {
    _stream->print(text);
  }
  _stream->write(0x0A); // LF
}

void PhomemoAdvanced::printf(const char* format, ...) {
  if (!_connected || !format) return;
  
  va_list args;
  va_start(args, format);
  
  char buffer[256];
  vsnprintf(buffer, sizeof(buffer), format, args);
  print(buffer);
  
  va_end(args);
}

void PhomemoAdvanced::printCentered(const char* text) {
  if (!text) return;
  
  PhomemoAlignment old_align = (PhomemoAlignment)_current_alignment;
  setAlignment(ALIGN_CENTER);
  println(text);
  setAlignment(old_align);
}

void PhomemoAdvanced::printRight(const char* text) {
  if (!text) return;
  
  PhomemoAlignment old_align = (PhomemoAlignment)_current_alignment;
  setAlignment(ALIGN_RIGHT);
  println(text);
  setAlignment(old_align);
}

void PhomemoAdvanced::printBold(const char* text) {
  if (!_connected || !text) return;
  
  sendESCCommand(0x45, 1); // 太字ON
  print(text);
  sendESCCommand(0x45, 0); // 太字OFF
}

void PhomemoAdvanced::printUnderline(const char* text) {
  if (!_connected || !text) return;
  
  sendESCCommand(0x2D, 1); // アンダーラインON
  print(text);
  sendESCCommand(0x2D, 0); // アンダーラインOFF
}

void PhomemoAdvanced::printLine(char character, uint8_t length) {
  if (!_connected || length == 0) return;
  
  for (uint8_t i = 0; i < length; i++) {
    _stream->write(character);
  }
  println();
}

void PhomemoAdvanced::printBitmap(const uint8_t* bitmap, uint16_t width, uint16_t height) {
  if (!_connected || !bitmap || width == 0 || height == 0) return;
  
  uint16_t bytes_per_line = (width + 7) / 8;
  
  // 画像を255行ずつ分割
  for (uint16_t start_line = 0; start_line < height; start_line += PHOMEMO_M02_MAX_LINES) {
    uint16_t lines_to_send = min(PHOMEMO_M02_MAX_LINES, height - start_line);
    
    // ラスタービットマップコマンド
    uint8_t raster_cmd[] = {
      PHOMEMO_GS, 0x76, 0x30, 0x00,  // GS v 0 mode
      (uint8_t)(bytes_per_line & 0xFF),
      (uint8_t)((bytes_per_line >> 8) & 0xFF),
      (uint8_t)(lines_to_send & 0xFF),
      (uint8_t)((lines_to_send >> 8) & 0xFF)
    };
    
    sendCommand(raster_cmd, sizeof(raster_cmd));
    
    // ビットマップデータ送信
    for (uint16_t line = 0; line < lines_to_send; line++) {
      uint16_t src_line = start_line + line;
      const uint8_t* line_data = &bitmap[src_line * bytes_per_line];
      sendCommand(line_data, bytes_per_line);
    }
  }
}

void PhomemoAdvanced::feed(uint8_t lines) {
  if (!_connected || lines == 0) return;
  
  sendESCCommand(0x64, lines);
}

void PhomemoAdvanced::feedAndCut() {
  if (!_connected) return;
  
  feed(3);
  
  // フッターシーケンス
  uint8_t footer[] = {
    PHOMEMO_US, 0x11, 0x08,
    PHOMEMO_US, 0x11, 0x0E,
    PHOMEMO_US, 0x11, 0x07,
    PHOMEMO_US, 0x11, 0x09
  };
  
  sendCommand(footer, sizeof(footer));
}

void PhomemoAdvanced::feedMillimeters(uint8_t mm) {
  if (!_connected || mm == 0) return;
  
  // 1mm = 約8ドット
  uint8_t dots = mm * 8;
  sendESCCommand(0x4A, dots);
}

void PhomemoAdvanced::beep(uint8_t times) {
  if (!_connected || times == 0) return;
  
  for (uint8_t i = 0; i < times; i++) {
    sendESCCommand(0x42, 0x01);
    delay(200);
  }
}

void PhomemoAdvanced::enableDebug(bool enable) {
  debug_enabled = enable;
}

void PhomemoAdvanced::printStatus() {
  if (!_connected) return;
  
  Serial.println("=== Phomemo Printer Status ===");
  Serial.printf("Connected: %s\n", isConnected() ? "Yes" : "No");
  Serial.printf("Density: %d\n", _current_density);
  Serial.printf("Alignment: %d\n", _current_alignment);
  Serial.printf("Font Size: %d\n", _current_font_size);
  
  uint32_t serial = getSerialNumber();
  if (serial != 0) {
    Serial.printf("Serial Number: %lu\n", serial);
  }
  
  String firmware = getFirmwareVersion();
  if (firmware.length() > 0) {
    Serial.printf("Firmware: %s\n", firmware.c_str());
  }
  
  uint16_t battery = getBatteryLevel();
  if (battery != 0) {
    Serial.printf("Battery: %d\n", battery);
  }
  
  PhomemoPrinterState state = getPrinterState();
  Serial.printf("State: %d\n", state);
  
  Serial.println("===============================");
}

// 簡易QRコード生成（デモ用）
void PhomemoAdvanced::printQRCode(const char* data, uint8_t size) {
  if (!_connected || !data || size == 0) return;
  
  // 簡易QRコードパターン（実際のQRコード生成は別途実装が必要）
  uint8_t qr_pattern[] = {
    0xFF, 0x81, 0xBD, 0xA5, 0xBD, 0x81, 0xFF,
    0x81, 0x00, 0x24, 0x42, 0x24, 0x00, 0x81,
    0xBD, 0x24, 0x99, 0x5A, 0x99, 0x24, 0xBD,
    0xA5, 0x42, 0x5A, 0x00, 0x5A, 0x42, 0xA5,
    0xBD, 0x99, 0x5A, 0x99, 0x5A, 0x99, 0xBD,
    0x81, 0x00, 0x42, 0x24, 0x42, 0x00, 0x81,
    0xFF, 0x81, 0xBD, 0xA5, 0xBD, 0x81, 0xFF
  };
  
  // スケール調整して印刷
  printBitmap(qr_pattern, 56, 7);
}

uint8_t PhomemoAdvanced::calculateChecksum(const uint8_t* data, size_t len) {
  uint8_t checksum = 0;
  for (size_t i = 0; i < len; i++) {
    checksum ^= data[i];
  }
  return checksum;
}