#if defined(__AVR_ATmega328P__)
const int cds_pin = A0;     // Arduino Unoでのピン番号
const int dac_max = 1024;   // Arduino Unoでのアナログ入力の最大値
const float volt_max = 5.0; // Arduino Unoでのアナログ入力の最大値に対応する電圧
#elif defined(ESP32)
const int cds_pin = 35;     // ESP32でのピン番号
const int dac_max = 4096;   // ESP32でのアナログ入力の最大値
const float volt_max = 3.3; // ESP32でのアナログ入力の最大値に対応する電圧
#endif

void setup() {
  pinMode(cds_pin, INPUT); // CDSのピンを入力用にする
  Serial.begin(115200);    // 通信速度を115200bpsにする
}

void loop() {
  int dac;                    // int型の変数dacを宣言する
  float volt;                 // float型の変数voltsを宣言する

  dac = analogRead(cds_pin);  // 電圧を読み取って変数dacに代入する
  volt = volt_max * dac / dac_max; // 読み取った値を電圧に変換する
  Serial.println(volt);       // 変数voltの値をシリアルモニタに出力する
  delay(1000);                // 1秒待つ
}
