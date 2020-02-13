# Xair-OSC-Wifi-footswitch
ARDUINO / C++/C

You like this project and would like to see a commercial version:
* [Support the project via kick starter!](https://www.kickstarter.com/projects/xair/pedale-sans-fil-pour-commander-individuellement-un-mixer-son)
* [Subscribe to my newsletter](http://list.groupe-interval.fr/lists/?p=subscribe&id=1)



Drive an Xair mixer (XR16/XR18) with an ESP12-F (ESP8266) footswitch over OSC WIFI

(a more simple verison is also available for Raspberry , no OLED screen but led instead : https://github.com/infrafast/Xair-footswitch-RASPBERRY)


[![Video demo](http://img.youtube.com/vi/8BNjt5E4mcg/0.jpg)](https://youtu.be/y9CyGoYYEro)

3 switch (non latching or momentary)
switch Left : (connected to PIN D3) : use internal pull up
switch Left : (connected to PIN D4) : use internal pull up
switch tap  : (connected to PIN D0)  : need a pull down resistor of 1M connected to ground

Short push Tap: set the tap tempo of FX1 or switch between my monitor and my channel level control
Hold tap: Shows switch status/information
Short push/left/right: mute/unmute FX 1 and 3
Long push/left/right: mute/unmute FX 2 and 4
Hold left or right : increase and decrease the level of channel/fx/tap/monitor delay depending on the mode
Hold left and right: enter setup mode to configure the footswitch WIFI SSID and console to connect

The code manage mixer and wifi de-connexion + battery charge indication
Can be configured by web application
Show all channel names and manage bi-direction communication (if you change a value on the mixer, it is automatically reflected in the footswitch)


![image](https://user-images.githubusercontent.com/21040071/71670428-14fe0400-2d70-11ea-958e-ed8e403b05a7.png)

![image](https://user-images.githubusercontent.com/21040071/71670458-2d6e1e80-2d70-11ea-8b30-1f0d794dd4bf.png)

![image](https://user-images.githubusercontent.com/21040071/71670484-4080ee80-2d70-11ea-8a26-32d7928419d2.png)

![Webapplication to configure](https://user-images.githubusercontent.com/21040071/73779334-e15a3380-478c-11ea-923a-6042a878fa9a.png)

![user manual](https://user-images.githubusercontent.com/21040071/73779260-c1c30b00-478c-11ea-8f3d-1c95ec042091.png)

To do:
* auto update via web server and local http
* code compatibility with other mixer
* optimize the periodic read() to vaoid looping on all message command, instead do a compare against template and then retrieve the ID
* allow static IP configuration with no auto-discovery in case multiple mixer are on the network
* add a "nice" startup/loading animated screen
