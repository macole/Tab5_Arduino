#include <Arduino.h>
#include <algorithm>
#include <SPI.h>
#include <M5Unified.h>
// M5Unifiedの後にSD.hをincludeすることで、M5StackのSDライブラリを使用
#include <SD.h>

// ESP8266Audio ライブラリ
// ESP32-P4ではPDMがサポートされていないため、PDM関連のエラーを回避
#define SOC_I2S_SUPPORTS_PDM_TX 0
#include <AudioOutput.h>
#include <AudioFileSourceSD.h>
#include <AudioFileSourceID3.h>
#include <AudioGeneratorMP3.h>

// Tab5のSDカードピン設定
#define SD_SPI_CS_PIN   42
#define SD_SPI_SCK_PIN  43
#define SD_SPI_MOSI_PIN 44
#define SD_SPI_MISO_PIN 39

// M5Speaker用の仮想チャンネル
static constexpr uint8_t m5spk_virtual_channel = 0;

// 最大ファイル数
#define MAX_FILES 100

// M5Speaker用のAudioOutputクラス
class AudioOutputM5Speaker : public AudioOutput
{
  public:
    AudioOutputM5Speaker(m5::Speaker_Class* m5sound, uint8_t virtual_sound_channel = 0)
    {
      _m5sound = m5sound;
      _virtual_ch = virtual_sound_channel;
    }
    virtual ~AudioOutputM5Speaker(void) {};
    virtual bool begin(void) override { return true; }
    virtual bool ConsumeSample(int16_t sample[2]) override
    {
      if (_tri_buffer_index < tri_buf_size)
      {
        _tri_buffer[_tri_index][_tri_buffer_index  ] = sample[0];
        _tri_buffer[_tri_index][_tri_buffer_index+1] = sample[1];
        _tri_buffer_index += 2;
        return true;
      }
      flush();
      return false;
    }
    virtual void flush(void) override
    {
      if (_tri_buffer_index)
      {
        _m5sound->playRaw(_tri_buffer[_tri_index], _tri_buffer_index, hertz, true, 1, _virtual_ch);
        _tri_index = _tri_index < 2 ? _tri_index + 1 : 0;
        _tri_buffer_index = 0;
      }
    }
    virtual bool stop(void) override
    {
      flush();
      _m5sound->stop(_virtual_ch);
      return true;
    }

  protected:
    m5::Speaker_Class* _m5sound;
    uint8_t _virtual_ch;
    static constexpr size_t tri_buf_size = 1536;
    int16_t _tri_buffer[3][tri_buf_size];
    size_t _tri_buffer_index = 0;
    size_t _tri_index = 0;
};

// グローバル変数
String mp3Files[MAX_FILES];
int fileCount = 0;
int selectedIndex = 0;
int listScrollOffset = 0;
bool isPlaying = false;
bool isPaused = false;

// MP3再生関連
AudioGeneratorMP3 *mp3 = nullptr;
AudioFileSourceSD *file = nullptr;
AudioFileSourceID3 *id3 = nullptr;
AudioOutputM5Speaker *out = nullptr;

// UI定数
const int LIST_START_Y = 80;
const int LIST_ITEM_HEIGHT = 30;
const int MAX_VISIBLE_ITEMS = 15;
const int BUTTON_AREA_HEIGHT = 100;
int buttonAreaY = 0;
int buttonWidth = 0;

// MP3ファイル検出関数
void scanMP3Files() {
  fileCount = 0;
  File root = SD.open("/");
  if (!root) {
    Serial.println("Failed to open root directory");
    return;
  }
  
  if (!root.isDirectory()) {
    Serial.println("Root is not a directory");
    root.close();
    return;
  }
  
  File file = root.openNextFile();
  while (file && fileCount < MAX_FILES) {
    if (!file.isDirectory()) {
      String fileName = String(file.name());
      if (fileName.endsWith(".mp3") || fileName.endsWith(".MP3")) {
        if (!fileName.startsWith("/")) {
          fileName = "/" + fileName;
        }
        mp3Files[fileCount] = fileName;
        fileCount++;
        Serial.printf("Found MP3: %s\n", fileName.c_str());
      }
    }
    file = root.openNextFile();
  }
  root.close();
  
  Serial.printf("Total MP3 files found: %d\n", fileCount);
}

