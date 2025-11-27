#ifndef _READ_CDSH_
#define _READ_CDSH_       // _READ_CDS_Hマクロを定義
#include <Arduino.h>      // Arduino.hを読み込む

#if defined(ESP32)
const int dac_max = 4095; // ESP32のアナログ入力の最大値
#else
const int dac_max = 1023; // Arduino Unoのアナログ入力の最大値
#endif

double read_cds(int pin); // read_cds関数のプロトタイプ宣言
#endif
