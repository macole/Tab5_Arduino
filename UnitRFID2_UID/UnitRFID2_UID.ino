#include <M5Unified.h>
#include <MFRC522_I2C.h>

MFRC522_I2C mfrc522(0x28,-1);

void setup() {
  M5.begin();
  M5.Lcd.setTextSize(2);
  Wire.begin();
  mfrc522.PCD_Init();
}

void loop() {
  if(!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(200);
    return;
  }
  M5.Lcd.print("[UID]");
  for (byte i=0; i<mfrc522.uid.size; i++) {
    M5.Lcd.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    M5.Lcd.print(mfrc522.uid.uidByte[i], HEX);
  }
  M5.Lcd.println("");
  delay(500);
}