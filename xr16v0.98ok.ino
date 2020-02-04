#define _V "v0.98ok"
/*
 todo/bug:
	fix:	need to fix the tap calculation send and converted depending on FX ex. Hz
	todo:	implement battery charging icon by moving the acquire voltage into loop() and check if voltage's up or down. 
	todo:	implement "usb" connected icon
	todo:	implement another web page handler such /info to display all info...
	todo:	populate a combo to display list of available Wifi so user can select instead of entering manually the name (must still remain valid for hidden wifi)
  todo: pass javascript array for names to webconfig
  todo: FADER_SPEED as parameter
  todo: connect to mobile APP
  todo: cursor on mute screen to show level of FX
  fix:  level increment when decreasing totally avoid than we go to zero... we have to do in two steps.
  
 version control
	v0.95 	  implemented FX level control
			      code comment and clean up
	v0.96a    added levelbar indicator in fx mute	 + enhanced config screen
	v0.96ok   2 consecutives read to secure button input	
  v0.97ok   implement UI interface with screen scroll feature
  v0.98ok   start implemetation of chanel and fx name in webconfig
*/

#include <RemoteDebug.h>		    // allow telnet in addition to ////Serial.println
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WifiUDP.h>
#include <SSD1306.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <OSCMessage.h>       
#include <ArduinoTapTempo.h>	 // specific library making easy to calculate tap delay based on button press 
#include <ArduinoOTA.h> 		  // for WIFI program upload 
#include <WebConfig.h>			  // used to manage the persistent parameters thru web browser (has been customized), see https://github.com/GerLech/WebConfig
#include "OLEDDisplayUi.h"    

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
  0x00, 0x00, 0xC0, 0x07, 0x30, 0x1C, 0x18, 0x31, 0xCC, 0x67, 0xE4, 0x4F, 
  0xF6, 0x9F, 0xF2, 0x9F, 0xFA, 0xBF, 0xF2, 0x9F, 0xF2, 0xDF, 0xE4, 0x4F, 
  0xCC, 0x67, 0x18, 0x31, 0x70, 0x18, 0xC0, 0x07, };

unsigned char noblink_icon16x16[] = {
  0x00, 0x00, 0xC0, 0x07, 0x30, 0x18, 0x08, 0x20, 0x04, 0x40, 0x04, 0x40, 
  0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x02, 0x80, 0x04, 0x40, 
  0x04, 0x40, 0x08, 0x20, 0x30, 0x18, 0xC0, 0x07, };

unsigned char channel_icon16x16[] = {
  0x00, 0x00, 0x00, 0x00, 0x80, 0x19, 0x00, 0x3F, 0x60, 0x7E, 0xC0, 0x7F, 
  0x88, 0x3F, 0xF0, 0x3F, 0xE0, 0x6F, 0xF0, 0x47, 0xF8, 0x0B, 0xFC, 0x11, 
  0xFE, 0x02, 0x7F, 0x04, 0x3F, 0x00, 0x1F, 0x00, 0x0F, 0x00, };

unsigned char fx_icon16x16[] = {
  0xFF, 0x07, 0xFF, 0x03, 0xFF, 0x01, 0xFF, 0x00, 0x0F, 0x00, 0x0F, 0x00, 
  0xFF, 0x00, 0x7F, 0x00, 0xFF, 0xF3, 0xDF, 0x61, 0x8F, 0x33, 0x0F, 0x1F, 
  0x0F, 0x0E, 0x0F, 0x1F, 0x8F, 0x33, 0xCF, 0x61, 0xEF, 0xF3, };

unsigned char usb_icon16x16[] = {
  0x00, 0x00, 0x80, 0x01, 0xC0, 0x03, 0xE0, 0x07, 0x8C, 0x01, 0x9E, 0x31, 
  0x9E, 0x79, 0x8C, 0x31, 0xFC, 0x3F, 0xF8, 0x1F, 0x80, 0x01, 0xC0, 0x03, 
  0xE0, 0x07, 0xE0, 0x07, 0xC0, 0x03, 0x00, 0x00, };

unsigned char* signalStrenghtIcons[]={void_icon16x16,signal1_icon16x16,signal2_icon16x16,signal3_icon16x16,signal4_icon16x16 };
unsigned char* batteryStrenghtIcons[]={void_icon16x16,bat0_icon16x16,bat1_icon16x16,bat2_icon16x16,bat3_icon16x16};
unsigned char* blinkIcons[2][2]={
  {noblink_icon16x16,blink_icon16x16},
  {channel_icon16x16,fx_icon16x16}
};

