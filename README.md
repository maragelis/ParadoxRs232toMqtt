# ParadoxRs232toMqtt
This project uses a wemos esp8266 to read events of the serial bus of any Paradox alarm system and send it to Mqtt

  Alarm system serial on wemos D15 D13
	
  Debug messages can be read through usb on the wemos.
        
        
The 37 byte message is broken down into a json message with "Event Group" and "Sub-group" 

and one more dummy attribute which is the zone/partition label.



See wiki for more info on Groups and sub groups 

After flashing the wemos connect to its wifi, (paradoxdCTL), go to page 192.168.4.1 give it your wifi credentials and MQtt server address. Thats it  

Mqtt topics 

paradoxdCTL/out           <- all alarm event messages

paradoxdCTL/status       <- program messages

paradoxdCTL/in           <- in topic 

paradoxdCTL/status/Arm   <- Arm status message

paradoxdCTL/status/Zone  <- Zone status messages



json template 


{
 "password":"1234",
 "Command":"arm",
 "Subcommand":"0"
}

password is user 4 digit password

Command can be one of the following 

  arm,
  disarm,
  sleep,
  stay,
  bypass,
  armstate,
  zonestate,
  panelstatus (* causes some problems still looking into it )
	
  
  subcomand depends on command ,
	
  if arm,sleep,disarm subcomand is partition
	
  if bypass subcomand is zone (0-31) 
  
  if panelstatus subcomand 0 panel data 
  		 subcomand 1 panel voltage and battery data 	
  all others send 0
  
  
