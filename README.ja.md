# gob_easing

[English](README.md)

## 概要

基本的なイージング関数の実装です。  
全て constexpr 関数なので、コンパイル時計算を用いることができます。  
PlatformIO ArduinoIDE のライブラリの形式をとっていますが、C++11 以降の処理系全般で使用できます。  
いわゆる header-only なライブラリです。

イージングについては [https://easings.net/](https://easings.net/) を参照。

## 導入
環境によって適切な方法でインストールしてください
* git clone や Zip ダウンロードからの展開
* PlatformIO
```ini
lib_deps = https://github.com/GOB52/gob_easing.git
```
* ArduinoIDE ライブラリマネージャからのインストール

## 使い方
gob_easing.hpp をインクルードするだけです。  
namespace は goblib::easing です。  
```cpp
#include <gob_easing.hpp>

void foo()
{
    for(int i=0; i <= 100; ++i)
    {
        float t = (float)i / 100; // Clamp [0.0 - 1.0]
        float v = goblib::easing::inBack(t);  //Use this value for what you want to do.
    }
}
```
詳細は examples をご覧ください。  
* [グラフ、グラデーション、動作のデモ](examples/demo)
* [コンパイル時計算でテーブルを作成する](examples/lookup_table)


## ドキュメント
[Doxygen](https://www.doxygen.nl/) 用の[設定ファイル](doc/Doxyfile)と[シェルスクリプト](doc/doxy.sh)で作成できます。  
出力先は doc/html です。
```
bash doc/doxy.sh
```

または[GitHub Pages](https://gob52.github.io/gob_easing/)を参照してください。



