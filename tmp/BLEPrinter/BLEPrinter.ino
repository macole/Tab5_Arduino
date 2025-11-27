/**
 * Test print for BlueTooth Printer
 * Copyright 2020 fukuebiz
 * 
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 */
#include <M5Unified.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// debug log
# define log_d0(x) Serial.printf(x)
# define log_d(x, y) Serial.printf(x, y)
# define log_d2(x, y, z) Serial.printf(x, y, z)

# define PRINTER_NAME "BlueTooth Printer" // Printer BD Name
static esp_bd_addr_t peer_bd_addr = {0x11,0x22,0x33,0x44,0x55,0x66}; // Printer BD Adress

static String printer_service_uuid = "";
static String printer_characteristic_uuid = "";
std::map<std::string, BLERemoteService*> *pRemoteServices;
BLERemoteService *printerService;
std::map<std::string, BLERemoteCharacteristic*> *pRemoteCharacteristics;
BLERemoteCharacteristic *printerCharacteristic;
std::map<std::string, BLERemoteDescriptor*> *pRemoteDescriptors;
BLERemoteDescriptor *printerDescriptor;

volatile boolean deviceBleConnected = false; // flag BLE conneted

/*
 * Client callback
 */
class PrinterClientCallbacks : public BLEClientCallbacks
{
  void onConnect(BLEClient *pClient)
  {
    deviceBleConnected = true; // set ble connected flag
    log_d("%s connected\n", PRINTER_NAME);
  }

  void onDisconnect(BLEClient *pClient)
  {
    pClient->disconnect();
    deviceBleConnected = false; // clear ble connected flag
    log_d("%s disconnected\n", PRINTER_NAME);
  }
};

/*
 * Notify Callback
 */
static void notifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData, size_t length, bool isNotify)
{
    log_d2("notifyCallbacklength=%d, isNofity=%d\n", length, isNotify);
    log_d("uuid: %s\n", pBLERemoteCharacteristic->getUUID().toString().c_str());
}

/*
 * Connect to printer
 */
bool connectToServer(BLEAddress pAddress)
{
    log_d("Create a connection to addr: %s\n", pAddress.toString().c_str());
    BLEClient *pClient = BLEDevice::createClient();

    log_d0(" – Client created\n");
    pClient->setClientCallbacks(new PrinterClientCallbacks());

    log_d0(" – Connecting to server…\n");
    if (pClient->connect(pAddress)) // connect to the remove BLE Server.
    {
        log_d0(" - Connected to printer\n");
    }
    else
    {
        log_d0(" - Connection failed\n");
        return false;
    }

    log_d0("Retrieving service UUIDs...\n");
    pRemoteServices = pClient->getServices();
    for (const auto &p : *pRemoteServices) {
        String uuid = p.first.c_str();
        log_d("UUID: %s\n", uuid.c_str());
        if (!uuid.endsWith("-0000-1000-8000-00805f9b34fb"))
        {
            printer_service_uuid = uuid;
        }
    }
    if (printer_service_uuid == "")
    {
        log_d0(" – Printer service not found\n");
        return false;
    }

    printerService = pRemoteServices->at(printer_service_uuid.c_str());
    if (printerService == nullptr)
    {
        log_d(" – Service not found (UUID: %s)\n", printer_service_uuid.c_str());
        return false;
    }
    else
    {
        log_d(" – Service found (UUID: %s)\n", printer_service_uuid.c_str());
    }

    // printer characteristic
    log_d0("Retrieving printer characteristics...");
    pRemoteCharacteristics = printerService->getCharacteristics();
    for (const auto &p : *pRemoteCharacteristics) {
        String uuid = p.first.c_str();
        log_d("UUID: %s\n", uuid.c_str());
    }
    if (pRemoteCharacteristics->size() > 0)
    {
        printerCharacteristic = printerService->getCharacteristic(pRemoteCharacteristics->begin()->first.c_str());
        if (printerCharacteristic == nullptr)
        {
            log_d(" – Notify characteristic not found (UUID: %s)\n", pRemoteCharacteristics->begin()->first.c_str());
            return false;
        }
        else
        {
            log_d(" – Notify characteristic found (UUID: %s)\n", pRemoteCharacteristics->begin()->first.c_str());
        }
    }
    else
    {
        log_d0(" – Notify characteristic not found\n");
        return false;
    }
    printerCharacteristic->registerForNotify(notifyCallback, true); //register callback

  return true;
}

/*
 * Scan device & log
 */
