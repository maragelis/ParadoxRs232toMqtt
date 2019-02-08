#include <FS.h>   
//#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266SSDP.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <NTPtimeESP.h>

#define firmware "PARADOX_2.2.0"

#define mqtt_server       "192.168.4.225"
#define mqtt_port         "1883"

#define Hostname          "paradoxdCTL" //not more than 15 

#define Stay_Arm  0x01
#define Stay_Arm2 0x02
#define Sleep_Arm 0x03
#define Full_Arm 0x04
#define Disarm  0x05
#define Bypass 0x10
#define PGMon 0x32
#define PGMoff 0x33

#define MessageLength 37

#define LED LED_BUILTIN
#define Serial_Swap 1 //if 1 uses d13 d15 for rx/tx 0 uses default rx/tx

#define Hassio 1  // 1 enables 0 disables HAssio support
#define HomeKit 0 
#define SendAllE0events 1 //If you need all events set to 1 else 0 
#define SendEventDescriptions 1 //If you need all events set to 1 else 0 Can cause slow downs on heavy systems

/*
HomeKit id 
Characteristic.SecuritySystemCurrentState.STAY_ARM = 0;
Characteristic.SecuritySystemCurrentState.AWAY_ARM = 1;
Characteristic.SecuritySystemCurrentState.NIGHT_ARM = 2;
Characteristic.SecuritySystemCurrentState.DISARMED = 3;
Characteristic.SecuritySystemCurrentState.ALARM_TRIGGERED = 4;
*/

bool TRACE = 0;
bool OTAUpdate = 0;

NTPtime NTPch("gr.pool.ntp.org");
strDateTime dateTime;

 char *root_topicOut = "paradoxdCTL/out";
 char *root_topicStatus = "paradoxdCTL/status";
 char *root_topicIn = "paradoxdCTL/in";
 char *root_topicArmStatus = "paradoxdCTL/status/Arm";
 char *root_topicArmHomekit = "paradoxdCTL/HomeKit";
 char *root_topicZoneStatus = "paradoxdCTL/status/Zone";
//root_topicArmStatus

WiFiClient espClient;
// client parameters
PubSubClient client(espClient);

bool shouldSaveConfig = false;
bool ResetConfig = false;
bool PanelConnected =false;
bool PanelError = false;
bool RunningCommand=false;
bool JsonParseError=false;
bool waitfor010Message=false;
 
char inData[38]; // Allocate some space for the string
char outData[38];
byte pindex = 0; // Index into array; where to store the character

long lastStatusSent = 0;

ESP8266WebServer HTTP(80);

struct inPayload
{
  byte PcPasswordFirst2Digits;
  byte PcPasswordSecond2Digits;
  byte Command;
  byte Subcommand;
 } ;
 
typedef struct {
     byte armstatus;
     byte event;
     byte sub_event;    
     String dummy;
     
 } Payload;


 typedef struct {
     int intArmStatus;
     String stringArmStatus;
     int sent;
 } paradoxArm;

 paradoxArm hassioStatus;
 
 paradoxArm homekitStatus;
 
 Payload paradox;

  


//JsonObject& zones = root.createNestedObject("zones");

void setup() {
  pinMode(LED, OUTPUT);
  
  WiFi.mode(WIFI_STA);

  Serial.setRxBufferSize(384);
  Serial.begin(9600);
  Serial.flush(); // Clean up the serial buffer in case previous junk is there
  if (Serial_Swap)
  {
    Serial.swap();
  }

  Serial1.begin(9600);
  Serial1.flush();
  Serial1.setDebugOutput(true);
  trc("serial monitor is up");
  serial_flush_buffer();

  

  trc("Running MountFs");
  mountfs();

  setup_wifi();
  StartSSDP();
  
  ArduinoOTA.setHostname(Hostname);
  ArduinoOTA.begin();
  trc("Finnished wifi setup");
  delay(1500);
  
  NTPch.setRecvTimeout(5);
  dateTime = NTPch.getNTPtime(2.0, 1);

  char readymsg[64];
  sprintf(readymsg, "SYSTEM READY %s ", firmware);
  sendCharMQTT(root_topicStatus,readymsg);
  ArmState();

}

void loop() {
   readSerial();    
}


byte checksumCalculate(byte checksum) 
{
  while (checksum > 255) {
    checksum = checksum - (checksum / 256) * 256;
  }

  return checksum & 0xFF;
}

