# ParadoxRs232toMqtt
This project uses a wemos esp8266 to read events of the serial bus of any Paradox alarm system and send it to Mqtt

The TX from the alarm panel is connected directly to wemos RX , RX from panel to TX of wemos

Check jpg for connection with Paradox panel 
     
  Debug messages can be read through D8 TXD2 Pin on the wemos or if you prefer IO15.
        
        
The 37 byte message is broken down into a json message with "Event Group" and "Sub-group" 
and one more dummy attribute which is the zone/partition label.

See wiki for more info on Groups and sub groups 

After flashing the wemos connect to its wifi, (PARADOXController_AP), go to page 192.168.4.1 give it your wifi credentials and MQtt server address. Thats it  


20180721 Changed to user based password, use the same 4 digit code used on panel for control. 
20180804 Wiki added Home Assistant Config (works with node-red) 


Continue reading wiki ....
