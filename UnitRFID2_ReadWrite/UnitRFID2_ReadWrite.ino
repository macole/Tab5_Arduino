#include <M5Unified.h>
#include <MFRC522_I2C.h>

MFRC522_I2C mfrc522(0x28, -1); // デバイスアドレスとリセット端子番号（-1: リセットピン未使用）

// 変数宣言
byte read_data[16] = {};  // 読込みデータ格納用配列
byte write_data[16] = {}; // 書込みデータ格納用配列
int sector_num = 0;       // セクター番号格納用
int block_num = 0;        // ブロック番号格納用

// 関数 --------------------------------------------------------
// 全データ読み込みシリアルモニタ表示 ------------------------------------------
void readAllData() {
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak); // SAK（NFCタグの識別子）を取得し、変数に格納
  // カードタイプ確認
  if (piccType == MFRC522_I2C::PICC_TYPE_MIFARE_MINI ||
      piccType == MFRC522_I2C::PICC_TYPE_MIFARE_1K ||
      piccType == MFRC522_I2C::PICC_TYPE_MIFARE_4K) {
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));          // Mifare Classicのデータを出力
  } else if (piccType == MFRC522_I2C::PICC_TYPE_MIFARE_UL) {
    mfrc522.PICC_DumpMifareUltralightToSerial();        // Mifare Ultralightのデータを出力
  } else {
    M5.Display.setTextColor(TFT_RED, TFT_BLACK);
    M5.Display.fillRect(0, 57, M5.Display.width(), M5.Display.height() - 57, TFT_BLACK);  // 液晶のデータ表示部を黒塗りリセット
    M5.Display.setCursor(0, 57);                            // 座標表示
    M5.Display.println("This card is not supported.");      // サポートされていないカードである旨を表示
    delay(5000);
  }
  mfrc522.PICC_HaltA(); // 現在読み込んでいるカードをハルトする
  mfrc522.PCD_Init();   // RFIDリーダーを初期化する
}
// ブロックデータ読み込み ------------------------------------------
void readData(int sector, int block) {  // (セクター番号, ブロック番号)
  MFRC522_I2C::MIFARE_Key key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // MIFARE_Key構造体でセクター認証キーを準備（ポインタで指定するため）
  Serial.println("カードデータ読み取り");

  uint8_t address = sector * 4 + block;         // ブロックアドレスを算出
  // 読込み対象のセクターを認証する(認証キータイプ, ブロックアドレス, 認証キーポインタ, カードのUIDプロパティへのポインタ)
  if (mfrc522.PCD_Authenticate(MFRC522_I2C::PICC_CMD_MF_AUTH_KEY_A, address, &key, &(mfrc522.uid)) == MFRC522_I2C::STATUS_OK) {
    Serial.println("認証に成功しました");
    byte buffer[18];                            // 読込みデータ16byte + エラーチェック用CRC用2byte分の配列を準備
    byte bufferSize = sizeof(buffer);           // バッファサイズをポインタで指定できるように準備
    // 認証成功で読込み実行(ブロックアドレス, データ格納バッファ配列ポインタ, 読込みデータバッファサイズポインタ)
    if (mfrc522.MIFARE_Read(address, buffer, &bufferSize) == MFRC522_I2C::STATUS_OK) { // 通信成功なら
      for (int i = 0; i < bufferSize-2; i++) {  // 読込んだデータのCRCを除いた16byte分繰り返す
        read_data[i] = buffer[i];               // バッファを読み込みデータ配列に格納
        Serial.printf("%02X ", read_data[i]);   // 16byteの読み込みデータをシリアルモニタに出力
      }
      Serial.printf("\n%s\n", read_data);
    }
  } else {
    Serial.println("認証に失敗しました");
  }
  mfrc522.PCD_StopCrypto1();  // 認証済み解除
}
// ブロックデータ書込み -------------------------------------------
void writeData(int sector, int block) { // (セクター番号, ブロック番号)
  MFRC522_I2C::MIFARE_Key key = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // MIFARE_Key構造体でセクターキーを準備（ポインタで指定するため）
  Serial.println("カードデータ書込み");

  uint8_t address = sector * 4 + block;         // ブロックアドレスを算出
  // 書込み対象のセクターを認証する(認証キータイプ, ブロックアドレス, 認証キーポインタ, カードのUIDプロパティへのポインタ)
  if (mfrc522.PCD_Authenticate(MFRC522_I2C::PICC_CMD_MF_AUTH_KEY_A, address, &key, &(mfrc522.uid)) == MFRC522_I2C::STATUS_OK) {  // 通信成功なら
    Serial.println("認証に成功しました");
    if (address == 0 || block == 3) {                 // ブロックアドレスが0またはブロック3なら書込まず終了
        Serial.print("データブロックではないので書き込めません");
    } else {                                          // ブロック0〜2ならデータ書込み
      mfrc522.MIFARE_Write(address, write_data, 16);  // (ブロックアドレス, 書込みデータ, 書込みbyte数)
      // 書き込みデータをシリアルモニタに出力
      for (int i = 0; i < 16; i++) {            // 16byte分繰り返す
        Serial.printf("%02X ", write_data[i]);  // 16進数で出力
      }
    }
    Serial.printf("\n%s\n", write_data);
  } else {
    Serial.println("認証に失敗しました");
  }
  // カードから切断する
  mfrc522.PCD_StopCrypto1();  // 認証済み解除
}

