#include <M5Unified.h>

uint32_t count;

void setup() {
  //コンフィグでボードに合わせた自動設定をする
  auto cfg = M5.config();
  M5.begin(cfg);

  //Displayを使って描画可能
  M5.Display.setTextSize(8);
  M5.Display.print("Hello World");
  Serial.println("Hello World");
  count = 0;
}

void loop() {
  M5.Display.setCursor(100, 200);
  M5.Display.printf("COUNT: %d\n", count);
  Serial.printf("COUNT: %d\n", count);
  count++;
  delay(1000);
}
