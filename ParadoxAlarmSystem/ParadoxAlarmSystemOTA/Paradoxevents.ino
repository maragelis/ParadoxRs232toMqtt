
String getEvent(byte event)
{
  switch (event)
    {
      case 00:
        return F("Zone OK");
      break;
      
      case 01:
        return F("Zone open ");
      break;
  
      case 02:
        return F("Partition status");
      break;

    case 03:
        return F("Bell status");
      break;
      
    case 06:
        return F("Non-reportable event");
      break;

    case 12:
        return F("Cold start wireless zone");
      break;

    case 13:
        return F("Cold start wireless module");
      break;

    case 14:
        return F("Bypass programming");
      break;

    case 15:
        return F("User code activated output");
      break;

    case 16:
        return F("Wireless smoke maintenance signal");
      break;

    case 17:
        return F("Delay zone alarm transmission");
      break;

    case 18:
        return F("Zone signal strength weak 1");
      break;

    case 19:
        return F("Zone signal strength weak 2");
      break;

    case 20:
        return F("Zone signal strength weak 3");
      break;

    case 21:
        return F("Zone signal strength weak 4");
       break;
    
    case 24:
            return F("Fire delay started");
          break;

    case 26:
        return F("Software access");
      break;

    case 27:
        return F("Bus module event");
      break;

    case 28:
        return F("StayD pass acknowledged");
      break;

    case 29:
        return F("Arming with user");
      break;

    case 30:
        return F("Special arming");
      break;

    case 31:
        return F("Disarming with user");
      break;

    case 32:
        return F("Disarming after an alarm with user");
      break;

    case 33:
        return F("Alarm cancelled with user");
      break;

    case 34:
        return F("Special disarming");
      break;

    case 35:
        return F("Zone bypassed");
      break;

    case 36:
        return F("Zone in alarm");
      break;

    case 37:
        return F("Fire alarm");
      break;

    case 38:
        return F("Zone alarm restore");
      break;

    case 39:
        return F("Fire alarm restore");
      break;

    case 40:
        return F("Special alarm");
      break;

    case 41:
        return F("Zone shutdown");
      break;

    case 42:
        return F("Zone tampered");
      break;

    case 43:
        return F("Zone tamper restore");
      break;

    case 44:
        return F("New trouble");
      break;

    case 45:
        return F("Trouble restored");
      break;

    case 46:
        return F("Bus/EBus/wireless module new trouble");
      break;

    case 47:
        return F("Bus/EBus/wireless module trouble restored");
      break;

    case 48:
        return F("Special");
      break;

    case 49:
        return F("Low battery on zone");
      break;

    case 50:
        return F("Low battery on zone restore");
      break;

    case 51:
        return F("Zone supervision trouble");
      break;

    case 52:
        return F("Zone supervision restore");
      break;

    case 53:
        return F("Wireless module supervision trouble");
      break;

    case 54:
        return F("Wireless module supervision restore");
      break;

    case 55:
        return F("Wireless module tamper trouble");
      break;

    case 56:
        return F("Wireless module tamper restore");
      break;

    case 57:
        return F("Non-medical alarm");
      break;

    case 58:
        return F("Zone forced");
      break;

    case 59:
        return F("Zone included");
      break;

    case 60:
        return F("Remote low battery");
      break;

    case 61:
        return F("Remote low battery restore");
      break;

    case 64:
        return F("System status");
      break;

    
    default:
      return "";
      break;

    }
}
