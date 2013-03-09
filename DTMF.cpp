#include "DTMF.h"
#include "Arduino.h"

#define VERSION 2.0 //Software version

#define D0 45
#define D1 43
#define D2 41
#define D3 39
#define DV 19

int recChar;

DTMF::DTMF() {
	pinMode(D0, INPUT);
	pinMode(D1, INPUT);
	pinMode(D2, INPUT);
	pinMode(D3, INPUT);
	pinMode(DV, INPUT);
}

void DTMF::fw() {
	#if LAPTOP_CONTROL
		Serial.print("DTMF: ");
		Serial.print(VERSION);
		Serial.println();
	#else
		Serial2.print("DTMF: ");
		Serial2.print(VERSION);
		Serial2.println();
	#endif
}

void DTMF::init() {
	#if VERBOSE_BOOT
		#if LAPTOP_CONTROL
			Serial.println("DTMF init...");
		#else
			Serial2.println("DTMF init...");
		#endif
	#endif
}

void DTMF::getChar() {
	recChar = 0;

	if (digitalRead(D0) == HIGH) recChar = recChar + 1;
	if (digitalRead(D1) == HIGH) recChar = recChar + 2;
	if (digitalRead(D2) == HIGH) recChar = recChar + 4;
	if (digitalRead(D3) == HIGH) recChar = recChar + 8;

	if (recChar == 0) Serial2.println("D");
	if (recChar == 1) Serial2.println("1");
	if (recChar == 2) Serial2.println("2");
	if (recChar == 3) Serial2.println("3");
	if (recChar == 4) Serial2.println("4");
	if (recChar == 5) Serial2.println("5");
	if (recChar == 6) Serial2.println("6");
	if (recChar == 7) Serial2.println("7");
	if (recChar == 8) Serial2.println("8");
	if (recChar == 9) Serial2.println("9");
	if (recChar == 10) Serial2.println("0");
	if (recChar == 11) Serial2.println("*");
	if (recChar == 12) Serial2.println("#");
	if (recChar == 13) Serial2.println("A");
	if (recChar == 14) Serial2.println("B");
	if (recChar == 15) Serial2.println("C");
}