//----------------------------------------------------------//
// File: Navigation.cpp										//
// Description: Navigation and locomotion abstraction class //
// Author: Jeremy Dalton									//
// Creation Date: 23/10/2012								//
//----------------------------------------------------------//

#include "Navigation.h"
#include "Arduino.h"

// Navigation Control variables
#define MODE	0
#define VERSION	1.5	// Software version
#define FWDbufferSize 1
#define RWDbufferSize 20

// Speed variables
#define BASE_SPEED 100
#define k100 1
#define k90 0.9
#define k80 0.8
#define k70 0.7
#define k60 0.6
#define k50 0.5
#define k40 0.4
#define k30 0.3
#define k20 0.2
#define k10 0.1
#define k00 0

// Turning Variables
#define TURN_SPEED	100
#define LR_TURN_DELAY	400

// Target Acquisition variables
#define REEDpin 23

// Ring buffer initialisation
RunningAverage buff(FWDbufferSize);
RunningAverage revbuff(RWDbufferSize);

Navigation::Navigation() {
	pinMode(REEDpin, INPUT);
}

void Navigation::fw() {
	#if LAPTOP_CONTROL
		Serial.print("Nav: ");
		Serial.print(VERSION);
		Serial.println();
		drive.fw();
		line.fw();
	#else
		Serial2.print("Nav: ");
		Serial2.print(VERSION);
		Serial2.println();
		drive.fw();
		line.fw();
	#endif
}

