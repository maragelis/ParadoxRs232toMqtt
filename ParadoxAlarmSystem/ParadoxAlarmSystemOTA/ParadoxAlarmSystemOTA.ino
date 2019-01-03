#include <FS.h>   
#include <SoftwareSerial.h>
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




#define mqtt_server       "192.168.4.225"
#define mqtt_port         "1883"
#define Hostname          "paradoxdCTL" //not more than 15 

#define paradoxRX  13
#define paradoxTX  15

#define Stay_Arm  0x01
#define Stay_Arm2 0x02
#define Sleep_Arm 0x03
#define Full_Arm 0x04
#define Disarm  0x05
#define Bypass 0x10

#define MessageLength 37

#define LED LED_BUILTIN

#define TRACE 0

const char *root_topicOut = "paradoxdCTL/out";
const char *root_topicStatus = "paradoxdCTL/status";
const char *root_topicIn = "paradoxdCTL/in";
const char *root_topicArmStatus = "paradoxdCTL/status/Arm";
const char *root_topicZoneStatus = "paradoxdCTL/status/Zone";
//root_topicArmStatus

WiFiClient espClient;
// client parameters
PubSubClient client(espClient);
SoftwareSerial paradoxSerial(paradoxRX, paradoxTX, false ,256);

bool shouldSaveConfig = false;
bool ResetConfig = false;
bool PannelConnected =false;
bool PanelError = false;
bool RunningCommand=false;

unsigned long lastReconnectAttempt = 0UL;
unsigned long ul_Interval = 5000UL;


 
char inData[38]; // Allocate some space for the string
char outData[38];
byte pindex = 0; // Index into array; where to store the character
 
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
  
 Payload paradox;

 

void setup() {
  pinMode(LED, OUTPUT);
  blink(100);
  delay(1000);
  WiFi.mode(WIFI_STA);

  paradoxSerial.begin(9600);
  paradoxSerial.flush();

  Serial.begin(9600);
  Serial.flush(); // Clean up the serial buffer in case previous junk is there
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
  lastReconnectAttempt = 0;
  digitalWrite(LED, HIGH);
}

void loop() {
  
   readSerial();
        
   if ( (inData[0] & 0xF0)!=0xE0){ // re-align serial buffer
    
    
    serial_flush_buffer();
  }
  

}
void StartSSDP()
{
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {

    Serial.printf("Starting HTTP...\n");
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

    Serial.printf("Starting SSDP...\n");
    SSDP.setSchemaURL("description.xml");
    SSDP.setDeviceType("upnp:rootdevice");
    SSDP.setHTTPPort(80);
    SSDP.setName(Hostname);
    SSDP.setSerialNumber(WiFi.macAddress());
    SSDP.setURL(String("http://") + WiFi.localIP().toString().c_str() +"/index.html");
    SSDP.setModelName("ESP8266Wemos");
    SSDP.setModelNumber("WEMOSD1");
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


void SendJsonString(byte armstatus, byte event,byte sub_event  ,String dummy)
{
  String retval = "{ \"armstatus\":" + String(armstatus) + ", \"event\":" + String(event) + ", \"sub_event\":" + String(sub_event) + ", \"dummy\":\"" + String(dummy) + "\"}";
  sendMQTT(root_topicOut,retval); 
   
}

void sendMQTT(String topicNameSend, String dataStr){
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > ul_Interval) {
      lastReconnectAttempt = now;
      trc("client mqtt not connected, trying to connect");
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0UL;
      }
    }
  } 
  else {
    // MQTT loop
    
    client.loop();
 }
    char topicStrSend[26];
    topicNameSend.toCharArray(topicStrSend,26);
    char dataStrSend[200];
    dataStr.toCharArray(dataStrSend,200);
    boolean pubresult = client.publish(topicStrSend,dataStrSend);
    trc("sending ");
    trc(dataStr);
    trc("to ");
    trc(topicNameSend);

}

