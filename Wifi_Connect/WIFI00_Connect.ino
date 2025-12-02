#include <M5Unified.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

//const char* ssid = "your_ssid";
//const char* pass = "your_pass";
const char *ssid     = "TP-Link_3DE3";
const char *pass = "11411550";

const char* rootCA =
"-----BEGIN CERTIFICATE-----\n" \
"MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n" \
"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n" \
"VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n" \
"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n" \
"ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n" \
"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n" \
"mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n" \
"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n" \
"mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n" \
"XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n" \
"dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n" \
"jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n" \
"BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n" \
"DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n" \
"9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n" \
"jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n" \
"Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n" \
"ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n" \
"R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n" \
"-----END CERTIFICATE-----\n";

void setup() {
  // M5Unifiedの初期化
  auto cfg = M5.config();
  M5.begin(cfg);
  
  // シリアルポートの初期化
  Serial.begin(115200);
  delay(500);
  
  // ディスプレイの初期化
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(WHITE, BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.println("WiFi Connecting...");
  M5.Display.println(ssid);
  
  // アクセスポイントに接続
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Display.print(".");
  }
  
  // ESP32のIPアドレスを出力
  Serial.println("WiFi Connected.");
  Serial.print("IP = ");
  Serial.println(WiFi.localIP());
  
  // ディスプレイにIPアドレスを表示
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextSize(5);
  M5.Display.println("WiFi Connected!");
  M5.Display.println();
  M5.Display.setTextSize(3);
  M5.Display.print("IP: ");
  M5.Display.println(WiFi.localIP());
  M5.Display.println();
  M5.Display.println("Setting time...");
  
  // 時刻を合わせる
  setClock();
  
  M5.Display.println("Time set!");
  delay(1000);

  // ディスプレイをクリア
  M5.Display.fillScreen(BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextSize(5);
  M5.Display.println("Connecting to");
  M5.Display.println("Yahoo News...");
  
  // WiFiClientSecureクラスのオブジェクトを生成する
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    // ルート証明書を設定する
    client->setCACert(rootCA);
    {
      // https://news.yahoo.co.jp/pickup/rss.xmlにアクセスする
      HTTPClient https;
      if (https.begin(*client, "https://news.yahoo.co.jp/pickup/rss.xml")) {
        M5.Display.println("Requesting...");
        int status = https.GET();
        if (status > 0) {
          if (status == HTTP_CODE_OK) {
            // 得られたXMLをシリアルモニタに出力する
            String xml = https.getString();
            Serial.print(xml);
            
            // ディスプレイに成功メッセージを表示
            M5.Display.fillScreen(BLACK);
            M5.Display.setCursor(0, 0);
            M5.Display.setTextSize(5);
            M5.Display.println("Success!");
            M5.Display.println();
            M5.Display.setTextSize(1);
            M5.Display.println("HTTP Status: OK");
            M5.Display.print("Data length: ");
            M5.Display.println(xml.length());
            M5.Display.println();
            M5.Display.println("Check Serial");
            M5.Display.println("for XML data");
          }
          else {
            Serial.print("HTTP Error ");
            Serial.println(status);
            
            // ディスプレイにエラーを表示
            M5.Display.fillScreen(BLACK);
            M5.Display.setCursor(0, 0);
            M5.Display.setTextSize(5);
            M5.Display.println("HTTP Error");
            M5.Display.setTextSize(3);
            M5.Display.print("Status: ");
            M5.Display.println(status);
          }
        }
        else {
          Serial.println("Get Failed");
          
          // ディスプレイにエラーを表示
          M5.Display.fillScreen(BLACK);
          M5.Display.setCursor(0, 0);
          M5.Display.setTextSize(2);
          M5.Display.println("Get Failed");
        }
        https.end();
      }
      else {
        Serial.print("Connect error");
        
        // ディスプレイにエラーを表示
        //M5.Display.fillScreen(BLACK);
        M5.Display.setCursor(0, 0);
        M5.Display.setTextSize(3);
        M5.Display.println("Connect Error");
      }
    }
    delete client;
  }
}

void loop() {
  M5.update();
  delay(100);
}

void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    yield();
    nowSecs = time(nullptr);
  }
}
