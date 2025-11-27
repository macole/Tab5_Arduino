#include <Servo.h>           // Servo.hをインクルード

#define SERVO_PIN 3          // 定数SERVO_PINにピン番号(3)を割り当て

Servo myServo;               // Servoクラスの変数myServoを宣言

void setup() {
  myServo.attach(SERVO_PIN); // サーボモーターのピン番号を指定
}

void loop() {
  myServo.write(0);          // 0度にする 
  delay(1000);               // 1秒待つ
  myServo.write(90);         // 90度にする 
  delay(1000);               // 1秒待つ
  myServo.write(180);        // 180度にする 
  delay(1000);               // 1秒待つ
  myServo.write(90);         // 90度にする 
  delay(1000);               // 1秒待つ
}
