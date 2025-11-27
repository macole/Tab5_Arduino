#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  WiFi.beginSmartConfig();
  Serial.println("Waiting smart config");
  while (!WiFi.smartConfigDone()) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.println("Smart config done");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("WiFi Connected.");
  // これ以後に他の初期化処理を入れる
}

void loop() {
  // loop関数で行いたい処理を入れる
}
