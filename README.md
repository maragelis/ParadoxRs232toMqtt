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


Event Group Event   Group Description Sub-group         Sub-group   Description
00                  Zone OK                             01 to 32    Zone number
01                  Zone open                           99          Any zone number
02                  Partition status
                                                        00 to 01 -
                                                        02 Silent alarm
                                                        03 Buzzer alarm
                                                        04 Steady alarm
                                                        05 Pulsed alarm
                                                        06 Strobe
                                                        07 Alarm stopped
                                                        08 Squawk ON (partition 1only)
                                                        09 Squawk OFF (partition 1 only)
                                                        10 Ground start (partition 1 only)
                                                        11 Disarm partition
                                                        12 Arm partition
                                                        13 Entry delay started
                                                        14 Exit delay started
                                                        15 Pre-alarm delay
                                                        16 Report confirmation
                                                        99 Any partition status event
03                 Bell status (partition 1 only)
                                                        00 Bell OFF
                                                        01 Bell ON
                                                        02 Bell squawk arm
                                                        03 Bell squawk disarm
                                                        99 Any bell status event
06                 Non-reportable event
                                                        00 Telephone line trouble
                                                        01 CLEAR + ENTER, or was pressed for 3 secs. (partition 1 only)
                                                        02 -
                                                        03 Arm in Stay mode
                                                        04 Arm in Sleep mode
                                                        05 Arm in Force mode
                                                        06 Full arm when armed in Stay mode
                                                        07 PC fail to communicate (partition 1 only)
                                                        08 Utility key 1 pressed (keys 1 and 2; partition 1 only)
                                                        09 Utility key 2 pressed (keys 4 and 5; partition 1 only)
                                                        10 Utility key 3 pressed (keys 7 and 8; partition 1 only)
                                                        11 Utility key 4 pressed (keys 2 and 3; partition 1 only)
                                                        12 Utility key 5 pressed (keys 5 and 6; partition 1 only)
                                                        13 Utility key 6 pressed (keys 8 and 9; partition 1 only)
                                                        14 Tamper generated alarm
                                                        15 Supervision loss generated alarm
                                                        16 -
                                                        17 -
                                                        18 -
                                                        19 -
                                                        20 Full arm when armed in Sleep mode
                                                        21 Firmware upgrade (partition 1 only; non-PGM event)
                                                        22 -
                                                        23 StayD mode activated
                                                        24 StayD mode deactivated
                                                        25 IP registration status change
                                                        26 GPRS registration status change
                                                        27 Armed with trouble(s)
                                                        28 Supervision alert
                                                        29 Supervision alert restore
                                                        30 Armed with remote with low battery
                                                        99 Any non-reportable event
08              Button pressed on remote (Data B) 
                                                        01 to 32 Remote control number
                                                        99 Any remote control number
09              Button pressed on remote (Data C)
                                                        01 to 32 Remote control number
                                                        99 Any remote control number
10              Button pressed on remote (Data D)
                                                        01 to 32 Remote control number
                                                        99 Any remote control number
11              Button pressed on remote (Data E)
                                                        01 to 32 Remote control number
                                                        99 Any remote control number
12              Cold start wireless zone
                                                        01 to 32 Zone number
                                                        99 Any zone number
13              Cold start wireless module (partition 1 only)
                                                        01 to 16 Output number
                                                        17 to 18 Wireless repeater
                                                        19 to 26 Wireless keypad
                                                        27 to 30 Wireless siren
                                                        99 Any output number
14             Bypass programming
                                                        01 to 32 User number
                                                        99 Any user number
15             User code activated output (partition 1 only)
                                                        01 to 32 User number
                                                        99 Any user number
16             Wireless smoke maintenance signal
                                                        01 to 32 Zone number
                                                        99 Any zone number
17             Delay zone alarm transmission
                                                        01 to 32 Zone number
                                                        99 Any zone number
18            Zone signal strength weak 1 (partition 1 only)
                                                        01 to 32 Zone number
                                                        99 Any zone number
