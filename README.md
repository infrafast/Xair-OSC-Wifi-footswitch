# Xair-OSC-Wifi-footswitch
ARDUINO / C++/C

Drive an Xair mixer (XR16/XR18) with an ESP12-F (ESP8266) footswitch over OSC WIFI

3 switch (non latching or momentary)
switch Left : (connected to PIN D3) : use internal pull up
switch Left : (connected to PIN D4) : use internal pull up
switch tap  : (connected to PIN D0)  : need a pull down resistor of 1M connected to ground

Short push Tap: set the tap tempo of FX1
Hold tap: Shows switch status/information
Short push/left/right: mute/unmute FX 1 and 3
Long push/left/right: mute/unmute FX 2 and 4
Hold left or right : increase and decrease the volume of 
Hold left and right: enter setup mode to configure the footswitch WIFI SSID and console to connect

The code manage mixer and wifi de-connexion + battery charge indication

To do: 
- configure the channel to be used via webinterface
- bugfix: in some certain circumstance, button 1 is switching fx 4... 


![image](https://user-images.githubusercontent.com/21040071/71670428-14fe0400-2d70-11ea-958e-ed8e403b05a7.png)

![image](https://user-images.githubusercontent.com/21040071/71670458-2d6e1e80-2d70-11ea-8b30-1f0d794dd4bf.png)

![image](https://user-images.githubusercontent.com/21040071/71670484-4080ee80-2d70-11ea-8a26-32d7928419d2.png)

![image](https://user-images.githubusercontent.com/21040071/71671183-a1a9c180-2d72-11ea-855f-05da0fe4b014.png)

[![demo](http://img.youtube.com/vi/8BNjt5E4mcg/0.jpg)](http://www.youtube.com/watch?v=8BNjt5E4mcg)

