// 復帰回数を表す変数
RTC_DATA_ATTR int count;

// タッチ割り込みの関数
void onTouch() {
}

void setup() {
  // シリアルモニタを初期化
  Serial.begin(115200);
  // 復帰回数を表示
  count++;
  Serial.print(count);
  Serial.println("回目の復帰");
  // 復帰理由を表示
  int cause = esp_sleep_get_wakeup_cause();
  if (cause == ESP_SLEEP_WAKEUP_TIMER) {
    Serial.println("一定周期が経過");
  }
  else if (cause == ESP_SLEEP_WAKEUP_TOUCHPAD) {
    Serial.println("タッチされた");
  }
  else if (cause == ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("スイッチが押された");
  }
  else {
    Serial.println("通常起動");
  }
  // 30秒ごとにDeepSleepから復帰
  esp_sleep_enable_timer_wakeup(30 * 1000000);
  // 4番ピンがタッチされたらDeepSleepから復帰
  touchAttachInterrupt(T0, onTouch, 40);
  esp_sleep_enable_touchpad_wakeup();
  // スイッチが押されたらDeepSleepから復帰
//  esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, HIGH);
  // DeepSleepに入る
  esp_deep_sleep_start();
}

void loop() {
  // put your main code here, to run repeatedly:

}