void Navigation::init() {
	#if VERBOSE_BOOT
		#if LAPTOP_CONTROL
			Serial.println("Navigation init...");
		#else
			Serial2.println("Navigation init...");
		#endif
	#endif

	blocksTravelled = 0;
	currPosition = 0;
	currPattern = 0;
	lastPattern = 0;
	inMotion = 0;

	#if VERBOSE_BOOT
		#if LAPTOP_CONTROL
			Serial.println("|_ Sensor buffer init...");
			Serial.print("|_ FWD Ring buffer length: ");
			Serial.println(FWDbufferSize);
			Serial.print("|_ RWD Ring buffer length: ");
			Serial.println(RWDbufferSize);
			Serial.println("|_ Clearing sensor buffers...");
		#else
			Serial2.println("|_ Sensor buffer init...");
			Serial2.print("|_ FWD Ring buffer length: ");
			Serial2.println(FWDbufferSize);
			Serial2.print("|_ RWD Ring buffer length: ");
			Serial2.println(RWDbufferSize);
			Serial2.println("|_ Clearing sensor buffers...");
		#endif
	#endif
	buff.clear();
	revbuff.clear();

	drive.init();
	line.init();
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
	// Basic Mode
	// -> Read tracking info to prepare for motion
	// -> Detect if pattern has changed
	// -> Pattern has changed then increment done blocks
	// -> Check if we've travelled the required distance
	// -> Calculate wheel speed based on tracking info
	// -> Check if in motion or not
	// -> Update wheel speed / begin motion
	blocksTravelled = -1;
	lastPattern = 0;
	buff.clear();
	while (blocksTravelled < blocks) {

		// Check to see if on a target
		//if (digitalRead(REEDpin) == 1) {
		//	blocksTravelled = blocks + 1;
		//	Serial1.println("I found a target!");
		//}

		// push tracking info to the ring buffer
		buff.addValue( line.tracking(verbose, debug) );

		// store non-buffered pattern info from array
		currPattern = line.getPrevPatternSide();

		if (currPattern != lastPattern) {
			if (currPattern == 1 || currPattern == 2 && lastPattern == 0 || lastPattern == 1 || lastPattern == 2) {
				blocksTravelled++;
			}
			lastPattern = currPattern;
		}

		// set buffered position info to ring buffer average
		currPosition = buff.getAverage();

		// no correction required (centred on line)
		if (currPosition == 0) {
			// forwards full speed
			inMotion = 1;
			drive.forward(verbose, debug, 1, (BASE_SPEED*k100));
		}
		else if (currPosition > 0) {
			// correction left (deviation right)
			if (currPosition <= 2) {
				// small correction left
				// -> check if in motion or not
				if (inMotion == 0) {
					// -> drive right motor slowly
					drive.forward(verbose, debug, 2, (BASE_SPEED*k50));
				}
				else {
					// -> slow left motor
					drive.forward(verbose, debug, 3, (BASE_SPEED*k90));
				}

			}
			else if (currPosition <= 4 && currPosition > 2) {
				// small-medium correction left
				// -> check if in motion or not
				if (inMotion == 0) {
					// -> drive right motor slowly
					drive.forward(verbose, debug, 2, (BASE_SPEED*k50));
				}
				else {
					// -> slow left motor
					drive.forward(verbose, debug, 3, (BASE_SPEED*k80));
				}

			}
			else if (currPosition <= 6 && currPosition > 4) {
				// medium-large correction left
				// -> check if in motion or not
				if (inMotion == 0) {
					// -> drive right motor slowly
					drive.forward(verbose, debug, 2, (BASE_SPEED*k50));
				}
				else {
					// -> slow left motor
					drive.forward(verbose, debug, 3, (BASE_SPEED*k70));
				}

			}
			else {
				// large correction left
				// -> check if in motion or not
				if (inMotion == 0) {
					// -> drive right motor slowly
					drive.forward(verbose, debug, 2, (BASE_SPEED*k50));
				}
				else {
					// -> slow left motor
					drive.forward(verbose, debug, 3, (BASE_SPEED*k60));
				}

			}
		}
		else if (currPosition < 0) {
			// correction right required (deviation left)
			if (currPosition >= -2) {
				// small correction right
				// -> check if in motion or not
				if (inMotion == 0) {
					// -> drive left motor slowly
					drive.forward(verbose, debug, 3, (BASE_SPEED*k50));
				}
				else {
					// -> slow right motor
					drive.forward(verbose, debug, 2, (BASE_SPEED*k90));
				}

			}
			else if (currPosition >= -4 && currPosition < -2 ) {
				// small correction right
				// -> check if in motion or not
				if (inMotion == 0) {
					// -> drive left motor slowly
					drive.forward(verbose, debug, 3, (BASE_SPEED*k50));
				}
				else {
					// -> slow right motor
					drive.forward(verbose, debug, 2, (BASE_SPEED*k80));
				}

			}
			else if (currPosition >= -6 && currPosition < -4 ) {
				// small correction right
				// -> check if in motion or not
				if (inMotion == 0) {
					// -> drive left motor slowly
					drive.forward(verbose, debug, 3, (BASE_SPEED*k50));
				}
				else {
					// -> slow right motor
					drive.forward(verbose, debug, 2, (BASE_SPEED*k70));
				}

			}
			else {
				// large correction right
				// -> check if in motion or not
				if (inMotion == 0) {
					// -> drive left motor slowly
					drive.forward(verbose, debug, 3, (BASE_SPEED*k50));
				}
				else {
					// -> slow right motor
					drive.forward(verbose, debug, 2, (BASE_SPEED*k60));
				}
			}
		}
	}

	inMotion = 0;
	drive.stop(verbose, debug, 1);

	currPosition = line.tracking(verbose, debug);
	while (currPosition != 0) {
		currPosition = line.tracking(verbose, debug);

		if (currPosition > 0) {
			drive.stop(verbose, debug, 1);
			drive.forward(verbose, debug, 2, (BASE_SPEED*k50));
		}
		if (currPosition < 0) {
			drive.stop(verbose, debug, 1);
			drive.forward(verbose, debug, 3, (BASE_SPEED*k50));
		}
	}

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

	// -> Read tracking info to prepare for motion
	// -> Detect if pattern has changed
	// -> Pattern has changed then increment done blocks
	// -> Check if we've travelled the required distance
	// -> Calculate wheel speed based on tracking info
	// -> Check if in motion or not
	// -> Update wheel speed / begin motion
	blocksTravelled = -1;
	lastPattern = 0;
	revbuff.clear();
	while (blocksTravelled < blocks) {

		revbuff.addValue( line.tracking(verbose, debug) );
		currPosition = revbuff.getAverage();
		
		currPattern = line.getPatternSide();

		if (currPattern != lastPattern) {
			if (currPattern == 1 || currPattern == 2 && lastPattern == 0 || lastPattern == 1 || lastPattern == 2) {
				blocksTravelled++;
			}
			lastPattern = currPattern;
		}

		if (currPosition == 0) {
			// backwards full speed
			inMotion = 1;
			drive.backward(verbose, debug, 1, (BASE_SPEED*k50));
		}
		else if (currPosition > 0) {
			//correction left
			drive.stop(verbose, debug, 1);
			while (currPosition != 0) {

				revbuff.addValue( line.tracking(verbose, debug) );
				currPosition = revbuff.getAverage();

				drive.backward(verbose, debug, 3, (BASE_SPEED*k50));
			}
			drive.stop(verbose, debug, 1);
		}
		else if (currPosition < 0) {
			// correction right
			drive.stop(verbose, debug, 1);
			while (currPosition != 0) {

				revbuff.addValue( line.tracking(verbose, debug) );
				currPosition = revbuff.getAverage();

				drive.backward(verbose, debug, 2, (BASE_SPEED*k50));
			}
			drive.stop(verbose, debug, 1);
		}

		drive.stop(verbose, debug, 1);
	}

	currPosition = line.tracking(verbose, debug);

	while (currPosition != 0) {
		currPosition = line.tracking(verbose, debug);

		if (currPosition > 0) {
			drive.stop(verbose, debug, 1);
			drive.forward(verbose, debug, 2, (BASE_SPEED*k30));
		}
		if (currPosition < 0) {
			drive.stop(verbose, debug, 1);
			drive.forward(verbose, debug, 3, (BASE_SPEED*k30));
		}

		drive.stop(verbose, debug, 1);
	}

inMotion = 0;
return true;
}

