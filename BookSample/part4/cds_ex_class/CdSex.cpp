#include "CdSex.h"

CdSex::CdSex(int _pin) : CdS(_pin) {
  // 基底クラスのコンストラクタを呼び出すだけ
}

int CdSex::read_raw(void) {
  int dac;

  dac = analogRead(pin);  // CdSセルの値を読み込む
  return dac;             // 読み込んだ値をそのまま返す
}
