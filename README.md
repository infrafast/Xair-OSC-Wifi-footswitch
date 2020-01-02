# Xair-OSC-Wifi-footswitch
Drive an Xair mixer (XR16/XR18) with an ESP12-F (ESP8266) footswitch over OSC WIFI

3 switch (non latching or momentary)
switch Left : (connected to PIN D3) : use internal pull up
switch Left : (connected to PIN D3) : use internal pull up
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
