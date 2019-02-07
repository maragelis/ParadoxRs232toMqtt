char* getSubEvent(byte event, byte sub_event)
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
            return "Silent alarm";
           break;

           case 3:
            return "Buzzer alarm";
           break;

           case 4: 
            return "Steady alarm";
           break;

           case 5:
            return "Pulsed alarm";
            break;

            case 6:
             return "Strobe";
            break;

            case 7: 
              return "Alarm stopped";
              break;

            case 8:
              return "Squawk ON";
             break;

            case 9: 
            return "Squawk OFF";
            break; 

            case 10: 
             return "Ground start";
           break;

            case 11: 
            return "Disarm partition";
            break;

            case 12: 
            return "Arm partition";
            break;

            case 13:
             return "Entry delay started";
            break;

            case 14:
              return "Exit delay started";
            break;

            case 15:
              return "Pre-alarm delay";
             break;

            case 16: 
              return "Report confirmation";
            break;

            case 99: 
              return "Any partition status event";
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
           return "Bell OFF";
          break;

          case 1: 
           return "Bell ON";
          break;
          
          case 2: 
           return "Bell squawk arm";
          break;

          case 3: 
           return "Bell squawk disarm";
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
           return "Telephone line trouble";
          break;

          case 1: 
           return "CLEAR + ENTER";
          break;

          case 3: 
           return "Arm in Stay mode";
          break;

          case 4: 
           return "Arm in Sleep mode";
          break;

          case 5: 
           return "Arm in Force mode";
          break;

          case 6: 
           return "Full arm when armed in Stay mode";
          break;

          case 7: 
           return "PC fail to communicate";
          break;

          case 8: 
           return "Utility key 1-2 pressed";
          break;

          case 9: 
           return "Utility key 4-5 pressed";
          break;

          case 10: 
           return "Utility key 7-8 pressed";
          break;

          case 11: 
           return "Utility key 2-3 pressed";
          break;

          case 12: 
           return "Utility key 5-6 pressed";
          break;

          case 13: 
           return "Utility key 8-9 pressed";
          break;

          case 14: 
           return "Tamper generated alarm";
          break;

          case 15: 
           return "Supervision loss generated alarm";
          break;

          case 20: 
           return "Full arm when armed in Sleep mode";
          break;

          case 23: 
           return "StayD mode activated";
          break;

          case 24: 
           return "StayD mode deactivated";
          break;

          case 25: 
           return "IP registration status change";
          break;

          case 26: 
           return "GPRS registration status change";
          break;

          case 27: 
           return "Armed with trouble(s)";
          break;

          case 28: 
           return "Supervision alert";
          break;

          case 29: 
           return "Supervision alert restore";
          break;

          case 30: 
           return "Armed with remote with low battery";
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
           return "Auto-arming";
          break;

          case 1: 
           return "Late to close";
          break;

          case 2: 
           return "No movement arming";
          break;


          case 3: 
           return "Partial arming";
          break;

          case 4: 
           return "Quick arming";
          break;

          case 5: 
           return "Arming through WinLoad/BabyWare";
          break;

            case 6: 
           return "Arming with keyswitch";
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
           return "Auto-arm cancelled";
          break;

          case 1: 
           return "Disarming through WinLoad/BabyWare";
          break;

          case 2: 
           return "Disarming through WinLoad/BabyWare after alarm";
          break;

          case 3: 
           return "Alarm cancelled through WinLoad/BabyWare";
          break;

          case 4: 
           return "Paramedical alarm cancelled";
          break;

          case 5: 
           return "Disarm with keyswitch";
          break;

          case 6: 
           return "Disarm with keyswitch after an alarm";
          break;

          case 7: 
           return "larm cancelled with keyswitch";
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
           return "Panic non-medical emergency";
          break;

          case 1: 
           return "Panic medical";
          break;

          case 2: 
           return "Panic fire";
          break;

          case 3: 
           return "Recent closing";
          break;

          case 4: 
           return "Global shutdown";
          break;

          case 5: 
           return "Duress alarm";
          break;

          case 6: 
           return "Keypad lockout";
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
           return "AC failure";
          break;

          case 2: 
           return "Battery failure";
          break;

          case 3: 
           return "Auxiliary current overload";
          break;

          case 4: 
           return "Bell current overload";
          break;

          case 5: 
           return "Bell disconnected";
          break;

          case 6: 
           return "Clock loss";
          break;

          case 7: 
           return "Fire loop trouble";
          break;

          case 8: 
           return "Fail call station telephone # 1";
          break;

          case 9: 
           return "Fail call station telephone # 2";
          break;

          case 11: 
           return "Fail to communicate with voice report";
          break;

          case 12: 
           return "RF jamming";
          break;

          case 13: 
           return "GSM RF jamming";
          break;

          case 14: 
           return "GSM no service";
          break;

          case 15: 
           return "GSM supervision lost";
          break;

          case 16: 
           return "Fail to communicate IP receiver 1";
          break;

          case 17: 
           return "Fail to communicate IP receiver 2";
          break;

          case 18: 
           return "IP module no service";
          break;

          case 19: 
           return "IP module supervision loss";
          break;

          case 20: 
           return "Fail to communicate IP receiver 1";
          break;

          case 21: 
           return "Fail to communicate IP receiver 2";
          break;

          case 22: 
           return "GSM/GPRS module tamper trouble";
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
           return "AC failure";
          break;

          case 2: 
           return "Battery failure";
          break;

          case 3: 
           return "Auxiliary current overload";
          break;

          case 4: 
           return "Bell current overload";
          break;

          case 5: 
           return "Bell disconnected";
          break;

          case 6: 
           return "Clock loss";
          break;

          case 7: 
           return "Fire loop trouble";
          break;

          case 8: 
           return "Fail call station telephone # 1";
          break;

          case 9: 
           return "Fail call station telephone # 2";
          break;

          case 11: 
           return "Fail to communicate with voice report";
          break;

          case 12: 
           return "RF jamming";
          break;

          case 13: 
           return "GSM RF jamming";
          break;

          case 14: 
           return "GSM no service";
          break;

          case 15: 
           return "GSM supervision lost";
          break;

          case 16: 
           return "Fail to communicate IP receiver 1";
          break;

          case 17: 
           return "Fail to communicate IP receiver 2";
          break;

          case 18: 
           return "IP module no service";
          break;

          case 19: 
           return "IP module supervision loss";
          break;

          case 20: 
           return "Fail to communicate IP receiver 1";
          break;

          case 21: 
           return "Fail to communicate IP receiver 2";
          break;

          case 22: 
           return "GSM/GPRS module tamper trouble";
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
           return "Software log on";
          break;

          case 3: 
           return "Software log off";
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
