#ifndef _CDSEXH_
#define _CDSEXH_
#include <CdS.h>

class CdSex : public CdS {
  public:
    CdSex(int _pin);     // コンストラクタ
    int read_raw(void);  // read_rawメンバ関数
};
#endif
