#include "tinyusb.h"
#include "class/hid/hid_device.h"
#include <hal/hal.h>

// HID レポート記述子(キーボード)
static const uint8_t hid_keyboard_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()
};

// TinyUSB HID コールバック
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                                hid_report_type_t report_type,
                                uint8_t* buffer, uint16_t reqlen)
{
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                            hid_report_type_t report_type,
                            uint8_t const* buffer, uint16_t bufsize)
{
}

class UsbKeyboardEmulatorApp : public mooncake::AppBase {
private:
    lv_obj_t* _screen = nullptr;
    lv_obj_t* _keyboard = nullptr;
    bool _usb_initialized = false;
    
public:
    void onCreate() override {
        setAppInfo().name = "USB Keyboard Emulator";
        initUsbDevice();
    }
    
    void initUsbDevice() {
        // TinyUSB設定
        const tinyusb_config_t tusb_cfg = {
            .device_descriptor = NULL,  // デフォルト使用
            .string_descriptor = NULL,  // デフォルト使用
            .external_phy = false,
            .configuration_descriptor = NULL,
        };
        
        ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
        
        // HID デバイス初期化
        const tinyusb_config_hid_t hid_cfg = {
            .subclass = HID_SUBCLASS_NONE,
            .protocol = HID_ITF_PROTOCOL_KEYBOARD,
            .report_desc = hid_keyboard_report_descriptor,
            .report_desc_len = sizeof(hid_keyboard_report_descriptor),
        };
        
        ESP_ERROR_CHECK(tusb_hid_init(&hid_cfg));
        _usb_initialized = true;
    }
    
    void onOpen() override {
        LvglLockGuard lock;
        
        // UI作成
        _screen = lv_obj_create(lv_scr_act());
        lv_obj_set_size(_screen, GetHAL()->getDisplayWidth(), 
                       GetHAL()->getDisplayHeight());
        
        // タイトル
        lv_obj_t* title = lv_label_create(_screen);
        lv_label_set_text(title, "USB Keyboard Emulator");
        lv_obj_set_style_text_font(title, &lv_font_montserrat_28, 0);
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
        
        // ステータス
        lv_obj_t* status = lv_label_create(_screen);
        lv_label_set_text(status, tud_mounted() ? 
                         "USB Connected" : "Connect to PC via USB-C");
        lv_obj_align(status, LV_ALIGN_TOP_MID, 0, 50);
        
        // 仮想キーボードボタン配列
        createVirtualKeyboard();
    }
    
    void createVirtualKeyboard() {
        // 簡易的な仮想キーボードUI
        const char* keys[] = {
            "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
            "A", "S", "D", "F", "G", "H", "J", "K", "L",
            "Z", "X", "C", "V", "B", "N", "M"
        };
        
        int x_offset = 50;
        int y_offset = 150;
        int btn_width = 80;
        int btn_height = 60;
        int spacing = 10;
        
        for (int row = 0; row < 3; row++) {
            int keys_in_row = (row == 0) ? 10 : (row == 1) ? 9 : 7;
            
            for (int col = 0; col < keys_in_row; col++) {
                int key_index = (row == 0) ? col : 
                                (row == 1) ? 10 + col : 19 + col;
                
                lv_obj_t* btn = lv_btn_create(_screen);
                lv_obj_set_size(btn, btn_width, btn_height);
                lv_obj_set_pos(btn, 
                    x_offset + col * (btn_width + spacing),
                    y_offset + row * (btn_height + spacing));
                
                lv_obj_add_event_cb(btn, key_event_cb, 
                                   LV_EVENT_CLICKED, 
                                   (void*)(intptr_t)key_index);
                
                lv_obj_t* label = lv_label_create(btn);
                lv_label_set_text(label, keys[key_index]);
                lv_obj_center(label);
            }
        }
    }
    
    static void key_event_cb(lv_event_t* e) {
        int key_index = (int)(intptr_t)lv_event_get_user_data(e);
        
        // USBデバイスが接続されているか確認
        if (!tud_mounted()) {
            return;
        }
        
        // HID キーボードレポート送信
        uint8_t keycode = HID_KEY_A + key_index;  // 簡易変換
        
        // キー押下レポート
        uint8_t report[8] = {0};
        report[2] = keycode;  // キーコード
        tud_hid_keyboard_report(1, 0, report);
        
        // 少し待機
        vTaskDelay(pdMS_TO_TICKS(50));
        
        // キー解放レポート
        uint8_t release_report[8] = {0};
        tud_hid_keyboard_report(1, 0, release_report);
    }
    
    void onClose() override {
        if (_screen) {
            LvglLockGuard lock;
            lv_obj_del(_screen);
            _screen = nullptr;
        }
    }
};