void StartSSDP()
{
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {

    Serial1.printf("Starting HTTP...\n");
    HTTP.on("/index.html", HTTP_GET, []() {
      HTTP.send(200, "text/plain", Hostname);
    });
    HTTP.on("/", HTTP_GET, []() {
      HTTP.send(200, "text/plain", Hostname);
    });

    HTTP.on("/description.xml", HTTP_GET, []() {
      SSDP.schema(HTTP.client());
    });
    HTTP.begin();

    Serial1.printf("Starting SSDP...\n");
    SSDP.setSchemaURL("description.xml");
    SSDP.setDeviceType("upnp:rootdevice");
    SSDP.setHTTPPort(80);
    SSDP.setName(Hostname);
    SSDP.setSerialNumber(WiFi.macAddress());
    SSDP.setURL(String("http://") + WiFi.localIP().toString().c_str() +"/index.html");
    SSDP.setModelName("ESP8266Wemos");
    SSDP.setModelNumber(firmware);
    SSDP.setModelURL("https://github.com/maragelis/ParadoxRs232toMqtt");
    SSDP.setManufacturer("PM ELECTRONICS");
    SSDP.setManufacturerURL("https://github.com/maragelis/");
    SSDP.begin();

    if (!MDNS.begin(Hostname)) {
    trc("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
    trc("mDNS responder started");

  

  // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);

    trc("Ready!\n");
  }
}



void updateArmStatus()
{
  bool datachanged = false;
  if (paradox.event == 2)
  {
    switch (paradox.sub_event)
    {
      case 4:
        hassioStatus.stringArmStatus = "triggered";
        homekitStatus.stringArmStatus = "ALARM_TRIGGERED";
        homekitStatus.intArmStatus=4;
        datachanged=true;
        
      break;

      case 11:
        hassioStatus.stringArmStatus = "disarmed";
        homekitStatus.stringArmStatus = "DISARMED";
        homekitStatus.intArmStatus = 3;
        datachanged=true;
        
        break;

      case 12:
         hassioStatus.stringArmStatus = "armed_away";
         homekitStatus.stringArmStatus = "AWAY_ARM";
         homekitStatus.intArmStatus = 1;
         
         datachanged=true;
        break;

      default : break;
    }
  }
  else if (paradox.event == 6)
  {
    if (paradox.sub_event == 3)
    {
      datachanged=true;
      hassioStatus.stringArmStatus = "armed_home";
      homekitStatus.stringArmStatus = "STAY_ARM";
      homekitStatus.intArmStatus = 0;
      
    }
    else if ( paradox.sub_event == 4)
    {
      datachanged=true;
      hassioStatus.stringArmStatus = "armed_home";
      homekitStatus.stringArmStatus = "NIGHT_ARM";
      homekitStatus.intArmStatus = 2;
      
    }
    
  }
  
        
}

void sendArmStatus()
{
  char output[128];
  StaticJsonBuffer<128> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
        if (Hassio)
        {
          sendMQTT(root_topicArmStatus,hassioStatus.stringArmStatus);  
        }
        if (HomeKit)
        {
          root["Armstatus"]=homekitStatus.intArmStatus;
          root["ArmStatusD"]=homekitStatus.stringArmStatus ;
          root.printTo(output);
          sendCharMQTT(root_topicArmHomekit,output); 
        }
}


void processMessage(byte armstatus, byte event, byte sub_event, String dummy )
{
  if ((Hassio || HomeKit) && (event == 2 || event == 6))
  {
    updateArmStatus(); 
      
  }   

  if   ((Hassio || HomeKit) && (event == 30 || (event==2 && sub_event==11)))
  {
    if (( homekitStatus.sent != homekitStatus.intArmStatus)   )
    {
      sendArmStatus();
      homekitStatus.sent = homekitStatus.intArmStatus;
    }
      
  } 

  if ((Hassio ) && (event == 1 || event == 0))
  {
    char ZoneTopic[80];
    char stateTopic[80];

    String zone = String(root_topicOut) + "/zone";
    zone.toCharArray(ZoneTopic, 80);

    String state_topic = String(root_topicOut) + "/state";
    state_topic.toCharArray(stateTopic, 80);

    if (event == 1 || event == 0)
    {

      zone = String(ZoneTopic) + String(sub_event);
      zone.toCharArray(ZoneTopic, 80);

      String zonestatus="OFF";

      if (event==1 )
      {
        zonestatus = "ON";
      }

      sendMQTT(ZoneTopic, zonestatus);
    }
    
  }
  
  if ((HomeKit ) && (event == 1 || event == 0))
  {
    char output[128];
    StaticJsonBuffer<128> jsonBuffer;
    JsonObject& homekitmsg = jsonBuffer.createObject();
    homekitmsg["zone"]=sub_event;
    dummy.trim();
    homekitmsg["zoneName"]=String(dummy);
    homekitmsg["state"]=event==1?true:false;
    homekitmsg.printTo(output);
    sendCharMQTT(root_topicArmHomekit,output); 
  }

  if (SendAllE0events)
  {
    char outputMQ[256];
    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["event"]=event;
    if (SendEventDescriptions)
    {
      root["sub_eventD"]=getSubEvent(event,sub_event);
      root["eventD"]=getEvent(event);
    }
    root["sub_event"]=sub_event;
    root["sub_eventD"]=getSubEvent(event,sub_event);
    root["data"]=dummy;
    root.printTo(outputMQ);
    sendCharMQTT(root_topicOut,outputMQ); 
  }
    
}



void sendMQTT(String topicNameSend, String dataStr){
    handleMqttKeepAlive();
    char topicStrSend[40];
    topicNameSend.toCharArray(topicStrSend,26);
    char dataStrSend[200];
    dataStr.toCharArray(dataStrSend,200);
    boolean pubresult = client.publish(topicStrSend,dataStrSend);
    trc("sending ");
    trc(dataStr);
    trc("to ");
    trc(topicNameSend);

}

void sendCharMQTT(char* topic, char* data)
{
  handleMqttKeepAlive();
  
  boolean pubresult = client.publish(topic, data);
  
}


void readSerial(){
  while (Serial.available()<37  )  
  { 
      if (OTAUpdate)
      {
        ArduinoOTA.handle();
      }
      handleMqttKeepAlive();
      HTTP.handleClient();
      yield();
  }                            
  {
       readSerialData();       
  }

}

void readSerialData() {
  pindex=0;
  
  while(pindex < 37) // Paradox packet is 37 bytes 
  {
      inData[pindex++]=Serial.read();  
  } 
  inData[++pindex]=0x00; // Make it print-friendly
  
  traceInData();

  if ((inData[0] & 0xF0) == 0xE0)
  { 
    trc("start  answer_E0");
    answer_E0();  
  }
  else if ((inData[0] & 0xF0) == 0x00)
  {
    trc("start answer_00");
    answer_00();
  }
  else if ((inData[0] & 0xF0) == 0x10)
  {
    trc("start answer_10");
    answer_10();
  }
  else if ((inData[0] & 0xF0) == 0x30)
  {
    trc("start answer_30");
    answer_30();
  }
  else if ((inData[0] & 0xF0) == 0x40)
  {
    trc("start answer_40");
    answer_40();
  }
  else if ((inData[0] & 0xF0) == 0x70)
  {
    trc("start answer_70");
    answer_70();
  }
  else if (((inData[0] & 0xF0) == 0x50) && (inData[3] == 0x00))
  {
    trc("start ansPanelStatus0");
    ansPanelStatus0();
  }
  else if (((inData[0] & 0xF0) == 0x50) && (inData[3]  == 0x01))
  {
    trc("start ansPanelStatus1");
    ansPanelStatus1();
  }
  else
  {
    trc("start serial_flush_buffer");
    serial_flush_buffer();
    blink(100);
  }
    
}

void blink(int duration) {
   
  digitalWrite(LED_BUILTIN,LOW);
  delay(duration);
  digitalWrite(LED_BUILTIN,HIGH);
 
}

void saveConfigCallback () {
  trc("Should save config");
  shouldSaveConfig = true;
}

void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
   if (RunningCommand){
     trc("Command already Running exiting");
      return;
    }
  trc("Hey I got a callback ");
  // Conversion to a printable string
  payload[length] = '\0';
  inPayload data;
  
  trc("JSON Returned! ====");
  String callbackstring = String((char *)payload);
  
  if (callbackstring == "Trace=1" || callbackstring == "trace=1" || callbackstring=="TRACE=1")
  {
    TRACE=1;
    Serial1.println("Trace is ON");
    return ;
  }
  else if (callbackstring == "Trace=0" || callbackstring == "trace=0" || callbackstring=="TRACE=0")
  {
    TRACE=0;
    Serial1.println("Trace is OFF");
    return ;
  }
  else if (callbackstring == "OTA=0")
  {
    OTAUpdate=0;
    Serial1.println("OTA update is OFF");
  }
  else if (callbackstring == "OTA=1")
  {
    OTAUpdate=1;
    Serial1.println("OTA update is ON");
  }
  else if (callbackstring=="")
  {
    trc("No payload data");
    return;
  }
    else
    {
      trc("parsing Recievied Json Data");
      data = Decodejson((char *)payload);
      if (JsonParseError)
      {
        trc("Error parsing Json Command") ;
        JsonParseError=false;
        return;
      }
      trc("Json Data is ok ");
      PanelError = false;
      trc("Start login");
      if (!PanelConnected)
        doLogin(data.PcPasswordFirst2Digits, data.PcPasswordSecond2Digits);
      trc("end login");
      
    }

  RunningCommand=true;
  if (!PanelConnected)
  {
    trc("Problem connecting to panel");
    sendMQTT(root_topicStatus, "{\"status\":\"Problem connecting to panel\"}");
  }else if (data.Command == 0x50  ) 
  {
    trc("Running panel status command");
    if (data.Subcommand==0)
    {
     PanelStatus0();
    }
    if (data.Subcommand==1)
    {
     PanelStatus1(false);
    }
  }
  else if (data.Command == 0x91  )  {
    trc("Running ArmState");
    ArmState();
  }
  else if (data.Command == 0x30)
  {
    trc("Running Setdate");
    panelSetDate();
  }
  
  else if (data.Command != 0x00  )  {
    trc("Running Command");
    ControlPanel(data);
  } 
  else  {
    trc("Bad Command ");
    sendMQTT(root_topicStatus, "{\"status\":\"Bad Command\" }");
  }
  
  RunningCommand=false;
  
}


byte getPanelCommand(String data){
  byte retval=0x00;

  data.toLowerCase();
  if (data == "stay" || data=="0")
  {
    retval = Stay_Arm; 
  }
  else if (data == "arm" || data=="1")
  {    
    retval= Full_Arm;
  }
  else if (data == "sleep" || data=="2")
  {
    retval= Sleep_Arm; 
  }
  else if (data == "disarm" || data == "3")
  {
   retval=Disarm;  
  }
  else if (data == "bypass" || data == "10")
  {
    retval=Bypass; 
  }
  else if (data == "pgm_on" || data == "pgmon")
  {
    retval = PGMon;
  }
  else if (data == "pgm_off" || data == "pgmoff")
  {
    retval = PGMoff;
  }
  else if (data == "panelstatus" )
  {
    retval=0x50;
  }
  else if (data == "setdate")
  {
    retval=0x30; 
  }
  else if (data == "armstate")
  {
    retval=0x91;
  }
  
    if(TRACE)
    {
      Serial1.print("returned command = ");
      Serial1.println(retval , HEX);
    }
  return retval;
}



void panelSetDate(){

  dateTime = NTPch.getNTPtime(2.0, 1);
  
  if (dateTime.valid)
  {
    
    byte actualHour = dateTime.hour;
    byte actualMinute = dateTime.minute;
    byte actualyear = (dateTime.year - 2000) & 0xFF ;
    byte actualMonth = dateTime.month;
    byte actualday = dateTime.day;
  

    byte data[MessageLength] = {};
    byte checksum;
    memset(data, 0, sizeof(data));

    data[0] = 0x30;
    data[4] = 0x21;         //Century
    data[5] = actualyear;   //Year
    data[6] = actualMonth;  //Month
    data[7] = actualday;    //Day
    data[8] = actualHour;   //Time
    data[9] = actualMinute; // Minutes
    data[33] = 0x05;

    checksum = 0;
    for (int x = 0; x < MessageLength - 1; x++)
    {
      checksum += data[x];
    }

    data[36] = checksumCalculate(checksum);
    trc("sending setDate command to panel");
    Serial.write(data, MessageLength);
    
  }else
  {
    trc("ERROR getting NTP Date ");
    sendMQTT(root_topicStatus,"{\"status\":\"ERROR getting NTP Date  \" }");
  }
}


void ControlPanel(inPayload data){
  byte armdata[MessageLength] = {};
  byte checksum;
  memset(armdata,0, sizeof(armdata));

  armdata[0] = 0x40;
  armdata[2] = data.Command;
  armdata[3] = data.Subcommand;;
  armdata[33] = 0x05;
  armdata[34] = 0x00;
  armdata[35] = 0x00;
  checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += armdata[x];
  }
  armdata[36] = checksumCalculate(checksum);
  
  trc("sending Arm command to panel");
  Serial.write(armdata, MessageLength);
}

