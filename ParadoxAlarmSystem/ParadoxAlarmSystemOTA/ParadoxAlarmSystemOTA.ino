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




#define mqtt_server       "192.168.2.230"
#define mqtt_port         "1883"
#define Hostname          "paradoxdev" //not more than 15 

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

#define TRACE 1

  const char *root_topicOut = "paradoxdev/out";
  const char *root_topicStatus = "paradoxdev/status";
  const char *root_topicIn = "paradoxdev/in";

WiFiClient espClient;
// client parameters
PubSubClient client(espClient);
SoftwareSerial paradoxSerial(paradoxRX, paradoxTX);

bool shouldSaveConfig = false;
bool ResetConfig = false;
bool PannelConnected =false;
bool PanelError = false;

unsigned long lastReconnectAttempt = 0UL;
unsigned long ul_Interval = 5000UL;


 
char inData[38]; // Allocate some space for the string
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
   pinMode(LED_BUILTIN,OUTPUT);
    blink(100);
    delay(1000);
    WiFi.mode(WIFI_STA);
    
    paradoxSerial.begin(9600);
    paradoxSerial.flush();
     
     
    Serial.begin(9600);
    Serial.flush(); // Clean up the serial buffer in case previous junk is there
    trc("serial monitor is up");
    
  
    
    blink(1000);
    serial_flush_buffer();



  pinMode(LED_BUILTIN, OUTPUT);
  blink(100);
  delay(1000);

  trc("Running MountFs");
  mountfs();

  setup_wifi();
  StartSSDP();
  

  ArduinoOTA.setHostname("ParadoxControllerV2Dev");
  ArduinoOTA.begin();
  trc("Finnished wifi setup");
  delay(1500);
  lastReconnectAttempt = 0;
  wifi_station_set_hostname(Hostname);  
  
  sendMQTT(root_topicStatus,"ParadoxController Vdev");
  //PanelStatus0();
  
}

void loop() {
  
   readSerial();
        
   if ( (inData[0] & 0xF0)!=0xE0){ // re-align serial buffer
    
    blink(200);
    serial_flush_buffer();
  }
  

}