bool Navigation::turnRight(int verbose, int debug) {
	if (verbose >= 3) Serial.println("Nav Right");

		// -> Left motor FWD, right motor BKD
		// -> update tracking info
		// -> When position = 0 stop
		// -> read tracking info
		// -> get to 0

	while (line.readPatternSide(verbose, debug) != 1 && line.readPatternSide(verbose, debug) != 2){
		drive.forward(verbose, debug, 1, TURN_SPEED);
	}

	drive.right(verbose, debug, TURN_SPEED);

	delay(LR_TURN_DELAY);

	//while (line.tracking(verbose, debug) != 0) {
	//}

	while (line.readPatternSide(verbose, debug) == 3 || line.readPatternSide(verbose, debug) == 4){
	}

	drive.stop(verbose, debug, 1);

	return true;
}

bool Navigation::turnLeft(int verbose, int debug) {
	if (verbose >= 3) Serial.println("Nav Left");

		// -> Left motor FWD, right motor BKD
		// -> update tracking info
		// -> When position = 0 stop
		// -> read tracking info
		// -> get to 0
	while (line.readPatternSide(verbose, debug) != 1 && line.readPatternSide(verbose, debug) != 2){
		drive.forward(verbose, debug, 1, TURN_SPEED);
	}

	drive.left(verbose, debug, TURN_SPEED);

	delay(LR_TURN_DELAY);

	//while (line.tracking(verbose, debug) != 0) {
	//}

	while (line.readPatternSide(verbose, debug) == 3 || line.readPatternSide(verbose, debug) == 4){
	}

	drive.stop(verbose, debug, 1);

	return true;
}

bool Navigation::targetEngage(int verbose, int debug) {

			
	while (digitalRead(REEDpin) != 0) {
	drive.forward(verbose, debug, 1, BASE_SPEED*k60);
		}

	drive.stop(verbose, debug, 1);

	currPosition = line.tracking(verbose, debug);
	while (currPosition != 0) {
		currPosition = line.tracking(verbose, debug);

		if (currPosition > 0) {
			drive.stop(verbose, debug, 1);
			drive.forward(verbose, debug, 2, (BASE_SPEED*k50));
		}
		if (currPosition < 0) {
			drive.stop(verbose, debug, 1);
			drive.forward(verbose, debug, 3, (BASE_SPEED*k50));
		}
	}

	drive.stop(verbose, debug, 1);

	return true;
}

bool Navigation::targetRetract(int verbose, int debug) {
		while (line.readPatternSide(verbose, debug) != 1 && line.readPatternSide(verbose, debug) != 2){
		drive.backward(verbose, debug, 1, BASE_SPEED*k60);
	}
		delay(500);
		drive.stop(verbose, debug, 1);
		forward(verbose, debug, 0);

	return true;
}

//------------------------------------------------------------------//
// Aim for the moon; even if you miss you’ll land among the stars	//
//	-W. Clement Stone												//
//------------------------------------------------------------------//