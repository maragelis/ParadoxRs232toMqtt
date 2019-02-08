String getSubEvent(byte event, byte sub_event)
{
 
    switch (event)
    {
      case 00:
        return "";
      break;
      
      case 01:
        return "";
      break;
  
      case 02:
        switch (sub_event)
        {
          case 2:
            return F("Silent alarm");
           break;

           case 3:
            return F("Buzzer alarm");
           break;

           case 4: 
            return F("Steady alarm");
           break;

           case 5:
            return F("Pulsed alarm");
            break;

            case 6:
             return F("Strobe");
            break;

            case 7: 
              return F("Alarm stopped");
              break;

            case 8:
              return F("Squawk ON");
             break;

            case 9: 
            return F("Squawk OFF");
            break; 

            case 10: 
             return F("Ground start");
           break;

            case 11: 
            return F("Disarm partition");
            break;

            case 12: 
            return F("Arm partition");
            break;

            case 13:
             return F("Entry delay started");
            break;

            case 14:
              return F("Exit delay started");
            break;

            case 15:
              return F("Pre-alarm delay");
             break;

            case 16: 
              return F("Report confirmation");
            break;

            case 99: 
              return F("Any partition status event");
             break;

             default: 
              return "";
              break;
            
        }
      break;

    case 03:
        switch (sub_event)
        {
          case 0: 
           return F("Bell OFF");
          break;

          case 1: 
           return F("Bell ON");
          break;
          
          case 2: 
           return F("Bell squawk arm");
          break;

          case 3: 
           return F("Bell squawk disarm");
          break;

          default: 
           return "";
          break;
        }
      break;
      
    case 06:
         switch (sub_event)
        {
          case 0: 
           return F("Telephone line trouble");
          break;

          case 1: 
           return F("CLEAR + ENTER");
          break;

          case 3: 
           return F("Arm in Stay mode");
          break;

          case 4: 
           return F("Arm in Sleep mode");
          break;

          case 5: 
           return F("Arm in Force mode");
          break;

          case 6: 
           return F("Full arm when armed in Stay mode");
          break;

          case 7: 
           return F("PC fail to communicate");
          break;

          case 8: 
           return F("Utility key 1-2 pressed");
          break;

          case 9: 
           return F("Utility key 4-5 pressed");
          break;

          case 10: 
           return F("Utility key 7-8 pressed");
          break;

          case 11: 
           return F("Utility key 2-3 pressed");
          break;

          case 12: 
           return F("Utility key 5-6 pressed");
          break;

          case 13: 
           return F("Utility key 8-9 pressed");
          break;

          case 14: 
           return F("Tamper generated alarm");
          break;

          case 15: 
           return F("Supervision loss generated alarm");
          break;

          case 20: 
           return F("Full arm when armed in Sleep mode");
          break;

          case 23: 
           return F("StayD mode activated");
          break;

          case 24: 
           return F("StayD mode deactivated");
          break;

          case 25: 
           return F("IP registration status change");
          break;

          case 26: 
           return F("GPRS registration status change");
          break;

          case 27: 
           return F("Armed with trouble(s)");
          break;

          case 28: 
           return F("Supervision alert");
          break;

          case 29: 
           return F("Supervision alert restore");
          break;

          case 30: 
           return F("Armed with remote with low battery");
          break;

         
          default: 
           return "";
          break;
        }
      break;

    case 12:
        return "";
      break;

    case 13:
        return "";
      break;

    case 14:
        return "";
      break;

    case 15:
        return "";
      break;

    case 16:
        return "";
      break;

    case 17:
        return "";
      break;

    case 18:
        return "";
      break;

    case 19:
        return "";
      break;

    case 20:
        return "";
      break;

    case 21:
        return "";
       break;
    
    case 24:
            return "";
          break;

    case 26:
        return "";
      break;

    case 27:
        return "";
      break;

    case 28:
        return "";
      break;

    case 29:
        return "";
      break;

    case 30:
        switch (sub_event)
        {
          case 0: 
           return F("Auto-arming");
          break;

          case 1: 
           return F("Late to close");
          break;

          case 2: 
           return F("No movement arming");
          break;


          case 3: 
           return F("Partial arming");
          break;

          case 4: 
           return F("Quick arming");
          break;

          case 5: 
           return F("Arming through WinLoad/BabyWare");
          break;

            case 6: 
           return F("Arming with keyswitch");
          break;

          default:
           return "";
           break;
        }
      break;

    case 31:
        return "";
      break;

    case 32:
        return "";
      break;

    case 33:
        return "";
      break;

    case 34:
         switch (sub_event)
        {
          case 0: 
           return F("Auto-arm cancelled");
          break;

          case 1: 
           return F("Disarming through WinLoad/BabyWare");
          break;

          case 2: 
           return F("Disarming through WinLoad/BabyWare after alarm");
          break;

          case 3: 
           return F("Alarm cancelled through WinLoad/BabyWare");
          break;

          case 4: 
           return F("Paramedical alarm cancelled");
          break;

          case 5: 
           return F("Disarm with keyswitch");
          break;

          case 6: 
           return F("Disarm with keyswitch after an alarm");
          break;

          case 7: 
           return F("larm cancelled with keyswitch");
          break;

          default:
          return "";
          break;
        }
      break;

    case 35:
        return "";
      break;

    case 36:
        return "";
      break;

    case 37:
        return "";
      break;

    case 38:
        return "";
      break;

    case 39:
        return "";
      break;

    case 40:
         switch (sub_event)
        {
          case 0: 
           return F("Panic non-medical emergency");
          break;

          case 1: 
           return F("Panic medical");
          break;

          case 2: 
           return F("Panic fire");
          break;

          case 3: 
           return F("Recent closing");
          break;

          case 4: 
           return F("Global shutdown");
          break;

          case 5: 
           return F("Duress alarm");
          break;

          case 6: 
           return F("Keypad lockout");
          break;

           default:
          return "";
          break;

          
        }
      break;

    case 41:
        return "";
      break;

    case 42:
        return "";
      break;

    case 43:
        return "";
      break;

    case 44:
         switch (sub_event)
        {
          case 1: 
           return F("AC failure");
          break;

          case 2: 
           return F("Battery failure");
          break;

          case 3: 
           return F("Auxiliary current overload");
          break;

          case 4: 
           return F("Bell current overload");
          break;

          case 5: 
           return F("Bell disconnected");
          break;

          case 6: 
           return F("Clock loss");
          break;

          case 7: 
           return F("Fire loop trouble");
          break;

          case 8: 
           return F("Fail call station telephone # 1");
          break;

          case 9: 
           return F("Fail call station telephone # 2");
          break;

          case 11: 
           return F("Fail to communicate with voice report");
          break;

          case 12: 
           return F("RF jamming");
          break;

          case 13: 
           return F("GSM RF jamming");
          break;

          case 14: 
           return F("GSM no service");
          break;

          case 15: 
           return F("GSM supervision lost");
          break;

          case 16: 
           return F("Fail to communicate IP receiver 1");
          break;

          case 17: 
           return F("Fail to communicate IP receiver 2");
          break;

          case 18: 
           return F("IP module no service");
          break;

          case 19: 
           return F("IP module supervision loss");
          break;

          case 20: 
           return F("Fail to communicate IP receiver 1");
          break;

          case 21: 
           return F("Fail to communicate IP receiver 2");
          break;

          case 22: 
           return F("GSM/GPRS module tamper trouble");
          break;

           default:
          return "";
          break;

        }
      break;

    case 45:
         switch (sub_event)
        {
          case 1: 
           return F("AC failure");
          break;

          case 2: 
           return F("Battery failure");
          break;

          case 3: 
           return F("Auxiliary current overload");
          break;

          case 4: 
           return F("Bell current overload");
          break;

          case 5: 
           return F("Bell disconnected");
          break;

          case 6: 
           return F("Clock loss");
          break;

          case 7: 
           return F("Fire loop trouble");
          break;

          case 8: 
           return F("Fail call station telephone # 1");
          break;

          case 9: 
           return F("Fail call station telephone # 2");
          break;

          case 11: 
           return F("Fail to communicate with voice report");
          break;

          case 12: 
           return F("RF jamming");
          break;

          case 13: 
           return F("GSM RF jamming");
          break;

          case 14: 
           return F("GSM no service");
          break;

          case 15: 
           return F("GSM supervision lost");
          break;

          case 16: 
           return F("Fail to communicate IP receiver 1");
          break;

          case 17: 
           return F("Fail to communicate IP receiver 2");
          break;

          case 18: 
           return F("IP module no service");
          break;

          case 19: 
           return F("IP module supervision loss");
          break;

          case 20: 
           return F("Fail to communicate IP receiver 1");
          break;

          case 21: 
           return F("Fail to communicate IP receiver 2");
          break;

          case 22: 
           return F("GSM/GPRS module tamper trouble");
          break;

           default:
          return "";
          break;

        }
      break;

    case 46:
        return "";
      break;

    case 47:
        return "";
      break;

    case 48:
        switch (sub_event)
        {
          case 2: 
           return F("Software log on");
          break;

          case 3: 
           return F("Software log off");
          break;

          
           default:
          return "";
          break;
        }
      break;

    case 49:
        return "";
      break;

    case 50:
        return "";
      break;

    case 51:
        return "";
      break;

    case 52:
        return "";
      break;

    case 53:
        return "";
      break;

    case 54:
        return "";
      break;

    case 55:
        return "";
      break;

    case 56:
        return "";
      break;

    case 57:
        return "";
      break;

    case 58:
        return "";
      break;

    case 59:
        return "";
      break;

    case 60:
        return "";
      break;

    case 61:
        return "";
      break;

    case 64:
        return "";
      break;

    
    default:
      return "";
      break;
    }
}
