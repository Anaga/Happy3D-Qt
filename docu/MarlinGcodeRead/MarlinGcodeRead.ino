#include "MarlinSerial.h"
#include "MarlinGcodeRead.h"
#include "Laser.h"
#include <SPI.h>


// This determines the communication speed of the printer
#define BAUDRATE 115200

#define MSG_ERR_LINE_NO "Line Number is not Last Line Number+1, Last Line:"
#define MSG_ERR_NO_CHECKSUM "No Checksum with line number, Last Line:"
#define MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM "No Line Number with checksum, Last Line:"
#define MSG_ERR_CHECKSUM_MISMATCH "checksum mismatch, Last Line:"
#define MSG_OK "ok"
#define MSG_ERR_STOPPED "Printer stopped due to errors. Fix the error and use M999 to restart!. (Temperature is reset. Set it before restarting)"


//The ASCII buffer for recieving from the serial:
#define MAX_CMD_SIZE 96
#define BUFSIZE 4
#define AXIS_RELATIVE_MODES {false, false, false, false}
#define NUM_AXIS 4 // The axis order in all axis related arrays is X, Y, Z, E

// Create laser instance (with laser pointer connected to digital pin 5)
Laser laser(5);

int in1 = 6;//left
int in2 = 7;//Right
int in3 = 8;//Pull
int in4 = 9;//Push

float e = 10;  // distance betweeen two mirrors
float d = 400;  // distance between mirror and powder bed
int exptime = 100; // us
int pointdis = 500; // um
int hatchdis = 500; // um

bool axis_relative_modes[] = AXIS_RELATIVE_MODES;
float current_position[NUM_AXIS] = { 0.0, 0.0, 0.0, 0.0 };

const char axis_codes[4] = { 'X', 'Y', 'Z', 'E' };
static float destination[4] = { 0.0, 0.0, 0.0, 0.0 };

static float distance1 = 1;
static int direction1 = 0;
static float speed1 = 1;

static float distance2 = 30;
static int direction2 = 0;
static float speed2 = 1;

int PUL1 = 24; //define Pulse pin
int DIR1 = 23; //define Direction pin
int ENA1 = 22; //define Enable Pin

int PUL2 = 27; //define Pulse pin
int DIR2 = 26; //define Direction pin
int ENA2 = 25; //define Enable Pin

static long gcode_N, gcode_LastN, Stopped_gcode_LastN = 0;
static bool relative_mode = false;  //Determines Absolute or Relative Coordinates

static char cmdbuffer[BUFSIZE][MAX_CMD_SIZE];
static bool fromsd[BUFSIZE];
static char serial_char;
static int serial_count = 0;
static boolean comment_mode = false;
static char *strchr_pointer; // just a pointer to find chars in the cmd string like X, Y, Z,



static int bufindr = 0;
static int bufindw = 0;
static int buflen = 0;

static unsigned long previous_millis_cmd = 0;

bool Stopped = false;

//things to write to serial from Programmemory. saves 400 to 2k of RAM.
#define SerialprintPGM(x) serialprintPGM(MYPGM(x))
FORCE_INLINE void serialprintPGM(const char *str)
{
	char ch = pgm_read_byte(str);
	while (ch)
	{
		MYSERIAL.write(ch);
		ch = pgm_read_byte(++str);
	}
}




