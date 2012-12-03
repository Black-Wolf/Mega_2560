//----------------------------------------------------------//
// File: Navigation.cpp										//
// Description: Navigation and locomotion abstraction class //
// Author: Jeremy Dalton									//
// Creation Date: 23/10/2012								//
//----------------------------------------------------------//

#include "Navigation.h"
#include "Arduino.h"

#define consKp	2
#define consKi	5
#define consKd	1
#define aggKp	2
#define aggKi	5
#define aggKd	1
#define SETPOINT	100
#define MODE	0
#define HIGH_SPEED	255
#define MID_SPEED	200
#define LOW_SPEED	150
#define TURN_SPEED	200
#define TURN_DELAY	500
#define VERSION	0.1	// Software version

double setpoint, input, output;
PID pid(&input, &output, &setpoint, consKp, consKi, consKd, DIRECT);

Navigation::Navigation() {

}

void Navigation::fw() {
	Serial.print("Nav: ");
	Serial.print(VERSION);
	Serial.println();
	drive.fw();
	line.fw();
}

void Navigation::init(int verbose, int debug) {
	if (verbose >= 2) {
		Serial.println("Navigation init...");
		if (MODE == 1) {
			Serial.println("|_ *Advanced Mode*");
		}
		else {
			Serial.println("|_ *Basic Mode*");
		}
	}

	blocksTravelled = 0;
	currPosition = 0;
	currPattern = 0;
	lastPattern = 0;
	currSpeed = 0;

	if (verbose >= 2) Serial.println("|_ PID init...");
	input = 0;
	setpoint = SETPOINT;
	pid.SetMode(MANUAL);
	if (verbose >= 2) Serial.println("|_ PID to Manual Mode");

	drive.init(verbose);
	line.init(verbose, debug);
}

//------------------------------------------------------------------------//
// Motor select options:	1=> Both only	2=> Right only	3=> Left only //
// Position options:	+ve=> Correct left	-ve=> Correct right			  //					
//------------------------------------------------------------------------//

bool Navigation::forward(int verbose, int debug, int blocks) {
	if (verbose >= 3) {
		Serial.print("Nav Forward: ");
		Serial.print(blocks);
		Serial.print(" Blocks");
		Serial.println();
	}

	if (MODE == 1) {
		// Advanced Mode
		// -> Read tracking info to prepare for motion
		// -> Detect if pattern has changed
		// -> Pattern has changed then increment done blocks
		// -> Check if we've travelled the required distance
		// -> Compute PID
		// -> Update wheel speed / begin motion
		blocksTravelled = 0;
		while (blocksTravelled < blocks) {

			currPosition = line.tracking(verbose, debug);
			currPattern = line.getPattern();

			if (currPattern != lastPattern) {
				blocks++;
				lastPattern = currPattern;
			}

			// more code goes here

		}
	}
	else {
		// Basic Mode
		// -> Read tracking info to prepare for motion
		// -> Detect if pattern has changed
		// -> Pattern has changed then increment done blocks
		// -> Check if we've travelled the required distance
		// -> Calculate wheel speed based on tracking info
		// -> Check if in motion or not
		// -> Update wheel speed / begin motion
		blocksTravelled = 0;
		while (blocksTravelled < blocks) {

			currPosition = line.tracking(verbose, debug);
			currPattern = line.getPattern();

			if (currPattern != lastPattern) {
				blocks++;
				lastPattern = currPattern;
			}

			if (currPosition == 0) {
				// forwards full speed
				currSpeed = HIGH_SPEED;
				drive.forward(verbose, debug, 1, currSpeed);
			}
			else if (currPosition > 0) {
				//correction left
				if (currPosition < 4) {
					// small correction left
					// -> check if in motion or not
					if (currSpeed == 0) {
						// -> drive right motor slowly
						currSpeed = LOW_SPEED;
						drive.forward(verbose, debug, 2, currSpeed);
					}
					else {
						// -> slow left motor
						currSpeed = MID_SPEED;
						drive.forward(verbose, debug, 3, currSpeed);
					}

				}
				else {
					// large correction left
					// -> check if in motion or not
					if (currSpeed == 0) {
						// -> drive right motor slowly
						currSpeed = MID_SPEED;
						drive.forward(verbose, debug, 2, currSpeed);
					}
					else {
						// -> slow left motor
						currSpeed = LOW_SPEED;
						drive.forward(verbose, debug, 3, currSpeed);
					}

				}
			}
			else if (currPosition < 0) {
				// correction right
				if (currPosition < 4) {
					// small correction right
					// -> check if in motion or not
					if (currSpeed == 0) {
						// -> drive left motor slowly
						currSpeed = LOW_SPEED;
						drive.forward(verbose, debug, 3, currSpeed);
					}
					else {
						// -> slow right motor
						currSpeed = MID_SPEED;
						drive.forward(verbose, debug, 2, currSpeed);
					}

				}
				else {
					// large correction right
					// -> check if in motion or not
					if (currSpeed == 0) {
						// -> drive left motor slowly
						currSpeed = MID_SPEED;
						drive.forward(verbose, debug, 3, currSpeed);
					}
					else {
						// -> slow right motor
						currSpeed = LOW_SPEED;
						drive.forward(verbose, debug, 2, currSpeed);
					}
				}
			}
		}
	}

	pid.SetMode(MANUAL);
	drive.stop(verbose, debug, 1);
	return true;
}

