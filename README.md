# ParadoxRs232toMqtt
This project uses a wemos esp8266 to read events of the serial bus of any Paradox alarm system and send it to Mqtt

The TX from the alarm panel is connected directly to wemos RX, I used  a voltage divider a 2.2KΩ and a  4.7kΩ resisters, seeing 
as paradox serial bus is 5Vdc. 
     Paradox SP   
       [-----|
        | 13V|        2.2KΩ
        | GRD|-------/\/\---|--------WEMOS GROUND
        | RX |        4.7ΚΩ |--------WEMOS RX
        | TX |-------/\/\---|
       [-----| 
        
        
The 37 byte message is broken down into a json message with "Event Group" and "Sub-group" 
and one more dummy attribute which is the zone/partition label.

See wiki for more info on Groups and sub groups 

