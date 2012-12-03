//------------------------------------------------------//
// File: Line.cpp										//
// Description: Line following sensor abstraction class //
// Author: Jeremy Dalton								//
// Creation Date: 23/10/2012							//
//------------------------------------------------------//

// This is pretty bad practice when it comes to witing a class
// but it's the only way that seems to work properly and not
// cause the first two sensors to not be read

#include "Line.h"
#include "QTRSensors.h"
#include "Arduino.h"

#define NUM_SENSORS		8		// number of sensors used
#define TIMEOUT			2500	// waits for 2500 us for sensor outputs to go low
#define EMITTER_PIN		22		// emitter is controlled by digital pin 2
#define THRESHOLD		500		// Line positioning threshold
#define CAL_LOOPS		400		// Calibration loops to perform
#define VERSION			1.0		// Software version

//int lineRead[NUM_SENSORS];
unsigned char PIN_NUMS[] = {23, 24, 25, 26, 27, 28, 29, 30};
unsigned int rawValues[NUM_SENSORS];
unsigned int calValues[NUM_SENSORS];

QTRSensorsRC QTR(PIN_NUMS, NUM_SENSORS, TIMEOUT, EMITTER_PIN);

Line::Line() 
{
	pattern = 0;
	prevPattern = 0;
}

void Line::fw() {
	Serial.print("Lin: ");
	Serial.print(VERSION);
	Serial.println();
}

void Line::init(int verbose, int debug) {
	if (verbose >= 3) Serial.println("|_ Line init...");

	int i = 0;
	digitalWrite(13, HIGH);

	if (verbose >= 1) Serial.println("|__ Sensor calibration starting...");
	for (i = 0; i < CAL_LOOPS; i++)  // make the calibration take about 10 seconds
	{
		QTR.calibrate();       // 400 reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
	}

	if (debug >= 3) {
		// print the calibration minimum values measured when emitters were on
		Serial.println("Calibrated Minimum: ");
		for (i = 0; i < NUM_SENSORS; i++)
		{
			Serial.print(QTR.calibratedMinimumOn[i]);
			Serial.print(' ');
		}
		Serial.println();

		// print the calibration maximum values measured when emitters were on
		Serial.println("Calibrated Maximum: ");
		for (i = 0; i < NUM_SENSORS; i++)
		{
			Serial.print(QTR.calibratedMaximumOn[i]);
			Serial.print(' ');
		}
		Serial.println();
	}
	if (verbose >= 1) Serial.println("|__ Sensor calibration complete");

	digitalWrite(13, LOW);
}

void Line::readArray(int verbose, int debug) {
	if (verbose >= 3) Serial.println("->Refreshing Sensor Data");
	QTR.read(rawValues);
	QTR.readCalibrated(calValues);

	if (debug >= 3) {
		int i = 0;
		// Having a look at the raw data from the array
		Serial.println("Raw Values: ");

		for (i = 0; i < NUM_SENSORS; i++)
		{
			Serial.print(rawValues[i]);
			Serial.print(' ');
		}

		Serial.println();
		Serial.println();

		// Having a look at the calibrated data from the array
		Serial.println("Calibrated Values");

		for (i = 0; i < NUM_SENSORS; i++)
		{
			Serial.print(calValues[i]* 10 / 1001);
			Serial.print(' ');
		}

		Serial.println();
		Serial.println();
	};
}

int Line::readPattern(int verbose, int debug) {
	if (verbose >= 3) Serial.println("->Determining Colour Pattern");

	if (calValues[0] <= THRESHOLD && calValues[7] >= THRESHOLD) 
	{
		if (debug >=3) Serial.println("Found Black - White Pattern");
		return 1;	// Black - White
	}
	else if (calValues[0] >= THRESHOLD && calValues[7] <= THRESHOLD)
	{
		if (debug >=3) Serial.println("Found White - Black Pattern");
		return 2;	// White - Black
	}
	else if (calValues[0] <= THRESHOLD && calValues[7] <= THRESHOLD)
	{
		if (debug >=3) Serial.println("Found White - White Pattern");
		return 3;	// White - White
	}
	else if (calValues[0] >= THRESHOLD && calValues[7] >= THRESHOLD)
	{
		if (debug >=3) Serial.println("Found Black - Black Pattern");
		return 4;	// Black - Black
	}

}

int Line::tracking (int verbose, int debug) {
	position = 0;

	readArray(verbose, debug); // update array data on every line tracking request

	pattern = readPattern(verbose, debug);	// determine the colour pattern under the sensor array

	if (pattern == 1 || pattern ==2) {
		prevPattern = pattern;	// if we see either 'on line' pattern store as previous pattern seen
	}

	for (int i = 0; i < NUM_SENSORS; i++)
	{
		if (calValues[i] <= THRESHOLD) // Is white
		{
			//lineRead[i] = 1; 
			position++;
		}
		else // Is black
		{
			//lineRead[i] = 0;	
			position--;
		}
	}

	//if (debug >= 4) {
	//	for (int i = 0; i < NUM_SENSORS; i++)
	//	{
	//		Serial.print(lineRead[i]);
	//		Serial.print(' ');
	//	}
	//}

	if (debug >= 3) {
		Serial.println();
		Serial.println("Previous Pattern:");
		Serial.println(prevPattern);
		Serial.println("Current Pattern:");
		Serial.println(pattern);
		Serial.println();
	}

	if (pattern == 1) // Black - White
	{
		position = position * 1;
	}
	else if (pattern == 2) // White - Black
	{
		position = position * -1;
	}
	else if (pattern == 3) // White - White
	{
		if (prevPattern == 1) {
			position = position * 1;	// If we see White - White but had Black - White
		}
		else if (prevPattern == 2) {
			position = position * -1;	// If we see White - White but had White - Black
		}
	}
	else if (pattern == 4) // Black - Black
	{
		if (prevPattern == 1) {
			position = position * 1;	// If we see Black - Black but had Black - White
		}
		else if (prevPattern == 2) {
			position = position * -1;	// If we see Black - Black but had White - Black
		}
	}

	return position;
}

int Line::getPattern() {
	return pattern;
}

int Line::getPrevPattern() {
	return prevPattern;
}


//----------------------------------------------------------//
// Two roads diverged in a wood and I - I took the one less	//
// traveled by, and that has made all the difference.		//
//	-Robert Frost											//
//----------------------------------------------------------//