void PanelDisconnect(){
  byte data[MessageLength] = {};
  byte checksum;
  memset(data, 0, sizeof(data));

  data[0] = 0x70;
  data[2] = 0x05;
  data[33] = 0x05;

  checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }
  data[36] = checksumCalculate(checksum);  
  Serial.write(data, MessageLength);
}

void PanelStatus0()
{
  byte data[MessageLength] = {};
  byte checksum;
  memset(data, 0, sizeof(data));

  serial_flush_buffer();
  data[0] = 0x50;
  data[1] = 0x00;
  data[2] = 0x80;
  data[3] = 0x00;
  data[33] = 0x05;
 checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }

  data[36] = checksumCalculate(checksum);
  trc("sending Panel Status 0 command to panel");
  Serial.write(data, MessageLength);   
}



void ArmState()
{
    sendArmStatus();
}

void PanelStatus1(bool ShowOnlyState)
{
  byte data[MessageLength] = {};
  byte checksum;
  memset(data, 0, sizeof(data));

  serial_flush_buffer();
  data[0] = 0x50;
  data[1] = 0x00;
  data[2] = 0x80;
  data[3] = 0x01;
  data[33] = 0x05;

  checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }
  data[36] = checksumCalculate(checksum);
  trc("sending Panel Status 1 command to panel");
  Serial.write(data, MessageLength);

}

