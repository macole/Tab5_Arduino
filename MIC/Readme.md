# MIC レコーダー/プレーヤー解説

## プログラム概要

このスケッチは M5Stack Tab5 の内蔵マイクとスピーカーを用いてオーディオを録音・波形表示し、タッチ操作で再生するデモです。録音データは大きなリングバッファに格納され、LCD にリアルタイム波形として描画されます。タッチイベントで再生モードに入り、記録済みオーディオをスピーカーから出力します。

## 主要パラメータ

```1:12:MIC/MIC.ino
static constexpr size_t record_number     = 256;      // バッファ段数
static constexpr size_t record_length     = 720;      // 1段のサンプル数 (LCD幅に合わせ推奨)
static constexpr size_t record_size       = record_number * record_length;
static constexpr size_t record_samplerate = 17000;    // サンプリングレート (17 kHz)
static int16_t prev_y[record_length];                 // 前回描画した縦線の開始位置
static int16_t prev_h[record_length];                 // 前回描画した縦線の高さ
static size_t rec_record_idx  = 2;                    // 次に録音するバッファ段
static size_t draw_record_idx = 0;                    // 次に描画するバッファ段
static int16_t *rec_data;                             // 録音データ領域（ヒープ割当）
```

`record_number × record_length` の 2 次元バッファにサンプルを順次書き込み、描画と再生をリング形式で管理します。

## `setup()` 処理

```15:37:MIC/MIC.ino
M5.begin();
w = M5.Display.width();
M5.Display.startWrite();
M5.Display.setRotation(0);
M5.Display.setTextDatum(top_center);
M5.Display.setTextColor(WHITE);
M5.Display.setFont(&fonts::FreeSansBoldOblique18pt7b);

rec_data = (int16_t*)heap_caps_malloc(record_size * sizeof(int16_t), MALLOC_CAP_8BIT);
memset(rec_data, 0, record_size * sizeof(int16_t));
M5.Speaker.setVolume(255);

M5.Speaker.end(); // マイクとスピーカーは同時使用不可
M5.Mic.begin();
M5.Display.fillCircle(300, 30, 20, RED);
M5.Display.drawString("REC", w / 2, 18);
M5.Display.drawString("Touch screen to play the record", w / 2, 200);
```