// PERSISTENT PARAMETERS
WebConfig conf;
String params = "["
  "{"
  "'name':'ssid',"
  "'label':'Wifi name',"
  "'type':"+String(INPUTTEXT)+","
  "'default':''"
  "},"
  "{"
  "'name':'pwd',"
  "'label':'Wifi Pass',"
  "'type':"+String(INPUTTEXT)+","
  "'default':''"
  "},"
  "{"
  "'name':'mixer',"
  "'label':'Mixer',"
  "'type':"+String(INPUTSELECT)+","
  "'options':["
  "{'v':'10023','l':'XR18'},"
  "{'v':'10024','l':'XR16'}],"
  "'default':'10024'"
  "},"  
  "{"
  "'name':'duration',"
  "'label':'Button delay',"
  "'type':"+String(INPUTRANGE)+","
  "'min':60,'max':100,"
  "'default':'80'"
  "},"
  "{"
  "'name':'mode',"
  "'label':'Mode',"
  "'type':"+String(INPUTSELECT)+","
  "'options':["
  "{'v':'0','l':'MUTE CONTROL'},"
  "{'v':'1','l':'VOLUME CONTROL'}],"
  "'default':'0'"
  "},"   
  "{"
  "'name':'channel0',"
  "'label':'My channel',"
  "'type':"+String(INPUTRANGE)+","
  "'min':1,'max':18,"
  "'options':["
  "{'v':'channel','l':'getCaptionName'}]," 
  "'default':'1'"
  "},"
  "{"
  "'name':'mymonitor',"
  "'label':'My monitor',"
  "'type':"+String(INPUTRANGE)+","
  "'min':1,'max':8,"
  "'default':'1'"
  "},"  
  "{"
  "'name':'fxtapcontrol',"
  "'label':'FX tap',"
  "'type':"+String(INPUTRANGE)+","
  "'min':1,'max':4,"
  "'default':'1'"
  "},"  
  "{"
  "'name':'channel1',"
  "'label':'Channel 1',"
  "'type':"+String(INPUTRANGE)+","
  "'min':1,'max':18,"
  "'default':'1'"
  "},"  
  "{"
  "'name':'channel2',"
  "'label':'Channel 2',"
  "'type':"+String(INPUTRANGE)+","
  "'min':1,'max':18,"
  "'default':'1'"
  "},"  
  "{"
  "'name':'channel3',"
  "'label':'Channel 3',"
  "'type':"+String(INPUTRANGE)+","
  "'min':1,'max':18,"
  "'default':'1'"
  "},"  
  "{"
  "'name':'channel4',"
  "'label':'Channel 4',"
  "'type':"+String(INPUTRANGE)+","
  "'min':1,'max':18,"
  "'default':'1'"  
  "}"                           
  "]";

// BEHRINGER PROTOCOL IMPLEMENTATION
const char LEVELCOMMAND_STRING[] = "/ch/%02d/mix/%02d/level";  		// setlevel of channel xx on bus yy     7,8,9,10 are FX bus
const char CHGETNAMECOMMAND_STRING[] = "/ch/%02d/config/name";		// fx %i on master   
const char FXMUTECOMMAND_STRING[] = "/rtn/%i/mix/on";    			// fx mute on master
const char TAPCOMMAND_STRING[] = "/fx/%i/par/01"; 					//set parameter 01 = tap delay on FX %i
const char FXGETNAMECOMMAND_STRING[] = "/rtn/%i/config/name";    	// fx %i on master
char OSCCommand[40];												// this is the buffer used to generate the OSC message using all template above

// replicate names as they are in the console
String FXName[]={"FX1????????","FX2????????","FX3????????","FX4????????"};
String CHName[]={"CH1????????","CH2????????","CH3????????","CH4????????","CH5????????","CH6????????","CH7????????","CH8????????","CH9????????","CH10???????","CH11???????","CH12???????","CH13???????","CH14???????","CH15???????","CH16???????","CH17???????","CH18???????"};

// VARIABLES USED TO MAP MIXER STATE/SETTINGS
int volumes[] = {0,0,0,0,0,0,0,0,0};    //mychannel,channel1, channel2, channel3, channel4, fx1,fx2, fx3, fx4 on my channel     
boolean fxOn[]={true,true,true,true};	//state of the master mute
long tapDelay = 1;     					// interval at which to blink (milliseconds) usually 100ms-2000ms
long tapElapsedTime=0;		 
ArduinoTapTempo tapTempo;	

// GUI Elements and variables 
#define MODE_MUTE 0
#define MODE_LEVEL 1
#define 	SUBMODE_LEVEL_CH 0 
#define 	SUBMODE_LEVEL_FX 1
int selector=0;
#define CONTROL_SCREEN_MUTE 0
#define CONTROL_SCREEN_LEVEL 1
#define INFO_SCREEN 2
#define CONFIG_SCREEN 3
#define SEARCHING_MIXER 4
int screenID = INFO_SCREEN;
byte fxOrChVolumeControl = SUBMODE_LEVEL_CH;
int barGraph=0;
int levelIncrement = 1;
#define BLINK_DELAY 500
long rssiVal=0;
int batteryIconIndex=3;
String friendlyName="",mixer="undefined";      // channel or fx name 
SSD1306  display(0x3C, D2, D1);     //D2=SDA D1=SCK
OLEDDisplayUi ui     ( &display );
// This array keeps function pointers to all frames
// frames are the single views that slide in

