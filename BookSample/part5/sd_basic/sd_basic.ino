#include <SPI.h>
#include <SD.h>

#ifdef ESP32
#define W_MODE FILE_APPEND
#else
#define W_MODE FILE_WRITE
#endif
#define CS_PIN 4

void setup() {
  File f;
  char ch;
  
  // シリアルモニタの初期化
  Serial.begin(115200);
  // SDカードモジュールの初期化
  if (SD.begin(CS_PIN)) {
    Serial.println("SD OK");
  }
  else {
    Serial.println("SD Failed");
    while(1);
  }
  // sample.txtファイルを読み込み用に開く
  f = SD.open("/sample.txt", FILE_READ);
  // ファイルを開けた場合
  if (f) {
    // ファイルの中身を読み込んでシリアルモニタに出力
    Serial.println("Read from SD");
    Serial.println("-------------");
    while (f.available()) {
      ch = f.read();
      Serial.print(ch);
    }
    Serial.println("-------------");
    Serial.println("Read end");
    // ファイルを閉じる
    f.close();
  }
  // ファイルを開けなかった場合
  else {
    // ファイルがなかったことを表示
    Serial.println("Not found sample.txt");
  }
  // sample.txtファイルを書き込み用に開く
  f = SD.open("/sample.txt", W_MODE);
  // ファイルを開けた場合
  if (f) {
    // ファイルの最後に「sample」の文字列を書き込む
    f.println("Sample");
    // ファイルを閉じる
    f.close();
    Serial.println("Print 'Sample' to SD");
  }
  else {
    // ファイルを開けなかったことを表示
    Serial.println("Open sample.txt failed");
  }
}

void loop() {

}
