// Serial - シリアル通信 + LED制御 (M5Unified 対応 / 画面表示付き)
//
// M5Stack Tab5 上で、シリアルモニタとディスプレイの両方に
// LED のオン / オフ状態を表示するサンプルです。

#include <M5Unified.h>

lgfx::LGFX_Device* display = nullptr;

void setup() {
  // M5Unified の初期化（Tab5 用の推奨パターン）
  auto cfg = M5.config();
  M5.begin(cfg);
  display = &M5.Display;

  // ディスプレイ設定
  display->setRotation(1);
  display->setTextSize(2);
  display->clear();
  display->setCursor(0, 0);
  display->println("Serial - Ready");

  // GPIO とシリアルの設定
  pinMode(3, OUTPUT);          // 3番ピンを出力用にする（外付けLED想定）
  Serial.begin(115200);        // 通信速度を115200bpsにする
  Serial.println("Serial - Ready"); // シリアルモニタにメッセージを送信する
}

void loop() {
  M5.update();

  // LED ON
  digitalWrite(3, HIGH);          // 3番ピンをオンにする
  Serial.println("LED ON");       // シリアルモニタに「LED ON」を送信する  

  display->clear();
  display->setCursor(0, 0);
  display->println("Serial");
  display->setCursor(0, 24);
  display->println("Status: LED ON");

  delay(1000);                    // 1秒間待つ

  // LED OFF
  digitalWrite(3, LOW);           // 3番ピンをオフにする
  Serial.println("LED OFF");      // シリアルモニタに「LED OFF」を送信する  

  display->clear();
  display->setCursor(0, 0);
  display->println("Serial");
  display->setCursor(0, 24);
  display->println("Status: LED OFF");

  delay(1000);                    // 1秒間待つ
}
