//----------------------------------------------//
// File: Drive.cpp								//
// Description: Motor control abstraction class //
// Author: Jeremy Dalton                        //
// Creation Date: 23/10/2012                    //
//----------------------------------------------//

#include "Drive.h"
#include "Arduino.h"

#define VERSION	2.5	// Software version

#define rampDelay	1	 // Delay between speed step changes
#define RMclkPin	3	// Right Motor clock pin
#define RMdirPin	51	// Right motor direction pin
#define RMprePin	49	// Right Motor preset pin
#define LMclkPin	2	// Left Motor clock pin
#define LMdirPin	47	// Left Motor direction pin
#define LMprePin	53	// Left Motor preset pin

Drive::Drive() {
	pinMode(RMdirPin, OUTPUT);
	pinMode(RMprePin, OUTPUT);
	pinMode(LMdirPin, OUTPUT);
	pinMode(LMprePin, OUTPUT);
}

void Drive::fw() {
	#if LAPTOP_CONTROL
		Serial.print("Drv: ");
		Serial.print(VERSION);
		Serial.println();
	#else
		Serial2.print("Drv: ");
		Serial2.print(VERSION);
		Serial2.println();
	#endif
}

void Drive::init() {
	#if VERBOSE_BOOT
		#if LAPTOP_CONTROL
			Serial.println("|_ Drive init...");
		#else
			Serial2.println("|_ Drive init...");
		#endif
	#endif

	Drive::RMspe = 0;
	Drive::RMdir = 0;
	Drive::RMpre = 1;
	Drive::LMspe = 0;
	Drive::LMdir = 0;
	Drive::LMpre = 1;
	Drive::BMspe = 0;

	analogWrite(RMclkPin, RMspe);
	digitalWrite(RMdirPin, RMdir);
	digitalWrite(RMprePin, RMpre);
	analogWrite(LMclkPin, LMspe);
	digitalWrite(LMdirPin, LMdir);
	digitalWrite(LMprePin, LMpre);
}

void Drive::right(int verbose, int debug, int speed) {
	if (verbose >= 3) Serial.println("Right Turn");
	RMdir = 0;
	LMdir = 0;
	digitalWrite(RMdirPin, RMdir);
	digitalWrite(LMdirPin, LMdir);
	rampUp(debug, speed, 1);
}

void Drive::left(int verbose, int debug, int speed) {
	if (verbose >= 3) Serial.println("Left Turn");
	RMdir = 1;
	LMdir = 1;
	digitalWrite(RMdirPin, RMdir);
	digitalWrite(LMdirPin, LMdir);
	rampUp(debug, speed, 1);
}

