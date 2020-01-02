#include <RemoteDebug.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WifiUDP.h>
#include <SSD1306.h>
//#include <TimeLib.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <OSCMessage.h>       // For OSC support
#include <ArduinoTapTempo.h>
#include <ArduinoOTA.h> // for WIFI upload 
#include <EEPROM.h>

unsigned char  signal4_icon16x16[] = {
  0x00, 0x00, 0x00, 0x20, 0x00, 0x30, 0x00, 0x38, 0x00, 0x38, 0x00, 0x3A, 
  0x00, 0x3B, 0x80, 0x3B, 0x80, 0x3B, 0xA0, 0x3B, 0xB0, 0x3B, 0xB8, 0x3B, 
  0xB8, 0x3B, 0xBA, 0x3B, 0xBB, 0x3B, 0xBB, 0x3B, };

unsigned char  signal3_icon16x16[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x00, 0x03, 0x80, 0x03, 0x80, 0x03, 0xA0, 0x03, 0xB0, 0x03, 0xB8, 0x03, 
  0xB8, 0x03, 0xBA, 0x03, 0xBB, 0x03, 0xBB, 0x03, };

unsigned char  signal2_icon16x16[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x30, 0x00, 0x38, 0x00, 
  0x38, 0x00, 0x3A, 0x00, 0x3B, 0x00, 0x3B, 0x00, };
  
unsigned char  signal1_icon16x16[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x03, 0x00, };

unsigned char noBattery_icon16x16[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x1F, 0xFE, 0x3F, 0xFF, 0x7F, 
  0x07, 0x60, 0x07, 0xE6, 0xE7, 0xEF, 0xC7, 0x60, 0xFF, 0x7F, 0xFE, 0x3F, 
  0xFC, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };


unsigned char bat0_icon16x16[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0x01, 0x40, 0x01, 0xC0, 
  0x01, 0xC0, 0x01, 0xC0, 0x01, 0xC0, 0x01, 0xC0, 0x01, 0xC0, 0x01, 0x40, 
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };


