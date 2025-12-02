#include "M5Unified.h"

void setup()
{
    auto cfg = M5.config();
    cfg.output_power = true;  // ensure display power when using battery units
    M5.begin(cfg);

    M5.Display.setTextSize(2);
    M5.Display.setCursor(0, 0);
    M5.Display.println("Wi-Fi Antenna");

    // Use internal Wi-Fi antenna
    auto& ioe = M5.getIOExpander(0);
    ioe.digitalWrite(0, false);
    M5.Display.println("Mode: Internal");

    // Use external Wi-Fi antenna
    // auto& ioe = M5.getIOExpander(0);
    // ioe.digitalWrite(0, true);
    // M5.Display.println("Mode: External");
}

void loop()
{
} 