bool Navigation::backward(int verbose, int debug, int blocks) {
	if (verbose >= 3) {
		Serial.print("Nav Backward: ");
		Serial.print(blocks);
		Serial.print(" Blocks");
		Serial.println();
	}

	if (MODE == 1) {
		// Advanced Mode
		// -> Read tracking info to prepare for motion
		// -> Detect if pattern has changed
		// -> Pattern has changed then increment done blocks
		// -> Check if we've travelled the required distance
		// -> Compute PID
		// -> Update wheel speed / begin motion
		blocksTravelled = 0;
		while (blocksTravelled < blocks) {

			currPosition = line.tracking(verbose, debug);
			currPattern = line.getPattern();

			if (currPattern != lastPattern) {
				blocks++;
				lastPattern = currPattern;
			}

			// more code goes here

		}
	}
	else {
		// Basic Mode
		// -> Read tracking info to prepare for motion
		// -> Detect if pattern has changed
		// -> Pattern has changed then increment done blocks
		// -> Check if we've travelled the required distance
		// -> Calculate wheel speed based on tracking info
		// -> Check if in motion or not
		// -> Update wheel speed / begin motion
		blocksTravelled = 0;
		while (blocksTravelled < blocks) {

			currPosition = line.tracking(verbose, debug);
			currPattern = line.getPattern();

			if (currPattern != lastPattern) {
				blocks++;
				lastPattern = currPattern;
			}

			if (currPosition == 0) {
				// forwards full speed
				currSpeed = HIGH_SPEED;
				drive.backward(verbose, debug, 1, currSpeed);
			}
			else if (currPosition > 0) {
				//correction left
				if (currPosition < 4) {
					// small correction left
					// -> check if in motion or not
					if (currSpeed == 0) {
						// -> drive right motor slowly
						currSpeed = LOW_SPEED;
						drive.backward(verbose, debug, 2, currSpeed);
					}
					else {
						// -> slow left motor
						currSpeed = MID_SPEED;
						drive.backward(verbose, debug, 3, currSpeed);
					}

				}
				else {
					// large correction left
					// -> check if in motion or not
					if (currSpeed == 0) {
						// -> drive right motor slowly
						currSpeed = MID_SPEED;
						drive.backward(verbose, debug, 2, currSpeed);
					}
					else {
						// -> slow left motor
						currSpeed = LOW_SPEED;
						drive.backward(verbose, debug, 3, currSpeed);
					}

				}
			}
			else if (currPosition < 0) {
				// correction right
				if (currPosition < 4) {
					// small correction right
					// -> check if in motion or not
					if (currSpeed == 0) {
						// -> drive left motor slowly
						currSpeed = LOW_SPEED;
						drive.backward(verbose, debug, 3, currSpeed);
					}
					else {
						// -> slow right motor
						currSpeed = MID_SPEED;
						drive.backward(verbose, debug, 2, currSpeed);
					}

				}
				else {
					// large correction right
					// -> check if in motion or not
					if (currSpeed == 0) {
						// -> drive left motor slowly
						currSpeed = MID_SPEED;
						drive.backward(verbose, debug, 3, currSpeed);
					}
					else {
						// -> slow right motor
						currSpeed = LOW_SPEED;
						drive.backward(verbose, debug, 2, currSpeed);
					}
				}
			}
		}
	}

	pid.SetMode(MANUAL);
	drive.stop(verbose, debug, 1);
	return true;
}

bool Navigation::turnRight(int verbose, int debug) {
	if (verbose >= 3) Serial.println("Nav Right");

	if (MODE == 1) {
		// Advanced Mode

	}
	else {
		// Basic Mode
		// -> Left motor FWD, right motor BKD
		// -> update tracking info
		// -> When position = 0 stop
		currSpeed = TURN_SPEED;
		drive.forward(verbose, debug, 3, currSpeed);
		drive.backward(verbose, debug, 2, currSpeed);
		delay(TURN_DELAY);
		// read tracking info
		// get to 0
	}

	return true;
}

bool Navigation::turnLeft(int verbose, int debug) {
	if (verbose >= 3) Serial.println("Nav Left");

	if (MODE == 1) {
		// Advanced Mode

	}
	else {
		// Basic Mode
		// -> Left motor BKD, right motor FWD
		// -> update tracking info
		// -> When position = 0 stop
		currSpeed = TURN_SPEED;
		drive.forward(verbose, debug, 2, currSpeed);
		drive.backward(verbose, debug, 3, currSpeed);
		delay(TURN_DELAY);
		//read tracking info
		//get to 0
	}

	return true;
}

//------------------------------------------------------------------//
// Aim for the moon; even if you miss you’ll land among the stars	//
//	-W. Clement Stone												//
//------------------------------------------------------------------//