void doLogin(byte pass1, byte pass2){
  byte data[MessageLength] = {};
  byte data1[MessageLength] = {};
  byte checksum;

  trc("Running doLogin Function");

  memset(data, 0, sizeof(data));
  memset(data1, 0, sizeof(data1));

  
  data[0] = 0x5f;
  data[1] = 0x20;
  data[33] = 0x05;
  data[34] = 0x00;
  data[35] = 0x00;

  checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }
  data[36] = checksumCalculate(checksum);
   trc("sending command 0x5f to panel");
    Serial.write(data, MessageLength);
  
    while(!waitfor010Message)
    {
      readSerial();
      yield();
      trc("waiting 0x00 from panel");

    }
    
    trc("got callback from 0x5f command");
      data1[0] = 0x00;
      data1[4] = inData[4];
      data1[5] = inData[5];
      data1[6] = inData[6];
      data1[7] = inData[7];
      data1[7] = inData[8];
      data1[9] = inData[9];
      //data1[10] = pass1; //panel pc password digit 1 & 2
      //data1[11] = pass2; //panel pc password digit 3 & 4
      data1[10] = 0x00;
      data1[11] = 0x00;
      data1[13] = 0x55;
      data1[14] = pass1; //panel pc password digit 1 & 2
      data1[15] = pass2; //panel pc password digit 3 & 4
      data1[33] = 0x05;

      checksum = 0;
      for (int x = 0; x < MessageLength - 1; x++)
      {
        checksum += data1[x];
      }
      data1[36] = checksumCalculate(checksum);
      
      waitfor010Message=false;
      trc("sending command 0x00 to panel");

      Serial.write(data1, MessageLength);         
      while(!waitfor010Message)
      {
        readSerial();
        yield();
        trc("waiting 0x10 from panel");
      }
}

