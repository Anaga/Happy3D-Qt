// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}
boolean sendFlag = true;
String incomingCommand;

// the loop routine runs over and over again forever:
void loop() {

    if (Serial.available() > 0) {
    // get incoming string:
    incomingCommand = Serial.readString();
    commandParser(incomingCommand);
  }
  
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0);
  if(sendFlag)
  {
  // print out the value you read:
    Serial.print("Oxygen value:");
    float oxygen = (sensorValue/1024.0)*5.0*6.2;
    Serial.println(oxygen);
  }
  delay(100);        // delay in between reads for stability
}

void commandParser(String command){
   Serial.print("read value:");
   Serial.println(command);

   if (command.startsWith("serial start")) {
      sendFlag = true;
      return;
   }
   
   if (command.startsWith("serial stop")) {
      sendFlag = false;
      return;
   }
   char firstLetter = command[0];
   switch (firstLetter){
    case 'L': Serial.println("Left up");break;
    case 'F': Serial.println("Left down");break;
    
    case 'R': Serial.println("Rigth up");break;
    case 'I': Serial.println("Rigth down");break;
    
    case 'U': Serial.println("Pull");break;
    case 'H': Serial.println("Hold");break;
    case 'D': Serial.println("Push");break;
    
    case 'B': Serial.println("Pump start");break;
    case 'V': Serial.println("Pump reverse");break;
    case 'T': Serial.println("Pump stop");break;

    case 'M': Serial.println("Gas on");break;
    case 'N': Serial.println("Gas off");break;

    case 'P': Serial.println("Laser on");break;
    case 'Q': Serial.println("Laser off");break;
    
    default: Serial.println("Unknow command!");break;
   }
}
