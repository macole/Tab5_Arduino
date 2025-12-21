# Touch Drawing - タッチお絵描き

M5Stack Tab5のマルチタッチ機能を使ったお絵描きプログラムです。

## 📋 概要

タッチした場所に図形を描画するシンプルなお絵描きアプリケーションです。マルチタッチに対応しており、複数の指で同時に描画できます。タッチIDによって異なる図形（丸、×印、三角）が描画されます。

## 🎨 機能

### 描画機能
- **マルチタッチ対応**: 最大3点同時タッチで描画可能
- **タッチID別図形**:
  - **ID 0（1本目の指）**: 塗りつぶし円 ⚫
  - **ID 1（2本目の指）**: ×印 ❌
  - **ID 2以降（3本目以降の指）**: 三角形 🔺
- **リアルタイム座標表示**: 
  - Raw座標（生データ）
  - Convert座標（変換後データ）

### クリア機能
- **Clearボタン**: 画面右下にボタン配置
- **ワンタッチクリア**: ボタンをタッチすると画面全体をクリア
- **ボタン再描画**: クリア後も自動的にボタンが再表示

## 🔧 必要なハードウェア

- M5Stack Tab5
- タッチスクリーン（内蔵）

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```

## 🚀 セットアップ

### ボード設定
- Board: ESP32P4 Dev Module
- USB CDC on boot: Enabled
- Flash Size: 16MB (128Mb)
- Partition Scheme: Custom
- PSRAM: Enabled

### プログラムのアップロード
Arduino IDEまたはArduino CLIでプログラムをアップロードしてください。

## 💻 使用方法

### 基本操作
1. プログラムを起動すると、画面右下に「Clear」ボタンが表示されます
2. 画面の好きな場所をタッチすると図形が描画されます
3. 複数の指で同時にタッチすると、それぞれ異なる図形が描画されます
4. 「Clear」ボタンをタッチすると画面がクリアされます

### 描画のコツ
- **1本指**: 連続してタッチすると線のように描画できます（丸の連続）
- **2本指**: 2本の指で同時にタッチすると、丸と×印が同時に描画されます
- **3本指**: 3本の指で同時にタッチすると、丸・×印・三角が同時に描画されます

## 📊 画面レイアウト

```
┌─────────────────────────────────────┐
│ Raw X:123  Y:456                    │ ← 生タッチ座標（ID 0）
│ Raw X:234  Y:567                    │ ← 生タッチ座標（ID 1）
│ Raw X:345  Y:678                    │ ← 生タッチ座標（ID 2）
│                                     │
│ Convert X:123  Y:456                │ ← 変換後座標（ID 0）
│ Convert X:234  Y:567                │ ← 変換後座標（ID 1）
│ Convert X:345  Y:678                │ ← 変換後座標（ID 2）
│                                     │
│                                     │
│         [描画エリア]                 │
│                                     │
│                        ┌─────────┐  │
│                        │  Clear  │  │ ← クリアボタン
│                        └─────────┘  │
└─────────────────────────────────────┘
```

## 🔍 プログラムの動作

### 初期化フロー
1. M5Unified初期化
2. タッチスクリーン検出確認
3. EPDモードを最速モードに設定（`epd_fastest`）
4. 描画開始（`startWrite()`）
5. Clearボタンの描画

### メインループ
1. タッチ検出（最大3点）
2. 座標変換（`convertRawXY`）
3. Clearボタンがタッチされたかチェック
   - タッチされた場合: 画面クリア → ボタン再描画
   - タッチされていない場合: 図形描画
4. 座標情報の表示更新

### 描画処理の詳細

#### タッチIDによる図形の選択
```cpp
switch (tp[i].id) {
  case 0:  // 1本目の指 → 塗りつぶし円
    M5.Display.fillCircle(tp[i].x, tp[i].y, s);
    break;
  case 1:  // 2本目の指 → ×印
    M5.Display.drawLine(tp[i].x-s, tp[i].y-s, tp[i].x+s, tp[i].y+s);
    M5.Display.drawLine(tp[i].x-s, tp[i].y+s, tp[i].x+s, tp[i].y-s);
    break;
  default: // 3本目以降の指 → 三角形
    M5.Display.fillTriangle(tp[i].x-s, tp[i].y+s, 
                           tp[i].x+s, tp[i].y+s, 
                           tp[i].x, tp[i].y-s);
    break;
}
```

#### 図形サイズの計算
```cpp
int s = tp[i].size + 3;  // タッチサイズ + 3ピクセル
```
タッチの圧力に応じて図形サイズが変化します。

## 📝 コードの主要なポイント

### Clearボタンの実装
```cpp
#define CLEAR_BUTTON_W 80  // ボタン幅
#define CLEAR_BUTTON_H 30  // ボタン高さ