struct inPayload Decodejson(char *Payload){
  inPayload indata;
  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject(Payload);
  if (!root.success())
  {
    indata = {0x00,0x00,0x00,0x00};
    trc("JSON parsing failed!");
    JsonParseError=true;
    return indata;
  }
  else
  {
    char charpass1[4];
    char charpass2[4];
    char charsubcommand[4];
    
    String password = root["password"];
    String command = root["Command"];
    String subcommand = root["Subcommand"];

    String pass1 = password.substring(0, 2);
    String pass2 = password.substring(2, 4);

    // trc(pass1);
    // trc(pass2);

    pass1.toCharArray(charpass1, 4);
    pass2.toCharArray(charpass2, 4);
    subcommand.toCharArray(charsubcommand,4);

    unsigned long number1 = strtoul(charpass1, nullptr, 16);
    unsigned long number2 = strtoul(charpass2, nullptr, 16);
    unsigned long number3 = strtoul(charsubcommand, nullptr, 16);

    if (number2 < 10)
      number2 = number2 + 160;

    if (number1 < 10)
      number1 = number1 + 160;

    byte PanelPassword1 = number1 & 0xFF; 
    byte PanelPassword2 = number2 & 0xFF; 
    byte SubCommand = number3 & 0xFF;

    byte CommandB = getPanelCommand(command) ;
  
    inPayload data1 = {PanelPassword1, PanelPassword2, CommandB, SubCommand};

    return data1;
  }
  return indata;
}