// 画面描画関数
void drawUI() {
  M5.Display.clear();
  M5.Display.setTextColor(WHITE);
  M5.Display.setFont(&fonts::lgfxJapanMinchoP_16);
  
  // ヘッダー
  M5.Display.setCursor(10, 10);
  M5.Display.printf("MP3 Player");
  M5.Display.setCursor(200, 10);
  M5.Display.printf("Files: %d", fileCount);
  
  // ボリューム表示
  uint8_t volume = M5.Speaker.getVolume();
  int volumePercent = (volume * 100) / 255;
  M5.Display.setCursor(10, 30);
  M5.Display.setTextColor(CYAN);
  M5.Display.printf("Volume: %d%%", volumePercent);
  
  // 再生状態表示
  M5.Display.setCursor(10, 50);
  if (isPlaying && !isPaused) {
    M5.Display.setTextColor(GREEN);
    M5.Display.print("▶ Playing: ");
  } else if (isPaused) {
    M5.Display.setTextColor(YELLOW);
    M5.Display.print("⏸ Paused: ");
  } else {
    M5.Display.setTextColor(WHITE);
    M5.Display.print("⏹ Stopped");
  }
  
  if (selectedIndex >= 0 && selectedIndex < fileCount) {
    String fileName = mp3Files[selectedIndex];
    // ファイル名からパスを除く
    int lastSlash = fileName.lastIndexOf('/');
    if (lastSlash >= 0) {
      fileName = fileName.substring(lastSlash + 1);
    }
    M5.Display.print(fileName);
  }
  
  M5.Display.setTextColor(WHITE);
  M5.Display.drawLine(0, 70, M5.Display.width(), 70, WHITE);
  
  // ファイルリスト表示
  M5.Display.setCursor(10, LIST_START_Y);
  M5.Display.print("File List:");
  
  int visibleStart = listScrollOffset;
  int visibleEnd = min(listScrollOffset + MAX_VISIBLE_ITEMS, fileCount);
  
  for (int i = visibleStart; i < visibleEnd; i++) {
    int y = LIST_START_Y + 30 + (i - visibleStart) * LIST_ITEM_HEIGHT;
    
    if (i == selectedIndex) {
      // 選択中のファイルをハイライト
      M5.Display.fillRect(5, y - 5, M5.Display.width() - 10, LIST_ITEM_HEIGHT, CYAN);
      M5.Display.setTextColor(BLACK);
    } else {
      M5.Display.setTextColor(WHITE);
    }
    
    String fileName = mp3Files[i];
    // ファイル名からパスを除く
    int lastSlash = fileName.lastIndexOf('/');
    if (lastSlash >= 0) {
      fileName = fileName.substring(lastSlash + 1);
    }
    
    M5.Display.setCursor(15, y);
    if (i == selectedIndex) {
      M5.Display.print("> ");
    } else {
      M5.Display.print("  ");
    }
    M5.Display.print(fileName);
  }
  
  // ボタンエリア
  M5.Display.drawLine(0, buttonAreaY, M5.Display.width(), buttonAreaY, WHITE);
  M5.Display.setTextColor(WHITE);
  const char* btnLabels1[5] = {"[<<]", "[=]", "[>>]", "[Vol-]", "[Vol+]"};
  const char* btnLabels2[5] = {"Prev", "Stop", "Next", "Vol-", "Vol+"};
  for (int i = 0; i < 5; i++) {
    int centerX = buttonWidth * i + buttonWidth / 2;
    int label1Width = M5.Display.textWidth(btnLabels1[i]);
    int label2Width = M5.Display.textWidth(btnLabels2[i]);
    M5.Display.setCursor(centerX - label1Width / 2, buttonAreaY + 20);
    M5.Display.print(btnLabels1[i]);
    M5.Display.setCursor(centerX - label2Width / 2, buttonAreaY + 45);
    M5.Display.print(btnLabels2[i]);
  }
}

// MP3再生関数
void playMP3(int index) {
  if (index < 0 || index >= fileCount) {
    Serial.println("Invalid file index");
    return;
  }
  
  // 既存の再生を停止
  stopMP3();
  
  String filePath = mp3Files[index];
  Serial.printf("Playing: %s\n", filePath.c_str());
  
  file = new AudioFileSourceSD(filePath.c_str());
  if (!file || !file->isOpen()) {
    Serial.printf("Failed to open file: %s\n", filePath.c_str());
    delete file;
    file = nullptr;
    return;
  }
  
  id3 = new AudioFileSourceID3(file);
  out = new AudioOutputM5Speaker(&M5.Speaker, m5spk_virtual_channel);
  mp3 = new AudioGeneratorMP3();
  
  if (mp3->begin(id3, out)) {
    isPlaying = true;
    isPaused = false;
    selectedIndex = index;
    drawUI();
    Serial.println("MP3 playback started");
  } else {
    Serial.println("Failed to start MP3 playback");
    stopMP3();
  }
}

// MP3停止関数
void stopMP3() {
  if (mp3) {
    mp3->stop();
    delete mp3;
    mp3 = nullptr;
  }
  if (id3) {
    delete id3;
    id3 = nullptr;
  }
  if (file) {
    delete file;
    file = nullptr;
  }
  if (out) {
    delete out;
    out = nullptr;
  }
  isPlaying = false;
  isPaused = false;
}