void readSerialQuick(){
  while (paradoxSerial.available()<37  )  
     { 
      //client.loop();
      }                            
    
     { 
       readSerialData();
     }

}

void readSerial(){
  while (paradoxSerial.available()<37  )  
     { 
      ArduinoOTA.handle();
      client.loop();
      HTTP.handleClient();
      
     }                            
    
     {
       
       readSerialData();
       digitalWrite(LED, LOW);
     }

}

void readSerialData() {
 
    
     
        pindex=0;
        
        while(pindex < 37) // Paradox packet is 37 bytes 
        {
            inData[pindex++]=paradoxSerial.read();            
        }
       
            inData[++pindex]=0x00; // Make it print-friendly
           
              if ((inData[0] & 0xF0) == 0xE0)
              { // Does it look like a valid packet?
                paradox.armstatus = inData[0];
                paradox.event = inData[7];
                paradox.sub_event = inData[8];
                String zlabel = String(inData[15]) + String(inData[16]) + String(inData[17]) + String(inData[18]) + String(inData[19]) + String(inData[20]) + String(inData[21]) + String(inData[22]) + String(inData[23]) + String(inData[24]) + String(inData[25]) + String(inData[26]) + String(inData[27]) + String(inData[28]) + String(inData[29]) + String(inData[30]);
                if (inData[14]!= 1){
                paradox.dummy = zlabel;
                }
                SendJsonString(paradox.armstatus, paradox.event, paradox.sub_event, paradox.dummy);
                if (inData[7] == 48 && inData[8] == 3)
                {
                  PannelConnected = false;
                  trc("panel logout");
                   //sendMQTT(root_topicStatus, "panel logout");
                }
                if (inData[7] == 48 && inData[8] == 2 && !PannelConnected)
                {
                  PannelConnected = true;
                  trc("panel Login");
                     //sendMQTT(root_topicStatus, "panel Login");
                }
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
  
  trc("JSON Returned! ====");
  String callbackstring = String((char *)payload);
  inPayload data = Decodejson((char *)payload);
  PanelError= false;
  trc("Start login");
  doLogin(data.PcPasswordFirst2Digits, data.PcPasswordSecond2Digits);
  trc("end login");
  int cnt = 0;
  while (!PannelConnected && cnt < 10)
  {
    readSerial();
    cnt++;    
  }
   

  RunningCommand=true;
  if (!PannelConnected)
  {
    trc("Problem connecting to panel");
    sendMQTT(root_topicStatus, "{\"status\":\"Problem connecting to panel\"}");
  }else if (data.Command == 0x90  ) 
  {
    
    trc("Running panel status command");
    if (data.Subcommand==0)
    {
     PanelStatus0(false,0);
    }
    if (data.Subcommand==1)
    {
     PanelStatus1(false);
    }
  }
  else if (data.Command == 0x91  )  {
    trc("Running Setdate");
      ArmState();
  } 
   else if (data.Command == 0x92  )  {
    trc("Running ZoneState");
      ZoneState(data.Subcommand);
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

  else if (data == "panelstatus" )
  {
    retval=0x90;
    trc("PAnelStatus command ");
    
  }

  else if (data == "setdate")
  {
    retval=0x89;
    
  }
  else if (data == "armstate")
  {
    retval=0x91;
    
  }
   else if (data == "zonestate")
  {
    retval=0x92;
    
  }
  else if (data == "disconnect" || data == "99")
  {
    retval=0x00;
    PanelDisconnect();
  }
  Serial.print("returned command = ");
  Serial.println(retval , HEX);
  return retval;
}



void panelSetDate(){
  byte data[MessageLength] = {};
  byte checksum;
   for (int x = 0; x < MessageLength; x++)
  {
    data[x] = 0x00;
  }

  data[0] = 0x30;
  data[4] = 0x21;
  data[5] = 0x18;
  data[6] = 0x05;
  data[7] = 0x05;
  data[8] = 0x13;
  data[9] = 0x22;
  data[33] = 0x01;

   checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }

  while (checksum > 255)
  {
    checksum = checksum - (checksum / 256) * 256;
  }

  data[36] = checksum & 0xFF;

  paradoxSerial.write(data, MessageLength);
  
}


void ControlPanel(inPayload data){
  byte armdata[MessageLength] = {};
  byte checksum;
  for (int x = 0; x < MessageLength; x++)
  {
    armdata[x] = 0x00;
  }

  armdata[0] = 0x40;
  armdata[2] = data.Command;
  armdata[3] = data.Subcommand;;
  armdata[33] = 0x01;
  armdata[34] = 0x00;
  armdata[35] = 0x00;
  checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += armdata[x];
  }

  while (checksum > 255)
  {
    checksum = checksum - (checksum / 256) * 256;
  }

  armdata[36] = checksum & 0xFF;
  
  
  while (paradoxSerial.available()>37)
  {
    trc("serial cleanup");
    readSerial();
  }

  trc("sending Data");
  paradoxSerial.write(armdata, MessageLength);
  readSerialQuick();

  if ( inData[0]  >= 40 && inData[0] <= 45)
  {
    sendMQTT(root_topicStatus, "{\"status\":\"Command success\"} ");
    trc(" Command success ");
    }
  

}

