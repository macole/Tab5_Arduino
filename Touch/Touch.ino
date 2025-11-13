#include <M5Unified.h>
#include <M5GFX.h>

m5::touch_detail_t touchDetail;
static int32_t w;
static int32_t h;

LGFX_Button button;

void setup() {
  M5.begin();

  w = M5.Lcd.width(); //画面の幅
  h = M5.Lcd.height();//画面の高さ
  
  M5.Lcd.fillScreen(WHITE);
  M5.Display.setRotation(0);
  M5.Display.setTextDatum(top_center);
  M5.Display.drawString("Button Released", w / 2, 0, &fonts::FreeMonoBold24pt7b);

  button.initButton(&M5.Lcd, w / 2, h / 2, 200, 200, TFT_BLUE, TFT_YELLOW, TFT_BLACK, "BTN", 4, 4);
  button.drawButton();
}

void loop() {
  M5.update();
  touchDetail = M5.Touch.getDetail();//タッチデータを取得

  if (touchDetail.isPressed()) {
    if(button.contains(touchDetail.x, touchDetail.y)){
        M5.Display.drawString("Button  Pressed", w / 2, 0, &fonts::FreeMonoBold24pt7b);
    }
  }
  else {
    M5.Display.drawString("Button Released", w / 2, 0, &fonts::FreeMonoBold24pt7b);
  }
} 