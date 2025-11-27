# Phomemo Tab5 プロジェクト - クイックセットアップガイド

## 📁 プロジェクト概要

このプロジェクトは、富士電機DX推進業務での活用を想定したPhomemo M02サーマルプリンター用Arduinoライブラリです。

**保存場所**: `/Arduino_Projects/PhomemoTab5/`

## 🚀 クイックスタート

### 1. 必要なハードウェア
- **ESP32開発ボード** (推奨: ESP32-DevKitC)
- **Phomemo M02プリンター** 
- USBケーブル、電源

### 2. Arduino IDE準備
```
1. Arduino IDE 2.0以上をインストール
2. ファイル→環境設定→追加ボードマネージャーURL:
   https://dl.espressif.com/dl/package_esp32_index.json
3. ツール→ボード→ボードマネージャー→"ESP32"検索→インストール
```

### 3. プロジェクトファイル使用手順

#### 初心者向け：基本版
1. `PhomemoTab5.ino` を開く
2. ESP32ボードを選択してアップロード
3. シリアルモニター(115200bps)で動作確認

#### 上級者向け：完全版
1. `PhomemoTab5Example.ino` を開く
2. 同フォルダに `PhomemoAdvanced.h` と `PhomemoAdvanced.cpp` を配置
3. アップロード→全機能利用可能

### 4. 接続手順
```
1. ESP32プログラムアップロード
2. Phomemo M02電源ON
3. スマホ/PCのBluetooth設定
4. "Tab5_PhomemoM02" を検索・ペアリング
5. シリアルモニターで接続確認
6. 'test' コマンドでテスト印刷
```

## 💼 業務活用例

### 製造ライン用途
```arduino
// 作業指示書印刷
printer.setAlignment(ALIGN_CENTER);
printer.printText("作業指示書", FONT_LARGE);
printer.printText("工程: 組立工程A");
printer.printText("担当者: 田中太郎");
printer.printQRCode("PROC_A_001");
```

### 品質管理用途
```arduino
// 検査結果ラベル
printer.printText("検査結果: 合格");
printer.printf("日時: %s\n", getCurrentDateTime());
printer.printText("検査員: システム");
printer.feedAndCut();
```

### 進捗管理用途
```arduino
// 進捗レポート
printer.printText("=== 日次進捗 ===", FONT_MEDIUM);
printer.printf("計画: %d個\n", planned);
printer.printf("実績: %d個\n", actual);
printer.printf("達成率: %.1f%%\n", ratio);
```

## 🔧 カスタマイズポイント

### 1. 独自コマンド追加
`processCommand()` 関数に追加:
```cpp
else if (command.startsWith("worklist:")) {
    printWorkList(command.substring(9));
}
```

### 2. 定期印刷機能
```cpp
// タイマー割り込みで定期レポート印刷
void printHourlyReport() {
    printer.printText("定時レポート");
    printer.printf("時刻: %02d:00\n", hour);
    // 生産データ取得・印刷
}
```

### 3. センサー連動
```cpp
// 品質チェック完了時の自動印刷
void onQualityCheckComplete(bool passed) {
    printer.printText(passed ? "OK" : "NG", FONT_LARGE);
    printer.feedAndCut();
}
```

## 📱 コマンドリファレンス

| コマンド | 説明 | 使用例 |
|----------|------|--------|
| `text:` | テキスト印刷 | `text:Hello World` |
| `large:` | 大文字印刷 | `large:重要` |
| `center:` | 中央寄せ | `center:タイトル` |
| `density:` | 濃度調整 | `density:4` |
| `qr:` | QRコード | `qr:https://example.com` |
| `test` | テスト印刷 | `test` |
| `receipt` | レシートサンプル | `receipt` |
| `status` | 状態表示 | `status` |

## 🔍 トラブルシューティング

### 接続エラー
```
✗ 症状: "Bluetooth connection timeout"
✓ 解決: プリンター再起動→ペアリング削除→再接続
```

### 印刷品質
```
✗ 症状: 印刷が薄い
✓ 解決: density:5 コマンドで濃度最大
```

### 文字化け
```
✗ 症状: 日本語が文字化け
✓ 解決: 英数字のみ使用（日本語対応は今後実装予定）
```

## 📞 サポート

プロジェクト保存場所: AI Drive `/Arduino_Projects/PhomemoTab5/`

いつでもこのフォルダからファイルを取得して使用できます。

---

**作成者**: DX推進エンジニア  
**更新日**: 2024-11-15  
**バージョン**: 1.0.0