#include <M5Unified.h>

// 画面クリアボタンの領域定義
#define CLEAR_BUTTON_W 80
#define CLEAR_BUTTON_H 30

static int clearButtonX = 0;
static int clearButtonY = 0;

void drawClearButton(void)
{
  clearButtonX = M5.Display.width() - 100;
  clearButtonY = M5.Display.height() - 40;
  
  M5.Display.setColor(TFT_DARKGRAY);
  M5.Display.fillRect(clearButtonX, clearButtonY, CLEAR_BUTTON_W, CLEAR_BUTTON_H);
  M5.Display.setColor(TFT_WHITE);
  M5.Display.drawRect(clearButtonX, clearButtonY, CLEAR_BUTTON_W, CLEAR_BUTTON_H);
  M5.Display.setTextDatum(textdatum_t::middle_center);
  M5.Display.drawString("Clear", clearButtonX + CLEAR_BUTTON_W / 2, clearButtonY + CLEAR_BUTTON_H / 2);
}

bool isClearButtonTouched(int x, int y)
{
  return (x >= clearButtonX && x < clearButtonX + CLEAR_BUTTON_W &&
          y >= clearButtonY && y < clearButtonY + CLEAR_BUTTON_H);
}

void setup(void)
{
  auto cfg = M5.config();
  M5.begin(cfg);
  
  M5.Display.setFont(&fonts::Font4);

  if (!M5.Touch.isEnabled())
  {
    M5.Display.setTextDatum(textdatum_t::middle_center);
    M5.Display.drawString("Touch not found.", M5.Display.width() / 2, M5.Display.height() / 2);
  }
  else
  {
    // クリアボタンを描画
    drawClearButton();
  }

  M5.Display.setEpdMode(epd_mode_t::epd_fastest);
  M5.Display.startWrite();
}

void loop(void)
{
  M5.update();
  
  lgfx::touch_point_t tp[3];

  int nums = M5.Display.getTouchRaw(tp, 3);
  if (nums)
  {
    M5.Display.convertRawXY(tp, nums);

    // クリアボタンがタッチされたかチェック
    bool clearPressed = false;
    for (int i = 0; i < nums; ++i)
    {
      if (isClearButtonTouched(tp[i].x, tp[i].y))
      {
        clearPressed = true;
        break;
      }
    }

    if (clearPressed)
    {
      // 画面をクリア
      M5.Display.waitDisplay();
      M5.Display.clear();
      M5.Display.display();
      // クリアボタンを再描画
      drawClearButton();
    }
    else
    {
      // 通常のタッチ表示処理
      for (int i = 0; i < nums; ++i)
      {
        M5.Display.setCursor(16, 16 + i * 24);
        M5.Display.printf("Raw X:%03d  Y:%03d", tp[i].x, tp[i].y);
      }

      for (int i = 0; i < nums; ++i)
      {
        M5.Display.setCursor(16, 128 + i * 24);
        M5.Display.printf("Convert X:%03d  Y:%03d", tp[i].x, tp[i].y);
      }
      M5.Display.display();

      M5.Display.setColor(M5.Display.isEPD() ? TFT_BLACK : TFT_WHITE);
      for (int i = 0; i < nums; ++i)
      {
        int s = tp[i].size + 3;
        switch (tp[i].id)
        {
        case 0:
          M5.Display.fillCircle(tp[i].x, tp[i].y, s);
          break;
        case 1:
          M5.Display.drawLine(tp[i].x-s, tp[i].y-s, tp[i].x+s, tp[i].y+s);
          M5.Display.drawLine(tp[i].x-s, tp[i].y+s, tp[i].x+s, tp[i].y-s);
          break;
        default:
          M5.Display.fillTriangle(tp[i].x-s, tp[i].y +s, tp[i].x+s, tp[i].y+s, tp[i].x, tp[i].y-s);
          break;
        }
        M5.Display.display();
      }
    }
  }
  delay(1);
}