// 初期設定 ------------------------------------------------------
void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);        // 本体初期化
  M5.Power.begin();     // 電源初期化
  Serial.begin(115200); // シリアル通信初期化（Tab5は115200 baud）
  Wire.begin();         // I2C通信初期化
  mfrc522.PCD_Init();   // RFID2（MFRC522）初期化

  // 初期画面表示
  M5.Display.setTextSize(2);                          // テキストサイズ
  M5.Display.setCursor(0, 0);
  M5.Display.println("RFID2  Card  Reader / Writer"); // タイトル表示
  M5.Display.println("Please put the card");
  M5.Display.fillRect(0, 51, M5.Display.width(), 2, TFT_WHITE);  // 横線
}
// メイン ------------------------------------------------------
void loop() {
  // 新しいカードが読み込まれない、または、カードのシリアル番号が読み取れないならループ
  while (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(100); // 待機時間
  }
  // カードが読み取られたら以下を実行

  M5.update();  // タッチ入力の更新
  
  // タッチ操作でセクター/ブロック番号を変更、読み書きを実行
  auto touch_count = M5.Touch.getCount();
  if (touch_count > 0) {
    auto t = M5.Touch.getDetail(0);
    int touch_x = t.x;
    int touch_y = t.y;
    
    // 画面を4分割して操作エリアを定義
    int screen_width = M5.Display.width();
    int screen_height = M5.Display.height();
    int area_width = screen_width / 2;
    int area_height = screen_height / 2;
    
    // 左上: セクター番号＋1
    if (touch_x < area_width && touch_y < area_height && t.wasPressed()) {
      sector_num = (sector_num + 1) % 16; // 16なら0リセット
      delay(200);
    }
    // 右上: ブロック番号＋1
    else if (touch_x >= area_width && touch_y < area_height && t.wasPressed()) {
      block_num = (block_num + 1) % 3;    // 3なら0リセット
      delay(200);
    }
    // 左下: ブロックデータ読込み
    else if (touch_x < area_width && touch_y >= area_height && t.wasPressed()) {
      readData(sector_num, block_num);    // データ読込み関数(セクター番号, ブロック番号)
      delay(200);
    }
    // 右下: 全データ読込みシリアルモニタ表示
    else if (touch_x >= area_width && touch_y >= area_height && t.wasPressed()) {
      readAllData();                      // シリアルモニタ表示関数
      delay(200);
    }
  }

  // シリアルモニタから書込みデータを設定
  if (Serial.available()) {             // 受信データがあれば
    int bytesRead = Serial.available(); // 受信データバイト数を取得
    if (bytesRead <= 16) {              // 16byte以下なら
      for (int i = 0; i < 16; i++) {    // 書込みデータ配列を0で初期化
        write_data[i] = 0x00;
      }
      // 受信データを書込みデータ配列に格納
      for (int i = 0; i < bytesRead; i++) {
        write_data[i] = Serial.read();
      }
      Serial.printf("%s\n", write_data);
      writeData(sector_num, block_num); // データ書込み関数(セクター番号, ブロック番号)
    } else {
      Serial.println("文字数が16文字を超えています");
      while (Serial.available()) {      // 16byte以上のデータは破棄
        Serial.read();
      }
    }
  }

  // 液晶表示
  // カードデータ表示
  M5.Display.fillRect(0, 57, M5.Display.width(), M5.Display.height() - 57, TFT_BLACK);  // 液晶のデータ表示部を黒塗りリセット
  M5.Display.setCursor(0, 57);                  // 座標表示
  M5.Display.setTextSize(2);
  
  // タッチ操作エリアの表示
  M5.Display.setTextColor(TFT_YELLOW, TFT_BLACK);
  M5.Display.println("Touch: Sector+ | Block+ | Read | All");
  
  // UID表示
  String uidString = "";                                  // UIDシリアル
  char byte_string[3];                                    // 1byteの16進数表示を格納する文字配列を準備
  for (byte i = 0; i < mfrc522.uid.size; i++) {           // 取得したbyte数分繰り返す
    sprintf(byte_string, "%02X", mfrc522.uid.uidByte[i]); // 2桁で大文字の16進数として文字配列に格納
    uidString += String(byte_string) + " ";               // String型でスペース区切りで連結
  }
  M5.Display.setTextColor(TFT_CYAN, TFT_BLACK);
  M5.Display.printf("UID : %s\n", uidString.c_str());         // Cスタイルの文字列に変換して表示
  // Mifareカード(タグ)タイプ表示
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);  // SAK（Select Acknowledge:NFCタグの識別子）を取得し、変数に格納
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);                      // 文字色
  M5.Display.print("TYPE No. ");
  M5.Display.println(piccType);                               // SAKを表示
  M5.Display.print("TYPE Name : ");
  M5.Display.println(mfrc522.PICC_GetTypeName(piccType));     // SAKを基にタイプ名を表示
  // 書込みデータ、読み込みデータ確認表示
  M5.Display.setTextColor(TFT_GREEN, TFT_BLACK);
  M5.Display.printf("Sector : %d/Block : %d/Add : %d\n", sector_num, block_num, sector_num * 4 + block_num);  // ブロック情報
  M5.Display.printf("Write : %s\n", write_data);  // 書込みデータ
  M5.Display.setTextColor(TFT_ORANGE, TFT_BLACK);
  M5.Display.printf("Read : %s\n", read_data);    // 読み込みデータ
  
  delay(100); // 遅延時間
}
