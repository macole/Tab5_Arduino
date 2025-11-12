#include "M5Unified.h"

void setup()
{
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Display.setRotation(3);
    M5.Display.setTextDatum(top_center);
    M5.Display.setFont(&fonts::FreeMonoBold24pt7b);
    M5.Display.clear(WHITE);
    M5.Display.drawString("Tab5 Power Manager", M5.Lcd.width() / 2, 0);
}

void loop()
{
    M5.Display.clear();
    bool bat_ischarging = M5.Power.isCharging();
    M5.Display.setCursor(0, 100);
    M5.Display.printf("Bat Charging: %s\n", bat_ischarging ? "Yes" : "No");
    int bat_vol = M5.Power.getBatteryVoltage();
    M5.Display.printf("Bat Voltage: %dmv\n", bat_vol);
    int bat_level = M5.Power.getBatteryLevel();
    M5.Display.printf("Bat Level: %d\n", bat_level);
    delay(1000);
} 