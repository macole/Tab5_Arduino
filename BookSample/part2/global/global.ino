int x;

void setup() {
  Serial.begin(115200);  // シリアルモニタを初期化
  x = 0;                 // グローバル変数xに0を代入する
}

void loop() {
  x++;                   // グローバル変数xに0を代入する
  Serial.println(x);
}