// タッチ処理関数
void handleTouch() {
  auto t = M5.Touch.getDetail();
  if (t.wasPressed()) {
    int x = t.x;
    int y = t.y;
    
    // ファイルリストエリアのタッチ判定
    if (y >= LIST_START_Y + 30 && y < buttonAreaY) {
      int itemIndex = listScrollOffset + (y - LIST_START_Y - 30) / LIST_ITEM_HEIGHT;
      if (itemIndex >= 0 && itemIndex < fileCount) {
        selectedIndex = itemIndex;
        playMP3(selectedIndex);
      }
    }
    // ボタンエリアのタッチ判定
    else if (y >= buttonAreaY && y <= buttonAreaY + BUTTON_AREA_HEIGHT) {
      int section = buttonWidth > 0 ? min(max(x / buttonWidth, 0), 4) : 0;
      switch (section) {
        case 0: // 前の曲
          if (selectedIndex > 0) {
            selectedIndex--;
            playMP3(selectedIndex);
          }
          break;
        case 1: // 停止
          stopMP3();
          drawUI();
          break;
        case 2: // 次の曲
          if (selectedIndex < fileCount - 1) {
            selectedIndex++;
            playMP3(selectedIndex);
          }
          break;
        case 3: { // 音量- 256/20=13Step
          uint8_t vol = M5.Speaker.getVolume();
          if (vol > 0) {
            M5.Speaker.setVolume(vol - 13);
            drawUI();
          }
          break;
        }
        case 4: { // 音量+
          uint8_t vol = M5.Speaker.getVolume();
          if (vol < 255) {
            M5.Speaker.setVolume(vol + 13);
            drawUI();
          }
          break;
        }
      }
    }
  }
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  
  Serial.begin(115200);
  Serial.println("MP3 Player Starting...");
  
  // ディスプレイ設定
  M5.Display.setRotation(1);
  M5.Display.clear();
  M5.Display.setTextColor(WHITE);
  M5.Display.setFont(&fonts::lgfxJapanMinchoP_16);
  M5.Display.setCursor(10, 10);
  M5.Display.print("Initializing...");
  buttonAreaY = std::max<int>(M5.Display.height() - BUTTON_AREA_HEIGHT, LIST_START_Y + 30 + (MAX_VISIBLE_ITEMS * LIST_ITEM_HEIGHT));
  buttonWidth = std::max<int>(1, M5.Display.width() / 5);
  
  // SDカード初期化
  M5.Display.setCursor(10, 40);
  M5.Display.print("SD Card: ");
  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
  // Tab5のSDカードピンを使用（GPIO_NUM_4ではなく、SD_SPI_CS_PINを使用）
  if (!SD.begin((gpio_num_t)SD_SPI_CS_PIN, SPI, 25000000)) {
    M5.Display.setTextColor(RED);
    M5.Display.print("FAILED");
    Serial.println("SD card initialization failed!");
    while (1) {
      delay(1000);
    }
  } else {
    M5.Display.setTextColor(GREEN);
    M5.Display.print("OK");
    Serial.println("SD card initialized");
  }
  
  // スピーカー設定
  {
    auto spk_cfg = M5.Speaker.config();
    spk_cfg.sample_rate = 96000; // 高品質再生
    M5.Speaker.config(spk_cfg);
  }
  M5.Speaker.begin();
  M5.Speaker.setVolume(128); // 初期音量50%
  
  // MP3ファイル検出
  M5.Display.setCursor(10, 70);
  M5.Display.setTextColor(WHITE);
  M5.Display.print("Scanning MP3 files...");
  scanMP3Files();
  
  if (fileCount == 0) {
    M5.Display.setCursor(10, 100);
    M5.Display.setTextColor(RED);
    M5.Display.print("No MP3 files found!");
    Serial.println("No MP3 files found on SD card");
    delay(3000);
  } else {
    M5.Display.setCursor(10, 100);
    M5.Display.setTextColor(GREEN);
    M5.Display.printf("Found %d MP3 files", fileCount);
    Serial.printf("Found %d MP3 files\n", fileCount);
    delay(1000);
  }
  
  // UI描画
  drawUI();
  
  Serial.println("MP3 Player Ready");
}

void loop() {
  M5.update();
  
  // MP3再生処理
  if (mp3 && mp3->isRunning()) {
    if (!mp3->loop()) {
      // 再生終了
      mp3->stop();
      Serial.println("Playback finished");
      
      // 次の曲に自動スキップ
      if (selectedIndex < fileCount - 1) {
        selectedIndex++;
        playMP3(selectedIndex);
      } else {
        stopMP3();
        drawUI();
      }
    }
  }
  
  // タッチ処理
  if (M5.Touch.getCount() > 0) {
    handleTouch();
  }
  
  delay(10);
}

