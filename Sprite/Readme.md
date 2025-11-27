# Sprite - スプライト（メモリ描画領域）デモ

メモリ描画領域（スプライト）を使用した描画デモプログラムです。直接描画とメモリ描画の違いを視覚的に比較できます。

## 📋 概要

このプログラムは、M5Stack Tab5の画面に直接描画する方法と、メモリ描画領域（スプライト）を使用する方法の違いをデモンストレーションします。直接描画ではチラツキが発生しますが、メモリ描画領域を使用すると滑らかな表示が可能です。

## 🔧 必要なハードウェア

- M5Stack Tab5

## 📚 必要なライブラリ

- **M5Unified**: M5Stack統合ライブラリ
  ```bash
  arduino-cli lib install "M5Unified@0.2.10"
  ```

- **M5GFX**: M5Stack用グラフィックライブラリ
  ```bash
  arduino-cli lib install "M5GFX@0.2.15"
  ```

## 🚀 セットアップ

1. **ボード設定**
   - Board: ESP32P4 Dev Module
   - USB CDC on boot: Enabled
   - Flash Size: 16MB (128Mb)
   - Partition Scheme: Custom
   - PSRAM: Enabled

2. **プログラムのアップロード**
   - Arduino IDEまたはArduino CLIでプログラムをアップロード

## 💻 使用方法

プログラムをアップロードすると、画面が上下2分割され、上半分に直接描画、下半分にメモリ描画領域を使用した描画が表示されます。両者の違い（チラツキの有無）を比較できます。

## 📊 機能

- **直接描画デモ**: 画面に直接描画（チラツキあり）
- **メモリ描画領域デモ**: スプライトを使用した描画（チラツキなし）
- **グラデーション表示**: 赤→緑→青のグラデーションを表示
- **日本語フォント対応**: 日本語テキストの表示

## 🔍 プログラムの動作

1. **初期化**
   - M5Stackの初期化
   - メモリ描画領域（スプライト）の作成
   - 画面サイズの半分のスプライトを作成

2. **描画ループ**
   - **上半分（直接描画）**:
     - 画面に直接テキストとグラデーションを描画
     - チラツキが発生する
   - **下半分（メモリ描画）**:
     - スプライトにテキストとグラデーションを描画
     - スプライトを画面に一括転送
     - チラツキが発生しない

## 📝 コードの主要なポイント

### スプライトの作成
```cpp
M5Canvas canvas(&M5.Display);
canvas.createSprite(M5.Display.width(), M5.Display.height()/2);
```

### 直接描画
```cpp
M5.Display.fillRect(0, 0, width, height, BLACK);
M5.Display.println("直接表示");
M5.Display.println("チラツキあり");
```

### メモリ描画領域への描画
```cpp
canvas.fillRect(0, 0, width, height, BLACK);
canvas.println("メモリ描画領域表示");
canvas.println("チラツキなし");
```

### スプライトの表示
```cpp
canvas.pushSprite(&M5.Display, 0, M5.Display.height()/2);
```

## 💡 スプライトの利点

1. **チラツキの防止**: メモリ上で描画を完了してから一括転送するため、チラツキが発生しない
2. **高速描画**: 複数の描画操作をまとめて実行できる
3. **部分更新**: 画面の一部だけを更新できる

## ⚙️ カスタマイズ

### スプライトサイズの変更
```cpp
canvas.createSprite(width, height);  // サイズを変更
```

### 表示位置の変更
```cpp
canvas.pushSprite(&M5.Display, x, y);  // 座標を変更
```

## 📖 参考資料

- [M5GFX ライブラリ](https://github.com/m5stack/M5GFX)
- [M5Unified ライブラリ](https://github.com/m5stack/M5Unified)

## 📄 ライセンス

このプログラムは教育・学習目的で提供されています。

---

**作成日**: 2025年11月  
**対象デバイス**: M5Stack Tab5  
**動作確認**: ✅ 正常動作確認済み