void StartSSDP()
{
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {

    Serial.printf("Starting HTTP...\n");
    HTTP.on("/index.html", HTTP_GET, []() {
      HTTP.send(200, "text/plain", "ParadoxController Vdev!");
    });
    HTTP.on("/description.xml", HTTP_GET, []() {
      SSDP.schema(HTTP.client());
    });
    HTTP.begin();

    Serial.printf("Starting SSDP...\n");
    SSDP.setSchemaURL("description.xml");
    SSDP.setHTTPPort(80);
    SSDP.setName("Paradox Alarm Controller");
    SSDP.setSerialNumber(WiFi.macAddress());
    SSDP.setURL("index.html");
    SSDP.setModelName("ESP8266Wemos");
    SSDP.setModelNumber("WEMOSD1");
    SSDP.setModelURL("https://github.com/maragelis/ParadoxRs232toMqtt");
    SSDP.setManufacturer("PM ELECTRONICS");
    SSDP.setManufacturerURL("https://github.com/maragelis/");
    SSDP.begin();

    Serial.printf("Ready!\n");
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
void readSerial() {
 
    
     while (paradoxSerial.available()<37  )  
     { 
      ArduinoOTA.handle();
      client.loop();
      HTTP.handleClient();
     }                            
    
     { 
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
                paradox.dummy = zlabel;

                SendJsonString(paradox.armstatus, paradox.event, paradox.sub_event, paradox.dummy);
                if (inData[7] == 48 && inData[8] == 3)
                {
                  PannelConnected = false;
                 // trc("panel logout");
                   sendMQTT(root_topicStatus, "panel logout");
                }
                if (inData[7] == 48 && inData[8] == 2 && !PannelConnected)
                {
                  PannelConnected = true;
                  //trc("panel Login");
                     sendMQTT(root_topicStatus, "panel Login");
                }
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
  trc("Hey I got a callback ");
  // Conversion to a printable string
  payload[length] = '\0';
  
  trc("JSON Returned! ====");
  String callbackstring = String((char *)payload);
  inPayload data = Decodejson((char *)payload);
  PanelError= false;
  
  doLogin(data.PcPasswordFirst2Digits, data.PcPasswordSecond2Digits);

  int cnt = 0;
  while (!PannelConnected && cnt < 10)
  {
    readSerial();
    cnt++;    
  }
  
  if (!PannelConnected)
  {
    trc("Problem connecting to panel");
    sendMQTT(root_topicStatus, "Problem connecting to panel");
  } else if (data.Command != 0x00 && PannelConnected )  {
      ControlPanel(data);
  }else  {
    trc("Bad Command ");
    sendMQTT(root_topicStatus, "Bad Command ");
  }
  
  
}


byte getPanelCommand(String data){
  byte retval=0x00;
  if (data == "stay" || data == "Stay" || data == "STAY" || data=="0")
  {
    
    retval = Stay_Arm;
    
  }
  else if (data == "arm" || data == "Arm" || data == "ARM" || data=="1")
  {    
    retval= Full_Arm;
  }
  else if (data == "sleep" || data == "Sleep" || data == "SLEEP" || data=="2")
  {
    
    retval= Sleep_Arm;
    
  }
  else if (data == "disarm" || data == "Disarm" || data == "DISARM" || data == "3")
  {
    
    retval=Disarm;
    
  }

  else if (data == "bypass" || data == "Bypass" || data == "BYPASS" || data == "10")
  {
    
    retval=Bypass;
    
  }

  else if (data == "setdate")
  {
    retval=0x00;
    panelSetDate();
  }
  else if (data == "disconnect" || data == "Disconnect" || data == "DISCONNECT" || data == "99")
  {
    retval=0x00;
    PanelDisconnect();
  }

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
  data[6] = 0x02;
  data[7] = 0x06;
  data[8] = 0x01;
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
  readSerial();

  if ( inData[0]  >= 40 && inData[0] <= 45)
  {
    sendMQTT(root_topicStatus, "Command success ");
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
  readSerial();

  
}
void PanelStatus0()
{
  //Panel Status 0 
  byte data[MessageLength] = {};
  byte data1[MessageLength] = {};
  byte checksum;

  for (int x = 0; x < MessageLength; x++)
  {
      data[x]=0x00;
      //data1[x]=0x00;
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
    readSerial();

    bool Timer_Loss = bitRead(inData[4],7);
    bool PowerTrouble  = bitRead(inData[4],1);

sendMQTT(root_topicStatus,"Timer_Loss=" +String(inData[4]) );
    sendMQTT(root_topicStatus,"Timer_Loss=" +String(Timer_Loss) );
     sendMQTT(root_topicStatus,"PowerTrouble=" +String(PowerTrouble) );

  

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
  data[33] = 0x01;
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
    // for (int x = 0; x < MessageLength; x++)
    // {
    //   paradoxSerial.print("Address-");
    //   paradoxSerial.print(x);
    //   paradoxSerial.print("=");
    //   paradoxSerial.println(data[x], HEX);
    // }
  }
    paradoxSerial.write(data, MessageLength);
    readSerial();
    if (TRACE)
    {
      // for (int x = 0; x < MessageLength; x++)
      // {
      //   paradoxSerial.print("replAddress-");
      //   paradoxSerial.print(x);
      //   paradoxSerial.print("=");
      //   paradoxSerial.println(inData[x], HEX);
      // }
    }
      data1[0] = 0x00;
      data1[4] = inData[4];
      data1[5] = inData[5];
      data1[6] = inData[6];
      data1[7] = inData[7];
      data1[7] = inData[8];
      data1[9] = inData[9];
      data1[10] = pass1; //panel pc password digit 1 & 2
      data1[11] = pass2; //panel pc password digit 3 & 4
      data1[33] = 0x01;

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
        //   paradoxSerial.print("SendinGINITAddress-");
        //   paradoxSerial.print(x);
        //   paradoxSerial.print("=");
        //   paradoxSerial.println(data1[x], HEX);
        // }
      }

      paradoxSerial.write(data1, MessageLength);
      readSerial();
      if (TRACE)
      {
        // for (int x = 0; x < MessageLength; x++)
        // {
        //   paradoxSerial.print("lastAddress-");
        //   paradoxSerial.print(x);
        //   paradoxSerial.print("=");
        //   paradoxSerial.println(inData[x], HEX);
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

    trc(pass1);
    trc(pass2);

    pass1.toCharArray(charpass1, 4);
    pass2.toCharArray(charpass2, 4);
    subcommand.toCharArray(charsubcommand,4);

    
        
        trc(password);
        trc(command);

        trc(charpass1);
        trc(charpass2);

  unsigned long number1 = strtoul(charpass1, nullptr, 16);
  unsigned long number2 = strtoul(charpass2, nullptr, 16);
  unsigned long number3 = strtoul(charsubcommand, nullptr, 16);

  byte PanelPassword1 = number1 & 0xFF; 
  byte PanelPassword2 = number2 & 0xFF; 
  byte SubCommand = number3 & 0xFF;

  byte CommandB = getPanelCommand(command) ;

    if (TRACE)
    {
      Serial.print("0x");
      Serial.println(PanelPassword1, HEX);
      Serial.print("0x");
      Serial.println(PanelPassword2, HEX);
    }
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
    

    
    if (!wifiManager.autoConnect("PARADOXController_AP", "")) {
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
  
    trc("local ip : ");
    Serial.println(WiFi.localIP());
  
    
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

    if (client.connect(charBuf,root_topicStatus,0,false,"Paradox Disconnected")) {
    // Once connected, publish an announcement...
      //client.publish(root_topicOut,"connected");
      trc("connected");
      sendMQTT(root_topicStatus,"Paradox Connected");
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
 
