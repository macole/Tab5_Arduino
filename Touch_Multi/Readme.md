# Touch Multi Touch プログラム解説

## プログラム全体の概要

このプログラムは、M5Stack Tab5 が持つ 5 点マルチタッチ機能を利用し、同時に触れている指の座標をリアルタイムに表示します。`M5.Lcd.getTouchRaw()` を直接使用して生のタッチ座標を取得し、タッチの有無に応じて画面表示を切り替えます。

## プログラムの流れ

### 1. 初期設定（1-20行目）

```1:20:Touch_Multi/Touch_Multi.ino
#include <M5Unified.h>
#include <M5GFX.h>

m5::touch_point_t touchPoint[5];//Tab5 supports up to 5-point touch
static bool drawed = false;
static int32_t w;
static int32_t h;

void setup() {
  M5.begin();

  w = M5.Lcd.width();
  h = M5.Lcd.height();
  
  M5.Lcd.fillScreen(WHITE);
  M5.Display.setRotation(0);
  M5.Display.setTextDatum(top_center);
  M5.Display.drawString("Touch not found", w / 2, 0, &fonts::FreeMonoBold24pt7b);
  M5.Display.setFont(&fonts::FreeMonoBold24pt7b);
}
```

- **ライブラリ**: `M5Unified.h` と `M5GFX.h` のみで実装を完結
- **タッチ配列**: `m5::touch_point_t touchPoint[5]` で最大 5 点分を確保
- **`M5.begin()`**: 本体とタッチコントローラ、LCD を初期化
- **解像度取得**: `M5.Lcd.width()` / `height()` で画面サイズを取得し、描画に利用
- **ディスプレイ初期化**:
  - 背景を白で塗りつぶし (`fillScreen`)
  - 回転を 0 度に固定 (`setRotation`)
  - テキストの基準点を上中央に設定 (`setTextDatum(top_center)`)
  - 初期メッセージとして「Touch not found」を表示
  - 以降で同じフォントを使うため `setFont()` を一度だけ呼ぶ

### 2. ループ処理（22-42行目）

```22:42:Touch_Multi/Touch_Multi.ino
void loop() {
  M5.update();
  int nums = M5.Lcd.getTouchRaw(touchPoint, 5);  

  if (nums)
  {
    M5.Display.drawString(" Touch detail: ", w / 2, 0, &fonts::FreeMonoBold24pt7b);
    for (int i = 0; i < nums; i++)
    {
      M5.Display.setCursor(50, 160 + i * 48);
      M5.Display.printf("Point %d X:%04d  Y:%04d", i+1, touchPoint[i].x, touchPoint[i].y);
    }
    drawed = true;
  }
  else if (drawed){
    drawed = false;
    M5.Display.clear(WHITE);
    M5.Display.drawString("Touch not found", w / 2, 0, &fonts::FreeMonoBold24pt7b);
  }
  vTaskDelay(1);
}
```

1. **`M5.update()`**  
   - 内部でタッチやボタンの状態を更新
2. **`M5.Lcd.getTouchRaw(touchPoint, 5)`**  
   - 生のタッチ座標を最大 5 点まで取得  
   - 戻り値 `nums` は実際に検出されたタッチ数
3. **タッチがある場合** (`nums > 0`)
   - 画面上部に「Touch detail」を表示
   - 1 点ごとに `X/Y` 座標を一覧表示  
   - `drawed` フラグを `true` にして、タッチ中であることを記録
4. **タッチがなくなった場合** (`nums == 0 && drawed == true`)
   - 直前まで表示していた座標をクリアし、再び「Touch not found」に戻す
   - `drawed` を `false` に戻して状態をリセット
5. **`vTaskDelay(1)`**  
   - FreeRTOS のタスクを 1 tick 休止させ、CPU を他タスクと共有

## 主要 API の詳細

### `M5.Lcd.getTouchRaw(m5::touch_point_t* buf, std::size_t count)`
- **役割**: タッチコントローラから生のタッチデータを取得
- **引数**:
  - `buf`: 座標を書き込む配列
  - `count`: 取得する最大ポイント数
- **戻り値**: 検出されたタッチ数（0〜5）
- **備考**:
  - マルチタッチ対応デバイスでは、それぞれの指に固有の `id` が割り当てられる
  - `M5.Touch.getDetail()` と異なり、タッチ状態の判別ロジックは自前で実装する必要がある

### `m5::touch_point_t`
- `x`, `y`: ピクセル座標（`int16_t`）
- `size`: タッチの大きさ（指の面積など、センサー依存）
- `id`: タッチポイント ID（0〜4）

### `M5.Display.drawString(text, x, y, font)`
- 任意のフォントで文字列を描画
- 第 4 引数を省略すると、直前に `setFont()` で設定したフォントを使用

### `M5.Display.printf(format, ...)`
- `printf` と同じ書式でテキストを描画
- `setCursor()` で指定した座標から描画が始まる

### `M5.Display.clear(color)`
- 画面全体を指定色で塗りつぶす
- `fillScreen()` と同義

### `vTaskDelay(ticks)`
- FreeRTOS の API を Arduino 互換で利用できる関数
- `ticks` は FreeRTOS の OS ティック数（Tab5 の場合 1 tick ≒ 1ms）

## プログラムの特徴

1. **リアルタイムなマルチタッチ表示**  
   同時に触れた指の位置を一覧表示し、デバッグやキャリブレーションに利用可能。

2. **生データの直接取得**  
   `M5.Touch.getDetail()` を使わず、`getTouchRaw()` の生データを扱うため、独自ジェスチャ判定や座標補正を実装しやすい。

3. **軽量な画面更新ロジック**  
   タッチがある時だけ情報を上書きし、タッチがなくなったらまとめてクリアするため、ちらつきを抑制。

4. **FreeRTOS 連携**  
   `vTaskDelay(1)` を挟むことで、他タスクと CPU 資源を共有しつつ滑らかな描画を維持。

## 応用アイデア

- タッチポイントごとに色分けして円を描画すると、視覚的にわかりやすいデバッグツールになる。
- `touchPoint[i].id` を利用して、同じ指が動いているかどうかを追跡できる。
- `size` 値を使って指の押し込み強度を推定し、圧力感知風の UI を実装できる。
- `getTouchRaw()` で取得した座標を `M5.Touch.convertRawXY()` に渡し、さらに高精度な座標補正を行うことも可能。

このプログラムを出発点に、マルチタッチを活用したジェスチャ UI やタッチ描画アプリへ発展させることができます。

