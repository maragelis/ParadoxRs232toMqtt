void answer_E0()
{
                paradox.armstatus = inData[0];
                paradox.event = inData[7];
                paradox.sub_event = inData[8];
                String zlabel = String(inData[15]) + String(inData[16]) + String(inData[17]) + String(inData[18]) + String(inData[19]) + String(inData[20]) + String(inData[21]) + String(inData[22]) + String(inData[23]) + String(inData[24]) + String(inData[25]) + String(inData[26]) + String(inData[27]) + String(inData[28]) + String(inData[29]) + String(inData[30]);
                if (inData[14]!= 1){
                paradox.dummy = zlabel;
                }
                processMessage(paradox.armstatus, paradox.event, paradox.sub_event, paradox.dummy);
                if (inData[7] == 48 && inData[8] == 3)
                {
                  PanelConnected = false;
                }
                else if (inData[7] == 48 && inData[8] == 2 )
                {
                  PanelConnected = true;
                }
}


void answer_00()
{
  waitfor010Message=true;
}

void answer_10()
{
  PanelConnected = true;
  waitfor010Message=true;
}


void answer_30()
{
  trc("Answer 0x30 returned");
}



void answer_40()
{
  trc("Answer 0x40 returned");
}


void answer_70()
{
  trc("Answer 0x70 returned");
}


void ansPanelStatus0()
{
   bool Timer_Loss = bitRead(inData[4],7);
    bool PowerTrouble  = bitRead(inData[4],1);
    bool ACFailureTroubleIndicator = bitRead(inData[6],1);
    bool NoLowBatteryTroubleIndicator = bitRead(inData[6],0);
    bool TelephoneLineTroubleIndicator = bitRead(inData[8],0);
    int ACInputDCVoltageLevel = inData[15];
    int PowerSupplyDCVoltageLevel =inData[16];
    int BatteryDCVoltageLevel=inData[17];

    
        StaticJsonBuffer<256> jsonBuffer;
        JsonObject& root = jsonBuffer.createObject();
        root["Timer_Loss"]=String(Timer_Loss);
        root["PowerTrouble"]=String(PowerTrouble);
        root["ACFailureTrouble"]=String(ACFailureTroubleIndicator);
        root["TelephoneLineTrouble"]=String(TelephoneLineTroubleIndicator);
        root["PSUDCVoltage"]=String(PowerSupplyDCVoltageLevel);
        root["BatteryDCVoltage"]=String(BatteryDCVoltageLevel);
        root["BatteryTrouble"]=String(NoLowBatteryTroubleIndicator);
        char output[256];
        root.printTo(output);
        sendCharMQTT(root_topicStatus,output);  
    
    String Zonename ="";
    int zcnt = 0;
        
    for (int i = 19 ; i <= 22;i++)
    {
      StaticJsonBuffer<256> jsonBuffer;
        JsonObject& zonemq = jsonBuffer.createObject();
     for (int j = 0 ; j < 8;j++) 
       {
         Zonename = "Z" + String(++zcnt);

       
        zonemq[Zonename] =  bitRead(inData[i],j);
        
        //trc (retval);
       
       }
       char Zonemq[256];
        zonemq.printTo(Zonemq);
        sendCharMQTT(root_topicStatus,Zonemq); 
    }
    
    
}

void ansPanelStatus1()
{
   
  bool Fire=bitRead(inData[17],7);
  bool Audible=bitRead(inData[17],6);
  bool Silent=bitRead(inData[17],5);
  bool AlarmFlg=bitRead(inData[17],4);
  bool StayFlg=bitRead(inData[17],2);
  bool SleepFlg=bitRead(inData[17],1);
  bool ArmFlg=bitRead(inData[17],0);

    StaticJsonBuffer<256> jsonBuffer;
    char panelst[256];
        JsonObject& panelstatus1 = jsonBuffer.createObject();
        panelstatus1["Fire"]=Fire;
        panelstatus1["Audible"]=Audible;
        panelstatus1["Silent"]=Silent;
        panelstatus1["AlarmFlg"]=AlarmFlg;
        panelstatus1["StayFlg"]=StayFlg;
        panelstatus1["SleepFlg"]=SleepFlg;
        panelstatus1["ArmFlg"]=ArmFlg;
        panelstatus1["zoneisbypassed"]=bool(bitRead(inData[18],3));
            
        panelstatus1.printTo(panelst);
        sendCharMQTT(root_topicStatus,panelst);  


     if (AlarmFlg)
    {
       hassioStatus.stringArmStatus="triggered";
       homekitStatus.stringArmStatus="ALARM_TRIGGERED";
       homekitStatus.intArmStatus=4;
    }
    else if (StayFlg)
    {
       hassioStatus.stringArmStatus="armed_home";
       homekitStatus.stringArmStatus="STAY_ARM";
       homekitStatus.intArmStatus=0;
    }else if (SleepFlg)
    {
        hassioStatus.stringArmStatus="armed_home";
       homekitStatus.stringArmStatus="NIGHT_ARM";
       homekitStatus.intArmStatus=2;
    }
    else if (ArmFlg)
    {
        hassioStatus.stringArmStatus = "armed_away";
         homekitStatus.stringArmStatus = "AWAY_ARM";
         homekitStatus.intArmStatus = 1;
    }
    else if (!SleepFlg && !StayFlg && !ArmFlg)
    {
        hassioStatus.stringArmStatus = "disarmed";
        homekitStatus.stringArmStatus = "DISARMED";
        homekitStatus.intArmStatus = 3;
    }
    
    else
    {
        hassioStatus.stringArmStatus = "unknown";
        homekitStatus.stringArmStatus = "unknown";
        homekitStatus.intArmStatus = 99;
    }
    //sendMQTT(root_topicArmStatus,retval);
    sendArmStatus();
    

}
