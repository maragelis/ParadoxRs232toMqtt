# ParadoxRs232toMqtt

This project uses a wemos esp8266 to read events of the serial bus of any Paradox alarm system and send it to Mqtt

Alarm system serial to wemos through RX/TX<br>
  or <br>
  paradoxTX gpio15 wemos d8 
  paradoxRX gpio13 wemos d7<br>
  using serial_swap 1<br>
  
<br> Arduino IDE settings<br>


Edit the PubSubClient.h header file and change MQTT_MAX_PACKET_SIZE to 256<br>

Libs <br>
wifimanager<br>
pubsubclient<br>
arduinojson<br>
NTPtimeESP<br>

Set Hassio flag to 1 for Home assistant see wiki (Home Assistant in V2)<br> 

        
        
The 37 byte message is broken down into a json message with "Event Group" and "Sub-group" 

and one more dummy attribute which is the zone/partition label.<br> 



See wiki for more info on Groups and sub groups <br> 

After flashing the wemos connect to its wifi, (paradoxdCTL), go to page 192.168.4.1 give it your wifi credentials and MQtt server address. Thats it  
<br> 

Mqtt topics 

paradoxdCTL/out           <- all alarm event messages

paradoxdCTL/status       <- program messages

paradoxdCTL/in           <- in topic 

<br>HomeAssistant mqtt topics<br>

paradoxdCTL/hassio/Arm/zoneX where x is zone number from 1-32

paradoxdCTL/hassio/Arm/zoneX gives values ON and OFF

paradoxdCTL/hassio/Arm gives values:

disarmed<br>
armed_home<br>
armed_away<br>
triggered<br>
<br> 

json Command payload template <br>
{
 "password":"1234",
 "Command":"arm",
 "Subcommand":"0"
}

<br> 

password is user 4 digit password

Command can be one of the following 


  arm,<br> 
  disarm,<br> 
  sleep,<br> 
  stay,<br> 
  bypass,<br> 
  armstate,<br> 
  panelstatus <br> 
  setdate <br> 
  PGM_ON<br> 
  PGM_OFF<br> 
	
  
  subcomand depends on command ,<br> 
	
  if Command is arm,sleep,disarm subcomand is partition<br> 
	
  if Command is bypass subcomand is zone number from 0 to 31 <br> 
  
  if Command is panelstatus subcomand 0 = panel data <br> 
  		or subcomand 1 = panel voltage and battery data <br> 	
 
  
<br>
<br> 


20190212 Added retain message on hassio/Arm topic<br>
	Added the ability to add credentials to mqtt.<br>
	Added Homekit topic for Homebridge plugin. (comming soon). <br>	
	

20190130 added PGM support (command "PGM_ON" subcomand "0-31)
  
20190114 V2 Live (Homeassistant)

20190104 Added wiki Node-red v2 flow 

20190103 Added v2 test branch (stable working) 

20180804 Wiki added Home Assistant Config (works with node-red) 

20180721 Changed to user based password, use the same 4 digit code used on panel for control. 



Continue reading wiki ....

