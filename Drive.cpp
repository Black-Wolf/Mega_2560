//----------------------------------------------//
// File: Drive.cpp								//
// Description: Motor control abstraction class //
// Author: Jeremy Dalton                        //
// Creation Date: 23/10/2012                    //
//----------------------------------------------//

#include "Drive.h"
#include "Arduino.h"

#define version	0.5	// Software version

#define rampDelay	10	 // Delay between speed step changes
#define RMclkPin	2	// Right Motor clock pin
#define RMdirPin	3	// Right motor direction pin
#define RMprePin	4	// Right Motor preset pin
#define LMclkPin	5	// Left Motor clock pin
#define LMdirPin	6	// Left Motor direction pin
#define LMprePin	7	// Left Motor preset pin

Drive::Drive() {

}

void Drive::fw() {
	Serial.print("Drv: ");
	Serial.print(version);
	Serial.println();
}

void Drive::init(int verbose) {
	if (verbose >= 2) Serial.println("-> drive init");
	Drive::RMspe = 0;
	Drive::RMdir = 0;
	Drive::RMpre = 0;
	Drive::LMspe = 0;
	Drive::LMdir = 0;
	Drive::LMpre = 0;
	Drive::BMspe = 0;

	// Generate a Fibonacci array of values up to 255, quick to compute and roughly logarithmic
	// 14 steps seems about the right length, with a short change between speed values it won't
	// tie up the cpu ramping speed for too long.
	Drive::rampArray[0] = 0;
	Drive::rampArray[1] = 1;
	for(int i = 2; i <= 12; i++) Drive::rampArray[i] = Drive::rampArray[i-1] + Drive::rampArray[i-2];
	Drive::rampArray[13] = 255;

	if (verbose >= 3) for(int i = 0; i <= 13; i++) Serial.println(Drive::rampArray[i]);

}

void Drive::forward(int debug, int motor, int ramp) {
	//Ramp select options:	1=> Up	2=> Down

	RMdir = 1;
	LMdir = 1;
	digitalWrite(RMdirPin, RMdir);
	digitalWrite(LMdirPin, LMdir);

	if (ramp == 1) {
		rampUp(13,motor);
	};
	if (ramp == 2) {
		rampDown(0,motor);
	};
}

void Drive::backward(int debug, int motor, int ramp) {
	//Ramp select options:	1=> Up	2=> Down

	RMdir = 0;
	LMdir = 0;
	digitalWrite(RMdirPin, RMdir);
	digitalWrite(LMdirPin, LMdir);

	if (ramp == 1) {
		rampUp(13,motor);
	};
	if (ramp == 2) {
		rampDown(0,motor);
	};
}

void Drive::rampUp(int maxSpeed, int motor) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only	
	switch (motor) {
	case '1':
		for (int i = BMspe; i <= maxSpeed; i++) {
			analogWrite(RMclkPin, rampArray[i]);
			analogWrite(LMclkPin, rampArray[i]);
			delay(rampDelay);
		}
		BMspe = maxSpeed;
		RMspe = maxSpeed;
		LMspe = maxSpeed;
		break;
	case '2':
		for (int i = RMspe; i <= maxSpeed; i++) {
			RMspe = maxSpeed;
			analogWrite(RMclkPin, rampArray[RMspe]);
			delay(rampDelay);
		}
		break;
	case '3':
		for (int i = LMspe; i <= maxSpeed; i++) {
			LMspe = maxSpeed;
			analogWrite(LMclkPin, rampArray[LMspe]);
			delay(rampDelay);
		}
		break;
	default:

		break;
	}
}

void Drive::rampDown(int minSpeed, int motor) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only
	switch (motor) {
	case '1':
		for (int i = BMspe; i >= minSpeed; i--) {
			analogWrite(RMclkPin, rampArray[i]);
			analogWrite(LMclkPin, rampArray[i]);
			delay(rampDelay);
		}
		BMspe = minSpeed;
		RMspe = minSpeed;
		LMspe = minSpeed;
		break;
	case '2':
		for (int i = BMspe; i >= minSpeed; i--) {
			RMspe = minSpeed;
			analogWrite(RMclkPin, rampArray[RMspe]);
			delay(rampDelay);
		}
		break;
	case '3':
		for (int i = BMspe; i >= minSpeed; i--) {
			LMspe = minSpeed;
			analogWrite(LMclkPin, rampArray[LMspe]);
			delay(rampDelay);
		}
		break;
	default:

		break;
	}
}

//----------------------------------------------------------//
// But I have promises to keep, and miles to go before I	//
// sleep, and miles to go before I sleep.					//
//	-Robert Frost											//
//----------------------------------------------------------//