void PanelDisconnect(){
  byte data[MessageLength] = {};
  byte checksum;
  for (int x = 0; x < MessageLength; x++)
  {
    data[x] = 0x00;
  }

  data[0] = 0x70;
  data[2] = 0x05;
  data[33] = 0x01;

  checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }

  while (checksum > 255)
  {
    checksum = checksum - (checksum / 256) * 256;
  }

  data[36] = checksum & 0xFF;

  paradoxSerial.write(data, MessageLength);
  readSerialQuick();

  
}

void PanelStatus0(bool showonlyZone ,int zone)
{
  byte data[MessageLength] = {};
  byte checksum;
  for (int x = 0; x < MessageLength; x++)
  {
    data[x] = 0x00;
  }

  serial_flush_buffer();
  data[0] = 0x50;
  data[1] = 0x00;
  data[2] = 0x80;
  data[3] = 0x00;
  data[33] = 0x01;
 checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }

  while (checksum > 255)
  {
    checksum = checksum - (checksum / 256) * 256;
  }

  data[36] = checksum & 0xFF;

  paradoxSerial.write(data, MessageLength);
  
  readSerialQuick();

    bool Timer_Loss = bitRead(inData[4],7);
    bool PowerTrouble  = bitRead(inData[4],1);
    bool ACFailureTroubleIndicator = bitRead(inData[6],1);
    bool NoLowBatteryTroubleIndicator = bitRead(inData[6],0);
    bool TelephoneLineTroubleIndicator = bitRead(inData[8],0);
    int ACInputDCVoltageLevel = inData[15];
    int PowerSupplyDCVoltageLevel =inData[16];
    int BatteryDCVoltageLevel=inData[17];

//sendMQTT(root_topicStatus,"Timer_Loss=" +String(inData[4]) );
String retval = "{ \"Timer_Loss\":\""  + String(Timer_Loss) + "\"" +
                  ",\"PowerTrouble\":\""  + String(PowerTrouble) + "\"" +
                  ",\"ACFailureTrouble\":\""  + String(ACFailureTroubleIndicator) + "\"" +
                  ",\"TelephoneLineTrouble\":\""  + String(TelephoneLineTroubleIndicator) + "\"" +
                  ",\"PSUDCVoltage\":\""  + String(PowerSupplyDCVoltageLevel) + "\"" +
                  ",\"BatteryDCVoltage\":\""  + String(BatteryDCVoltageLevel) + "\"" +
                  ",\"BatteryTrouble\":\"" + String(NoLowBatteryTroubleIndicator) + "\"}";

    trc(retval);
    if (!showonlyZone)
    {
    sendMQTT(root_topicStatus,retval);
    }
    String Zonename ="";
    int zcnt = 0;
    for (int i = 19 ; i <= 22;i++)
    {
     for (int j = 0 ; j < 8;j++) 
       {
         Zonename = "Z" + String(++zcnt);

        retval = "{ \""+ Zonename +"\" :\""+ bitRead(inData[i],j) +"\"}" ;
        trc (retval);
        if ((zone==0 && bitRead(inData[i],j) == 1) || zone== zcnt)
        {
           sendMQTT(root_topicZoneStatus ,retval);
        }
       }
    }
}

