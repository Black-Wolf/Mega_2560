//----------------------------------------------------------//
// File: Navigation.cpp										//
// Description: Navigation and locomotion abstraction class //
// Author: Jeremy Dalton									//
// Creation Date: 23/10/2012								//
//----------------------------------------------------------//

#include "Navigation.h"
#include "Arduino.h"

#define version 0.1	// Software version

Navigation::Navigation() {

}

void Navigation::fw() {
	Serial.print("Nav: ");
	Serial.print(version);
	Serial.println();
	drive.fw();
	line.fw();
}

void Navigation::init(int verbose) {
	if (verbose >= 2) Serial.println("Navigation init...");
	drive.init(verbose);
	line.init(verbose);
	blocks = 0;
}

//Motor select options:		|	Ramp select options:
//1=> Both only				|	1=> Up
//2=> Right only			|	2=> Down
//3=> Left only				|

bool Navigation::forward(int verbose, int debug, int blocks) {

	return true;
}

bool Navigation::backward(int verbose, int debug, int blocks) {

	return true;
}

bool Navigation::turnRight(int verbose, int debug) {

	return true;
}

bool Navigation::turnLeft(int verbose, int debug) {

	return true;
}

//------------------------------------------------------------------//
// Aim for the moon; even if you miss you’ll land among the stars	//
//	-W. Clement Stone												//
//------------------------------------------------------------------//