// Utility variables to control and monitor the flow
#define MONITOR_REFRESH 9000  // REFRESH AND CHECK MIXER EVERY 9 SECONDS
RemoteDebug Debug;				    // telnet 
long loopElapsedTime = 0;
long blinkElapsedTime = 0;
byte tap = 0 ;      			    // icon to show ON or OFF
long previousMillis = 0;  		// will store last time tap was updated
long currentMillis = 0;			  // 
#define FADER_SPEED 150       //ms


// HARDWARE FEATURES AND SETTINGS
#define LEFTB 	0
#define RIGHTB 	1
#define MIDDLEB 2
#define DEBOUNCE_TIME 25
int previousButtonState[]={false,false,false};
int button[]={D3,D4,D0};
int push_time[]={0,0};
int countTapDown = 0;
bool previousTapButtonState = 0;

//	VOLTAGE MEASUREMENT TO DISPLAY THE BATTERY CHARGE ICON
const int numReadings = 5;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int averageVoltage = 0;         // the averageVoltage
int inputPin = A0;              // voltage in A0
byte lowBat = 0;				

//OSC AND MESSAGING MANAGEMENT
int RepeatRead       = 10;       				// Max repeat reading of OSC returns
int iValue            = -1;						// values returned by the mixer
float fValue          = -0.1;
char   PacketBuffer[UDP_TX_PACKET_MAX_SIZE];   	
String CommandStr;							
char   TypeStr; 
String ExpectedStr;
char sValue[25];

//NETWORK SETTINGS AND VARIABLES
WiFiUDP OscUDP;                       	// A OscUDP instance to let us send and receive packets over OscUDP
IPAddress outIP(192,168,100,0);       	
const unsigned int localPort = 8888;	// local port to listen for OSC packets (actually not used for sending)
ESP8266WebServer server;

// Function called every MONITOR_REFRESH ms to start over the dialog mixer->footswitch and check if we didn't loose connectivity
void restartMonitor(){  
  //Debug.println("restartMonitor\n");
  acquireVoltage();						        // we read the voltage not at every loop to avoid having values changing too often
  if (outIP[3]!=255 && outIP[3]!=0){	// if the variable has not been set or was force to these value, it means we cannot not read from the mixer
	  SendOSC("/xremote","");				    // otherwise we ask mixer to send message for the next 10 sec (see behringer protocol)
    getMixerState();		              // and retrieve mixer state to refresh values (we could remove the ("if OSCread")
  }
}

bool findMixer(){
  //Debug.println("findMixer(): ");
  outIP = WiFi.localIP();							//we use our own IP to forge a broadcast adress. ex. 192.168.100.255
  outIP[3] = 255;  									//broadcast
  // we don't pull at each loop to avoid flooding the network with query, instead we do it at the BLINK period
  if (millis() - blinkElapsedTime > BLINK_DELAY){
    OscUDP.flush();                      
    barGraph = (barGraph+10) % 100;
    screenID = SEARCHING_MIXER;
    ui.switchToFrame(screenID);    
    SendOSC("/xinfo","");
    if (OSCRead()){
      outIP.fromString(sValue);            //we retrieve the full IP of the mixer
      getMixerState();                    // retrieve all values from mixer that can cahnge dynamically
      if (getChannelAndFxNames()){        //and also the one which are more "static" (channel names)
        if (conf.getInt("mode")==MODE_LEVEL) screenID=CONTROL_SCREEN_LEVEL;  else screenID = CONTROL_SCREEN_MUTE;
        ui.transitionToFrame(screenID);         //restore the previous screen
        return true;
      }
    }
  }
  return false;   
}

boolean initConnection() {
  boolean wifiConnected = false;
  screenID = INFO_SCREEN;  
  ui.switchToFrame(screenID);      
  WiFi.mode(WIFI_STA);
  barGraph=0;
  if (conf.values[0] != "") {
    WiFi.begin(conf.values[0].c_str(),conf.values[1].c_str());
    while (WiFi.status() != WL_CONNECTED && (millis() - loopElapsedTime < MONITOR_REFRESH)){
      ui.update(); 
      delay(MONITOR_REFRESH/100);
      barGraph = (barGraph+1) % 100;
    }
    if (WiFi.status() == WL_CONNECTED){
      wifiConnected = true; 
      OscUDP.begin(localPort);                    
	    if (findMixer()) getMixerState();    
    }
  }
  return wifiConnected;
}

void handleRoot() {
  getChannelAndFxNames();
  
  conf.chNames="'claude','vocalback','ride','snare','tom','kick','charley','-','PC-L','PC-R(clic)','Anto-voc','vocwifi','-','laurent','mitch','anto-gui','-','-',";
  conf.moniNames="'1-Gauche','2-Centre','3-Droite','4-Basse','RECORD','-','INDISPO','INDISPO',";
  conf.fxNames="";  

  for (int i=0;i<4;i++) conf.fxNames+="'"+FXName[i]+"',";

  conf.handleFormRequest(&server);				// use to display parameter and save them
  if (server.hasArg("SAVE")) {
    setMixerType();
    uint8_t cnt = conf.getCount();
    ////Serial.println("*********** Configuration ************");
    for (uint8_t i = 0; i<cnt; i++) {
      //Serial.print(conf.getName(i));
      //Serial.print(" = ");
      ////Serial.println(conf.values[i]);
    }
  }
}

