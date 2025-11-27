#if defined(__AVR_ATmega328P__)
const int cds_pin = A0;     // Arduino Unoでのピン番号
const int dac_max = 1024;   // Arduino Unoでのアナログ入力の最大値
#elif defined(ESP32)
const int cds_pin = 35;     // ESP32でのピン番号
const int dac_max = 4096;   // ESP32でのアナログ入力の最大値
#endif

void setup() {
  pinMode(cds_pin, INPUT); // CDSのピンを入力用にする
  Serial.begin(115200);    // 通信速度を115200bpsにする
}

void loop() {
  int dac;                  // int型の変数dacを宣言する
  double val;               // float型の変数valを宣言する

  val = read_cds(cds_pin);  // CdSの値を読み取る
  Serial.println(val);      // 変数valの値をシリアルモニタに出力する
  delay(1000);              // 1秒待つ
}

double read_cds(int pin) {
  int analogIn;
  double ret;

  analogIn = analogRead(pin);              // 引数で指定されたピンからアナログ入力値を読み取る
  ret = (double) analogIn / dac_max * 100; // アナログ入力値を元に0～100の値を求める
  return ret;                              // 戻り値としてh変数retの値を返す
}
