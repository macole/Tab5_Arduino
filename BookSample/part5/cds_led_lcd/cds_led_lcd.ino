#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define ADRS 0x3F       // I2Cのアドレス
#define CHARS 16        // 1行あたりの文字数
#define LINES 2         // 行数

#ifdef ESP32
#define LED_PIN 5       // LEDのピン番号
#define CDS_PIN 35      // CdSセルのピン番号
#define THRES 2000      // 明暗を判断する値
#else
#define LED_PIN 3
#define CDS_PIN A0
#define THRES 500
#endif

// LiquidCrystal_I2Cクラスの変数lcdを宣言する
LiquidCrystal_I2C lcd(ADRS, CHARS, LINES);

void setup() {
  pinMode(CDS_PIN, INPUT);       // CDS用のピンを入力用にする
  pinMode(LED_PIN, OUTPUT);      // LED用のピンを出力用にする
  lcd.init();                    // ディスプレイを初期化する
  lcd.backlight();               // ディスプレイのバックライトをオンにする
}

void loop() {
  int dac;                       // int型の変数dacを宣言する
  char buf[5];                   // dacの値を4桁に変換するためのバッファ
  
  dac = analogRead(CDS_PIN);     // 電圧を読み取って変数dacに代入する
  sprintf(buf, "%4d", dac);      // dacの値を4桁にする
  lcd.setCursor(0, 0);           // 左上にカーソルを移動する
  lcd.print("CdS = ");           // 「CdS = 」をディスプレイに出力する
  lcd.print(buf);                // 変数dacの値をディスプレイに出力する
  lcd.setCursor(0, 1);           // 2行目の先頭にカーソルを移動する
  lcd.print("LED = ");           // 「, LED = 」をディスプレイに出力する
  if (dac < THRES) {             // 変数datが定数THRESより小さい(＝暗い)かどうかを判断する
    digitalWrite(LED_PIN, HIGH); // LEDを点灯する
    lcd.print("  ON");           // ディスプレイに「ON」と出力する
  }
  else {
    digitalWrite(LED_PIN, LOW);  // LEDを消灯する
    lcd.print(" OFF");           // ディスプレイに「OFF」と出力する
  }
  delay(100);                    // 0.1秒待つ
}
