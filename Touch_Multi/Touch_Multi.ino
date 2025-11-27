#include <M5Unified.h>
#include <M5GFX.h>

m5::touch_point_t touchPoint[5];//Tab5 supports up to 5-point touch
static bool drawed = false;
static int32_t w;
static int32_t h;

void setup() {
  M5.begin();

  w = M5.Lcd.width();
  h = M5.Lcd.height();
  
  M5.Lcd.fillScreen(WHITE);
  M5.Display.setRotation(0);
  M5.Display.setTextDatum(top_center);
  M5.Display.drawString("Touch not found", w / 2, 0, &fonts::FreeMonoBold24pt7b);
  M5.Display.setFont(&fonts::FreeMonoBold24pt7b);
}

void loop() {
  M5.update();
  int nums = M5.Lcd.getTouchRaw(touchPoint, 5);  

  if (nums)
  {
    M5.Display.drawString(" Touch detail: ", w / 2, 0, &fonts::FreeMonoBold24pt7b);
    for (int i = 0; i < nums; i++)
    {
      M5.Display.setCursor(50, 160 + i * 48);
      M5.Display.printf("Point %d X:%04d  Y:%04d", i+1, touchPoint[i].x, touchPoint[i].y);
    }
    drawed = true;
  }
  else if (drawed){
    drawed = false;
    M5.Display.clear(WHITE);
    M5.Display.drawString("Touch not found", w / 2, 0, &fonts::FreeMonoBold24pt7b);
  }
  vTaskDelay(1);
} 