void ZoneState(int zone)
{
  PanelStatus0(true,zone);
}

void ArmState()
{
  PanelStatus1(true);
}

void PanelStatus1(bool ShowOnlyState)
{
  byte data[MessageLength] = {};
  byte checksum;
  for (int x = 0; x < MessageLength; x++)
  {
    data[x] = 0x00;
  }

  serial_flush_buffer();
  data[0] = 0x50;
  data[1] = 0x00;
  data[2] = 0x80;
  data[3] = 0x01;
  data[33] = 0x01;

  checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }

  while (checksum > 255)
  {
    checksum = checksum - (checksum / 256) * 256;
  }

  data[36] = checksum & 0xFF;

  paradoxSerial.write(data, MessageLength);

    readSerialQuick();

  bool Fire=bitRead(inData[17],7);
  bool Audible=bitRead(inData[17],6);
  bool Silent=bitRead(inData[17],5);
  bool AlarmFlg=bitRead(inData[17],4);
  bool StayFlg=bitRead(inData[17],2);
  bool SleepFlg=bitRead(inData[17],1);
  bool ArmFlg=bitRead(inData[17],0);


//sendMQTT(root_topicStatus,"Timer_Loss=" +String(inData[4]) );
String retval = "{ \"Fire\":\""  + String(Fire) + "\"" +
                  ",\"Audible\":\""  + String(Audible) + "\"" +
                  ",\"Silent\":\""  + String(Silent) + "\"" +
                  ",\"AlarmFlg\":\""  + String(AlarmFlg) + "\"" +
                  ",\"StayFlg\":\""  + String(StayFlg) + "\"" +
                  ",\"SleepFlg\":\""  + String(SleepFlg) + "\"" +
                  ",\"ArmFlg\":\"" + String(ArmFlg) + "\"}";


    trc(retval);
    if (!ShowOnlyState)
    {
    sendMQTT(root_topicStatus,retval);
    }

     if (AlarmFlg)
    {
       retval = "{ \"PanelArmStatus\":4,\"description\":\"ALARM_TRIGGERED\"}" ;
       
    }
    else if (StayFlg)
    {
       retval = "{ \"PanelArmStatus\":0,\"description\":\"STAY_ARM\"}" ;
    }else if (SleepFlg)
    {
       retval = "{ \"PanelArmStatus\":2,\"description\":\"NIGHT_ARM\"}" ;
    }
    else if (ArmFlg)
    {
       retval = "{ \"PanelArmStatus\":1,\"description\":\"AWAY_ARM\"}" ;
    }
    else if (!SleepFlg && !StayFlg && !ArmFlg)
    {
       retval = "{ \"PanelArmStatus\":3,\"description\":\"DISARMED\"}" ;
    }
    
    else
    {
       retval = "{ \"PanelArmStatus\":99,\"description\":\"unknown\"}" ;
    }
    sendMQTT(root_topicArmStatus,retval);
    
  bool zoneisbypassed =bitRead(inData[18],3);
  bool ParamedicAlarm=bitRead(inData[19],7);
 



 retval = "{ \"zoneisbypassed\":\""  + String(zoneisbypassed) + "\"" +
                  ",\"ParamedicAlarm\":\"" + String(ParamedicAlarm) + "\"}";

    trc(retval);
    if (!ShowOnlyState)
    {
    sendMQTT(root_topicStatus,retval);
    }
  

}
void doLogin(byte pass1, byte pass2){
  byte data[MessageLength] = {};
  byte data1[MessageLength] = {};
  byte checksum;

  for (int x = 0; x < MessageLength; x++)
  {
      data[x]=0x00;
      data1[x]=0x00;
  }

    serial_flush_buffer();
  data[0] = 0x5f;
  data[1] = 0x20;
  data[33] = 0x05;
  data[34] = 0x00;
  data[35] = 0x00;
  data[33] = 0x01;

  checksum = 0;
  for (int x = 0; x < MessageLength - 1; x++)
  {
    checksum += data[x];
  }

  while (checksum > 255)
  {
    checksum = checksum - (checksum / 256) * 256;
  }

  data[36] = checksum & 0xFF;

  if (TRACE)
  {
    for (int x = 0; x < MessageLength; x++)
    {
      Serial.print("Address-");
      Serial.print(x);
      Serial.print("=");
      Serial.println(data[x], HEX);
    }
  }
   
    paradoxSerial.write(data, MessageLength);
    
    readSerialQuick();
    if (TRACE)
    {
       for (int x = 0; x < MessageLength; x++)
       {
         Serial.print("replAddress-");
         Serial.print(x);
         Serial.print("=");
         Serial.println(inData[x], HEX);
       }
    }
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
      while (checksum > 255)
      {
        checksum = checksum - (checksum / 256) * 256;
      }

      data1[36] = checksum & 0xFF;

      if (TRACE)
      {
        // for (int x = 0; x < MessageLength; x++)
        // {
        //   Serial.print("SendinGINITAddress-");
        //   Serial.print(x);
        //   Serial.print("=");
        //   Serial.println(data1[x], HEX);
        // }
      }

      paradoxSerial.write(data1, MessageLength);
      readSerialQuick();
      if (TRACE)
      {
        // for (int x = 0; x < MessageLength; x++)
        // {
        //   Serial.print("lastAddress-");
        //   Serial.print(x);
        //   Serial.print("=");
        //   Serial.println(inData[x], HEX);
        // }
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

    
        
        // trc(password);
        // trc(command);

        // trc(charpass1);
        // trc(charpass2);

  unsigned long number1 = strtoul(charpass1, nullptr, 16);
  unsigned long number2 = strtoul(charpass2, nullptr, 16);
  unsigned long number3 = strtoul(charsubcommand, nullptr, 16);

  byte PanelPassword1 = number1 & 0xFF; 
  byte PanelPassword2 = number2 & 0xFF; 
  byte SubCommand = number3 & 0xFF;

  byte CommandB = getPanelCommand(command) ;

    // if (TRACE)
    // {
    //   Serial.print("0x");
    //   Serial.println(PanelPassword1, HEX);
    //   Serial.print("0x");
    //   Serial.println(PanelPassword2, HEX);
    // }
      inPayload data1 = {PanelPassword1, PanelPassword2, CommandB, SubCommand};

      return data1;
  }

  return indata;
}

void serial_flush_buffer(){
  while (paradoxSerial.read() >= 0)
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
  
    //read updated parameters
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
  
    //trc("local ip : ");
    //Serial.println(WiFi.localIP());
  
    
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
    // Attempt to connect
    // If you  want to use a username and password, uncomment next line and comment the line if (client.connect("433toMQTTto433")) {
    //if (client.connect("433toMQTTto433", mqtt_user, mqtt_password)) {
    // and set username and password at the program beginning
    String mqname =  WiFi.macAddress();
    char charBuf[50];
    mqname.toCharArray(charBuf, 50) ;

    if (client.connect(charBuf,root_topicStatus,0,false,"{\"status\":\"Paradox Disconnected\"}")) {
    // Once connected, publish an announcement...
      //client.publish(root_topicOut,"connected");
      trc("connected");
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
  Serial.println(msg);
 // sendMQTT(root_topicStatus,msg);
  }
}
 
