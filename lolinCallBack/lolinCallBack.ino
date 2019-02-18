
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

void setup() {

  Serial.begin(115200);
  inputString.reserve(200);
  Serial.printf("\n startup");
}

// the loop function runs over and over again forever
void loop() {

  if (stringComplete) {
    Serial.printf("Return ");
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

    while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }

  delay(10);                    
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