void setup() {
  // put your setup code here, to run once:
	MYSERIAL.begin(BAUDRATE);
	SERIAL_PROTOCOLLNPGM("start");
	//SERIAL_ECHO_START;
	laser.init();

   pinMode(in1, OUTPUT);
   pinMode(in2, OUTPUT);
   pinMode(in3, OUTPUT);
   pinMode(in4, OUTPUT);
   digitalWrite(in1, HIGH);
   digitalWrite(in2, HIGH);
   digitalWrite(in3, HIGH);
   digitalWrite(in4, HIGH);

   pinMode(PUL1, OUTPUT);
   pinMode(DIR1, OUTPUT);
   pinMode(ENA1, OUTPUT);

   pinMode(PUL2, OUTPUT);
   pinMode(DIR2, OUTPUT);
   pinMode(ENA2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
	//MYSERIAL.println(buflen);
	if (buflen < (BUFSIZE - 1))
		get_command();
	if (buflen)
	{
		process_commands();
		buflen = (buflen - 1);
		bufindr = (bufindr + 1) % BUFSIZE;
	}

}


void get_command()
{
	//delay(1000);
	while (MYSERIAL.available() > 0 && buflen < BUFSIZE) {
		//MYSERIAL.println("Hello");
		serial_char = MYSERIAL.read();
		/*if (serial_char == 'L')
		{
			digitalWrite(in1, LOW);//left up
		}

		else if (serial_char == 'F')
		{
			digitalWrite(in1, HIGH);//left down
		}

		else if (serial_char == 'R')
		{
			digitalWrite(in2, LOW);//Right up
		}

		else if (serial_char == 'I')
		{
			digitalWrite(in2, HIGH);//Right down
		}



		else if (serial_char == 'U')//Pull
		{
			digitalWrite(in3, LOW);
			digitalWrite(in4, HIGH);
		}
		else if (serial_char == 'H')//Hold
		{
			digitalWrite(in3, HIGH);
			digitalWrite(in4, HIGH);
		}
		else if (serial_char == 'D')//Push
		{
			digitalWrite(in4, LOW);
			digitalWrite(in3, HIGH);
		}*/
		//MYSERIAL.println("Hello");
		//MYSERIAL.println(serial_char);
		if (serial_char == '\n' ||
			serial_char == '\r' ||
			(serial_char == ':' && comment_mode == false) ||
			serial_count >= (MAX_CMD_SIZE - 1))
		{
			//MYSERIAL.println("Hello");
			if (!serial_count) { //if empty line
				comment_mode = false; //for new command
				return;
			}
			cmdbuffer[bufindw][serial_count] = 0; //terminate string
			if (!comment_mode) {
				comment_mode = false; //for new command
				fromsd[bufindw] = false;
				if (strstr(cmdbuffer[bufindw], "N") != NULL)
				{
					strchr_pointer = strchr(cmdbuffer[bufindw], 'N');
					gcode_N = (strtol(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL, 10));
					if (gcode_N != gcode_LastN + 1 && (strstr(cmdbuffer[bufindw], "M110") == NULL)) {
						SERIAL_ERROR_START;
						SERIAL_ERRORPGM(MSG_ERR_LINE_NO);
						SERIAL_ERRORLN(gcode_LastN);
						//Serial.println(gcode_N);
						FlushSerialRequestResend();
						serial_count = 0;
						return;
					}

					if (strstr(cmdbuffer[bufindw], "*") != NULL)
					{
						byte checksum = 0;
						byte count = 0;
						while (cmdbuffer[bufindw][count] != '*') checksum = checksum ^ cmdbuffer[bufindw][count++];
						strchr_pointer = strchr(cmdbuffer[bufindw], '*');

						if ((int)(strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL)) != checksum) {
							SERIAL_ERROR_START;
							SERIAL_ERRORPGM(MSG_ERR_CHECKSUM_MISMATCH);
							SERIAL_ERRORLN(gcode_LastN);
							FlushSerialRequestResend();
							serial_count = 0;
							return;
						}
						//if no errors, continue parsing
					}
					else
					{
						SERIAL_ERROR_START;
						SERIAL_ERRORPGM(MSG_ERR_NO_CHECKSUM);
						SERIAL_ERRORLN(gcode_LastN);
						FlushSerialRequestResend();
						serial_count = 0;
						return;
					}

					gcode_LastN = gcode_N;
					//if no errors, continue parsing
				}
				else  // if we don't receive 'N' but still see '*'
				{
					if ((strstr(cmdbuffer[bufindw], "*") != NULL))
					{
						SERIAL_ERROR_START;
						SERIAL_ERRORPGM(MSG_ERR_NO_LINENUMBER_WITH_CHECKSUM);
						SERIAL_ERRORLN(gcode_LastN);
						serial_count = 0;
						return;
					}
				}
				if ((strstr(cmdbuffer[bufindw], "G") != NULL)) {
					strchr_pointer = strchr(cmdbuffer[bufindw], 'G');
					switch ((int)((strtod(&cmdbuffer[bufindw][strchr_pointer - cmdbuffer[bufindw] + 1], NULL)))) {
					case 0:
					case 1:
					case 2:
					case 3:
						if (Stopped == false) { // If printer is stopped by an error the G[0-3] codes are ignored.
							SERIAL_PROTOCOLLNPGM(MSG_OK);
						}
						else {
							SERIAL_ERRORLNPGM(MSG_ERR_STOPPED);
						}
						break;
					default:
						break;
					}

				}
				bufindw = (bufindw + 1) % BUFSIZE;
				buflen += 1;
			}
			serial_count = 0; //clear buffer
		}
		else
		{
			if (serial_char == ';') comment_mode = true;
			if (!comment_mode) cmdbuffer[bufindw][serial_count++] = serial_char;
		}
		
	}
	//delay(3000);
	/*MYSERIAL.print(cmdbuffer[0][1]);
	MYSERIAL.print(cmdbuffer[0][2]);
	MYSERIAL.print(cmdbuffer[0][3]);
	MYSERIAL.print(cmdbuffer[0][4]);
	MYSERIAL.print(cmdbuffer[0][5]);
	MYSERIAL.print(cmdbuffer[0][6]);
	MYSERIAL.print(cmdbuffer[0][7]);
	MYSERIAL.println(cmdbuffer[1][8]);
	MYSERIAL.print(cmdbuffer[1][1]);
	MYSERIAL.print(cmdbuffer[1][2]);
	MYSERIAL.print(cmdbuffer[1][3]);
	MYSERIAL.print(cmdbuffer[1][4]);
	MYSERIAL.print(cmdbuffer[1][5]);
	MYSERIAL.print(cmdbuffer[1][6]);
	MYSERIAL.print(cmdbuffer[1][7]);
	MYSERIAL.println(cmdbuffer[1][8]);*/
}




