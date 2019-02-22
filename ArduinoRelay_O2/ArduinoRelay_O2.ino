/*
 * ABO - modify this code to be a fake press plat
 * will use pot to fake O2 sensor
 * 
 */
int in1 = 7;//left
int in2 = 6;//Right
int in3 = 5;//Pull
int in4 = 4;//Push
int in0 = 3;//Gas
String rx_data;
int pinLed = 13;
boolean sendFlag = false;
boolean readCompleted = false;
String serialString = "";

const int sensorPin = A0;
//const int laserpower = A1;


void setup() {
  Serial.begin(115200);
  
  /*pinMode(pinLed,OUTPUT);
   
   pinMode(in0, OUTPUT);
   pinMode(in1, OUTPUT);
   pinMode(in2, OUTPUT);
   pinMode(in3, OUTPUT);
   pinMode(in4, OUTPUT);

   digitalWrite(in0, LOW);
   digitalWrite(in1, HIGH);
   digitalWrite(in2, HIGH);
   digitalWrite(in3, HIGH);
   digitalWrite(in4, HIGH);

   pinMode(8,OUTPUT); //Pump Speed, Green
   digitalWrite(8, HIGH);
   pinMode(9,OUTPUT); //Pump A Function, Write
   digitalWrite(9, LOW);
   pinMode(10,OUTPUT); //Pump B Function, Grey
   digitalWrite(10, LOW);
   pinMode(11,OUTPUT); //laser
   digitalWrite(11, LOW);

   */
   serialString.reserve(200);
}
void loop() {
  int sensorVal = analogRead(sensorPin);  
   if(Serial.available()>0)    
     { 
         rx_data=Serial.read();
         if(rx_data=="L") 
         {
            Serial.println("digitalWrite(in1, LOW);//left up");
         }
         else if(rx_data=="F")
         {
            Serial.println("digitalWrite(in1, HIGH);//left down");
         }
         else if(rx_data=="R")
         {
            Serial.println("digitalWrite(in2, LOW);//Right up");
         }
         else if(rx_data=="I")
         {
            Serial.println("digitalWrite(in2, HIGH);//Right down");
         }


         
         else if(rx_data=="U") //Pull
         {
            Serial.println("digitalWrite(in3, LOW);");
            Serial.println("digitalWrite(in4, HIGH);");
         }
         else if(rx_data=="H")//Hold
         {
            Serial.println("digitalWrite(in3, HIGH);");
            Serial.println("digitalWrite(in4, HIGH);");
         }
         else if(rx_data=="D")//Push
         {
            Serial.println("digitalWrite(in4, LOW);");
            Serial.println("digitalWrite(in3, HIGH);");            
         }


         else if(rx_data=="B")//Pump Start
         {
            Serial.println("digitalWrite(9, HIGH);");
            Serial.println("digitalWrite(10, LOW);");
         }
         else if(rx_data=="V")//Pump Reverse
         {
            Serial.println("digitalWrite(9, LOW);");
            Serial.println("digitalWrite(10, HIGH);");
         }
         else if(rx_data=="T")//Pump Stop
         {
            Serial.println("digitalWrite(9, LOW);");
            Serial.println("digitalWrite(10, LOW);");
         }

         else if(rx_data=="M")//Gas on
         {
            Serial.println("digitalWrite(3, HIGH);");

         }
         else if(rx_data=="N")//Gas off
         {
            Serial.println("digitalWrite(3, LOW);");

         }
         else if(rx_data=="P")//laser on
         {
            Serial.println("digitalWrite(11, HIGH);");

         }
         
         else if(rx_data=="Q")//laser off
         {
            Serial.println("digitalWrite(11, LOW);");

         }
    

     }
     
  if(readCompleted)
  {
    Serial.print("read value:");
    Serial.println(serialString);

   
    if(serialString == "serial start")
    {
      sendFlag = true;
    }
    else if(serialString == "serial stop")
    {
      sendFlag = false;
    }
    serialString = "";
    readCompleted = false;
  }
if(sendFlag)
  {
    //Serial.println("digitalWrite(pinLed, HIGH);");
    Serial.print("Oxygen value:");
    float oxygen = (sensorVal/1024.0)*5.0*6.2;
    Serial.println(oxygen);
    delay(100);
  }
  else
  {
    //Serial.println("digitalWrite(pinLed, LOW);");
    delay(100);//
  }
  
  
 /*if(Serial.available()>0)    
     { 
         rx_data=Serial.read();
         if(rx_data=='L') 
         {
            digitalWrite(in1, LOW);//left up
         }
         else if(rx_data=='F')
         {
            digitalWrite(in1, HIGH);//left down
         }
         else if(rx_data=='R')
         {
            digitalWrite(in2, LOW);//Right up
         }
         else if(rx_data=='I')
         {
            digitalWrite(in2, HIGH);//Right down
         }


         
         else if(rx_data=='U') //Pull
         {
            digitalWrite(in3, LOW);
            digitalWrite(in4, HIGH);
         }
         else if(rx_data=='H')//Hold
         {
            digitalWrite(in3, HIGH);
            digitalWrite(in4, HIGH);
         }
         else if(rx_data=='D')//Push
         {
            digitalWrite(in4, LOW);
            digitalWrite(in3, HIGH);            
         }


         else if(rx_data=='B')//Pump Start
         {
            digitalWrite(9, HIGH);
         }
         else if(rx_data=='T')//Pump Stop
         {
            digitalWrite(9, LOW);
         }

     }*/
}

void serialEvent()
{
  while(Serial.available())//arduino.cc/en/Serial/Available
  {
    char inChar = (char)Serial.read();
    if(inChar != '\n')
    {
      serialString += inChar;
    }
    else
    {
      readCompleted = true;
    }
  }
}
