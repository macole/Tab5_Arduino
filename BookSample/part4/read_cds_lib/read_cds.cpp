#include "read_cds.h"

double read_cds(int pin) {
  int dac;
  double cds;
  
  dac = analogRead(pin);              // CdSの入力値を読み込む
  cds = (double) dac / dac_max * 100; // 読み込んだ値を0～100に変換する
  return cds;                         // 戻り値を返す
}
