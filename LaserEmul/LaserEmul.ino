String incomingCommand;// incoming serial command
int dotCounter = 0;

bool motorInit = false;
bool laserInit = false;

int motorSpeed = 0;
int laser_del =0;
int laser_step =0;
int laser_pow =0;


class Motor
{
  bool m_xInit = false;
  bool m_yInit = false;
  int m_xSpeed = 0;
  int m_ySpeed = 0;
  
public:
  String inputCommand(String command);
};

String Motor::inputCommand(String command)
{
 String strValue;
 int iValue;
 String strReturn;
  if (command.startsWith("#mx=S,")){
    strValue = command.substring(6);
    strReturn+="$MX:S,";
    iValue = strValue.toInt();
    m_xSpeed = iValue;
    strReturn+=String(m_xSpeed);
    strReturn+=",1";
    m_xInit = true;
    return strReturn;
  }
  
  if (command.startsWith("#my=S,")){
    strValue = command.substring(6);
    strReturn+="$MY:S,";
    iValue = strValue.toInt();
    m_ySpeed = iValue;
    strReturn+=String(m_ySpeed);
    strReturn+=",1";
    m_yInit = true;
    return strReturn;
  }
  
  if (command.startsWith("#mx=0")){
    m_xSpeed = 0;
    return("Stop plate move command!");
  }
  
  if (command.startsWith("#my=0")){
    m_ySpeed = 0;
  return("Stop wiper move command!");
  }

  if (command.startsWith("#mx=")){
    if (!m_xInit) return "Unknown input"; 
    return "$MX:M";
  }

  if (command.startsWith("#my=")){
    if (!m_yInit) return "Unknown input"; 
    return "$MY:M";
  }
  
  return "Unknown input";   
}

class Laser
{
  bool m_Init = false;

  int m_Delay = 0;
  int m_Step = 0;
  float m_Pow = 0.0;
   
public:
  String inputCommand(String command);
};

String Laser::inputCommand(String command)
{
     String strValue;
 int iValue;
 float fValue;
 String strReturn;
   if (command.startsWith("#del=")){
    strValue = command.substring(5);
    strReturn+="DELAY_";
    iValue = strValue.toInt();
    m_Delay = iValue;
    strReturn+="OK  ";
    strReturn+=String(m_Delay);
    strReturn+=" (us)";
    return strReturn;
  }

     if (command.startsWith("#step=")){
    strValue = command.substring(6);
    strReturn+="STEP_";
    iValue = strValue.toInt();
    m_Step = iValue;
    strReturn+="OK  ";
    return strReturn;
  }

    if (command.startsWith("#pow=")){
    strValue = command.substring(5);
    strReturn+="POWER_";
    fValue = strValue.toFloat();
    m_Pow = fValue;
    strReturn+="OK  ";
    return strReturn;
  }
  if (command.startsWith("#circle=")){
    long del_in_milSec = long(m_Step*m_Delay*0.02000374);
    delay(del_in_milSec);
    strReturn+="CIRCLE_OK  ";
    strReturn+=String(del_in_milSec);
    strReturn+=" (ms)";
    return strReturn;
  }

if (command.startsWith("#line=")){
      long del_in_milSec = long(m_Step*m_Delay*0.02000374);
    delay(del_in_milSec);
    strReturn+="LINE_OK  ";
    strReturn+=String(del_in_milSec);
    strReturn+=" (ms)";
    return strReturn;
  }
 
  return "Unknown input";   
};


Motor m_XY;
Laser las;
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Start up");
  Serial.println("Waiting connection"); // send an initial string
  establishContact();  // send a byte to establish contact until receiver responds
}

void loop() {
  if (Serial.available() > 0) {
    // get incoming string:
    incomingCommand = Serial.readString();
    commandParser(incomingCommand);
  }
}

void commandParser(String command){
  if (command[0]!='#'){
    Serial.println("Unknown input");
    return;
  }
 
  String motRet;
  String lasRet;
  char firstLetter = command[1];
  switch (firstLetter){
    //motor control
    case 'm':
      motRet = m_XY.inputCommand(command);
      Serial.println(motRet);
      break;
    case 'e':Serial.println("Emergy stop command!");break;
    //laser control
    case 'c': 
    case 'l':
    case 'd':
    case 's':
    case 'p':
      lasRet = las.inputCommand(command);
      Serial.println(lasRet);
      break;
    default: Serial.println("Unknow input!");break;
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    dotCounter++;
    if (dotCounter>80) {
      dotCounter=0;
      Serial.println();
    }
    Serial.print(".");
    delay(500);
  }
}
