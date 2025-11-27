void setup() {
  // 開始時刻と現在時刻のための変数を宣言
  unsigned long s_time, e_time;
  // break文で繰り返しを抜けたかどうかの変数を宣言
  boolean is_break = false;
    
  Serial.begin(115200);     // シリアルモニタの初期化
  pinMode(2, INPUT_PULLUP); // 2番ピンを入力用にする
  Serial.println("3");      // カウントダウンする
  delay(1000);
  Serial.println("2");
  delay(1000);
  Serial.println("1");
  delay(1000);
  Serial.println("Start");  // 「Start」と出力する
  s_time = millis();        // 開始時刻を変数s_timeに代入
  // 現在時刻を変数e_timeに代入し、
  //e_timeとs_timeの差が10秒以内の間は繰り返す
  while (e_time = millis(), e_time - s_time < 10000) {
    // スイッチが押されたかどうかを判断
    if (digitalRead(2) == LOW) {
      is_break = true;      // break文で繰り返しを抜けた 
      break;
    }
  }
  // break文で繰り返しを抜けたかどうかを判断
  if (is_break == true) {
    // break文で繰り返しを抜けた場合は、ボタンを押すまでの時間を出力
    Serial.print("時間は");
    Serial.print((double) (e_time - s_time) / 1000.0);
    Serial.println("秒でした");
  }
  else {
    // while文を最後まで実行した場合は、「10秒を超えました」と出力
    Serial.println("10秒を超えました");
  }
}

void loop() {
}
