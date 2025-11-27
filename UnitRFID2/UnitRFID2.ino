#include <M5Unified.h>
#include <MFRC522_I2C.h>

MFRC522_I2C mfrc522(0x28, -1);  // 0x28: I2C address of Unit RFID / RFID2; -1: reset pin (not connected)

void setup() {
  M5.begin();
  M5.Power.begin();       // 電源モジュールの初期化
  M5.Display.setTextSize(2);  // テキストサイズを2に設定
  M5.Display.setCursor(0, 0);
  M5.Display.println("      RFID2 Example");
  M5.Display.println("Tool>SerialMonitor 115200baud");

  Serial.begin(115200);
  Wire.begin();  // I2C通信の初期化
  mfrc522.PCD_Init();
}

void loop() {
  M5.update();

  // PICC: Proximity Integrated Circuit Card
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    M5.Display.setCursor(0, 40);  // 初期メッセージの下に表示
    M5.Display.setTextSize(2);

    uint8_t piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.print(F("PICC type: "));
    Serial.println(mfrc522.PICC_GetTypeName(piccType));
    
    M5.Display.print("PICC type: ");
    M5.Display.println(mfrc522.PICC_GetTypeName(piccType));

    // Check if the tag / card is of type MIFARE Classic
    if (piccType != MFRC522_I2C::PICC_TYPE_MIFARE_MINI
        && piccType != MFRC522_I2C::PICC_TYPE_MIFARE_1K
        && piccType != MFRC522_I2C::PICC_TYPE_MIFARE_4K) {
      Serial.println(F("This tag / card is not of type MIFARE Classic.\n"));
      M5.Display.println("Not MIFARE Classic");
      delay(500);
      return;
    }

    // Output the stored UID data
    M5.Display.print("UID: ");
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.printf("%02X ", mfrc522.uid.uidByte[i]);
      M5.Display.printf("%02X ", mfrc522.uid.uidByte[i]);
    }
    Serial.println("\n");
    M5.Display.println();
    delay(500);
  }
} 