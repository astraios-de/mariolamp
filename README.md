# mariolamp
Super Mario bed side lamp with 32x16 rgb led array driven by an ESP32.

[![IMAGE ALT TEXT HERE](http://img.youtube.com/vi/q2FRGy6CD5E/0.jpg)](http://www.youtube.com/watch?v=q2FRGy6CD5E)

http://www.youtube.com/watch?v=q2FRGy6CD5E

![grafik](https://user-images.githubusercontent.com/83372370/122374987-1d95ca80-cf63-11eb-9256-fa41f638a758.png)

This is a bedside lamp i made for my son's 6th birthday.

It is Super Mario themed and features a 32x16 pixel RGB led array for animations and lighting.

The mushroom and the koopa shell serve as control buttons (push and turn) for turning the lamp on/off, adjusting brightness and selecting the animation mode. Animations / games can also be manipulated / controlled.

It also has a USB A female connector to power / charge other gadgets.

I am driving this with a 5V/4A power supply.

## Parts list

* LED matrix I am using (2 of them): https://aliexpress.com/item/4000746284302.html
* ESP32 board: https://aliexpress.com/item/32801621054.html
* flip flop power switch module: https://aliexpress.com/item/33054170454.html
* NPN transistor
* encoder buttons: https://aliexpress.com/item/32863292306.html
* usb A female connector: https://www.amazon.de/gp/product/B07TVHRH5Q
* power connector: https://www.amazon.de/gp/product/B00H8WFGAM

## Power connection
The power supply is handled via a custom perfboard circuit in order to (un-)power the whole thing by the tip of a button. USB A female always stays powered for charging. The transistor used is an ordinary NPN.

![grafik](https://user-images.githubusercontent.com/83372370/122384968-86357500-cf6c-11eb-89b5-49354079889a.png)
![grafik](https://user-images.githubusercontent.com/83372370/122384980-8a619280-cf6c-11eb-957c-5d11b81834a2.png)
![grafik](https://user-images.githubusercontent.com/83372370/122384988-8b92bf80-cf6c-11eb-972d-62205b24c3f2.png)

## Assembly
Everything's held together by a bunch of M2x6mm screws (M2.6 for the USB A board).

## Images
![grafik](https://user-images.githubusercontent.com/83372370/122374786-f3440d00-cf62-11eb-95ac-6e1404ef1494.png)
![grafik](https://user-images.githubusercontent.com/83372370/122374875-05be4680-cf63-11eb-8f80-c797c668b0c5.png)
![grafik](https://user-images.githubusercontent.com/83372370/122374923-0eaf1800-cf63-11eb-9e22-7a99677a6d22.png)
![grafik](https://user-images.githubusercontent.com/83372370/122374906-0bb42780-cf63-11eb-8db0-6fda7d65e6ec.png)
![grafik](https://user-images.githubusercontent.com/83372370/122374933-1078db80-cf63-11eb-8b85-6363fd6eb8bc.png)
![grafik](https://user-images.githubusercontent.com/83372370/122374942-12db3580-cf63-11eb-8352-d38d12d9c388.png)
![grafik](https://user-images.githubusercontent.com/83372370/122374950-153d8f80-cf63-11eb-9e7b-ff073a9f289b.png)