void setMixerType(){
  if (conf.getInt("mixer")==10023) mixer="XR18";
  else if (conf.getInt("mixer")==10024) mixer="XR16";   
  else mixer="undefined";
}

// the config can be called either via the user by holding both button or during startup phase if no WIFI found
void enterConfig(){  
  tapDelay=1;             // we stop blinking the tap
  screenID=CONFIG_SCREEN;
  WiFi.mode(WIFI_AP);
  WiFi.softAP(conf.getApName(),"",1); 
  barGraph = 0 ;
  ui.switchToFrame(screenID);    
}

bool getfxOnState(int fxArrayIndex){
  sprintf(OSCCommand,FXMUTECOMMAND_STRING,fxArrayIndex+1); 
  SendOSC(OSCCommand,"");  
  if (OSCRead()){
    //Debug.println("Retrieved channel mute");
    fxOn[fxArrayIndex] = iValue;
    return true;
  }else return false;     
}

bool getChannelAndFxNames(){
  //Serial.printf("\n************************ GET CHANNEL AND FX NAMES *****************\n");
  for (int FXIndex=0;FXIndex<4;FXIndex++){
    sprintf(OSCCommand,FXGETNAMECOMMAND_STRING,FXIndex+1);
    SendOSC(OSCCommand,"");
    if (OSCRead()){
      //Debug.println("Retrieved fx name");
      FXName[FXIndex]=sValue;
    }else return false;    
  }
  int nbChannel=16; 
  if (conf.getInt("mixer")==10023) nbChannel=18;
  for (int CHIndex=0;CHIndex<nbChannel;CHIndex++){
    sprintf(OSCCommand,CHGETNAMECOMMAND_STRING,CHIndex+1);
    SendOSC(OSCCommand,"");
    if (OSCRead()){
      //Debug.println("Retrieved channel name");
      CHName[CHIndex]=sValue;
    }else return false;    
  } 
  return true;
}

void getAllfxOnState(){
  for (int i=0;i<4;i++) getfxOnState(i);
}

void getChannelVolume(){
  for (int i=0;i<5;i++){
    sprintf(OSCCommand,LEVELCOMMAND_STRING,conf.getInt(String("channel"+String(i)).c_str()),conf.getInt("mymonitor"));
    SendOSC(OSCCommand,"");
    if (OSCRead()){
      //Debug.println("Retrieved channel"+String(i)+"volume level");
      volumes[i] = fValue * 100;    
    }else{
      outIP[3]=255;
      //Debug.println("Lost mixer link... ");     	// we use this function zhich is periodically called by restartMonitor->getConsoleStatus to ensure connectivity    
      return;										                    // the loop() will capture the value 255 and trigger a console search
    }   
  }
}

void getFXVolume(){
	for (int bus=0;bus<4;bus++){
		sprintf(OSCCommand,LEVELCOMMAND_STRING,conf.getInt("channel0"),bus+7);     // command level of my channel to the bus 7,8,9,10 
		SendOSC(OSCCommand,"");
		if (OSCRead()){
			//Debug.println("Retrieved fxbus"+String(bus)+"volume level");
			volumes[bus+5] = fValue * 100;    
		}else return;
	}
}

void getTapDelay(){
  sprintf(OSCCommand,TAPCOMMAND_STRING,conf.getInt("fxtapcontrol"));
  SendOSC(OSCCommand,"");
  if (OSCRead()){
    //Debug.println("Retrieved tap");
    tapDelay = ((fValue*3000)+ 1.5);                 // linear float conversion from OSC value to msValue1. for FX1 tempo
    if (tapDelay > 1500) tapDelay = (tapDelay - 1);  // to correct some goofy floating point math issue 
  }  
}

void getMixerState(){
  /*we only poll rhe information can change dyncamially and be of interest
  example: we dont query the channel for instance, this should not change during a session, we only do this at startup
  but the volume and state mute, etc can be changed by others during the gig so we update them 
  */
  //Debug.println("Get mixer state...");
  getChannelVolume();						// note that we use this one to raise a disconnect issue if we find it.
  getFXVolume();
  getTapDelay();
  getAllfxOnState();   
}

