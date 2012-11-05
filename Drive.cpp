//----------------------------------------------//
// File: Drive.cpp								//
// Description: Motor control abstraction class //
// Author: Jeremy Dalton                        //
// Creation Date: 23/10/2012                    //
//----------------------------------------------//

#include "Drive.h"
#include "Arduino.h"

#define version	1.1	// Software version

#define rampDelay	10	 // Delay between speed step changes
#define RMclkPin	2	// Right Motor clock pin
#define RMdirPin	42	// Right motor direction pin
#define RMprePin	43	// Right Motor preset pin
#define LMclkPin	3	// Left Motor clock pin
#define LMdirPin	44	// Left Motor direction pin
#define LMprePin	45	// Left Motor preset pin

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
}

void Drive::forward(int debug, int motor, int speed) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only

	//-> Check direction is set to forwards
	//-> test current speed vs desired speed then call appropriate ramp function to change speed.

	switch (motor) {
	case '1':
		if (RMdir == 0) {
			RMdir = 1;
			//digital write direction to pin
		}
		if (LMdir == 0) {
			RMdir = 1;
			//digital write direction to pin
		}

		if (speed > BMspe) {
			rampUp(speed, motor);
		}
		else if (speed < BMspe) {
			rampDown(speed, motor);
		}
		else {
		}
		break;

	case '2':
		if (RMdir == 0) {
			RMdir = 1;
			//digital write direction to pin
		}

		if (speed > BMspe) {
			rampUp(speed, motor);
		}
		else if (speed < BMspe) {
			rampDown(speed, motor);
		}
		else {
		}
		break;

	case '3':
		if (LMdir == 0) {
			LMdir = 1;
			//digital write direction to pin
		}

		if (speed > BMspe) {
			rampUp(speed, motor);
		}
		else if (speed < BMspe) {
			rampDown(speed, motor);
		}
		else {
		}
		break;
	}
}

void Drive::backward(int debug, int motor, int speed) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only

	//-> Check direction is set to forwards
	//-> Each case tests current speed vs desired speed then calls then appropriate ramp function to change speed.

	switch (motor) {
	case '1':
		if (RMdir == 1) {
			RMdir = 0;
			//digital write direction to pin
		}
		if (LMdir == 1) {
			RMdir = 0;
			//digital write direction to pin
		}

		if (speed > BMspe) {
			rampUp(speed, motor);
		}
		else if (speed < BMspe) {
			rampDown(speed, motor);
		}
		else {
		}
		break;

	case '2':
		if (RMdir == 1) {
			RMdir = 0;
			//digital write direction to pin
		}

		if (speed > BMspe) {
			rampUp(speed, motor);
		}
		else if (speed < BMspe) {
			rampDown(speed, motor);
		}
		else {
		}
		break;

	case '3':
		if (LMdir == 1) {
			LMdir = 0;
			//digital write direction to pin
		}

		if (speed > BMspe) {
			rampUp(speed, motor);
		}
		else if (speed < BMspe) {
			rampDown(speed, motor);
		}
		else {
		}
		break;
	}
}

void Drive::stop(int debug, int motor = 1) {
	rampDown(0, motor);
};

void Drive::rampUp(int maxSpeed, int motor) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only	
	switch (motor) {
	case '1':
		for (int i = BMspe; i <= maxSpeed; i++) {
			//write 'i' to output
		}
		//sync new speed to BMspe
		//sync new speed to RMspe
		//sync new speed to LMspe
		break;

	case '2':
		for (int i = RMspe; i <= maxSpeed; i++) {
			//write 'i' to output
		}
		//sync new speed to RMspe
		break;

	case '3':
		for (int i = LMspe; i <= maxSpeed; i++) {
			//write 'i' to output
		}
		//sync new speed to LMspe
		break;
	}
}

void Drive::rampDown(int minSpeed, int motor) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only
	switch (motor) {
	case '1':
		for (int i = BMspe; i >= minSpeed; i--) {
			//write 'i' to output
		}
		//sync new speed to BMspe
		//sync new speed to RMspe
		//sync new speed to LMspe
		break;

	case '2':
		for (int i = RMspe; i >= minSpeed; i--) {
			//write 'i' to output
		}
		//sync new speed to RMspe
		break;

	case '3':
		for (int i = LMspe; i >= minSpeed; i--) {
			//write 'i' to output
		}
		//sync new speed to LMspe
		break;
	}
}

//----------------------------------------------------------//
// But I have promises to keep, and miles to go before I	//
// sleep, and miles to go before I sleep.					//
//	-Robert Frost											//
//----------------------------------------------------------//