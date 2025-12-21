/*
 * Conway's Game of Life - M5Stack Tab5版
 * 
 * The Game of Life, also known simply as Life, is a cellular automaton
 * devised by the British mathematician John Horton Conway in 1970.
 * https://en.wikipedia.org/wiki/Conway's_Game_of_Life
 */

#include <M5Unified.h>

M5Canvas canvas[2];

void setup(void) {
  auto cfg = M5.config();
  M5.begin(cfg);
  
  M5.Display.setColorDepth(8);
  M5.Display.setEpdMode(epd_mode_t::epd_fastest);

  // 横向きに設定（Tab5は1280x720）
  if (M5.Display.width() < M5.Display.height()) {
    M5.Display.setRotation(M5.Display.getRotation() ^ 1);
    M5.Display.setPivot(M5.Display.width() / 2 - 0.5, M5.Display.height() / 2 - 0.5);
  }

  // キャンバスサイズを計算（最大192x160）
  int z = 0;
  int width;
  do {
    width = M5.Display.width() / ++z;
  } while (width > 192);

  z = 0;
  int height;
  do {
    height = M5.Display.height() / ++z;
  } while (height > 160);

  // 2つのキャンバスを作成（ダブルバッファリング用）
  for (int i = 0; i < 2; i++) {
    canvas[i].setColorDepth(8);
    canvas[i].createSprite(width, height);
    canvas[i].createPalette();
    canvas[i].setPaletteColor(1, TFT_WHITE);
    canvas[i].setPivot(canvas[i].width() / 2 - 0.5, canvas[i].height() / 2 - 0.5);
  }
  
  // タイトル表示
  canvas[0].setTextColor(1);
  canvas[0].setTextDatum(textdatum_t::bottom_center);
  canvas[0].drawString("Macole's", canvas[0].width() >> 1, canvas[0].height() >> 1);
  canvas[0].setTextDatum(textdatum_t::top_center);
  canvas[0].drawString("Game of Life", canvas[0].width() >> 1, canvas[0].height() >> 1);
  canvas[0].pushRotateZoom(&M5.Display, 0, (float)M5.Display.width() / canvas[0].width(), 
                           (float)M5.Display.height() / canvas[0].height());
  delay(2000);
  M5.Display.clear();
}

void loop(void) {
  bool flip = false;
  int width = canvas[flip].width();
  int height = canvas[flip].height();
  int xz = M5.Display.width() / width;
  int yz = M5.Display.height() / height;

  // 初期パターンをランダムに生成
  int y = 1;
  do {
    int x = 1;
    do {
      if (random(6) == 0) { 
        canvas[flip].drawPixel(x, y, 1); 
      }
    } while (++x < width - 1);
  } while (++y < height - 1);

  int diff;
  M5.Display.startWrite();
  
  // ライフゲームのメインループ
  do {
    flip = !flip;
    diff = 0;

    auto old_buf = (uint8_t*)canvas[!flip].getBuffer();
    auto new_buf = (uint8_t*)canvas[flip].getBuffer();
    int width = canvas[flip].width();
    int height = canvas[flip].height();
    
    int py;
    int y = height - 1;
    int ny = 0;
    
    do {
      py = y;
      y = ny;
      if (++ny == height) ny = 0;

      int px;
      int x = width - 1;
      int nx = 0;
      
      do {
        px = x;
        x = nx;
        if (++nx == width) nx = 0;

        // 周囲8マスの生存セル数をカウント
        int neighbors = old_buf[px + py * width]
                      + old_buf[x + py * width]
                      + old_buf[nx + py * width]
                      + old_buf[px + y * width]
                      + old_buf[nx + y * width]
                      + old_buf[px + ny * width]
                      + old_buf[x + ny * width]
                      + old_buf[nx + ny * width];
        
        int idx = x + y * width;
        
        // ライフゲームのルール
        // - 生存セル: 周囲に2-3個の生存セルがあれば生存
        // - 死亡セル: 周囲に3個の生存セルがあれば誕生
        bool flg = (neighbors == 3) || (neighbors == 2 && old_buf[idx]);
        
        if (flg != new_buf[idx]) {
          M5.Display.fillRect(x * xz, y * yz, xz, yz, flg ? TFT_WHITE : TFT_BLACK);
          new_buf[idx] = flg;
          ++diff;
        }
      } while (nx);
    } while (ny);

    M5.Display.display();
  } while (diff);  // 変化がなくなるまで続ける
  
  M5.Display.endWrite();
}
