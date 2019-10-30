# ParadoxRs232toMqtt

This project uses a WEMOS ESP8266 to read the events of the serial bus on Paradox alarm systems and send them to an MQTT server

## Making a connection

There are 2 ways to connect the devices together:
- Alarm system serial to WEMOS through RX/TX<br>
- paradoxTX into GPIO15 (WEMOS D8), paradoxRX to GPIO13 (WEMOS D7), using serial_swap 1

## Arduino IDE settings

Edit the _PubSubClient.h_ header file and change _MQTT_MAX_PACKET_SIZE_ to _256_

Libraries:
- Arduino Core 2.4.1
- WifiManager by tzapu 0.12.0
- PubSubClient by Nick O`Leary 2.6.0

Set the _Hassio_ flag to _1_ for Home assistant, and check out the wiki (Home Assistant in V2)
        
The 37 byte message is broken down into a json message with "Event Group" and "Sub-group", and one more dummy attribute which is the zone/partition label.

See the wiki for more info on Groups and Sub-groups

After flashing the WEMOS board, connect to it's Wi-Fi (_paradoxdCTL_), open the 192.168.4.1 IP address in your browser, input your Wi-Fi credentials and MQTT server address. That's all.  

### MQTT Topics 

| Topic              | Notes                     |
|--------------------|---------------------------|
| paradoxdCTL/out    | All alarm event messages  |
| paradoxdCTL/status | The program messages      |
| paradoxdCTL/in     | Input topic               |

### HomeAssistant MQTT Topics

| Topic                        | Notes                                                     |
|------------------------------|-----------------------------------------------------------|
| paradoxdCTL/hassio/Arm/zoneX | Where x is zone number from 1-32                          |
| paradoxdCTL/hassio/Arm/zoneX | Gives values ON/OFF                                       |
| paradoxdCTL/hassio/Arm       | Gives values: disarmed, armed_home, armed_away, triggered |

### Sending commands

The command payloads are in JSON. Template:
```json
{
 "password":"1234",
 "Command":"arm",
 "Subcommand":"0"
}
```
The password is the user's 4 digit password.

A command can be any of the following:
- arm
- disarm
- sleep
- stay
- bypass
- armstate
- panelstatus
- setdate
- PGM_ON
- PGM_OFF
	
#### Subcommands depending on the main command
	
| Main Command     | Subcommand                     |
|------------------|--------------------------------|
| arm,sleep,disarm | partition                      |
| bypass           | The zone number from 0 to 31   |
| panelstatus      | panel data                     |
| panelstatus      | panel voltage and battery data |

### Release Logs

20190212:
- Added retain message on hassio/Arm topic<br>
- Added the ability to add credentials to mqtt.<br>
- Added Homekit topic for Homebridge plugin. (comming soon). <br>	
	
20190130: added PGM support (command "PGM_ON" subcomand "0-31)
  
20190114: V2 Live (Homeassistant)

20190104: Added wiki Node-red v2 flow 

20190103: Added v2 test branch (stable working) 

20180804: Wiki added Home Assistant Config (works with node-red) 

20180721: Changed to user based password, use the same 4 digit code used on panel for control. 



Continue reading the wiki for more information.
