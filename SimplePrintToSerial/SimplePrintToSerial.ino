int inByte = 0;         // incoming serial byte
String incomingCommand;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
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
    Serial.println("Not a command!");
    return;
  }
  
  if (command.startsWith("#mx=0")){
    Serial.println("Stop plate move command!");
    return;
  }
  
  if (command.startsWith("#my=0")){
    Serial.println("Stop wiper move command!");
    return;
  }
  
  char firstLetter = command[1];
  switch (firstLetter){
    //motor control
    case 'm':Serial.println("Move command!");break;
    case 'e':Serial.println("Emergy stop command!");break;
    //laser control
    case 'c':Serial.println("Circle command!");break;
    case 'l':Serial.println("Line command!");break;
    case 'd':Serial.println("Exposure time command!");break;
    case 's':Serial.println("Step command!");break;
    case 'p':Serial.println("Power command!");break;
    default: Serial.println("Unknow command!");break;
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("Waiting connection");   // send an initial string
    delay(700);
  }
}