void process_commands()
{
	unsigned long codenum; //throw away variable
	char *starpos = NULL;

	//MYSERIAL.println("Hello");
	if (code_seen('G'))
	{
		//MYSERIAL.println("Hello");
		switch ((int)code_value())
		{
		case 0: // G0 -> G1
		case 1: // G1
			if (Stopped == false) {
				get_coordinates(); // For X Y Z S
				
				prepare_move();
				//ClearToSend();
				return;
			}
			//break;
		}
	}

	else if (code_seen('m'))
	{
		switch ((int)code_value())
		{
		case 1: // motor1
			if (Stopped == false) {
				movemotor1(); // For r d v

				return;
			}
		case 2: // motor2
			if (Stopped == false) {
				movemotor2(); // For r d v

				return;
			}
		}	//break;
	}

	else if (code_seen('L'))
	{
        digitalWrite(in1, LOW);//left up
	}

	else if (code_seen('F'))
	{
        digitalWrite(in1, HIGH);//left down
	}	
    
    else if (code_seen('R'))
	{
        digitalWrite(in2, LOW);//Right up
	}    

    else if (code_seen('I'))
	{
        digitalWrite(in2, HIGH);//Right down
    }



    else if (code_seen('U'))//Pull
	{
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
    }
    else if (code_seen('H'))//Hold
	{
        digitalWrite(in3, HIGH);
        digitalWrite(in4, HIGH);
    }
    else if (code_seen('D'))//Push
	{
        digitalWrite(in4, LOW);
        digitalWrite(in3, HIGH); 
	}
	
	else if (code_seen('S'))
	{
		RecoaterSeq();
	}
	 
	else if (code_seen('C'))
	{
		for(int i = 0; i < 5; i++)
		{
			RecoaterSeq();
		    drawcube(0, 0, 20, 20);
	    }
	}

	else
	{
		//SERIAL_ECHO_START;
		//SERIAL_ECHO(cmdbuffer[bufindr]);
		//SERIAL_ECHOLNPGM("\"");
	}

	ClearToSend();
}

void RecoaterSeq()
{
	digitalWrite(in2, LOW);//Right up
	digitalWrite(in1, HIGH);//left down
	
	delay(100);
	digitalWrite(DIR2, HIGH);

	for (int i = 0; i<(80 * distance2); i++)
	{
		digitalWrite(PUL2, HIGH);
		delayMicroseconds(500 / speed2);
		digitalWrite(PUL2, LOW);
		delayMicroseconds(500 / speed2);
	}
	delay(100);

	digitalWrite(in2, HIGH);//Right down

	delay(100);

	digitalWrite(DIR1, HIGH);

	for (int i = 0; i<(80 * distance1); i++)
	{
		digitalWrite(PUL1, HIGH);
		delayMicroseconds(500 / speed1);
		digitalWrite(PUL1, LOW);
		delayMicroseconds(500 / speed1);
	}
	delay(100);

	digitalWrite(in1, LOW);//left up

	delay(100);
	digitalWrite(DIR2, LOW);

	for (int i = 0; i<(80 * distance2); i++)
	{
		digitalWrite(PUL2, HIGH);
		delayMicroseconds(500 / speed2);
		digitalWrite(PUL2, LOW);
		delayMicroseconds(500 / speed2);
	}
	delay(100);

	digitalWrite(in1, HIGH);//left down
}
void FlushSerialRequestResend()
{
	//char cmdbuffer[bufindr][100]="Resend:";
	MYSERIAL.flush();
	//SERIAL_PROTOCOLPGM(MSG_RESEND);
	SERIAL_PROTOCOLLN(gcode_LastN + 1);
	ClearToSend();
}

