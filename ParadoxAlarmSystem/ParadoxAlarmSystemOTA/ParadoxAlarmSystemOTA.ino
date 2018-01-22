#include <FS.h>   
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>




#define mqtt_server       "192.168.2.230"
#define mqtt_port         "1883"
#define Hostname          "433MhzBridge2"

#define paradoxRX  13
#define paradoxTX  15

#define TRACE 0


const char* root_topicOut = "/home/PARADOXt/out";
const char* root_topicStatus = "/home/PARADOXt/status";
const char* root_topicIn = "/home/PARADOXt/in";



WiFiClient espClient;
// client parameters
PubSubClient client(espClient);
SoftwareSerial paradoxSerial(paradoxRX, paradoxTX);

bool shouldSaveConfig = false;
bool ResetConfig = false;

long lastReconnectAttempt = 0;

#define RETRY_PERIOD 250    // How soon to retry (in ms) if ACK didn't come in
#define RETRY_LIMIT 5     // Maximum number of times to retry
#define ACK_TIME 25       // Number of milliseconds to wait for an ack
 
char inData[38]; // Allocate some space for the string
byte pindex = 0; // Index into array; where to store the character
 
#define LED LED_BUILTIN
 
 
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
     paradoxSerial.begin(9600);
     trc("serial monitor is up");
     
    Serial.begin(9600);
    Serial.flush(); // Clean up the serial buffer in case previous junk is there
   trc("Paradox serial monitor is up");
    
    blink(1000);
    serial_flush_buffer();

     trc("Running MountFs");
  mountfs();

  setup_wifi();
  ArduinoOTA.setHostname("ParadoxControllerV1");
  ArduinoOTA.begin();
  trc("Finnished wifi setup");
  delay(1500);
  lastReconnectAttempt = 0;
  wifi_station_set_hostname("ParadoxControllerV1");  
  
  sendMQTT(root_topicStatus,"ParadoxController V1.1");
}

void loop() {
   
  
  // put your main code here, to run repeatedly:
   readSerial();
        
    if ( (inData[0] & 0xF0)==0xE0){ // Does it look like a valid packet?
    paradox.armstatus=inData[0];
    paradox.event=inData[7];
    paradox.sub_event=inData[8]; 
   String zlabel=String(inData[15]) + String(inData[16]) + String(inData[17])+ String(inData[18])+ String(inData[19])
   + String(inData[20])+ String(inData[21])+ String(inData[22])+ String(inData[23])
   + String(inData[24])+ String(inData[25])+ String(inData[26])+ String(inData[27])
   + String(inData[28])+ String(inData[29])+ String(inData[30]) ; 
   paradox.dummy=zlabel;


    SendJsonString(paradox.armstatus,paradox.event,paradox.sub_event,paradox.dummy);
   
    // Send data via RF  
    
    
  }else //re-align buffer
  {
    blink(200);
    serial_flush_buffer();
  }
 

}

void SendJsonString(byte armstatus, byte event,byte sub_event  ,String dummy)
{
  String retval = "{ \"armstatus\":" + String(armstatus) + ", \"event\":" + String(event) + ", \"sub_event\":" + String(sub_event) + ", \"dummy\":\"" + String(dummy) + "\"}";
  sendMQTT(root_topicOut,retval); 
  if (TRACE)
    Serial.println(retval);  
}

void sendMQTT(String topicNameSend, String dataStr){
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      trc("client mqtt not connected, trying to connect");
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
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
 
    
     while (Serial.available()<37  )  
     { 
      ArduinoOTA.handle();
      client.loop();
     }                            
    
     { 
        pindex=0;
        
        while(pindex < 37) // Paradox packet is 37 bytes 
        {
            inData[pindex++]=Serial.read();
            
        }
       
            inData[++pindex]=0x00; // Make it print-friendly
      
    
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
 
  
 
}


void serial_flush_buffer()
{
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
    paradoxSerial.println(WiFi.localIP());
  
    
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
  paradoxSerial.println(msg);
  sendMQTT(root_topicStatus,msg);
  }
}
 
