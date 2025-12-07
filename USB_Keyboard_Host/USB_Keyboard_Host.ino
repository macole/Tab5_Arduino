// USB_Keyboard_Host - Tab5 に USBキーボードを接続して入力を受け取るサンプル
//
// Tab5 の USB Type-A ポートに USBキーボードを接続すると、
// キー入力を Tab5 側で処理できます。
// このサンプルでは、EspUsbHost ライブラリを使用しています。
//
// 参考: https://github.com/tanakamasayuki/EspUsbHost

#include <M5Unified.h>
#include "EspUsbHost.h"

// キーボード接続状態
bool keyboard_connected = false;

// 画面表示用のバッファ
String input_buffer = "";
const int max_display_lines = 10;
int current_line = 0;

// EspUsbHost を継承したクラス
class MyEspUsbHost : public EspUsbHost {
  void onKeyboardKey(uint8_t ascii, uint8_t keycode, uint8_t modifier) override {
    keyboard_connected = true;
    
    // シリアル出力
    if (' ' <= ascii && ascii <= '~') {
      // 印字可能文字
      Serial.printf("Key: '%c' (0x%02X) keycode=0x%02X modifier=0x%02X\n", 
                    ascii, ascii, keycode, modifier);
      
      // 画面に表示
      input_buffer += (char)ascii;
      
      // 画面幅を超えたら改行
      if (input_buffer.length() > 40) {
        input_buffer += "\n";
        current_line++;
        if (current_line >= max_display_lines) {
          // 画面をクリアして最初から
          M5.Display.fillRect(0, 40, M5.Display.width(), M5.Display.height() - 70, TFT_BLACK);
          input_buffer = "";
          current_line = 0;
        }
      }
      
      // 画面に表示
      M5.Display.setCursor(10, 50 + current_line * 30);
      M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
      M5.Display.print((char)ascii);
      
    } else if (ascii == '\r' || ascii == '\n') {
      // Enter キー
      Serial.println("Key: ENTER");
      input_buffer += "\n";
      current_line++;
      if (current_line >= max_display_lines) {
        M5.Display.fillRect(0, 40, M5.Display.width(), M5.Display.height() - 70, TFT_BLACK);
        input_buffer = "";
        current_line = 0;
      }
      M5.Display.setCursor(10, 50 + current_line * 30);
      
    } else if (ascii == '\b' || keycode == 0x2A) {
      // Backspace キー
      Serial.println("Key: BACKSPACE");
      if (input_buffer.length() > 0) {
        input_buffer.remove(input_buffer.length() - 1);
        // 画面からも削除（簡易実装）
        int x = 10 + (input_buffer.length() % 40) * 12;
        int y = 50 + current_line * 30;
        M5.Display.fillRect(x, y, 12, 24, TFT_BLACK);
      }
      
    } else {
      // その他の特殊キー
      Serial.printf("Key: 0x%02X (keycode=0x%02X modifier=0x%02X)\n", 
                    ascii, keycode, modifier);
    }
  }
  
  void onKeyboard(hid_keyboard_report_t report, hid_keyboard_report_t last_report) override {
    // キーボードレポート全体の処理（必要に応じて実装）
    // 修飾キーの状態を画面に表示
    static uint32_t last_modifier_display = 0;
    uint32_t now = millis();
    if (now - last_modifier_display > 100) {
      last_modifier_display = now;
      
      // 修飾キー表示エリアをクリア
      M5.Display.fillRect(0, M5.Display.height() - 60, M5.Display.width(), 60, TFT_BLACK);
      
      // 修飾キーの状態を表示
      M5.Display.setCursor(10, M5.Display.height() - 50);
      M5.Display.setTextSize(1);
      M5.Display.setTextColor(TFT_CYAN, TFT_BLACK);
      
      String modifiers = "";
      if (report.modifier & KEYBOARD_MODIFIER_LEFTCTRL) modifiers += "L-Ctrl ";
      if (report.modifier & KEYBOARD_MODIFIER_RIGHTCTRL) modifiers += "R-Ctrl ";
      if (report.modifier & KEYBOARD_MODIFIER_LEFTSHIFT) modifiers += "L-Shift ";
      if (report.modifier & KEYBOARD_MODIFIER_RIGHTSHIFT) modifiers += "R-Shift ";
      if (report.modifier & KEYBOARD_MODIFIER_LEFTALT) modifiers += "L-Alt ";
      if (report.modifier & KEYBOARD_MODIFIER_RIGHTALT) modifiers += "R-Alt ";
      if (report.modifier & KEYBOARD_MODIFIER_LEFTGUI) modifiers += "L-GUI ";
      if (report.modifier & KEYBOARD_MODIFIER_RIGHTGUI) modifiers += "R-GUI ";
      
      if (modifiers.length() > 0) {
        M5.Display.printf("Modifiers: %s", modifiers.c_str());
      } else {
        M5.Display.print("Modifiers: None");
      }
      
      // 現在押されているキーコードを表示
      M5.Display.setCursor(10, M5.Display.height() - 30);
      M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
      String keys = "Keys: ";
      bool has_keys = false;
      for (int i = 0; i < 6; i++) {
        if (report.keycode[i] != 0) {
          if (has_keys) keys += ", ";
          keys += "0x";
          if (report.keycode[i] < 0x10) keys += "0";
          char hex[3];
          sprintf(hex, "%02X", report.keycode[i]);
          keys += hex;
          has_keys = true;
        }
      }
      if (!has_keys) keys += "None";
      M5.Display.print(keys);
      
      M5.Display.setTextSize(2); // 元のサイズに戻す
    }
  }
  
  void onGone(const usb_host_client_event_msg_t *eventMsg) override {
    // USB デバイスが切断されたときの処理
    Serial.println("USB Keyboard disconnected");
    keyboard_connected = false;
    
    // 画面表示をクリア
    M5.Display.fillRect(0, 40, M5.Display.width(), M5.Display.height() - 70, TFT_BLACK);
    input_buffer = "";
    current_line = 0;
  }
};

MyEspUsbHost usbHost;

void setup() {
  // M5Unified 初期化
  auto cfg = M5.config();
  M5.begin(cfg);

  M5.Display.setRotation(1);
  M5.Display.clear();
  M5.Display.setTextSize(2);
  M5.Display.println("USB Keyboard Host");
  M5.Display.println("(EspUsbHost)");
  M5.Display.println("");
  M5.Display.println("Connect USB keyboard");
  M5.Display.println("to USB Type-A port");
  M5.Display.println("");
  M5.Display.println("Waiting for device...");

  Serial.begin(115200);
  delay(1000);
  Serial.println("USB Keyboard Host Demo (EspUsbHost)");

  // EspUsbHost 初期化
  usbHost.begin();
  // 日本語キーボードの場合は以下を有効化
  // usbHost.setHIDLocal(HID_LOCAL_Japan_Katakana);
  Serial.println("EspUsbHost initialized");
  M5.Display.println("Ready. Connect keyboard.");
}

void loop() {
  M5.update();
  
  // EspUsbHost のタスク処理（必須）
  usbHost.task();

  // ステータス表示
  static uint32_t last_status_display = 0;
  uint32_t now = millis();
  if (now - last_status_display > 500) {
    last_status_display = now;
    
    // ステータス表示
    M5.Display.setCursor(10, 10);
    M5.Display.setTextColor(keyboard_connected ? TFT_GREEN : TFT_YELLOW, TFT_BLACK);
    M5.Display.printf("Keyboard: %s", keyboard_connected ? "Connected" : "Waiting...");
  }

  delay(10);
}

