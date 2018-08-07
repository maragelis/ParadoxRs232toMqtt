# ParadoxRs232toMqtt
This project uses a wemos esp8266 to read events of the serial bus of any Paradox alarm system and send it to Mqtt

  Alarm system serial on wemos D15 D13
  Debug messages can be read through usb on the wemos or if you prefer IO15.
        
        
The 37 byte message is broken down into a json message with "Event Group" and "Sub-group" 
and one more dummy attribute which is the zone/partition label.



See wiki for more info on Groups and sub groups 

After flashing the wemos connect to its wifi, (PARADOXController_AP), go to page 192.168.4.1 give it your wifi credentials and MQtt server address. Thats it  

Continue reading wiki ....
