microSD カードを FAT32 形式でフォーマットし、ルートディレクトリに解像度 720*1280 の PNG 画像を2枚配置し、TestPicture01.png、TestPicture02.png と命名してください。（サンプル画像1、サンプル画像2 を直接ダウンロードしても構いません。画像の解像度が 720*1280 でない場合、プログラムの設定によって表示方式が変わり、表示が崩れる可能性があります。）

この SD カードを Tab5 に挿入してください。SD カードの端子面は Tab5 のスクリーンと同じ方向に向けます。上記コードを Arduino IDE にコピーしてコンパイルし、Tab5 に書き込みます。

このプログラムは SD カードに WriteTest.txt というテキストファイルを作成して文字列を書き込み、その後 SD カード内の PNG 画像2枚をループ再生します。


API
Tab5 microSD カード部分は Arduino 標準の SD ライブラリと、M5GFX ライブラリの drawPngFile を使用しています。関連 API の詳細は以下のドキュメントを参照してください。

SD | Arduino Doc
Guide to SD Storage | Arduino Doc
M5GFX API - drawPngFile