19            Zone signal strength weak 2 (partition 1 only)
                                                        01 to 32 Zone number
                                                        99 Any zone number
20            Zone signal strength weak 3 (partition 1 only)
                                                        01 to 32 Zone number
                                                        99 Any zone number
21            Zone signal strength weak 4 (partition 1 only)
                                                        01 to 32 Zone number
                                                        99 Any zone number
22            Button pressed on remote (see option 5, in table 22 on page 34)
                                                        01 to 32 Remote control number
                                                        99 Any remote control number
23            Button pressed on remote (see option 6, in table 22 on page 34)
                                                        01 to 32 Remote control number
                                                        99 Any remote control number
24            Fire delay started
                                                        01 to 32 Zone number
                                                        99 Any zone number
25 - - -
26            Software access (VDMP3, IP100, WinLoad, BabyWare)
                                                        00 Non-valid source ID
                                                        01 WinLoad/BabyWare direct
                                                        02 WinLoad/BabyWare through IP module
                                                        03 WinLoad/BabyWare through GSM module
                                                        04 WinLoad/BabyWare through modem
                                                        09 IP100 direct
                                                        10 VDMP3 direct
                                                        11 Voice through GSM module
                                                        12 Remote access
                                                        13 SMS through GSM module
                                                        99 Any software access
27            Bus module event
                                                        00 A bus module was added
                                                        01 A bus module was removed
                                                        02 2-way RF module communication failure
                                                        03 2-way RF module communication restored
                                                        99 Any bus module event
28           StayD pass acknowledged
                                                        01 to 32 Zone number
                                                        99 Any zone number
29            Arming with user
                                                        01 to 32 User number
                                                        99 Any user number
30            Special arming
                                                        00 Auto-arming (on-time/no movement)
                                                        01 Late to close
                                                        02 No movement arming
                                                        03 Partial arming
                                                        04 Quick arming
                                                        05 Arming through WinLoad/BabyWare
                                                        06 Arming with keyswitch
                                                        99 Any special arming
31            Disarming with user
                                                        01 to 32 User number
                                                        99 Any user number
32            Disarming after an alarm with user
                                                        01 to 32 User number
                                                        99 Any user number
33            Alarm cancelled with user
                                                        01 to 32 User number
                                                        99 Any user number
34            Special disarming
                                                        00 Auto-arm cancelled (on-time/no movement)
                                                        01 Disarming through WinLoad/BabyWare
                                                        02 Disarming through WinLoad/BabyWare after alarm
                                                        03 Alarm cancelled through WinLoad/BabyWare
                                                        04 Paramedical alarm cancelled
                                                        05 Disarm with keyswitch
                                                        06 Disarm with keyswitch after an alarm
                                                        07 Alarm cancelled with keyswitch
                                                        99 Any special disarming
35            Zone bypassed
                                                        01 to 32 Zone number
                                                        99 Any zone number
36            Zone in alarm
                                                        01 to 32 Zone number
                                                        99 Any zone number
37             Fire alarm
                                                        01 to 32 Zone number
                                                        99 Any zone number
38            Zone alarm restore
                                                        01 to 32 Zone number
                                                        99 Any zone number
39            Fire alarm restore
                                                        01 to 32 Zone number
                                                        99 Any zone number
40            Special alarm
                                                        00 Panic non-medical emergency
                                                        01 Panic medical (this panic alarm in not UL approved)
                                                        02 Panic fire
                                                        03 Recent closing
                                                        04 Global shutdown
                                                        05 Duress alarm
                                                        06 Keypad lockout (partition 1 only)
                                                        99 Any special alarm event
41            Zone shutdown
                                                        01 to 32 Zone number
                                                        99 Any zone number
42            Zone tampered
                                                        01 to 32 Zone number
                                                        99 Any zone number
43            Zone tamper restore
                                                        01 to 32 Zone number
                                                        99 Any zone number

