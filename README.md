# ParadoxRs232toMqtt

This project uses a wemos esp8266 to read events of the serial bus of any Paradox alarm system and send it to Mqtt


  Alarm system serial on wemos 
  paradoxTX gpio15 wemos d8 
  paradoxRX gpio13 wemos d7
	
  Debug messages can be read through usb on the wemos.<br> 
  
<br> Arduino IDE settings<br>

Edit the PubSubClient.h header file and change MQTT_MAX_PACKET_SIZE to 128<br>
Libs <br>
wifimanager<br>
pubsubclient<br>
arduinojson<br>


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

paradoxdCTL/status/Arm   <- Arm status message

paradoxdCTL/status/Zone  <- Zone status messages

<br> 

json template 


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
  zonestate,<br> 
  panelstatus <br> 
	
  
  subcomand depends on command ,<br> 
	
  if arm,sleep,disarm subcomand is partition<br> 
	
  if bypass subcomand is zone (0-31) <br> 
  
  if panelstatus subcomand 0 panel data <br> 
  		 subcomand 1 panel voltage and battery data <br> 	
  all others send 0<br> 
  
 
<br> 
  
20190114 V2 Live (Homeassistant)

20190104 Added wiki Node-red v2 flow 

20190103 Added v2 test branch (stable working) 

20180804 Wiki added Home Assistant Config (works with node-red) 

20180721 Changed to user based password, use the same 4 digit code used on panel for control. 



Continue reading wiki ....

