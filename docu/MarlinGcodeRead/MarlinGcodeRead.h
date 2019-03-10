
#ifndef MARLINGCODEREAD_H
#define MARLINGCODEREAD_H

#define  HardwareSerial_h // trick to disable the standard HWserial

#define  FORCE_INLINE __attribute__((always_inline)) inline

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>


#include <util/delay.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include  <avr/wdt.h>
#include  <avr/interrupt.h>

#if ARDUINO >= 100 
#if defined(__AVR_ATmega644P__)
#include "WProgram.h"
#else
#include "Arduino.h"
#endif
#else
#include "WProgram.h"
#endif

#include "MarlinSerial.h"

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include "WString.h"

#if defined(__AVR_AT90USB1286__)||defined(__AVR_AT90USB1287__)
#define MYSERIAL Serial
#else
#define MYSERIAL MSerial
#endif

#define MYPGM(s) PSTR(s)



#define SERIAL_PROTOCOL(x) MYSERIAL.print(x);
#define SERIAL_PROTOCOL_F(x,y) MYSERIAL.print(x,y);
#define SERIAL_PROTOCOLPGM(x) serialprintPGM(MYPGM(x));
#define SERIAL_PROTOCOLLN(x) {MYSERIAL.print(x);MYSERIAL.write('\n');}
#define SERIAL_PROTOCOLLNPGM(x) {serialprintPGM(MYPGM(x));MYSERIAL.write('\n');}

const char errormagic[] PROGMEM = "Error:";
const char echomagic[] PROGMEM = "echo:";
#define SERIAL_ERROR_START serialprintPGM(errormagic);
#define SERIAL_ERROR(x) SERIAL_PROTOCOL(x)
#define SERIAL_ERRORPGM(x) SERIAL_PROTOCOLPGM(x)
#define SERIAL_ERRORLN(x) SERIAL_PROTOCOLLN(x)
#define SERIAL_ERRORLNPGM(x) SERIAL_PROTOCOLLNPGM(x)

#define SERIAL_ECHO_START serialprintPGM(echomagic);
#define SERIAL_ECHO(x) SERIAL_PROTOCOL(x)
#define SERIAL_ECHOPGM(x) SERIAL_PROTOCOLPGM(x)
#define SERIAL_ECHOLN(x) SERIAL_PROTOCOLLN(x)
#define SERIAL_ECHOLNPGM(x) SERIAL_PROTOCOLLNPGM(x)

void get_command();
void process_commands();
void ClearToSend();

void get_coordinates();
void movemotor1();
void movemotor2();
void RecoaterSeq();
void prepare_move();

#endif