float code_value()
{
	return (strtod(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL));
}

long code_value_long()
{
	return (strtol(&cmdbuffer[bufindr][strchr_pointer - cmdbuffer[bufindr] + 1], NULL, 10));
}

bool code_seen(char code_string[]) //Return True if the string was found
{
	return (strstr(cmdbuffer[bufindr], code_string) != NULL);
}

bool code_seen(char code)
{
	strchr_pointer = strchr(cmdbuffer[bufindr], code);
	return (strchr_pointer != NULL);  //Return True if a character was found
}

void ClearToSend()
{
	previous_millis_cmd = millis();

	SERIAL_PROTOCOLLNPGM(MSG_OK);
}

void get_coordinates()
{
	bool seen[4] = { false,false,false,false };
	//X, Y, (Z, E)
	//MYSERIAL.println("get_coordinates");
	if (code_seen('X'))
	{
		destination[0] = (float)code_value() + (axis_relative_modes[0] || relative_mode)*current_position[0];
		seen[0] = true;
		//MYSERIAL.println(0);
		//MYSERIAL.println(destination[0]);
	}
	if (code_seen('Y'))
	{
		destination[1] = (float)code_value() + (axis_relative_modes[1] || relative_mode)*current_position[1];
		seen[1] = true;
		//MYSERIAL.println(1);
		//MYSERIAL.println(destination[1]);
	}
		//else destination[i] = current_position[i]; //Are these else lines really needed?
	


	if (code_seen('Z')) {

	}

	if (code_seen('L'))
	{

	}
	if (code_seen('F')) {
		
	}


}

void movemotor1()
{
	if (code_seen('r'))
	{
		direction1 = (int)code_value();
	}
	if (code_seen('d'))
	{
		distance1 = (float)code_value();
	}
	if (code_seen('v'))
	{
		speed1 = (float)code_value();
	}
	MYSERIAL.print("motor1: direction");
	MYSERIAL.print(direction1);
	MYSERIAL.print("  d:");
	MYSERIAL.print(distance1);
	MYSERIAL.print("mm  v:");
	MYSERIAL.print(speed1);
	MYSERIAL.println("mm/s");	delay(100);
	if(direction1 == 0)
	{    
		digitalWrite(DIR1, HIGH);
	
	    for (int i = 0; i<(80*distance1); i++) 
	    {
		    digitalWrite(PUL1, HIGH);
		    delayMicroseconds(500/speed1);
		    digitalWrite(PUL1, LOW);
		    delayMicroseconds(500 / speed1);
	    }
	    delay(100);
	}
	else if (direction1 == 1)
	{
		digitalWrite(DIR1, LOW);

		for (int i = 0; i<(80 * distance1); i++)
		{
			digitalWrite(PUL1, HIGH);
			delayMicroseconds(500 / speed1);
			digitalWrite(PUL1, LOW);
			delayMicroseconds(500 / speed1);
		}
		delay(100);
	}
}

void movemotor2()
{
	if (code_seen('r'))
	{
		direction2 = (int)code_value();
	}
	if (code_seen('d'))
	{
		distance2 = (float)code_value();
	}
	if (code_seen('v'))
	{
		speed2 = (float)code_value();
	}
	MYSERIAL.print("motor2: direction");
	MYSERIAL.print(direction2);
	MYSERIAL.print("  d:");
	MYSERIAL.print(distance2);
	MYSERIAL.print("mm  v:");
	MYSERIAL.print(speed2);
	MYSERIAL.println("mm/s");
	delay(100);
	if (direction2 == 0)
	{
		digitalWrite(DIR2, HIGH);

		for (int i = 0; i<(80 * distance2); i++)
		{
			digitalWrite(PUL2, HIGH);
			delayMicroseconds(500 / speed2);
			digitalWrite(PUL2, LOW);
			delayMicroseconds(500 / speed2);
		}
		delay(100);
	}
	else if (direction2 == 1)
	{
		digitalWrite(DIR2, LOW);

		for (int i = 0; i<(80 * distance2); i++)
		{
			digitalWrite(PUL2, HIGH);
			delayMicroseconds(500 / speed2);
			digitalWrite(PUL2, LOW);
			delayMicroseconds(500 / speed2);
		}
		delay(100);
	}
}