void Drive::forward(int verbose, int debug, int motor, int speed) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only

	//-> Check direction is set to forwards
	//-> test current speed vs desired speed then call appropriate ramp function to change speed.

	switch (motor) {
	case 1:
		if (verbose >= 3) Serial.println("Forward: Case 1 - Both Motors");
		if (RMdir == 0) {
			RMdir = 1;
			//digital write direction to pin
			if (verbose >= 3) Serial.println("Forward: Setting RMdir");
			digitalWrite(RMdirPin, RMdir);
		}
		if (LMdir == 1) {
			LMdir = 0;
			//digital write direction to pin
			if (verbose >= 3) Serial.println("Forward: Setting LMdir");
			digitalWrite(LMdirPin, LMdir);
		}

		if (speed > BMspe) {
			if (verbose >= 3) Serial.println("Forward: rampUp called");
			rampUp(debug, speed, motor);
		}
		else if (speed < BMspe) {
			if (verbose >= 3) Serial.println("Forward: rampDown called");
			rampDown(debug, speed, motor);
		}
		else if (speed == BMspe) {
			if (verbose >= 3) Serial.println("Forward: Nothing called");
		}
		break;

	case 2:
		if (verbose >= 3) Serial.println("Forward: Case 2 - Right Motor");
		if (RMdir == 0) {
			RMdir = 1;
			//digital write direction to pin
			if (verbose >= 3) Serial.println("Forward: Setting RMdir");
			digitalWrite(RMdirPin, RMdir);
		}

		if (speed > RMspe) {
			if (verbose >= 3) Serial.println("Forward: rampUp called");
			rampUp(debug, speed, motor);
		}
		else if (speed < RMspe) {
			if (verbose >= 3) Serial.println("Forward: rampDown called");
			rampDown(debug, speed, motor);
		}
		else if (speed == RMspe) {
			if (verbose >= 3) Serial.println("Forward: Nothing called");
		}
		break;

	case 3:
		if (verbose >= 3) Serial.println("Forward: Case 3 - Left Motor");
		if (LMdir == 1) {
			LMdir = 0;
			//digital write direction to pin
			if (verbose >= 3) Serial.println("Forward: Setting LMdir");
			digitalWrite(LMdirPin, LMdir);
		}

		if (speed > LMspe) {
			if (verbose >= 3) Serial.println("Forward: rampUp called");
			rampUp(debug, speed, motor);
		}
		else if (speed < LMspe) {
			if (verbose >= 3) Serial.println("Forward: rampDown called");
			rampDown(debug, speed, motor);
		}
		else if (speed == LMspe) {
			if (verbose >= 3) Serial.println("Forward: Nothing called");
		}
		break;

	default:

		break;

	}
}

void Drive::backward(int verbose, int debug, int motor, int speed) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only

	//-> Check direction is set to forwards
	//-> Each case tests current speed vs desired speed then calls then appropriate ramp function to change speed.

	switch (motor) {
	case 1:
		if (verbose >= 3) Serial.println("Backward: Case 1 - Both Motors");
		if (RMdir == 1) {
			RMdir = 0;
			//digital write direction to pin
			if (verbose >= 3) Serial.println("Backward: Setting RMdir");
			digitalWrite(RMdirPin, RMdir);
		}
		if (LMdir == 0) {
			LMdir = 1;
			//digital write direction to pin
			if (verbose >= 3) Serial.println("Backward: Setting LMdir");
			digitalWrite(LMdirPin, LMdir);
		}

		if (speed > BMspe) {
			if (verbose >= 3) Serial.println("Backward: rampUp called");
			rampUp(debug, speed, motor);
		}
		else if (speed < BMspe) {
			if (verbose >= 3) Serial.println("Backward: rampDown called");
			rampDown(debug, speed, motor);
		}
		else if (speed == BMspe) {
			if (verbose >= 3) Serial.println("Backward: Nothing called");
		}
		break;

	case 2:
		if (verbose >= 3) Serial.println("Backward: Case 2 - Right Motor");
		if (RMdir == 1) {
			RMdir = 0;
			//digital write direction to pin
			if (verbose >= 3) Serial.println("Backward: Setting RMdir");
			digitalWrite(RMdirPin, RMdir);
		}

		if (speed > RMspe) {
			if (verbose >= 3) Serial.println("Backward: rampUp called");
			rampUp(debug, speed, motor);
		}
		else if (speed < RMspe) {
			if (verbose >= 3) Serial.println("Backward: rampDown called");
			rampDown(debug, speed, motor);
		}
		else if (speed == RMspe) {
			if (verbose >= 3) Serial.println("Backward: Nothing called");
		}
		break;

	case 3:
		if (verbose >= 3) Serial.println("Backward: Case 3 - Left Motor");
		if (LMdir == 0) {
			LMdir = 1;
			//digital write direction to pin
			if (verbose >= 3) Serial.println("Backward: Setting LMdir");
			digitalWrite(LMdirPin, LMdir);
		}

		if (speed > LMspe) {
			if (verbose >= 3) Serial.println("Backward: rampUp called");
			rampUp(debug, speed, motor);
		}
		else if (speed < LMspe) {
			if (verbose >= 3) Serial.println("Backward: rampDown called");
			rampDown(debug, speed, motor);
		}
		else if (speed == LMspe) {
			if (verbose >= 3) Serial.println("Backward: Nothing called");
		}
		break;

	default:

		break;

	}
}