void overlayControl(OLEDDisplay *display, OLEDDisplayUiState* state) {  
  // depending if we are connected or not 
  if (WiFi.status() != WL_CONNECTED){
  //we blink wifi icon
    if (millis() - blinkElapsedTime > BLINK_DELAY) {
      if (rssiVal == 0) rssiVal=4 ; else rssiVal = 0;
    } //or we display the icon representing the signal strenght
  }else rssiVal=((100-abs(WiFi.RSSI()))/15); 

  // below values may vary depending on the battery you use, so you have to make your own
  // in our case we have a 5v with a 1MOHm connected to A1
  if (averageVoltage>600) batteryIconIndex = 4;
  if (averageVoltage>560 && averageVoltage<=600 ) batteryIconIndex = 3; 
  if (averageVoltage>530 && averageVoltage<=560 ) batteryIconIndex = 2; 
  if (averageVoltage>490 && averageVoltage<=530 ) batteryIconIndex = 1; 
  if (averageVoltage<=490){
  // houston : we are really short petrol... blink the icon as we probably only have 10min left
  if (millis() - blinkElapsedTime > BLINK_DELAY)
    if (batteryIconIndex==0) batteryIconIndex = 1; else batteryIconIndex = 0;
  }
  
  display->normalDisplay();
  // bottom screen remain the same for all
  display->drawProgressBar(17, display->getHeight()-16, 128-36-16-3, 15, barGraph);   
  display->drawXbm(0, display->getHeight()-16, 16, 16, signalStrenghtIcons[rssiVal]);    
  display->drawXbm(128-16, display->getHeight()-16, 16, 16, batteryStrenghtIcons[batteryIconIndex]); 
  display->drawXbm(display->getWidth()-16-16-3, display->getHeight()-16, 16, 16, blinkIcons[conf.getInt("mode")][conf.getInt("mode")==MODE_MUTE?tap:fxOrChVolumeControl]);
  
  if (millis() - blinkElapsedTime > BLINK_DELAY) blinkElapsedTime = millis();
}

void displayTopName(int16_t x, int16_t y){

  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(ArialMT_Plain_10);  
  for (int i=0;i<4;i++){
    // display tiny name on top
    if (conf.getInt("mode")==MODE_MUTE || (conf.getInt("mode")==MODE_LEVEL && fxOrChVolumeControl==SUBMODE_LEVEL_FX)) friendlyName = String(FXName[i]).substring(0,6);
    else friendlyName = String(CHName[conf.getInt(String("channel"+String(i+1)).c_str())-1]).substring(0,6); 
    friendlyName.toLowerCase();
    display.drawString(i*(display.getWidth()/4)+15+x, 0+y, friendlyName);                     
  }
  display.drawHorizontalLine(0+x,11+y,display.getWidth()-2);
  display.drawHorizontalLine(0+x,64-17+y,display.getWidth()-2);
}

void drawFrame_ControlModeMute(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) { 
  displayTopName(x,y);
  // display my channel
  barGraph=volumes[0];   
  // display square per channel
  display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display->setFont(ArialMT_Plain_10);    
  for (int i=0;i<4;i++){
    // and a rectangle fill (mute) or empty (unmute) plus its line representing the FX level set on mychannel
    int faderY = (volumes[i+5]*32/100);     // FX volumes start at position 5 . 24 pixel height max volmax=100
    display->drawString(i*(display->getWidth()/4)+16+x, 64-19-faderY+y, "=");                     
    display->fillRect(i*(display->getWidth()/4)+x, 64-19-faderY+y, (display->getWidth()/4-4), 1);              
    if (fxOn[i]) display->drawRect(i*(display->getWidth()/4)+x, 11+y, (display->getWidth()/4)-2, display->getHeight()-16-11);
    //if (fxOn[i]) display->drawRect(i*(display->getWidth()/4)+15+x, 11+y, 1, display->getHeight()-16-11);
    else display->fillRect(i*(display->getWidth()/4)+x, 11+y, (display->getWidth()/4)-2, display->getHeight()-16-11);

    
  }     
}

void drawFrame_ControlModeLevel(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  displayTopName(x,y);
  //depending which screen we are on we either display name of FX or Channel
  if (fxOrChVolumeControl==SUBMODE_LEVEL_CH) friendlyName = String(CHName[conf.getInt(String("channel"+String(selector+1)).c_str())-1]).substring(0,8);          
  else friendlyName = String(FXName[selector]).substring(0,8);  // we know index of FX (this is hard-defined and coded in the array) 
   barGraph=volumes[selector+(fxOrChVolumeControl*4)+1];    //fxOrChVolumeControl allow to jump to the index of volume for FX
  // display name in big
  display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display->setFont(ArialMT_Plain_24);
  friendlyName.toUpperCase();
  display->drawString(display->getWidth()/2+x, display->getHeight()/2+y, friendlyName);            
}


/* for this frame we don't use +x and +y as we want to exclusively have this frame use
 *  called by setToFrame to avoid that it scroll when pushing during searching console... 
 *  the use case is rare but has to be taken into accounbt
 *  
 */
void drawFrame_Info(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  char vBuf[]="00.00";            // to display the battery voltage in info screen
  display->normalDisplay();
  display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display->setFont(ArialMT_Plain_10); 
  display->drawString(display->getWidth()/2, 10, "Wifi: "+String(conf.values[0].c_str()));        
  display->drawString(display->getWidth()/2, 20 ,"IP: "+ WiFi.localIP().toString()+"/"+outIP[3]);     
  sprintf(vBuf,"%.2f",(float)averageVoltage*5.0/630.0);
  display->drawString(display->getWidth()/2, 30 ,"Signal: "+ String(WiFi.RSSI())+"dB bat.:"+String(vBuf)+"v"); 
  display->drawString(display->getWidth()/2, 40, mixer+" "+String(conf.getApName())+" "+String(_V));  
}


