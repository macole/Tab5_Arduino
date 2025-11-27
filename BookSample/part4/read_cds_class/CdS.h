#ifndef _CDSH_
#define _CDSH_       // _READ_CDS_Hマクロを定義
#include <Arduino.h>      // Arduino.hを読み込む

#if defined(ESP32)
const int dac_max = 4095; // ESP32のアナログ入力の最大値
#else
const int dac_max = 1023; // Arduino Unoのアナログ入力の最大値
#endif

class CdS {
  public:
    CdS(int _pin);        // コンストラクタ
    void begin(void);     // 初期化を行うメンバ関数
    double read(void);    // CdSの値を読み取るメンバ関数
  protected:
    int pin;              // ピン番号を記憶するためのメンバ変数
};
#endif
