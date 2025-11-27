#include <ESP32Servo.h>      // ESP32Servo.hをインクルード

#define SERVO_PIN 5          // 定数SERVO_PINにピン番号(5)を割り当て

Servo myServo;               // Servoクラスの変数myServoを宣言

void setup() {
  myServo.attach(SERVO_PIN); // サーボモーターのピン番号を指定
  myServo.write(90);         // 90度にする 
}

void loop() {
}