void scan()
{
    log_d0("UUID, Name, RSSI, Manufacturer\n");

    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(false);
    BLEScanResults* foundDevices = pBLEScan->start(3);
    int count = foundDevices->getCount();
    for (int i = 0; i < count; i++)
    {
        BLEAdvertisedDevice d = foundDevices->getDevice(i);
        log_d0(d.getAddress().toString().c_str());
        log_d0(", ");
        log_d0(d.getName().c_str());
        log_d0(", ");
        log_d("%d", d.getRSSI());
        log_d0(", ");
        if (d.haveManufacturerData())
        {
            String data = d.getManufacturerData();
            if (data.length() >= 2)
            {
                int manu = ((uint8_t)data[1]) << 8 | ((uint8_t)data[0]);
                log_d("%d", manu);
            }
        }
        log_d0("\n");
    }
    pBLEScan->clearResults();
}

void testCharacter()
{
    printerCharacteristic->writeValue(0x1b); // NORMAL TEXT
    printerCharacteristic->writeValue(0x21);
    printerCharacteristic->writeValue(0x00);
    printerCharacteristic->writeValue("[Font A]\n");
    printerCharacteristic->writeValue("!\"#$%&\'()*+,-./01234567890:;<=>?\n");
    printerCharacteristic->writeValue("@ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
    printerCharacteristic->writeValue(0x1b); // FONT Type B
    printerCharacteristic->writeValue(0x4d);
    printerCharacteristic->writeValue(0x01);
    printerCharacteristic->writeValue("[Font B]\n");
    printerCharacteristic->writeValue("!\"#$%&\'()*+,-./01234567890:;<=>?\n");
    printerCharacteristic->writeValue("@ABCDEFGHIJKLMNOPQRSTUVWXYZ\n");
}

void testJapanese()
{
    printerCharacteristic->writeValue(0x1b); // NORMAL TEXT
    printerCharacteristic->writeValue(0x21);
    printerCharacteristic->writeValue(0x00);
    printerCharacteristic->writeValue("[Chinese]\n");
    printerCharacteristic->writeValue(0x1c); // CHINESE MODE ON
    printerCharacteristic->writeValue(0x26);
    // 你好
    uint8_t chinese_data[] = {'\xc4', '\xe3', '\xba', '\xc3', '\x0a', '\0'};
    printerCharacteristic->writeValue(chinese_data, sizeof(chinese_data));
    printerCharacteristic->writeValue(0x1c); // CHINESE MODE OFF
    printerCharacteristic->writeValue(0x2e);
    printerCharacteristic->writeValue("[Japanese Hiragana]\n");
    printerCharacteristic->writeValue(0x1c); // CHINESE MODE ON
    printerCharacteristic->writeValue(0x26);
    // あいうえお
    uint8_t hiragana_data[] = {'\xA4', '\xA2', '\xA4', '\xA4', '\xA4', '\xA6', '\xA4', '\xA8', '\xA4', '\xAA', '\x0a', '\0'};
    printerCharacteristic->writeValue(hiragana_data, sizeof(hiragana_data));
    // １２３４５
    uint8_t numbers1_data[] = {'\xA3', '\xB1', '\xA3', '\xB2', '\xA3', '\xB3', '\xA3', '\xB4', '\xA3', '\xB5'};
    printerCharacteristic->writeValue(numbers1_data, sizeof(numbers1_data));
    // ６７８９０
    uint8_t numbers2_data[] = {'\xA3', '\xB6', '\xA3', '\xB7', '\xA3', '\xB8', '\xA3', '\xB9', '\xA3', '\xB0', '\x0a', '\0'};
    printerCharacteristic->writeValue(numbers2_data, sizeof(numbers2_data));
    printerCharacteristic->writeValue(0x1c); // CHINESE MODE OFF
    printerCharacteristic->writeValue(0x2e);
}

/*
 * Test print
 */
void testPrint()
{
    for (int i = 0; i < 3; i++)
    {
        if (deviceBleConnected)
        {
            testCharacter();
            testJapanese();
            break;
        }
        connectToServer(peer_bd_addr);
        sleep(3);
    }
}

/*
 * Setup
 */
void setup()
{
    auto cfg = M5.config();
    M5.begin(cfg);
    
    Serial.begin(115200);
    delay(500);
    
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(WHITE, BLACK);
    M5.Display.setCursor(0, 0);
    M5.Display.println("BLE print test.");
    M5.Display.println();
    M5.Display.println("A button: scan BLE & log.");
    M5.Display.println("B button: test print.");

    BLEDevice::init("M5Stack");
}

/*
 * main loop
 */
void loop()
{
    if(M5.BtnA.wasPressed())
    {
        scan();
    }
    if(M5.BtnB.wasPressed())
    {
        testPrint();
    }
    M5.update();
}