void Drive::stop(int verbose, int debug, int motor = 1) {
	if (verbose >= 3) Serial.println("Stop!");
	rampDown(debug, 0, motor);
};

void Drive::rampUp(int debug, int maxSpeed, int motor) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only	
	switch (motor) {
	case 1:
		if (debug >= 4) Serial.println("rampUp: Case 1 - Both Motors");
		int i;
		for (i = BMspe; i <= maxSpeed; i++) {
			//write 'i' to output
			if (debug >= 5) Serial.print("rampUp: ");
			if (debug >= 5) Serial.print(i);
			if (debug >= 5) Serial.println();
			analogWrite(RMclkPin, i);
			analogWrite(LMclkPin, i);
			delay(rampDelay);
		}
		i--;
		//sync new speed to BMspe
		BMspe = i;
		//sync new speed to RMspe
		RMspe = i;
		//sync new speed to LMspe
		LMspe = i;
		break;

	case 2:
		if (debug >= 4) Serial.println("rampUp: Case 2 - Right Motor");
		for (int i = RMspe; i <= maxSpeed; i++) {
			//write 'i' to output
			if (debug >= 5) Serial.print("rampUp: ");
			if (debug >= 5) Serial.print(i);
			if (debug >= 5) Serial.println();
			analogWrite(RMclkPin, i);
			delay(rampDelay);
		}
		i--;
		//sync new speed to RMspe
		RMspe = i;
		//sync new speed to BMspe
		BMspe = 0;
		break;

	case 3:
		if (debug >= 4) Serial.println("rampUp: Case 3 - Left Motor");
		for (int i = LMspe; i <= maxSpeed; i++) {
			//write 'i' to output
			if (debug >= 5) Serial.print("rampUp: ");
			if (debug >= 5) Serial.print(i);
			if (debug >= 5) Serial.println();
			analogWrite(LMclkPin, i);
			delay(rampDelay);
		}
		i--;
		//sync new speed to LMspe
		LMspe = i;
		//sync new speed to BMspe
		BMspe = 0;
		break;

	default:

		break;

	}
}

void Drive::rampDown(int debug, int minSpeed, int motor) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only
	switch (motor) {
	case 1:
		int i;
		if (debug >= 4) Serial.println("rampDown: Case 1 - Both Motors");
		for (i = BMspe; i >= minSpeed; i--) {
			//write 'i' to output
			if (debug >= 5) Serial.print("rampDown: ");
			if (debug >= 5) Serial.print(i);
			if (debug >= 5) Serial.println();
			analogWrite(RMclkPin, i);
			analogWrite(LMclkPin, i);
			delay(rampDelay);
		}
		i++;
		//sync new speed to BMspe
		BMspe = i;
		//sync new speed to RMspe
		RMspe = i;
		//sync new speed to LMspe
		LMspe = i;
		break;

	case 2:
		for (int i = RMspe; i >= minSpeed; i--) {
			//write 'i' to output
			if (debug >= 5) Serial.print("rampDown: ");
			if (debug >= 5) Serial.print(i);
			if (debug >= 5) Serial.println();
			analogWrite(RMclkPin, i);
			delay(rampDelay);
		}
		i++;
		//sync new speed to RMspe
		RMspe = i;
		//sync new speed to BMspe
		BMspe = 0;
		break;

	case 3:
		for (int i = LMspe; i >= minSpeed; i--) {
			//write 'i' to output
			if (debug >= 5) Serial.print("rampDown: ");
			if (debug >= 5) Serial.print(i);
			if (debug >= 5) Serial.println();
			analogWrite(LMclkPin, i);
			delay(rampDelay);
		}
		i++;
		//sync new speed to LMspe
		LMspe = i;
		//sync new speed to BMspe
		BMspe = 0;
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