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
                  PannelConnected = false;
                }
                else if (inData[7] == 48 && inData[8] == 2 )
                {
                  PannelConnected = true;
                }
}


void answer_00()
{
  waitfor010Message=true;

}

void answer_10()
{
  PannelConnected = true;
  waitfor010Message=true;
}


void answer_30()
{
  //DateOK
}



void answer_40()
{
  //ActionOK
}


void answer_70()
{
  //ActionOK
}
