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
}

void Drive::forward(int debug, int motor, int speed) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only

	//Each case tests current speed vs desired speed then call
	//then appropriate ramp function to change speed.

	// set direction to forwards

	switch (motor) {
	case '1':

		break;
	case '2':

		break;
	case '3':

		break;
	default:

		break;
	}
}

void Drive::backward(int debug, int motor, int speed) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only

	//Set direction to backwards

	switch (motor) {
	case '1':

		break;
	case '2':

		break;
	case '3':

		break;
	default:

		break;
	}

}

void Drive::stop(int debug, int motor) {
	rampDown(0, motor);
};

void Drive::rampUp(int maxSpeed, int motor) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only	
	switch (motor) {
	case '1':

		break;
	case '2':

		break;
	case '3':

		break;
	default:

		break;
	}
}

void Drive::rampDown(int minSpeed, int motor) {
	//Motor select options:	1=> Both only	2=> Right only	3=> Left only
	switch (motor) {
	case '1':

		break;
	case '2':

		break;
	case '3':

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