void drawFrame_Config(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->normalDisplay();
  display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display->setFont(ArialMT_Plain_10); 
  if (WiFi.softAPgetStationNum()==0){
    if (WiFi.status() != WL_CONNECTED) display->drawString(display->getWidth()/2, 10 ,"No WIFI " +String(conf.values[0].c_str()));
    display->drawString(display->getWidth()/2+x, 20+y ,"Connect SSID");
    display->drawString(display->getWidth()/2+x, 30+y ,String(conf.getApName()));
    display->drawString(display->getWidth()/2+x, 40+y ,"waiting...");
    barGraph = (barGraph+1) % 100;
  }else{
    display->drawString(display->getWidth()/2+x, 10+y ,"Connection established!");   
    display->drawString(display->getWidth()/2+x, 20+y ,"Open browser at");   
    display->drawString(display->getWidth()/2+x, 30+y ,"http://192.168.4.1");    
    display->drawString(display->getWidth()/2+x, 40+y ,"or http://"+String(conf.getApName())+".local");        
    barGraph=0;
  }
}

void drawFrame_SearchMixer(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->normalDisplay();
  display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display->setFont(ArialMT_Plain_24); 
  display->drawString(display->getWidth()/2+x, 16,"Searching");
  if (WiFi.status() == WL_CONNECTED) display->drawString(display->getWidth()/2+x, 40,mixer+"...");
  else display->drawString(display->getWidth()/2+x, 40,mixer+"+WIFI");
}


FrameCallback frames[] = { drawFrame_ControlModeMute, drawFrame_ControlModeLevel, drawFrame_Info, drawFrame_Config, drawFrame_SearchMixer };
// how many frames are there?
#define FRAMECOUNT  5
// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = { overlayControl };
#define OVERLAYCOUNT  1


void acquireVoltage() {
  // analog average reading input voltage
  total = total - readings[readIndex];
  readings[readIndex] = analogRead(inputPin);
  total = total + readings[readIndex];
  readIndex = readIndex + 1;
  if (readIndex >= numReadings) readIndex = 0;
  averageVoltage = total / numReadings; 
}

void setup() {
  delay(200);                            //Stable Wifi
  Serial.begin(115200);             
  ////Serial.println("\n");
  
  conf.setDescription(params);      // parameters defaut and structure
  conf.readConfig();          // read them from eeprom file
  setMixerType();
  ////Serial.println(params);       

  //reading of initial button state 
  delay(DEBOUNCE_TIME);
  for (int buttonIndex=0; buttonIndex<2;buttonIndex++){
    pinMode(button[buttonIndex], INPUT);
    previousButtonState[buttonIndex] = digitalRead(button[buttonIndex]) == HIGH;
  }
  previousTapButtonState = digitalRead(button[MIDDLEB])==LOW;
  // initialize all the voltage readings
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = analogRead(inputPin);
    total+= analogRead(inputPin);  
  }
  acquireVoltage();


  ui.setTargetFPS(30);
  ui.disableAllIndicators();
  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);
  // Add frames
  ui.setFrames(frames, FRAMECOUNT);
  // Add overlays
  ui.setOverlays(overlays, OVERLAYCOUNT);
  // Initialising the UI will init the display too.
  ui.init();
  ui.disableAutoTransition();   
  
  
  if (!initConnection()) enterConfig();     //go directly to AP_MODE  so user can enter websetup
  else{
    if (conf.getInt("mode")==MODE_LEVEL) screenID = CONTROL_SCREEN_LEVEL;  else screenID = CONTROL_SCREEN_MUTE;  
    ui.switchToFrame(screenID);  
  }
  
  Debug.begin(conf.getApName());            //start debug. Just connect to footswitch ip via telnet
  Debug.setResetCmdEnabled(true);
  
  ArduinoOTA.setHostname(conf.getApName());   // name will popup in the Arduino IDE menu to upload code
  ArduinoOTA.begin();              

  char dns[30];
  sprintf(dns,"%s.local",conf.getApName());   // more convenient for user to type in http://name.local 
  MDNS.begin(dns);

  server.on("/",handleRoot);
  server.begin(80);
}


