#include <FS.h>
#include <SPIFFS.h>

void setup() {
  File f;
  
  Serial.begin(115200);     // シリアルモニタを初期化
  SPIFFS.begin();           // SPIFFSを初期化
  // SPIFFSのルートフォルダにある「test.txt」ファイルを開く
  f = SPIFFS.open("/test.txt", FILE_READ);
  if (f) {                  // ファイルを開けたかどうかを調べる
    while (f.available()) { // ファイルにデータがある間繰り返す
      char ch = f.read();   // ファイルから1文字読み込む
      Serial.print(ch);     // 読み込んだ文字をシリアルモニタに出力
    }
    f.close();              // ファイルを閉じる
  }
}

void loop() {
}