void serial_flush_buffer(){
  while (Serial.read() >= 0)
  ;
}

void setup_wifi(){
  
    WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
    WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);

    WiFiManager wifiManager;
    if (ResetConfig)
    {
      trc("Resetting wifiManager");
      WiFi.disconnect();
      wifiManager.resetSettings();
    }
       
    if (mqtt_server=="" || mqtt_port=="")
    {
      trc("Resetting wifiManager");
      WiFi.disconnect();
      wifiManager.resetSettings();
      ESP.reset();
      delay(1000);
    }
    else
    {
      trc("values ar no null ");
    }


    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.setConfigPortalTimeout(180);
    
    wifiManager.addParameter(&custom_mqtt_server);
    wifiManager.addParameter(&custom_mqtt_port);
        
    if (!wifiManager.autoConnect(Hostname, "")) {
      trc("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.reset();
      delay(5000);
    }
    //if you get here you have connected to the WiFi
    trc("connected...yeey :)");
  
    strcpy(mqtt_server, custom_mqtt_server.getValue());
    strcpy(mqtt_port, custom_mqtt_port.getValue());
    
    //save the custom parameters to FS
    if (shouldSaveConfig) {
      trc("saving config");
      DynamicJsonBuffer jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["mqtt_server"] = mqtt_server;
      json["mqtt_port"] = mqtt_port;
      
      File configFile = SPIFFS.open("/config.json", "w");
      if (!configFile) {
        trc("failed to open config file for writing");
      }
  
      json.printTo(Serial);
      json.printTo(configFile);
      configFile.close();
      //end save
    }
  
    trc("Setting Mqtt Server values");
    trc("mqtt_server : ");
    trc(mqtt_server);
    trc("mqtt_server_port : ");
    trc(mqtt_port);

    trc("Setting Mqtt Server connection");
    unsigned int mqtt_port_x = atoi (mqtt_port); 
    client.setServer(mqtt_server, mqtt_port_x);
    
    client.setCallback(callback);
   
    reconnect();
    
    trc("");
    trc("WiFi connected");
    trc("IP address: ");
    trc((String)WiFi.localIP());
  
}

boolean reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    trc("Attempting MQTT connection...");
    String mqname =  WiFi.macAddress();
    char charBuf[50];
    mqname.toCharArray(charBuf, 50) ;

    if (client.connect(charBuf,root_topicStatus,0,false,"{\"status\":\"Paradox Disconnected\"}")) {
    // Once connected, publish an announcement...
      //client.publish(root_topicOut,"connected");
      trc("MQTT connected");
      sendMQTT(root_topicStatus, "{\"status\":\"Paradox connected\"}");
      //Topic subscribed so as to get data
      String topicNameRec = root_topicIn;
      //Subscribing to topic(s)
      subscribing(topicNameRec);
    } else {
      trc("failed, rc=");
      trc(String(client.state()));
      trc(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  return client.connected();
}

void handleMqttKeepAlive()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}

void subscribing(String topicNameRec){ // MQTT subscribing to topic
  char topicStrRec[26];
  topicNameRec.toCharArray(topicStrRec,26);
  // subscription to topic for receiving data
  boolean pubresult = client.subscribe(topicStrRec);
  if (pubresult) {
    trc("subscription OK to");
    trc(topicNameRec);
  }
}

void mountfs(){
   if (SPIFFS.begin()) {
    trc("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      trc("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        trc("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          trc("\nparsed json");

          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          
        } else {
          trc("failed to load json config");
          
        }
      }
    }
    else
    {
      trc("File /config.json doesnt exist");
      //SPIFFS.format();
      trc("Formatted Spiffs");    
    }
  } else {
    trc("failed to mount FS");
  }
}


void trc(String msg){
  if (TRACE) {
  Serial1.println(msg);
 // sendMQTT(root_topicStatus,msg);
  }
}

void traceInData()
{
  if (TRACE && (inData[0] & 0xF0) != 0xE0)
  {
    for (int x = 0; x < MessageLength; x++)
    {
      Serial1.print("Address-");
      Serial1.print(x);
      Serial1.print("=");
      Serial1.println(inData[x], HEX);
    }
  }
}



 