void prepare_move()
{
	laser.setScale(2);
	laser.setOffset(0, 0);
	drawcube(0, 0, 20, 20);
	//MYSERIAL.println(current_position[0]);
	//MYSERIAL.println(current_position[1]);
	//MYSERIAL.println(destination[0]);
	//MYSERIAL.println(destination[1]);
	if (destination[0] > 0 && destination[1] > 0 && current_position[0] > 0 && current_position[1] > 0)
	{
		float des_x = destination[0];
		float des_y = destination[1];
		float des_dx = atan(des_x / (e + sqrt(d * d + des_y * des_y))) * 180 / 3.1415926 / 2;  // degree of the mirror
		float des_dy = atan(des_y / d) * 180 / 3.1415926 / 2;  // degree of the mirror
		float  des_vx = 4096 * (des_dx / 30 / 4.096 * 5) * 4 - 3000; // degree number of the mirror
		float  des_vy = 4096 * (des_dy / 30 / 4.096 * 5) * 4 - 3000; // degree number of the mirror

		float cur_x = current_position[0];
		float cur_y = current_position[1];
		float cur_dx = atan(des_x / (e + sqrt(d * d + cur_y * cur_y))) * 180 / 3.1415926 / 2;  // degree of the mirror
		float cur_dy = atan(cur_y / d) * 180 / 3.1415926 / 2;  // degree of the mirror
		float  cur_vx = 4096 * (cur_dx / 30 / 4.096 * 5) * 4 - 3000; // degree number of the mirror
		float  cur_vy = 4096 * (cur_dy / 30 / 4.096 * 5) * 4 - 3000; // degree number of the mirror
		
		laser.drawline(cur_vx, cur_vy, des_vx, des_vy);
		MYSERIAL.print(cur_x);
		MYSERIAL.print("    ");
		MYSERIAL.print(cur_y);
		MYSERIAL.print("    ");
		MYSERIAL.print(des_x);
		MYSERIAL.print("    ");
		MYSERIAL.println(des_y);

		MYSERIAL.print(cur_vx);
		MYSERIAL.print("    ");		
		MYSERIAL.print(cur_vy);
		MYSERIAL.print("    ");
		MYSERIAL.print(des_vx);
		MYSERIAL.print("    ");
		MYSERIAL.println(des_vy);
	}
	current_position[0] = destination[0];
	current_position[1] = destination[1];
}











void laserpoint(float x, float y) {
	float dx = atan(x / (e + sqrt(d * d + y * y))) * 180 / 3.1415926 / 2;  // degree of the mirror
	float dy = atan(y / d) * 180 / 3.1415926 / 2;  // degree of the mirror
	float  vx = 4096 * (dx / 30 / 4.096 * 5) * 10; // degree number of the mirror
	float  vy = 4096 * (dy / 30 / 4.096 * 5) * 10; // degree number of the mirror
	laser.sendto(vx, vy);
	MYSERIAL.println(vx);
	MYSERIAL.println(vy);
}

void drawcube(float x0, float y0, float dx, float dy) {
	for (int i = 1; i <= 1000 * dx / hatchdis; i++) {
		for (int j = 1; j <= 1000 * dy / pointdis; j++) {
			float fi = i;
			float fj = j;

			if (i % 2) {
				float x1 = x0 + fi * hatchdis / 1000;
				float y1 = y0 + fj * pointdis / 1000;
				laser.on();
				laserpoint(x1, y1);
				delayMicroseconds(exptime);
				laser.off();
			}
			else {
				float x1 = x0 + fi * hatchdis / 1000;
				float y1 = dy - fj * pointdis / 1000;
				laser.on();
				laserpoint(x1, y1);
				delayMicroseconds(exptime);
				laser.off();
			}
		}
	}
	/*for (int i = 1; i <= 1000 * dx / hatchdis; i++) {
	for (int j = 1; j <= 1000 * dy / pointdis; j++) {
	float x1 = x0 + i * hatchdis / 1000;
	float y1 = y0 + j * pointdis / 1000;
	laser.on();
	laserpoint(x1, y1);
	delayMicroseconds(exptime);
	laser.off();
	}

	}*/

}