void drawClearButton(void) {
  clearButtonX = M5.Display.width() - 100;   // 右端から100px
  clearButtonY = M5.Display.height() - 40;   // 下端から40px
  
  M5.Display.fillRect(clearButtonX, clearButtonY, 
                     CLEAR_BUTTON_W, CLEAR_BUTTON_H);
  M5.Display.drawString("Clear", ...);
}
```

### タッチ判定
```cpp
bool isClearButtonTouched(int x, int y) {
  return (x >= clearButtonX && x < clearButtonX + CLEAR_BUTTON_W &&
          y >= clearButtonY && y < clearButtonY + CLEAR_BUTTON_H);
}
```

### マルチタッチ処理
```cpp
lgfx::touch_point_t tp[3];  // 最大3点
int nums = M5.Display.getTouchRaw(tp, 3);
if (nums) {
  M5.Display.convertRawXY(tp, nums);  // 座標変換
  // 各タッチポイントを処理
  for (int i = 0; i < nums; ++i) {
    // 図形描画
  }
}
```

### EPDモード設定
```cpp
M5.Display.setEpdMode(epd_mode_t::epd_fastest);  // 最速更新モード
M5.Display.startWrite();                          // 描画開始
```
高速描画のため、EPDモードを最速に設定しています。

## 🎨 カスタマイズ

### 図形の変更
`loop()`関数内の`switch`文で描画する図形を変更できます：

```cpp
case 0:
  // 塗りつぶし円の代わりに四角を描画
  M5.Display.fillRect(tp[i].x-s, tp[i].y-s, s*2, s*2);
  break;
```

### ボタンの位置変更
```cpp
clearButtonX = 10;   // 左端から10px
clearButtonY = 10;   // 上端から10px
```

### 図形のサイズ変更
```cpp
int s = tp[i].size * 2 + 5;  // より大きな図形
```

### 描画色の変更
```cpp
M5.Display.setColor(TFT_RED);      // 赤色で描画
M5.Display.setColor(TFT_GREEN);    // 緑色で描画
M5.Display.setColor(TFT_BLUE);     // 青色で描画
```

### タッチIDごとに色を変える
```cpp
switch (tp[i].id) {
  case 0:
    M5.Display.setColor(TFT_RED);    // 1本目は赤
    M5.Display.fillCircle(tp[i].x, tp[i].y, s);
    break;
  case 1:
    M5.Display.setColor(TFT_GREEN);  // 2本目は緑
    M5.Display.drawLine(...);
    break;
  default:
    M5.Display.setColor(TFT_BLUE);   // 3本目は青
    M5.Display.fillTriangle(...);
    break;
}
```

## 🔧 トラブルシューティング

### タッチが反応しない
- **原因**: タッチスクリーンが検出されていない
- **解決策**: 
  - 画面に「Touch not found.」と表示されているか確認
  - M5Unifiedライブラリのバージョンを確認
  - プログラムを再起動

### 座標がずれる
- **原因**: 画面の回転設定が適用されていない
- **解決策**: 
  - `convertRawXY()`が正しく呼ばれているか確認
  - 画面の向き（`setRotation()`）を確認

### Clearボタンが反応しない
- **原因**: ボタンの座標判定が正しくない
- **解決策**: 
  - `isClearButtonTouched()`の座標範囲を確認
  - `clearButtonX`と`clearButtonY`の値を確認

### 描画が遅い
- **原因**: `display()`の呼び出しが多すぎる
- **解決策**: 
  - EPDモードが`epd_fastest`に設定されているか確認
  - `startWrite()`が呼ばれているか確認

## 📖 参考資料

- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)
- [M5GFX タッチ機能](https://github.com/m5stack/M5GFX)

## 📄 ライセンス

このプログラムはMITライセンスの下で公開されています。

Copyright (c) 2025 macole

詳細はプロジェクトルートの`LICENSE`ファイルを参照してください。

---

**作成日**: 2025年12月21日  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み  
**機能**: マルチタッチお絵描き、クリアボタン付き
