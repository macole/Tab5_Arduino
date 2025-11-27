int dacs[10];            // データを記憶するための配列

void setup() {
  int i;
  
  pinMode(35, INPUT);    // 35番ピンを入力用にする
  Serial.begin(115200);  // 通信速度を115200bpsにする
  for (i = 0; i < 10; i++) {   // 10回繰り返す
    // メッセージを出力
    Serial.print(i);
    Serial.println("番目の初期値データを読み込んでいます");
    dacs[i] = analogRead(35);  // データを読み込む
    delay(500);                // 0.5秒待つ
  }
}

void loop() {
  // 変数の宣言
  int i, dac, dac_min, dac_max;
  static int ptr = 0;

  dacs[ptr] = analogRead(35);  // 電圧を読み取って配列dacのptr番目の要素に代入
  ptr++;                       // 変数ptrの値を1増やす
  if (ptr == 10) {             // 配列の一番最後の要素まで値を代入した？
    ptr = 0;                   // 次の代入先を最初の要素に戻す
  }
  dac_min = 9999;              // 最小値の初期値を9999にする
  dac_max = -1;                // 最大値の初期値を-1にする
  for (i = 0; i < 10; i++) {   // 変数iを0から9まで変化させながら繰り返す
    if (dacs[i] < dac_min) {   // 読み取った値が最小値より小さければ、
      dac_min = dacs[i];       // 最小値を更新する
    }
    else if (dacs[i] > dac_max) {  // 読み取った値が最大値より大きければ、
      dac_max = dacs[i];           // 最大値を更新する
    }
  }

  dac = 0;                     // 変数dacを0に初期化
  for (i = 0; i < 10; i++) {   // 配列dacsの各要素を変数dacに順に足す
    dac += dacs[i];
  }
  // 10回の合計から最大値と最小値を引き、8で割って平均する
  dac = (dac - dac_max - dac_min) / 8;
  Serial.println(dac);         // 変数dacの値をシリアルモニタに出力する
  delay(500);                  // 0.5秒待つ
}
