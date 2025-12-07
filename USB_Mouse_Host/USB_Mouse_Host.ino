// USB_Mouse_Host - Tab5 に USBマウスを接続して入力を受け取るサンプル
//
// Tab5 の USB Type-A ポートに USBマウスを接続すると、
// マウスの移動量やボタンクリックを Tab5 側で処理できます。
// このサンプルでは、EspUsbHost ライブラリを使用しています。
//
// 参考: https://github.com/tanakamasayuki/EspUsbHost

#include <M5Unified.h>
#include "EspUsbHost.h"

// マウス座標（画面表示用）
int mouse_x = 640;
int mouse_y = 360;
uint8_t last_buttons = 0;
bool mouse_connected = false;

// EspUsbHost を継承したクラス
class MyEspUsbHost : public EspUsbHost {
  void onMouse(hid_mouse_report_t report, uint8_t last_buttons) override {
    // マウスレポート全体の処理
    mouse_connected = true;
    
    // カーソル位置更新
    mouse_x += report.x * 2;  // 感度調整
    mouse_y += report.y * 2;
    
    // 画面範囲内に制限
    mouse_x = constrain(mouse_x, 0, M5.Display.width());
    mouse_y = constrain(mouse_y, 0, M5.Display.height());
    
    // シリアル出力
    if (report.x != 0 || report.y != 0) {
      Serial.printf("Mouse move: dx=%d dy=%d (x=%d y=%d)\n", 
                    report.x, report.y, mouse_x, mouse_y);
    }
    
    // ボタン状態の変化を検出
    uint8_t button_changes = report.buttons ^ last_buttons;
    if (button_changes & MOUSE_BUTTON_LEFT) {
      if (report.buttons & MOUSE_BUTTON_LEFT) {
        Serial.println("Left button pressed");
      } else {
        Serial.println("Left button released");
      }
    }
    if (button_changes & MOUSE_BUTTON_RIGHT) {
      if (report.buttons & MOUSE_BUTTON_RIGHT) {
        Serial.println("Right button pressed");
      } else {
        Serial.println("Right button released");
      }
    }
    if (button_changes & MOUSE_BUTTON_MIDDLE) {
      if (report.buttons & MOUSE_BUTTON_MIDDLE) {
        Serial.println("Middle button pressed");
      } else {
        Serial.println("Middle button released");
      }
    }
    
    last_buttons = report.buttons;
  }
  
  void onMouseButtons(hid_mouse_report_t report, uint8_t last_buttons) override {
    // ボタンのみの処理（必要に応じて実装）
  }
  
  void onMouseMove(hid_mouse_report_t report) override {
    // 移動のみの処理（必要に応じて実装）
  }
  
  void onGone(const usb_host_client_event_msg_t *eventMsg) override {
    // USB デバイスが切断されたときの処理
    Serial.println("USB Mouse disconnected");
    mouse_connected = false;
    last_buttons = 0;
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
  M5.Display.println("USB Mouse Host");
  M5.Display.println("(EspUsbHost)");
  M5.Display.println("");
  M5.Display.println("Connect USB mouse");
  M5.Display.println("to USB Type-A port");
  M5.Display.println("");
  M5.Display.println("Waiting for device...");

  Serial.begin(115200);
  delay(1000);
  Serial.println("USB Mouse Host Demo (EspUsbHost)");

  // EspUsbHost 初期化
  usbHost.begin();
  Serial.println("EspUsbHost initialized");
  M5.Display.println("Ready. Connect mouse.");
}

void loop() {
  M5.update();
  
  // EspUsbHost のタスク処理（必須）
  usbHost.task();

  // 画面にカーソル表示
  static uint32_t last_draw = 0;
  uint32_t now = millis();
  if (now - last_draw > 50) {
    last_draw = now;
    
    // 前のカーソルを消す（簡易実装）
    static int last_x = -1, last_y = -1;
    if (last_x >= 0 && last_y >= 0) {
      M5.Display.fillCircle(last_x, last_y, 8, TFT_BLACK);
    }
    
    // 新しいカーソルを描画
    if (mouse_connected) {
      M5.Display.fillCircle(mouse_x, mouse_y, 8, TFT_RED);
      last_x = mouse_x;
      last_y = mouse_y;
    }
    
    // ステータス表示
    M5.Display.setCursor(10, M5.Display.height() - 30);
    M5.Display.setTextColor(mouse_connected ? TFT_GREEN : TFT_YELLOW);
    M5.Display.printf("Mouse: %s", mouse_connected ? "Connected" : "Waiting...");
  }

  delay(10);
}

