#include "M5Unified.h"

void setup()
{
    M5.begin();
    M5.Display.clear(WHITE);
    M5.Display.setTextDatum(middle_center);
    M5.Display.setTextFont(&fonts::FreeMonoBold24pt7b);
    M5.Display.setTextSize(1);
    M5.Display.drawString("Speaker Test", M5.Display.width() / 2, M5.Display.height() / 2);
}

void loop()
{
    M5.Speaker.tone(10000, 60);
    delay(1000);
    M5.Speaker.tone(8000, 60);
    delay(1000);
    M5.Speaker.tone(6000, 60);
    delay(1000);
    M5.Speaker.tone(4000, 60);
    delay(1000);
    M5.Speaker.tone(2000, 60);
    delay(1000);
}   