unsigned char bat1_icon16x16[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0x01, 0x40, 0x1D, 0xC0, 
  0x1D, 0xC0, 0x1D, 0xC0, 0x1D, 0xC0, 0x1D, 0xC0, 0x1D, 0xC0, 0x01, 0x40, 
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char bat2_icon16x16[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0x01, 0x40, 0xDD, 0xC1, 
  0xDD, 0xC1, 0xDD, 0xC1, 0xDD, 0xC1, 0xDD, 0xC1, 0xDD, 0xC1, 0x01, 0x40, 
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char bat3_icon16x16[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x7F, 0x01, 0x40, 0xDD, 0xDD, 
  0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0x01, 0x40, 
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char batplug_icon16x16[] = {
  0x20, 0x08, 0x70, 0x1C, 0x50, 0x14, 0x50, 0x14, 0xFC, 0x7F, 0x04, 0x40, 
  0xFC, 0x7F, 0xF8, 0x3F, 0xF8, 0x3F, 0xF8, 0x3F, 0xF8, 0x3F, 0xE0, 0x0F, 
  0xC0, 0x07, 0x80, 0x03, 0x80, 0x03, 0x80, 0x03, }; 

unsigned char void_icon16x16[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

unsigned char blink_icon16x16[] = {
  0x00, 0x00, 0xC0, 0x07, 0xF0, 0x1F, 0xF8, 0x3F, 0xFC, 0x7F, 0xFC, 0x7F, 
  0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0xFC, 0x7F, 
  0xFC, 0x7F, 0xF8, 0x3F, 0xF0, 0x1F, 0xC0, 0x07, };

unsigned char noblink_icon16x16[] = {
  0x00, 0x00, 0xC0, 0x07, 0x30, 0x18, 0x08, 0x20, 0x04, 0x40, 0x04, 0x40, 
  0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x04, 0x40, 
  0x04, 0x40, 0x08, 0x20, 0x30, 0x18, 0xC0, 0x07, };

char channelLevelMsgCommand[] = "/ch/01/mix/02/level";
char tapCommand [] = "/fx/1/par/01";
char leftButtonPressedCommand[] = "/rtn/1/mix/on";
char leftButtonLongPressedCommand[] = "/rtn/2/mix/on";
char rightButtonPressedCommand[] = "/rtn/3/mix/on";
char rightButtonLongPressedPressedCommand[] = "/rtn/4/mix/on";

String ChannelName[]={"CH1Name","CH2Name","CH3Name","CH4Name"}; // 

int previousButtonState[]={false,false,false};

#define LEFTBPRESS 0
#define LEFTBLONGPRESS 1
#define RIGHTBPRESS 2
#define RIGHTBLONGPRESS 3

#define CONTROL_SCREEN 1
#define INFO_SCREEN 2
#define CONFIG_SCREEN 3

int screenID = INFO_SCREEN;

RemoteDebug Debug;
#define DEVICEID "XR16Footswitch"

int indexChannel=0;
char* buttonCommand[]={leftButtonPressedCommand,leftButtonLongPressedCommand,rightButtonPressedCommand,rightButtonLongPressedPressedCommand};

unsigned char* signalStrenghtIcons[]={void_icon16x16,signal1_icon16x16,signal2_icon16x16,signal3_icon16x16,signal4_icon16x16 };
unsigned char* batteryStrenghtIcons[]={void_icon16x16,bat0_icon16x16,bat1_icon16x16,bat2_icon16x16,bat3_icon16x16 };
unsigned char* blinkIcons[]={noblink_icon16x16,blink_icon16x16 };

//The next three variables are controlling the blink rate of the tempo LED
byte tap = 0 ;      // icon to show ON or OFF
long previousMillis = 0;  // will store last time LED was updated
long currentMillis = 0;

long tapDelay = 1;     // interval at which to blink (milliseconds) that is restricted to 100ms-2000ms
long tapElapsedTime=0;
ArduinoTapTempo tapTempo;

int levelIncrement = 1;
int barGraphVolumeLevel = 0;
long loopElapsedTime = 0;
long blinkElapsedTime = 0;
byte lowBat = 0;

// ***************************************************
int button[]={D3,D4,D0};
#define LEFTB 0
#define RIGHTB 1
#define MIDDLEB 2

// Define the number of samples to keep track of. The higher the number, the
// more the readings will be smoothed, but the slower the output will respond to
// the input. Using a constant rather than a normal variable lets us use this
// value to determine the size of the readings array.
const int numReadings = 5;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int averageVoltage = 0;                // the averageVoltage
int inputPin = A0;              // voltage in A0

int push_time[]={0,0};
int countTapDown = 0;
bool previousTapButtonState = 0;
//*******************
boolean channelOn[]={true,true,true,true};
int numberOfChannels = sizeof(channelOn)/sizeof(channelOn[0]);
// ***************************************************

const char *ssid = DEVICEID;            //Ap SSID

String Essid = "";                  //EEPROM Network SSID
String Epass = "";                 //EEPROM Network Password
String EoutPort = "";
String sssid = "";                  //Read SSID From Web Page
String passs = "";                 //Read Password From Web Page
String soutport = "";               //Read port From Web Page

WiFiUDP OscUDP;                                // A OscUDP instance to let us send and receive packets over OscUDP
int RepeatRead       = 10;       // Max repeat reading of OSC returns
int iValue            = -1;
float fValue          = -0.1;
char   PacketBuffer[UDP_TX_PACKET_MAX_SIZE];   // Setup global variables
String CommandStr;
char   TypeStr; 
String ExpectedStr;
char sValue[25];
//OSCErrorCode error;

long rssiVal=0;
int batteryIconIndex=3;
#define BLINK_DELAY 500

IPAddress outIP(192,168,100,0);       // IP of the XRxx in Comma Separated Octets, NOT dots!
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)
int outPort = 0;          // XR18=10023 XR16=10024

SSD1306  display(0x3C, D2, D1); // Yep, we can use SCL in this library too. Someone tell me the difference?

#define MONITOR_REFRESH 9000
ESP8266WebServer server(80);

void restartMonitor(){  
  Debug.println("restartMonitor\n");
  acquireVoltage();
  if (outIP[3]!=255 && outIP[3]!=0){
    SendOSC("/xremote","");
    getMixerState();
  }
}

bool findMixer(){
  Debug.print("findMixer(): ");
  outIP = WiFi.localIP();
  outIP[3] = 255;  //broadcast
  OscUDP.stop();      
  OscUDP.begin(localPort);        
  SendOSC("/xinfo","");
  if (OSCRead()){
    outIP.fromString(sValue);
    Debug.println(sValue);
    return true;    
  }else{
    Debug.println("no answer");
    return false;  
  }
}

void setup() {
//    Serial.begin(115200);             // DEBUG window
  delay(200);                            //Stable Wifi

  EEPROM.begin(512);

  for (int buttonIndex=0; buttonIndex<2;buttonIndex++){
    pinMode(button[buttonIndex], INPUT);
    previousButtonState[buttonIndex] = digitalRead(button[buttonIndex]) == HIGH;     
  }
  previousTapButtonState = digitalRead(button[MIDDLEB])==LOW;

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = analogRead(inputPin);
    total+= analogRead(inputPin);  
  }
  acquireVoltage();
  
  Debug.begin(DEVICEID);  
    
  display.init();
  screenID = INFO_SCREEN;

  ////////////////////////////Reading EEProm SSID-Password//////////////////////////////////////////////
  for (int i = 0; i < 32; ++i) Essid += char(EEPROM.read(i)); 
  for (int i = 32; i < 96; ++i) Epass += char(EEPROM.read(i)); 
  for (int i = 96; i < 160; ++i) EoutPort += char(EEPROM.read(i));
  if (Essid.length() > 1) outPort = EoutPort.toInt(); 

  if ( Essid.length() > 1 ) { 
    // Connect to WiFi network
    WiFi.mode(WIFI_STA);
    WiFi.begin(Essid.c_str(), Epass.c_str());
  
    //Wait for connection
    while (WiFi.status() != WL_CONNECTED && (millis() - loopElapsedTime < MONITOR_REFRESH)) GUIresfresh();
  
    loopElapsedTime = 0;
    if (WiFi.status() == WL_CONNECTED){
      
      Debug.begin(DEVICEID);        

      ArduinoOTA.setHostname(DEVICEID); // on donne une petit nom a notre module
      ArduinoOTA.begin(); // initialisation de l'OTA
    
      Debug.setResetCmdEnabled(true);  
      
      startWebService();
      if (findMixer()) getMixerState(); 
    }
    screenID = CONTROL_SCREEN;
    return;  
  }
  enterConfig();
}

void ClearEeprom(){
  Debug.println("Clearing Eeprom");
  for (int i = 0; i < 160; ++i) { EEPROM.write(i, 0); }
}

void enterConfig(){
  screenID=CONFIG_SCREEN;  
  WiFi.softAP(DEVICEID);      
  delay(10);
  startWebService();
}


void startWebService(){
  MDNS.begin(DEVICEID) ;     
  server.on("/", handleRoot);
  server.on("/update", handleUpdateCall);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/a",handleGetReq); 
  server.onNotFound(handleNotFound);
  server.begin();   
}

bool getchannelOnState(int channelIndex){
  SendOSC(buttonCommand[channelIndex],"");  
  if (OSCRead()){
    Debug.println("Retrieved channel mute");
    channelOn[channelIndex] = iValue;
    return true;
  }else return false;     
}


void getChannelNames(){
  for (int ChanIndex=0;ChanIndex<numberOfChannels;ChanIndex++){
    SendOSC(String("/rtn/"+String(ChanIndex+1)+"/config/name"),"");
    if (OSCRead()){
      Debug.println("Retrieved channel name");
      ChannelName[ChanIndex]=sValue;
    }    
  }
}

void getAllchannelOnState(){
  for (int channelIndex=0;channelIndex<numberOfChannels;channelIndex++) getchannelOnState(channelIndex);
}


void getChannelVolume(){ 
  SendOSC(channelLevelMsgCommand,"");
  if (OSCRead()){
    Debug.println("Retrieved volume level");
    barGraphVolumeLevel = fValue * 100;    
  }else{
    outIP[3]=255;
    Debug.println("Lost mixer link... ");        // we use this function zhich is periodically called by restartMonitor->getConsoleStatus to ensure connectivity    
  }
}

void getTapDelay(){
  SendOSC(tapCommand,"");
  if (OSCRead()){
    Debug.println("Retrieved tap");
    tapDelay = ((fValue*3000)+ 1.5);                 // linear float conversion from OSC value to msValue1. for FX1 tempo
    if (tapDelay > 1500) tapDelay = (tapDelay - 1);  // to correct some goofy floating point math issue 
  }  
}

void getMixerState(){
  Debug.println("Get mixer state...");
  getChannelVolume();
  getTapDelay();
  getAllchannelOnState();
  getChannelNames();    
}

void GUIresfresh(){ 
String mixer="",chName="";

  if (outPort==10023) mixer+="XR18";
  if (outPort==10024) mixer+="XR16";   
  display.drawString(display.getWidth()/2, 40 ,mixer);

  if (averageVoltage>600) batteryIconIndex = 4;
  if (averageVoltage>560 && averageVoltage<=600 ) batteryIconIndex = 3; 
  if (averageVoltage>530 && averageVoltage<=560 ) batteryIconIndex = 2; 
  if (averageVoltage>490 && averageVoltage<=530 ) batteryIconIndex = 1; 
  if (averageVoltage<=490){
    if (millis() - blinkElapsedTime > BLINK_DELAY)
      if (batteryIconIndex==0) batteryIconIndex = 1; else batteryIconIndex = 0;
  }

  if (WiFi.status() != WL_CONNECTED){
    if (millis() - blinkElapsedTime > BLINK_DELAY) {
      if (rssiVal == 0) rssiVal=4 ; else rssiVal = 0;
    }
  }else
    rssiVal=((100-abs(WiFi.RSSI()))/15);

  if (millis() - blinkElapsedTime > BLINK_DELAY) blinkElapsedTime = millis();
  
  display.clear();
  display.normalDisplay();

  // bottom screen remain the same for all
  display.drawXbm(0, display.getHeight()-16, 16, 16, signalStrenghtIcons[rssiVal]);    
  display.drawProgressBar(18, display.getHeight()-16, 128-36-16-5, 15, barGraphVolumeLevel);  
  display.drawXbm(display.getWidth()-16-16-3, display.getHeight()-16, 16, 16, blinkIcons[tap]); 
  display.drawXbm(128-16, display.getHeight()-16, 16, 16, batteryStrenghtIcons[batteryIconIndex]); 

  // then specific screen
  switch (screenID){
    case CONTROL_SCREEN:
      if (outIP[3]!=255 && outIP[3]!=0 && WiFi.status() == WL_CONNECTED){
        for (int indexChannel=0;indexChannel<numberOfChannels;indexChannel++){
          if (channelOn[indexChannel]) display.drawRect(indexChannel*(display.getWidth()/4), 11, (display.getWidth()/4)-2, display.getHeight()-16-11);
          else display.fillRect(indexChannel*(display.getWidth()/4), 11, (display.getWidth()/4)-2, display.getHeight()-16-11);
          
          display.setTextAlignment(TEXT_ALIGN_LEFT);
          display.setFont(ArialMT_Plain_10); //10 16 24
          chName = String(ChannelName[indexChannel]).substring(0,6);
          chName.toLowerCase();
          display.drawString(indexChannel*(display.getWidth()/4), 0, chName);        
        } 
      }
      if (outIP[3]==255 || outIP[3]==0){
          barGraphVolumeLevel = (barGraphVolumeLevel+10) % 100;
          display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
          display.setFont(ArialMT_Plain_16); //10 16 24
          display.drawString(display.getWidth()/2, 16, "Searching "+mixer);               
      }
      if (WiFi.status() != WL_CONNECTED){
          barGraphVolumeLevel = (barGraphVolumeLevel+10) % 100;
          display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
          display.setFont(ArialMT_Plain_16); //10 16 24
          display.drawString(display.getWidth()/2, 33, "Searching WIFI...");                  
      }
      
      break;
    case INFO_SCREEN:
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.setFont(ArialMT_Plain_10); //10 16 24
      display.drawString(display.getWidth()/2, 10, "Wifi: " + Essid);        
      display.drawString(display.getWidth()/2, 20 ,"IP: "+ WiFi.localIP().toString()+"/"+outIP[3]);     
      display.drawString(display.getWidth()/2, 30 ,"Signal: "+ String(WiFi.RSSI())+"dB bat.:"+String(averageVoltage)+"v"); 
      display.drawString(display.getWidth()/2, 40, "Mixer: "+mixer);    
      break;
    case CONFIG_SCREEN:
      display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
      display.setFont(ArialMT_Plain_10); //10 16 24  
      display.drawString(display.getWidth()/2, 10 ,"SSID: "+String(DEVICEID));
      display.drawString(display.getWidth()/2, 20 ,"Connect WIFI and open");   
      display.drawString(display.getWidth()/2, 30 ,"http://192.168.4.1");  
      if (WiFi.softAPgetStationNum()==0) display.drawString(display.getWidth()/2, 40 ,"Waiting connexion...");
      else display.drawString(display.getWidth()/2, 40 ,"Connection establised!");
      break;
  }

  display.display();      
}

void acquireVoltage() {
  // analog reading input voltage
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(inputPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;
  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
  // calculate the averageVoltage:
  averageVoltage = total / numReadings; 
}

void loop() { 

  ArduinoOTA.handle();
  Debug.handle();
  server.handleClient();
  GUIresfresh();

  if (outIP[3]==255 || outIP[3]==0) findMixer();

  boolean tapDown = digitalRead(button[MIDDLEB])==LOW;
  tapTempo.update(tapDown);
  if (!previousTapButtonState && tapDown){
    if (countTapDown<15){
      if (countTapDown<6){
        SendInt2OSC(tapCommand,tapTempo.getBeatLength());
        Debug.println("short tapDown");  
        display.invertDisplay();
      }        
    }
    if (countTapDown>6 and countTapDown<20){
      Debug.println("long tapDown");  
    }
    else if (countTapDown == 21){
      Debug.println("hold tap released");
      screenID = CONTROL_SCREEN; 
    }
    countTapDown=0;
  }
  previousTapButtonState = tapDown;
  if (!tapDown){
    countTapDown++;
    if (countTapDown>20){
       screenID = INFO_SCREEN; 
       countTapDown = 21; // to avoid overflow
       //Debug.println(countTapDown);    
    }
  }
  
  

  for (int i=0;i<2;i++){
    boolean input = digitalRead(button[i]) == HIGH;
    if (!previousButtonState[i] && input){
      if (push_time[i]<15){
        if (push_time[i]<6){
          Debug.println("left short push"); 
          indexChannel=0+(i*2);
        }
        if (push_time[i]>6 and push_time[i]<20){
          Debug.println("left long push"); 
          indexChannel=1+(i*2);
        }
        if (getchannelOnState(indexChannel)){ //only perform switch if mixer reply first
          channelOn[indexChannel] = !channelOn[indexChannel];
          SendInt2OSC(buttonCommand[indexChannel],channelOn[indexChannel]);
        }
      }else{
       Debug.println("hold button released"); 
       //input = previousButtonState;
      }
      push_time[i]=0;
      levelIncrement = 1;
    }
    previousButtonState[i]=input;
    if(!input){
      push_time[i]++;
      if (push_time[i]>20){
        delay(100-(levelIncrement*3));
        if (i==0){
          if (barGraphVolumeLevel>levelIncrement){
            Debug.println("down");   
            SendFloat2OSC(channelLevelMsgCommand,float((barGraphVolumeLevel-levelIncrement)/100.0));
            levelIncrement=levelIncrement+1;
          }         
        }else{
          if (barGraphVolumeLevel<100){
            Debug.println("up");     
            SendFloat2OSC(channelLevelMsgCommand,float((barGraphVolumeLevel+levelIncrement)/100.0));
            levelIncrement=levelIncrement+1;
          }
        }
        if (!previousButtonState[0] && !previousButtonState[1]){
          enterConfig();
        }       
      }
      
    }
    delay(20);
  }
  
  
  if (millis() - loopElapsedTime > MONITOR_REFRESH) {
    restartMonitor();
    loopElapsedTime = millis();
  }

  if (millis() - tapElapsedTime > (tapDelay / 1)) {
    if (tap==0 && tapDelay > 1) tap=1; else tap = 0;
    tapElapsedTime = millis();
  }
 
  // next sequence (for bargraph progression)
  //batteryIconIndex = (batteryIconIndex+1) % 4;
  //delay(50);
  
  if (OSCReadIn()){
    if (CommandStr == channelLevelMsgCommand) barGraphVolumeLevel = fValue * 100;
    if (CommandStr == tapCommand){
      tapDelay = ((fValue*3000)+ 1.5);                 // linear float conversion from OSC value to msValue1. for FX1 tempo
      if (tapDelay > 1500) tapDelay = (tapDelay - 1);  // to correct some goofy floating point math issue 
    }
    for (int channelIndex=0;channelIndex<numberOfChannels;channelIndex++) if (CommandStr == buttonCommand[channelIndex]) channelOn[channelIndex] = iValue;
  } 
}
  

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// WEBSERVER UTILITY ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this gets called if you enter http://yourwemosaddress/update?message=SomeMessageText in a web browser
void handleUpdateCall() { 

  String message = "";
  
  //Message parameter wasn't found  
  if (server.arg("message")== ""){     
  
    message = "Message Argument not found";
  
  }else{
    // message was found, let's display it on the OLED
    message = "";
    message += server.arg("message");     //Gets the value of the query parameter
    display.clear();
    display.drawString(display.getWidth()/2, display.getHeight()/2, message);
    display.display();
  
  }
  
  server.send(200, "text/plain", "OK");          //Returns the HTTP response
}

// this function is what gets called if you enter the base ip address of your Wemos in the browser
void handleRoot() {
  int Tnetwork=0,i=0,len=0;
  String st="",s="";                    //String array to store the SSID's of available networks
  Tnetwork = WiFi.scanNetworks();       //Scan for total networks available
  st = "<ul>";
  for (int i = 0; i < Tnetwork; ++i)
    {
      // Print SSID and RSSI for each network found
      st += "<li>";
      st +=i + 1;
      st += ": ";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      st += "</li>";
    }
      st += "</ul>";
   IPAddress ip = WiFi.softAPIP();                  //Get ESP8266 IP Adress
        //String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        s = "\n\r\n<!DOCTYPE HTML>\r\n<html><h1>"+String(DEVICEID)+"</h1> ";
        //s += ipStr;
        s += "<p>";
        s += st;
        s += "<p>";
        s += "<form method='get' action='a'><label>SSID: </label><input name='ssid' length=32><label>Password: </label><input name='pass' length=64><label>Console: </label><select name='outport'><option value='10023'>XR18</option><option value='10024'>XR16</option></select><input type='submit'></form>";
        s += "</html>\r\n\r\n";
      
    server.send( 200 , "text/html", s);
}

// this gets called if no function is for your address
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void handleGetReq(){

  if (server.hasArg("ssid") && server.hasArg("pass") && server.hasArg("outport")){  
   sssid=server.arg("ssid");//Get SSID
   passs=server.arg("pass");//Get Password
   soutport=server.arg("outport");//Get console port
  }

  if(sssid.length()>1 && passs.length()>1 && soutport.length()>1){
     ClearEeprom();//First Clear Eeprom
     delay(10);
     for (int i = 0; i < sssid.length(); ++i)
          {
            EEPROM.write(i, sssid[i]);
          }
        
     for (int i = 0; i < passs.length(); ++i)
          {
            EEPROM.write(32+i, passs[i]);
          }    
     for (int i = 0; i < soutport.length(); ++i)
          {
            EEPROM.write(96+i, soutport[i]);
          } 
          
     EEPROM.commit();
        
        String s = "\r\n\r\n<!DOCTYPE HTML>\r\n<html><h1>"+String(DEVICEID)+"</h1> ";
        s += "<p>Password Saved... Reset to boot into new wifi</html>\r\n\r\n";
        server.send(200,"text/html",s);
  }
      
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// OSC MESSAGE UTILITY ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

boolean OSCRead()              // Reread until return is what is expected (or it reaches 10 tries)
{
  int cntr = 1;
  boolean RespOK = OSCReadBack();
  while (!RespOK && cntr<10)
  {
    cntr++;
    RespOK = OSCReadBack();
  }
  return RespOK;
}

boolean OSCReadIn(){
  iValue = -1;
  fValue = -0.1;
  // Check if packet was received
  //Debug.println("ReadInOSC()");
  int size = OscUDP.parsePacket();
  if (size < 1) return false;
  // Get the packet
  OSCMessage msg;
  while (size--) msg.fill(OscUDP.read());
  // Check for errors
  if (msg.hasError()){
    Debug.println("error: "+msg.getError());
    return false;
  }
  // Parse return OSC command  
  msg.getAddress(PacketBuffer,0);                 // get message path
  CommandStr = String(PacketBuffer);              // assign to CommandStr
  TypeStr = msg.getType(0);                       // get data type
  
  if (msg.isInt(0))    iValue = msg.getInt(0);    // integer
  if (msg.isFloat(0))  fValue = msg.getFloat(0);  // float
  if (msg.isString(0)) msg.getString(0,sValue);   // string

  //Debug.println("Received: adress="+CommandStr+" iValue="+iValue+" fValue="+fValue+" sValue="+sValue);
  msg.empty();
  return true;
}

//============ Process OSC msg ============================================================================================  
boolean OSCReadBack()        
{
  //Debug.println("OSCReadBack");
  delay(10);  
  if (!OSCReadIn()) return false;
  if (CommandStr==ExpectedStr) return true; else return false;
}

//======== Send OSC w/Value ==========================================================================
void SendOSC(String CmdStr, String Val)  // send an OSC command (string) with option value (string)
{
  char PathBuffer[30];
  char ValBuffer[30];
  ExpectedStr = Val;

  Debug.println("SendOSC("+CmdStr+","+Val+")");
  
  CmdStr.toCharArray(PathBuffer, CmdStr.length()+1);                  // Transfer CmdStr to buffer
  if (Val="") SendCmd2OSC(PathBuffer);                              // No value
  else if (Val.toInt()) SendInt2OSC(PathBuffer,Val.toInt());        // Integer value
  else if (Val.toFloat()) SendFloat2OSC(PathBuffer,Val.toFloat());  // Float value
  else
  {                                                                 // String value
    CmdStr.toCharArray(ValBuffer, sizeof(ValBuffer));
    SendString2OSC(PathBuffer,ValBuffer);
  }
}

//======== Send OSC no Value (request) ================================================================
void SendCmd2OSC(char* address)       // Send OSC command (buffer), no value (status request)
{ 
  OSCMessage msg(address);
  ExpectedStr = address;
  Debug.println("OSCUdp.msg.send("+outIP.toString()+","+outPort+","+address+")");
  OscUDP.beginPacket(outIP, outPort);
  msg.send(OscUDP);
  OscUDP.endPacket();
  msg.empty();
}

//======== Send OSC w/Int ============================================================================
void SendInt2OSC(char* address, int val)   // Send OSC command (buffer) with integer value
{
  OSCMessage msg(address);
  ExpectedStr = address;
  msg.add(val);
  OscUDP.beginPacket(outIP, outPort);
  msg.send(OscUDP);
  OscUDP.endPacket();
  msg.empty();
}

//======= Send OSC w/string ==========================================================================  
void SendString2OSC(char* address, char* val)  // Send OSC command (buffer) with string value (buffer)
{ 
  OSCMessage msg(address);
  ExpectedStr = address;
  msg.add(val);
  OscUDP.beginPacket(outIP, outPort);
  msg.send(OscUDP);
  OscUDP.endPacket();
  msg.empty();
}

void SendFloat2OSC(char* address, float val)
{ 
  Debug.println("OSCUdp.msg.sendfloat("+outIP.toString()+","+outPort+","+address+")");  
  Debug.print(val);  
  OSCMessage msg(address);
  ExpectedStr = address;
  msg.add(val);
  OscUDP.beginPacket(outIP, outPort);
  msg.send(OscUDP);
  OscUDP.endPacket();
  msg.empty();
}
