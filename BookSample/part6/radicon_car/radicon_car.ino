#include <ESP32Servo.h>
#include <BluetoothSerial.h>

// 定数の定義
#define SERVO 5               // MG996R
#define PWMA 12               // TB6612FNGのPWMA
#define AIN1 13               // TB6612FNGのAIN1
#define AIN2 14               // TB6612FNGのAIN2
#define ECHO 32               // HC-SR04のECHO
#define TRIG 33               // HC-SR04のTRIG
#define LEDC_CHANNEL 6        // PWMのチャンネル
#define LEDC_BASE_FREQ 10000  // PWMの周波数
#define MAX_SPEED 3           // 速度の最大値
#define MIN_SPEED -1          // 速度の最小値
#define ANGLE_STEP 6          // 角度の増分
#define ANGLE_MAX 18          // 角度の最大値
#define MAX_WAIT 20000        // pulseIn関数のタイムアウト値
#define MIN_DIST 0.3          // 障害物までの最小距離
// Bluetoothシリアルにつける名前
const char *btname = "ESP32Car";
// MG996Rの角度調整
const int servo_offset = -5;
// 速度・角度・距離を保存するグローバル変数
int speed = 0;
int angle = 0;
double dist;
// MG996RとBluetoothシリアルに対応するグローバル変数
Servo myServo;
BluetoothSerial SerialBT;

void setup() {
  // Bluetoothシリアルの初期化
  SerialBT.begin(btname);
  // PWMの初期化
  ledcSetup(LEDC_CHANNEL, LEDC_BASE_FREQ, 8);
  ledcAttachPin(PWMA, LEDC_CHANNEL);
  // MG996Rの初期化
  myServo.attach(SERVO); // MG996Rのピン番号を指定
  myServo.write(90 + servo_offset);
  // GPIOピンの初期化
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() {
  // Bluetoothシリアルから文字を読み込んで処理する
  read_bt();
  // 障害物までの距離を測定する
  read_distance();
  // モーターを操作する
  operate_motor();
}

// Bluetoothシリアルから文字を読み込んで処理する
void read_bt() {
  // Bluetoothシリアルから文字を受信しているかどうかを判断する
  if (SerialBT.available()) {
    // Bluetoothシリアルから1文字読み込む
    int ch = SerialBT.read();
    // 文字が「e」の場合
    if (ch == 'e') {
      // speedがMAX_SPEEDより小さければ1増やす（加速）
      if (speed < MAX_SPEED) {
        speed++;
      }
    }
    // 文字が「x」の場合
    else if (ch == 'x') {
      // speedがMIN_SPEEDより大木れば1減らす（減速）
      if (speed > MIN_SPEED) {
        speed--;
      }
    }
    // 文字が「d」の場合
    else if (ch == 'd') {
      // angleがANGLE_MAXより小さければANGLE_STEPだけ増やす（ハンドルを右に切る）
      if (angle < ANGLE_MAX) {
        angle += ANGLE_STEP;
      }
    }
    // 文字が「s」の場合
    else if (ch == 's') {
      // angleが-ANGLE_MAXより大きいければANGLE_STEPだけ減らす（ハンドルを左に切る）
      if (angle > -ANGLE_MAX) {
        angle -= ANGLE_STEP;
      }
    }
    // 文字が「i」の場合
    else if (ch == 'i') {
      // モーターを止め、ハンドルを中心に戻す
      speed = 0;
      angle = 0;
    }
  }
}

// 障害物までの距離を測定する
void read_distance() {
  // 超音波を10マイクロ秒発する  
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  // 反射した超音波を受信するまでの時間を測る
  double time = pulseIn(ECHO, HIGH, MAX_WAIT);
  // タイムアウトした場合はMAX_WAITだけ時間がかかったことにする
  if (time == 0) {
    time = MAX_WAIT;
  }
  // 距離を求める
  dist = (time / 1000 / 1000) / 2 * 340;
}

// モーターを操作する
void operate_motor() {
  // speedがプラスで、障害物までの距離がMIN_DISTより大きい場合
  if (speed > 0 && dist > MIN_DIST) {
    // モーターを正転させる
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  }
  // speedがマイナスの場合
  else if (speed < 0) {
    // モーターを逆転させる
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  }
  // その他の場合
  else {
    // モーターを止める
    speed = 0;
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }
  // モーターの回転速度を決める
  ledcWrite(LEDC_CHANNEL, 30 + 40 * abs(speed));
  // MG996Rの角度を決める
  myServo.write(90 + angle + servo_offset);
}
