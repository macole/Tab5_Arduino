#include <M5Unified.h>    // ヘッダーを準備
#include <M5GFX.h>      // M5GFXライブラリのインクルード
M5Canvas canvas(&M5.Display);  // メモリ描画領域表示（スプライト）のインスタンスを作成(必要に応じて複数作成)

// 初期設定
void setup(void) {
  M5.begin();         // 本体初期化

  //M5.Display.clear(WHITE);
  //M5.Display.setTextDatum(middle_center);
  //M5.Display.setTextFont(&fonts::FreeMonoBold24pt7b);
  //M5.Display.setTextSize(1);
  //M5.Display.drawString("Speaker Test", M5.Display.width() / 2, M5.Display.height() / 2);

  // メモリ描画領域の初期設定（スプライト）
  //canvas.setColorDepth(8); // カラーモード設定（書かなければ初期値16bit。24bit（パネル性能によっては18bit）は対応していれば選択可）
                           // CORE2,Basic はメモリ描画領域サイズを大きく（320x173以上?）すると16bit以上で表示されないため8bit推奨
  canvas.createSprite(M5.Display.width(), M5.Display.height()/2); // メモリ描画領域サイズを縦半分の画面サイズで準備(必要に応じて複数作成)
}

// メイン処理
void loop(void) {
  // 指定した内容をその都度直接画面に表示（チラツキあり）--------------------------------------------------
  M5.Display.fillRect(0, 0, M5.Display.width(), 120, BLACK);  // 塗り潰し四角(指定色で画面上半分を塗りつぶす)
  M5.Display.setTextColor(WHITE);                      // テキスト色(文字色)
  M5.Display.setFont(&fonts::lgfxJapanGothic_40);      // フォント
  // テキスト表示
  M5.Display.setCursor(0, 0);                          // テキスト表示座標
  M5.Display.println("直接表示");
  M5.Display.println("チラツキあり");
  // グラデーション表示（直接表示）
  for (int i = 0; i < 50; i++) {
    M5.Display.drawGradientHLine( 0, 90 + i, 200, RED, GREEN);    // 赤から緑へのグラデーション水平線
    M5.Display.drawGradientHLine( 200, 90 + i, 200, GREEN, BLUE); // 緑から青へのグラデーション水平線
    M5.Display.drawGradientHLine( 400, 90 + i, 200, BLUE, RED);   // 青から赤へのグラデーション水平線
  }

  // メモリ内に描画した画面を一括出力（チラツキなし）-------------------------------------------------------
  canvas.fillRect(0, 0, M5.Display.width(), 120, BLACK); // 塗り潰し四角(指定色で範囲を塗りつぶす)
  canvas.setTextColor(WHITE);                     // テキスト色(文字色)
  canvas.setFont(&fonts::lgfxJapanGothic_40);     // フォント
  // テキスト表示
  canvas.setCursor(0, 0);                         // テキスト表示座標(メモリ描画領域)
  canvas.println("ﾒﾓﾘ描画領域表示");
  canvas.println("チラツキなし");
  // グラデーション表示（メモリ描画領域）
  for (int i = 0; i < 50; i++) {
    canvas.drawGradientHLine( 0, 90 + i, 200, RED, GREEN);   // 赤から緑へのグラデーション水平線
    canvas.drawGradientHLine( 200, 90 + i, 200, GREEN, BLUE);// 緑から青へのグラデーション水平線
    canvas.drawGradientHLine( 400, 90 + i, 200, BLUE, RED);  // 青から赤へのグラデーション水平線
  }
  // メモリ描画領域を座標を指定して一括表示（スプライト）
  canvas.pushSprite(&M5.Display, 0, M5.Display.height()/2); // スプライト画面が下半分に表示されるようy座標は高さの半分を指定 （全画面なら0）
  
  delay(100); // 遅延時間
}