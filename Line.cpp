//------------------------------------------------------//
// File: Line.cpp										//
// Description: Line following sensor abstraction class //
// Author: Jeremy Dalton								//
// Creation Date: 23/10/2012							//
//------------------------------------------------------//

#include "Line.h"
#include "QTRSensors.h"
#include "Arduino.h"

#define version 0.1	// Software version

Line::Line() {

}

void Line::fw() {
	Serial.print("Lin: ");
	Serial.print(version);
	Serial.println();
}

void Line::init(int verbose) {
	if (verbose >= 2) Serial.println("-> line init");

}

//----------------------------------------------------------//
// Two roads diverged in a wood and I - I took the one less	//
// traveled by, and that has made all the difference.		//
//	-Robert Frost											//
//----------------------------------------------------------//