
char* getEvent(byte event)
{
  switch (event)
    {
      case 00:
        return "Zone OK";
      break;
      
      case 01:
        return "Zone open ";
      break;
  
      case 02:
        return "Partition status";
      break;

    case 03:
        return "Bell status";
      break;
      
    case 06:
        return "Non-reportable event";
      break;

    case 12:
        return "Cold start wireless zone";
      break;

    case 13:
        return "Cold start wireless module";
      break;

    case 14:
        return "Bypass programming";
      break;

    case 15:
        return "User code activated output";
      break;

    case 16:
        return "Wireless smoke maintenance signal";
      break;

    case 17:
        return "Delay zone alarm transmission";
      break;

    case 18:
        return "Zone signal strength weak 1";
      break;

    case 19:
        return "Zone signal strength weak 2";
      break;

    case 20:
        return "Zone signal strength weak 3";
      break;

    case 21:
        return "Zone signal strength weak 4";
       break;
    
    case 24:
            return "Fire delay started";
          break;

    case 26:
        return "Software access";
      break;

    case 27:
        return "Bus module event";
      break;

    case 28:
        return "StayD pass acknowledged";
      break;

    case 29:
        return "Arming with user";
      break;

    case 30:
        return "Special arming";
      break;

    case 31:
        return "Disarming with user";
      break;

    case 32:
        return "Disarming after an alarm with user";
      break;

    case 33:
        return "Alarm cancelled with user";
      break;

    case 34:
        return "Special disarming";
      break;

    case 35:
        return "Zone bypassed";
      break;

    case 36:
        return "Zone in alarm";
      break;

    case 37:
        return "Fire alarm";
      break;

    case 38:
        return "Zone alarm restore";
      break;

    case 39:
        return "Fire alarm restore";
      break;

    case 40:
        return "Special alarm";
      break;

    case 41:
        return "Zone shutdown";
      break;

    case 42:
        return "Zone tampered";
      break;

    case 43:
        return "Zone tamper restore";
      break;

    case 44:
        return "New trouble";
      break;

    case 45:
        return "Trouble restored";
      break;

    case 46:
        return "Bus/EBus/wireless module new trouble";
      break;

    case 47:
        return "Bus/EBus/wireless module trouble restored";
      break;

    case 48:
        return "Special";
      break;

    case 49:
        return "Low battery on zone";
      break;

    case 50:
        return "Low battery on zone restore";
      break;

    case 51:
        return "Zone supervision trouble";
      break;

    case 52:
        return "Zone supervision restore";
      break;

    case 53:
        return "Wireless module supervision trouble";
      break;

    case 54:
        return "Wireless module supervision restore";
      break;

    case 55:
        return "Wireless module tamper trouble";
      break;

    case 56:
        return "Wireless module tamper restore";
      break;

    case 57:
        return "Non-medical alarm";
      break;

    case 58:
        return "Zone forced";
      break;

    case 59:
        return "Zone included";
      break;

    case 60:
        return "Remote low battery";
      break;

    case 61:
        return "Remote low battery restore";
      break;

    case 64:
        return "System status";
      break;

    
    default:
      return " ";
      break;

    }
}
