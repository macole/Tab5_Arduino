# Phomemo M02 for Tab5 - Arduino Library

Phomemo M02サーマルプリンター用のArduinoライブラリです。Tab5環境での使用に最適化されています。

![Phomemo M02](https://phomemo.com/cdn/shop/products/1_490e2c49-2f9e-4a9e-8c63-8cd33f7d33c5.jpg)

## 機能

- ✅ Bluetooth経由でのプリンター接続
- ✅ テキスト印刷（複数フォントサイズ対応）
- ✅ ビットマップ画像印刷
- ✅ QRコード印刷（簡易版）
- ✅ 印刷設定調整（濃度、位置合わせ等）
- ✅ プリンター状態監視
- ✅ 豊富なコマンドライン操作

## 対応ハードウェア

### プリンター
- Phomemo M02 (48mm幅サーマルプリンター)
- その他Phomemoプリンター（M110, M120, M220等）※要調整

### Arduino ボード
- **ESP32** (推奨) - 内蔵Bluetooth使用
- Arduino Nano 33 BLE
- 外部Bluetoothモジュール使用時: Arduino Uno + HC-05等

## ファイル構成

```
PhomemoTab5/
├── PhomemoTab5.ino          # 基本版プログラム
├── PhomemoAdvanced.h        # 拡張ライブラリヘッダー
├── PhomemoAdvanced.cpp      # 拡張ライブラリ実装
├── PhomemoTab5Example.ino   # 完全版サンプル
└── README.md               # このファイル
```

## セットアップ

### 1. ハードウェア準備

1. ESP32開発ボードを用意
2. Phomemo M02プリンターを用意
3. USB ケーブル、電源を準備

### 2. Arduino IDE設定

1. Arduino IDEを最新版にアップデート
2. ESP32ボードパッケージをインストール：
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
3. ボードマネージャーで "ESP32" を検索してインストール

### 3. ライブラリインストール

1. このリポジトリをダウンロード
2. Arduino IDEのスケッチフォルダにファイルをコピー
3. 必要に応じて依存ライブラリをインストール

### 4. 接続手順

#### ESP32使用時（推奨）

1. ESP32をUSBでPCに接続
2. Arduino IDEで適切なボードとポートを選択
3. `PhomemoTab5Example.ino`を開く
4. プログラムをアップロード
5. シリアルモニターを開く（115200 bps）
6. Phomemo M02の電源をON
7. デバイスのBluetooth設定で "Tab5_PhomemoM02" を検索
8. ペアリング実行
9. 接続完了メッセージを確認

#### 他ボード使用時

1. BluetoothモジュールをArduinoに接続：
   ```
   HC-05 -> Arduino
   VCC   -> 5V
   GND   -> GND
   RX    -> D2 (SoftwareSerial)
   TX    -> D3 (SoftwareSerial)
   ```
2. プログラムの接続部分を修正して使用

## 基本的な使用方法

### コマンド操作

シリアルモニターから以下のコマンドが使用できます：

```bash
# テキスト印刷
text:Hello World

# 大きなフォントで印刷
large:Big Title

# 中央寄せで印刷
center:Centered Text

# 右寄せで印刷
right:Right Aligned

# 太字印刷
bold:Bold Text

# 線の印刷
line:-30    # 30文字の「-」線

# 設定変更
density:4   # 印刷濃度設定（1-5）
align:1     # 位置合わせ（0=左,1=中央,2=右）
font:2      # フォントサイズ（0=小,1=中,2=大,3=特大）

# 特殊機能
qr:https://example.com  # QRコード印刷
status                  # プリンター状態表示
test                   # テストページ印刷
receipt                # サンプルレシート印刷

# 制御コマンド
feed                   # 紙送り
cut                    # 紙送り+カット
beep                   # ビープ音
reset                  # プリンター初期化
menu                   # コマンド一覧表示
```

### プログラムでの使用例

```cpp
#include "PhomemoAdvanced.h"

PhomemoAdvanced printer;

void setup() {
  // Bluetooth接続（ESP32）
  SerialBT.begin("MyPrinter");
  printer.begin(&SerialBT);
  
  // 基本設定
  printer.setDensity(3);
  printer.setAlignment(ALIGN_CENTER);
  
  // テキスト印刷
  printer.println("Hello World!");
  
  // 画像印刷
  uint8_t bitmap[] = {0xFF, 0x81, 0x81, 0xFF}; // 2x2 pixel
  printer.printBitmap(bitmap, 8, 4);
  
  // 紙送り
  printer.feedAndCut();
}
```

## API リファレンス

### 基本操作

| メソッド | 説明 |
|---------|------|
| `begin(Stream*)` | プリンター初期化 |
| `isConnected()` | 接続状態確認 |
| `print(text)` | テキスト印刷 |
| `println(text)` | テキスト印刷+改行 |
| `printf(format, ...)` | フォーマット印刷 |

### 設定

| メソッド | 説明 |
|---------|------|
| `setDensity(1-5)` | 印刷濃度設定 |
| `setAlignment(0-2)` | 位置合わせ設定 |
| `setFontSize(0-3)` | フォントサイズ設定 |

### 特殊機能

| メソッド | 説明 |
|---------|------|
| `printBitmap(data, w, h)` | ビットマップ印刷 |
| `printQRCode(data, size)` | QRコード印刷 |
| `printLine(char, len)` | 線描画 |
| `feed(lines)` | 紙送り |
| `feedAndCut()` | 紙送り+カット |

## トラブルシューティング

### 接続できない場合

1. プリンターの電源を確認
2. Bluetoothペアリングを削除して再実行
3. プリンターを再起動
4. ESP32を再起動

### 印刷が薄い・濃い場合

```cpp
printer.setDensity(5);  // 最大濃度
// または
printer.setDensity(1);  // 最小濃度
```

### 文字化けする場合

- ASCII文字（英数字）のみ使用
- 日本語印刷は別途文字変換が必要

### 印刷位置がずれる場合

```cpp
printer.setAlignment(ALIGN_LEFT);    // 左寄せ
printer.setAlignment(ALIGN_CENTER);  // 中央寄せ
printer.setAlignment(ALIGN_RIGHT);   // 右寄せ
```

## 開発・カスタマイズ

### 独自コマンドの追加

`processUserCommand()`関数にケースを追加：

```cpp
else if (command.startsWith("mycommand:")) {
  String param = command.substring(10);
  // 独自処理
  Serial.println("✓ Custom command executed");
}
```

### 日本語対応

Shift-JIS変換機能を実装する場合：

```cpp
String convertToShiftJIS(const String& utf8) {
  // UTF-8 -> Shift-JIS変換処理
  return converted_string;
}
```

### ビットマップ画像の作成

1. 画像を1bpp（白黒）に変換
2. バイト配列形式で定義：

```cpp
uint8_t my_image[] = {
  0xFF, 0x81, 0x81, 0xFF,  // 4x4 pixel example
  // ... more data
};
printer.printBitmap(my_image, width, height);
```

## 参考資料

- [Phomemo M02公式サイト](https://phomemo.com/products/m02-portable-printer)
- [プロトコル解析資料](https://github.com/vivier/phomemo-tools)
- [Web版実装](https://github.com/K0IN/Phomemo-M02-Web)
- [Python実装](https://github.com/theacodes/phomemo_m02s)

## ライセンス

MIT License

## 貢献

プルリクエストや問題報告は歓迎です。

---

**注意**: このライブラリはリバースエンジニアリングに基づいて作成されており、すべての機能が正常に動作することを保証するものではありません。商用利用の際は十分なテストを行ってください。