void loop() { 

  ArduinoOTA.handle();
  Debug.handle();
  server.handleClient();
  MDNS.update();


  ui.update();    

  if (screenID!=CONFIG_SCREEN){
    if (outIP[3]==255 || outIP[3]==0) findMixer();    //  the last digit of IP has been set to 255 to indicate an OSC message could not be read as expected, so we force a lookup of the mixer
  
    // first we read the TAP button
    boolean tapDown = digitalRead(button[MIDDLEB])==LOW;
    tapTempo.update(tapDown);
    if (!previousTapButtonState && tapDown){
      if (countTapDown<(conf.getInt("duration")*2)+3){
        if (countTapDown<conf.getInt("duration")){
        //short push
          switch (conf.getInt("mode")){
            case MODE_MUTE:
              sprintf(OSCCommand,TAPCOMMAND_STRING,conf.getInt("fxtapcontrol"));
              SendInt2OSC(OSCCommand,tapTempo.getBeatLength());
              //Debug.println("short tapDown");  
              display.invertDisplay();
            break;
            case MODE_LEVEL:
              if (fxOrChVolumeControl==SUBMODE_LEVEL_CH) fxOrChVolumeControl=SUBMODE_LEVEL_FX; else fxOrChVolumeControl=SUBMODE_LEVEL_CH;
              //Debug.println("toggle fxOrChVolumeControl");       
            break;
          } 
        }        
      }
      if (countTapDown>conf.getInt("duration") and countTapDown<(conf.getInt("duration")*3)+2){
        //Debug.println("long tapDown");  
        if (conf.getInt("mode")==MODE_LEVEL){
          conf.values[conf.getIndex("mode")]=String(MODE_MUTE); 
          screenID = CONTROL_SCREEN_MUTE;
        }else{
          conf.values[conf.getIndex("mode")]=String(MODE_LEVEL);
          screenID = CONTROL_SCREEN_LEVEL;
        }
        conf.writeConfig();
        ui.transitionToFrame(screenID);
      }
      else if (countTapDown == (conf.getInt("duration")*3)+3){
        //Debug.println("hold tap released");
        if (conf.getInt("mode")==MODE_LEVEL) screenID = CONTROL_SCREEN_LEVEL;  else screenID = CONTROL_SCREEN_MUTE;
        ui.switchToFrame(screenID);   //could be transitionToFrame
      }
      countTapDown=0;
      delay(DEBOUNCE_TIME);    
    }
    previousTapButtonState = tapDown;
    if (!tapDown){
      countTapDown++;
      if (countTapDown>(conf.getInt("duration")*3)+2){
         screenID = INFO_SCREEN; 
         ui.switchToFrame(screenID);  
         countTapDown = (conf.getInt("duration")*3)+3; // to avoid overflow
      }
    }
  
    for (int i=0;i<2;i++){
      boolean input;
      
      boolean input1 = digitalRead(button[i]) == HIGH;
      delay(1);
      boolean input2 = digitalRead(button[i]) == HIGH;
      if (input1==input2) input=input1;
      
      if (!previousButtonState[i] && input){
        if (push_time[i]<(conf.getInt("duration")*2)+3){
          if (push_time[i]<conf.getInt("duration")){
            //Debug.println("left short push"); 
            selector=0+(i*2);
          }
          if (push_time[i]>conf.getInt("duration") and push_time[i]<(conf.getInt("duration")*3)+10){
            //Debug.println("left long push"); 
            selector=1+(i*2);
          }
          switch (conf.getInt("mode")){
            case MODE_MUTE:
              if (getfxOnState(selector)){ //only perform switch if mixer reply first
                fxOn[selector] = !fxOn[selector];
                sprintf(OSCCommand,FXMUTECOMMAND_STRING,selector+1); 
                SendInt2OSC(OSCCommand,fxOn[selector]);
              }
              break;
            case MODE_LEVEL:
              // here goes the code executed when we press a button in level mode
              // since the index is setup above, nothing special to do but we could do additionnal action here...
            break;
          }
        }else{
         //Debug.println("hold button released");
         if (!previousButtonState[0] && !previousButtonState[1]) enterConfig(); // both button released
        }
        push_time[i]=0;
        levelIncrement = 1;
        delay(DEBOUNCE_TIME);            
      }
      previousButtonState[i]=input;
      if(!input){
        push_time[i]++;
        int whichChannel=0;
        if (push_time[i]>(conf.getInt("duration")*3)+10){
          delay(FADER_SPEED-(levelIncrement*2));
          switch (conf.getInt("mode")){
            case MODE_MUTE:
              // mode mute is easy: we only manage mychannel and we don't make usage of the selector
              sprintf(OSCCommand,LEVELCOMMAND_STRING,conf.getInt(String("channel0").c_str()),conf.getInt("mymonitor"));
              whichChannel=0;
            break;
            case MODE_LEVEL:
              // here we forge the right command using the selector and the switch fxOrChVolumeControl to properly define the right index in the different array
              if (fxOrChVolumeControl==SUBMODE_LEVEL_CH) sprintf(OSCCommand,LEVELCOMMAND_STRING,conf.getInt(String("channel"+String(selector+1)).c_str()),conf.getInt("mymonitor"));
              else sprintf(OSCCommand,LEVELCOMMAND_STRING,conf.getInt("channel0"),selector+7);  // mychannel=channel0 on bus 7+
              whichChannel=selector+(fxOrChVolumeControl*4)+1;
            break;
          }
          if (i==0){
            if (volumes[whichChannel]>levelIncrement){
              //Debug.println("down");   
              SendFloat2OSC(OSCCommand,float((volumes[whichChannel]-levelIncrement)/100.0));
              levelIncrement++;
            }         
          }else{
            if (volumes[whichChannel]<100){
              //Debug.println("up");     
              SendFloat2OSC(OSCCommand,float((volumes[whichChannel]+levelIncrement)/100.0));
              levelIncrement++;
            }
          }        
          if (!previousButtonState[0] && !previousButtonState[1]){
              screenID=CONFIG_SCREEN;
              ui.transitionToFrame(screenID);  
          }       
        }    
      }
    }
  
    if (millis() - loopElapsedTime > MONITOR_REFRESH) {
      restartMonitor();
      loopElapsedTime = millis();
    }
  
    // here we process any message we would have received from Mixer that we are only interested in
    // that's the case if someone mute/unmute an FX, we capture and update our state accordingly
    if (OSCReadIn()){
      // we have 5 channels volume to monitor
      for (int i=0;i<5;i++){  
        sprintf(OSCCommand,LEVELCOMMAND_STRING,conf.getInt(String("channel"+String(i)).c_str()),conf.getInt("mymonitor"));       
        if (CommandStr == OSCCommand) volumes[i] = fValue * 100;
      }
      // fx level of my channel to the bus 7,8,9,10
      for (int bus=0;bus<4;bus++){  
        sprintf(OSCCommand,LEVELCOMMAND_STRING,conf.getInt("channel0"),bus+7);      
        if (CommandStr == OSCCommand) volumes[bus+5] = fValue * 100;
      }    
      // then we look for the fx parameter 1 (tap delay) if someone changed it 
      sprintf(OSCCommand,TAPCOMMAND_STRING,conf.getInt("fxtapcontrol"));
      if (CommandStr == OSCCommand){
        tapDelay = ((fValue*3000)+ 1.5);                 // linear float conversion from OSC value to msValue1. for FX1 tempo
        if (tapDelay > 1500) tapDelay = (tapDelay - 1);  // to correct some goofy floating point math issue 
      }
      // and we finally check if we were not triggered by an FX mute/unmute
      for (int i=0;i<4;i++){
        sprintf(OSCCommand,FXMUTECOMMAND_STRING,i+1); 
        if (CommandStr == OSCCommand) fxOn[i] = iValue;
      }
    }    
  }

  if (millis() - tapElapsedTime > (tapDelay / 1)) {
    if (tap==0 && tapDelay > 1) tap=1; else tap = 0;
    tapElapsedTime = millis();
  }
  
}
  
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// OSC MESSAGE UTILITY ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

