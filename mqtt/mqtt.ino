/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with M5Core2 sample source code
*                          配套  M5Core2 示例源代码
* Visit for more information: https://docs.m5stack.com/en/core/core2
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/core/core2
*
* Describe: MQTT.
* Date: 2021/11/5
* Modified: Converted to M5Unified
*******************************************************************************
*/
#include <M5Unified.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

// WiFi認証情報とMQTTサーバーは secrets.h から読み込みます
// secrets.h.example をコピーして secrets.h を作成し、実際のSSID、パスワード、MQTTサーバーを設定してください
// Configure the name and password of the connected wifi and your MQTT Server host.
// 配置所连接wifi的名称、密码以及你MQTT服务器域名
#include "secrets.h"

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

void setupWifi();
void callback(char* topic, byte* payload, unsigned int length);
void reConnect();

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(WHITE, BLACK);
    
    setupWifi();
    client.setServer(mqtt_server,
                     1883);  // Sets the server details.  配置所连接的服务器
    client.setCallback(
        callback);  // Sets the message callback function.  设置消息回调函数
}

void loop() {
    if (!client.connected()) {
        reConnect();
    }
    client.loop();  // This function is called periodically to allow clients to
                    // process incoming messages and maintain connections to the
                    // server.
    // 定期调用此函数，以允许主机处理传入消息并保持与服务器的连接

    unsigned long now =
        millis();  // Obtain the host startup duration.  获取主机开机时长
    if (now - lastMsg > 2000) {
        lastMsg = now;
        ++value;
        snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld",
                 value);  // Format to the specified string and store it in MSG.
                          // 格式化成指定字符串并存入msg中
        M5.Display.print("Publish message: ");
        M5.Display.println(msg);
        client.publish("M5Stack",
                       msg);  // Publishes a message to the specified topic.
                              // 发送一条消息至指定话题
        if (value % 12 == 0) {
            M5.Display.fillScreen(BLACK);
            M5.Display.setCursor(0, 0);
        }
    }
}

void setupWifi() {
    delay(10);
    M5.Display.printf("Connecting to %s", ssid);
    WiFi.mode(
        WIFI_STA);  // Set the mode to WiFi station mode.  设置模式为WIFI站模式
    WiFi.begin(ssid, password);  // Start Wifi connection.  开始wifi连接

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Display.print(".");
    }
    M5.Display.printf("\nSuccess\n");
}

void callback(char* topic, byte* payload, unsigned int length) {
    M5.Display.print("Message arrived [");
    M5.Display.print(topic);
    M5.Display.print("] ");
    for (int i = 0; i < length; i++) {
        M5.Display.print((char)payload[i]);
    }
    M5.Display.println();
}

void reConnect() {
    while (!client.connected()) {
        M5.Display.print("Attempting MQTT connection...");
        // Create a random client ID.  创建一个随机的客户端ID
        String clientId = "M5Stack-";
        clientId += String(random(0xffff), HEX);
        // Attempt to connect.  尝试重新连接
        if (client.connect(clientId.c_str())) {
            M5.Display.printf("\nSuccess\n");
            // Once connected, publish an announcement to the topic.
            // 一旦连接，发送一条消息至指定话题
            client.publish("M5Stack", "hello world");
            // ... and resubscribe.  重新订阅话题
            client.subscribe("M5Stack");
        } else {
            M5.Display.print("failed, rc=");
            M5.Display.print(client.state());
            M5.Display.println("try again in 5 seconds");
            delay(5000);
        }
    }
}