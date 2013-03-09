//----------------------------------------------------------//
// File: Beacon.cpp											//
// Description: Control class for the beacon, tidies main	//
// Author: Jeremy Dalton									//
// Creation Date: 05/02/2013								//
//----------------------------------------------------------//

#include "Beacon.h"
#include "Arduino.h"
#include "Config.h"

#define VERSION 2.0

#define pin1 25
#define pin2 26


Beacon::Beacon() {
	pinMode(pin1, OUTPUT);
	pinMode(pin2, OUTPUT);
}

void Beacon::fw() {
	#if LAPTOP_CONTROL
		Serial.print("Bcn: ");
		Serial.print(VERSION);
		Serial.println();
	#else
		Serial2.print("Bcn: ");
		Serial2.print(VERSION);
		Serial2.println();
	#endif
}

void Beacon::init() {
	#if VERBOSE_BOOT
		#if LAPTOP_CONTROL
			Serial.println("Beacon init...");
		#else
			Serial2.println("Beacon init...");
		#endif
		digitalWrite(pin1, LOW);
		digitalWrite(pin2, LOW);
	#else
		digitalWrite(pin1, LOW);
		digitalWrite(pin2, LOW);
	#endif
}

void Beacon::pulse() {
	digitalWrite(pin1, LOW);
	digitalWrite(pin2, LOW);
}

void Beacon::right() {
	digitalWrite(pin1, HIGH);
	digitalWrite(pin2, LOW);
}

void Beacon::left() {
	digitalWrite(pin1, LOW);
	digitalWrite(pin2, HIGH);
}

void Beacon::active() {
	digitalWrite(pin1, HIGH);
	digitalWrite(pin2, HIGH);
}