boolean OSCRead()              // Reread until return is what is expected (or it reaches 10 tries)
{
  int cntr = 1;
  boolean RespOK = OSCReadBack();
  while (!RespOK && cntr<5)
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
  ////Debug.println("ReadInOSC()");
  int size = OscUDP.parsePacket();
  if (size < 1) return false;
  // Get the packet
  OSCMessage msg;
  while (size--) msg.fill(OscUDP.read());
  // Check for errors
  if (msg.hasError()){
    //Debug.println("error: "+msg.getError());
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
  //Serial.println("Received: adress="+CommandStr+" iValue="+iValue+" fValue="+fValue+" sValue="+sValue);  
  msg.empty();
  return true;
}

//============ Process OSC msg ============================================================================================  
boolean OSCReadBack()        
{
  ////Debug.println("OSCReadBack");
  delay(20);  
  if (!OSCReadIn()) return false;
  if (CommandStr==ExpectedStr) return true; else return false;
}

//======== Send OSC w/Value ==========================================================================
void SendOSC(String CmdStr, String Val)  // send an OSC command (string) with option value (string)
{
  char PathBuffer[30];
  char ValBuffer[30];
  ExpectedStr = Val;

  //Debug.println("SendOSC("+CmdStr+","+Val+")");
  //Serial.println("SendOSC("+CmdStr+","+Val+")");
  
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
  //Debug.println("OSCUdp.msg.send("+outIP.toString()+","+conf.getInt("mixer")+","+address+")");
  //Serial.println("OSCUdp.msg.send("+outIP.toString()+","+conf.getInt("mixer")+","+address+")");
  OscUDP.beginPacket(outIP, conf.getInt("mixer"));
  msg.send(OscUDP);
  OscUDP.endPacket();
  msg.empty();
}

//======== Send OSC w/Int ============================================================================
void SendInt2OSC(char* address, int val)   // Send OSC command (buffer) with integer value
{
  OSCMessage msg(address);
  ExpectedStr = address;
  //Debug.println("OSCUdp.msg.send("+outIP.toString()+","+conf.getInt("mixer")+","+address+","+String(val)+")");
  //Serial.println("OSCUdp.msg.send("+outIP.toString()+","+conf.getInt("mixer")+","+address+","+String(val)+")");  
  msg.add(val);
  OscUDP.beginPacket(outIP, conf.getInt("mixer"));
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
  OscUDP.beginPacket(outIP, conf.getInt("mixer"));
  msg.send(OscUDP);
  OscUDP.endPacket();
  msg.empty();
}

void SendFloat2OSC(char* address, float val)
{ 
  //Debug.println("OSCUdp.msg.sendfloat("+outIP.toString()+","+conf.getInt("mixer")+","+address+","+String(val)+")");  
  OSCMessage msg(address);
  ExpectedStr = address;
  msg.add(val);
  OscUDP.beginPacket(outIP, conf.getInt("mixer"));
  msg.send(OscUDP);
  OscUDP.endPacket();
  msg.empty();
}
