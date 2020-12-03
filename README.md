# Robot_System_Device_driver
ロボットシステム学課題1デバイスドライバ
---
### 概要   
講義内で作成したデバイスドライバを変更し作成した.    
野球で使用するBSOカウンタを作ってみた.  
---
### 環境
- Raspberry Pi 4 Model B(ubuntu18.04)  
- 使用GPIO(26,16,4,9,10,11,25)計7個  
- ブレッドボード  
- LED7個(緑3個, 黄色2個, 赤1個)  
- 抵抗200Ω7個  
- ジャンパー線
i![13137365742873](https://user-images.githubusercontent.com/53966271/100971348-0b7dad00-357a-11eb-8da2-01044700add6.jpg)  
---
### 実装方法
```sh
git clone https://github.com/RikuYokoo/Robot_System_Device_driver.git  
cd myled  
make  
sudo insmod myled.ko  
sudo chmod 666 /dev/myled0  
echo X[^1] > /dev/myled0  
```
[^1]Xにはコマンドが入る詳細は下のページ  
---
### コマンド  
- t...LEDが順番についていく(LEDのテスト)  
- r...ついているLEDが消える  
- b...緑色のLEDがつく。回数に応じてつく数が増える。4回目で黄色と緑色が消える。
- s...黄色のLEDがつく。回数に応じてつく数が増える。3回目で黄色と緑色が消える。赤がつく。  
- o...赤色のLEDがつく。回数に応じてつく数が増える。3回目でLEDがすべて消える。  
- c...緑色と黄色のLEDを消す。  
- k...LEDを7個使ったパフォーマンス。  
---
### コマンドまとめ  
|X|説明|
|:--:|:--|
|t|LEDのテスト|
|r|リセット|
|b|ボール|
|s|ストライク|
|o|アウト|
|c|s,bリセット|
|k|パフォーマンス|
---
### 動画
[動画](https://youtu.be/4Yu6ux_07ps)
---
### 工夫点
- ストライク、ボール、アウトをそれぞれs,b,oと同じコマンドでできる。  
- ストライク3つすなわち三振で赤(アウト)がつくようにし、ストライクとボールをリセットする。  
- ボール4つすなわちフォアボールでストライク、ボールをリセットする。  
- 三振を取ったときに`k`コマンドを入力するとパフォーマンスが始まり、アウトカウント(赤のLED)がつく。  
---
### ライセンス
[GNU General Public License v3.0](https://github.com/RikuYokoo/Robot_System_Device_driver/blob/main/COPYING)
---
