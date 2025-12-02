#include <M5Unified.h>
#include "USB.h"
#include "USBHID.h"

USBHID HID;

// マウス座標
int mouse_x = 640;
int mouse_y = 360;

// マウスレポートコールバック
void mouse_callback(void* arg, uint8_t* report, uint16_t len) {
    if (len >= 4) {
        // バイト0: ボタン状態
        uint8_t buttons = report[0];
        // バイト1: X移動量
        int8_t dx = (int8_t)report[1];
        // バイト2: Y移動量
        int8_t dy = (int8_t)report[2];
        // バイト3: ホイール
        int8_t wheel = (int8_t)report[3];
        
        // カーソル位置更新
        mouse_x += dx * 2;  // 感度調整
        mouse_y += dy * 2;
        
        // 画面範囲内に制限
        mouse_x = constrain(mouse_x, 0, 1280);
        mouse_y = constrain(mouse_y, 0, 720);
        
        // ボタン判定
        if (buttons & 0x01) {
            Serial.println("Left button clicked");
        }
        if (buttons & 0x02) {
            Serial.println("Right button clicked");
        }
        if (buttons & 0x04) {
            Serial.println("Middle button clicked");
        }
    }
}

void setup() {
    M5.begin();
    M5.Display.setRotation(3);
    M5.Display.clear();
    
    Serial.begin(115200);
    
    // USB Host初期化
    HID.begin();
    HID.onEvent(mouse_callback);
    
    M5.Display.setTextSize(2);
    M5.Display.println("USB Mouse Demo");
    M5.Display.println("Connect USB mouse");
}

void loop() {
    M5.update();
    
    // カーソル描画
    M5.Display.fillCircle(mouse_x, mouse_y, 5, RED);
    
    delay(10);
    
    // 軌跡を少しフェードアウト
    M5.Display.fillCircle(mouse_x, mouse_y, 5, 
                          M5.Display.color565(240, 240, 240));
}