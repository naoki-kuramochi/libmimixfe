# libmimixfe
mimi XFE module for Fairy I/O Tumbler

## はじめに

### 概要

T-01 用バイナリとして提供される libmimixfe.so とそのヘッダファイル、及び簡単な利用例を含むリポジトリです。 `include/` 以下にヘッダファイル、`lib/` 以下に libmimixfe.so 本体、`examples` 以下に利用例のソースコードが含まれます。利用例は、説明のための実装であり、プロダクション用の実装には好適ではない場合があります。

### 依存ライブラリ

LED リングの制御のために libtumbler.so が必要です（ https://github.com/FairyDevicesRD/tumbler )


### 利用例のビルド

T-01 実機上の Makefile が `examples/` 直下に用意されています。

``````````{.sh}
$ cd examples
$ make
``````````

適宜同梱の lixmimife.so 及び上記 libtumbler.so とリンクして実行してください。いくつかの利用例は `sudo` を必要とします。

## API 概要

libmimixfe は Tumbler の 18ch マイクを直接入力とし、設定に従った信号処理済音声を出力するライブラリです。最も単純には、信号処理として無処理であり、その場合は、18ch マイクから同期録音された 18ch 音声がそのまま出力されます。典型的には、信号処理として、例えば、エコーキャンセル、ビームフォーミング（多チャンネルノイズ抑制）、VAD（発話区間抽出）を行った音声が出力されます。これはつまり、Tumbler のスピーカーから再生されている音声をキャンセルし（エコーキャンセル）、特定方向のみの音声を取得することで、目的音声を強調し（ビームフォーミング）、人間の声の区間のみが抽出された（VAD）、音声が取得できるということになります。

処理済み音声が出力されるタイミングで、ユーザー定義のコールバック関数が、処理済み音声を引数に与えられた状態で呼ばれます。コールバック関数にはユーザー定義のデータを与えることも可能です。この仕組は、Linux 環境での録音・再生によく用いられる OSS である portaudio （Portable Cross-Platform Audio I/O） が提供している API と同様の仕組みであり、コールバック型 API と呼ばれることがあります。以降の記述でも、この呼称に従います。