1. **表示初期化**: 描画速度向上のため `startWrite()` を使用し、文字を中央寄せで表示。
2. **メモリ確保**: `heap_caps_malloc()` で録音バッファを PSRAM/8bit ヒープから確保。確保失敗時はヌルポインタになるため、実際の運用では NULL チェックが望ましい。
3. **オーディオ初期化**: 同時使用不可のためスピーカーを停止 (`M5.Speaker.end()`)、マイクを起動 (`M5.Mic.begin()`)。
4. **UI**: 録音中インジケータ (赤丸 + “REC”）と操作説明を表示。

## 録音・描画ループ

```39:80:MIC/MIC.ino
if (M5.Mic.isEnabled()) {
    static constexpr int shift = 6;
    auto data = &rec_data[rec_record_idx * record_length];
    if (M5.Mic.record(data, record_length, record_samplerate)) {
        data = &rec_data[draw_record_idx * record_length];
        if (w > record_length - 1) w = record_length - 1;
        for (int32_t x = 0; x < w; ++x) {
            M5.Display.writeFastVLine(x, prev_y[x], prev_h[x], TFT_BLACK); // 前の線を消す
            int32_t y1 = (data[x] >> shift);
            int32_t y2 = (data[x + 1] >> shift);
            if (y1 > y2) std::swap(y1, y2);
            int32_t y = (M5.Display.height() >> 1) + y1;
            int32_t h = (M5.Display.height() >> 1) + y2 + 1 - y;
            prev_y[x] = y;
            prev_h[x] = h;
            M5.Display.writeFastVLine(x, prev_y[x], prev_h[x], WHITE);     // 新しい線を描画
        }

        M5.Display.display();  // バッファ反映
        M5.Display.fillCircle(300, 30, 20, RED);
        M5.Display.drawString("REC", w / 2, 18);
        M5.Display.drawString("Touch screen to play the record", w / 2, 200);
        if (++draw_record_idx >= record_number) draw_record_idx = 0;
        if (++rec_record_idx  >= record_number) rec_record_idx  = 0;
    }
}
```

- `M5.Mic.record(buffer, length, samplerate)` は指定サンプル数の録音が完了すると `true` を返す非同期 API。
- `shift = 6` により 17bit 相当の波形データを縮小し、LCD 高さにフィットさせる。
- `writeFastVLine()` を使って縦線で波形を描画し、前フレームの線を黒で消してから上書き。
- `draw_record_idx` と `rec_record_idx` をリングバッファとして更新。
- `M5.Display.display()` はダブルバッファ構成の場合のみ必要だが、ここでは描画結果の反映を保証するために呼んでいる。

## 再生処理

```81:121:MIC/MIC.ino
if (M5.Touch.getCount() && M5.Touch.getDetail(0).wasClicked()) {
    if (M5.Speaker.isEnabled()) {
        while (M5.Mic.isRecording()) { delay(1); }
        M5.Display.clear();
        M5.Display.fillCircle(300, 30, 20, GREEN);
        M5.Display.drawString("PLAY", w / 2, 18);

        M5.Mic.end();
        M5.Speaker.begin();

        int start_pos = rec_record_idx * record_length;
        if (start_pos < record_size) {
            M5.Speaker.playRaw(&rec_data[start_pos], record_size - start_pos, record_samplerate, false, 1, 0);
        }
        if (start_pos > 0) {
            M5.Speaker.playRaw(rec_data, start_pos, record_samplerate, false, 1, 0);
        }
        do {
            delay(1);
            M5.update();
        } while (M5.Speaker.isPlaying());

        M5.Speaker.end();
        M5.Mic.begin();

        M5.Display.clear();
        M5.Display.fillCircle(300, 30, 20, RED);
        M5.Display.drawString("REC", w / 2, 18);
    }
}
```

1. タッチクリックで再生を開始（`wasClicked()`）。
2. 録音中断を待ってから UI を「PLAY」に切り替え。
3. マイクを停止 (`M5.Mic.end()`)、スピーカーを開始 (`M5.Speaker.begin()`）。
4. 現在の録音位置を起点に、後半→前半の順に `M5.Speaker.playRaw()` で再生し、リングバッファの循環順序を保持。
5. 再生終了 (`M5.Speaker.isPlaying()` が false) までポーリング。
6. 再びスピーカーを停止しマイクを開始、UI を録音状態に復帰。

## 主要 API 詳解

### `M5.Mic`
- `M5.Mic.begin() / end()`: マイクの有効・無効を切り替え。
- `M5.Mic.isEnabled()`: マイクが有効か判定。
- `M5.Mic.record(buffer, length, samplerate)`: 指定サンプル数の録音を行い、完了したら `true`。録音中に再度呼ぶと進行状況をチェック。
- `M5.Mic.isRecording()`: 録音処理が進行中か判定。

### `M5.Speaker`
- `M5.Speaker.begin() / end()`: スピーカーの起動・停止。マイクと同時使用不可のため明示的に切り替え。
- `M5.Speaker.setVolume(level)`: 0〜255 の音量設定。
- `M5.Speaker.playRaw(buffer, length, samplerate, isStereo, channel, startOffset)`: RAW バッファを再生。
- `M5.Speaker.isPlaying()`: 再生中かチェック。

### 描画関連
- `M5.Display.writeFastVLine(x, y, h, color)`: 高速な縦線描画。波形表示に使用。
- `M5.Display.startWrite() / endWrite()`: SPI バス占有を最適化し描画速度を向上。
- `M5.Display.display()`: バックバッファを持つディスプレイで実フレームへ転送。

### メモリ確保
- `heap_caps_malloc(size, MALLOC_CAP_8BIT)`: ESP-IDF のヒープ API。PSRAM など 8bit アクセス可能なメモリから確保。
- `memset(pointer, 0, bytes)`: 録音バッファをゼロ初期化。

### タッチ入力
- `M5.Touch.getCount()`: 現在のタッチポイント数。
- `M5.Touch.getDetail(index)`: `touch_detail_t` を取得。`wasClicked()` でタップを検知。

## 動作モード

| インジケータ | 意味                                   | マイク | スピーカー |
|--------------|----------------------------------------|--------|------------|
| 赤丸 + “REC” | 録音モード。波形がリアルタイム表示     | ON     | OFF        |
| 緑丸 + “PLAY”| 再生モード。タッチ後の録音内容を再生   | OFF    | ON         |

## 拡張アイデア

1. **録音長の調整**: `record_number` や `record_length` を変更して録音時間／解像度を調整。
2. **FFT 解析**: 録音データに FFT を適用してスペクトラムを表示。
3. **保存機能**: `SPIFFS` や `SD` へ録音ファイルを書き出し、後で再生できるようにする。
4. **VU メーター**: 波形に加えてピークレベルを棒グラフで表示。
5. **オーバーラップ再生**: 再生中に次の録音をバッファリングしておき、連続再生を実現。

## 注意点

- **メモリ確保**: `record_size` が大きいため、PSRAM を持たないモデルでは確保できない場合がある。必要に応じて値を小さくする。
- **リソース競合**: マイクとスピーカーを同時に使用するとノイズや HW 競合が起こるため、必ず `M5.Mic` と `M5.Speaker` を切り替える。
- **描画負荷**: `writeFastVLine` を大量に呼ぶため、`record_length` を画面幅より大きくするとパフォーマンスが低下する。必要に応じて縮小処理を検討。
- **タッチ判定**: `wasClicked()` はタップ時のみ `true`。長押しなどを判定したい場合は `isHolding()` などを併用。

このサンプルを基に、波形エディタ、ボイスメモ、周波数解析ツールなど高度なオーディオアプリケーションへ発展させることができます。

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み
