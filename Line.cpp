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
#define TIMEOUT			1500	// waits for 1500 us for sensor outputs to go low
#define EMITTER_PIN		36		// emitter is controlled by digital pin 2
#define THRESHOLD		500		// Line positioning threshold
#define CAL_LOOPS		400		// Calibration loops to perform
#define VERSION			0.5		// Software version

#define WHITE			29		// White side sensor IO pin
#define GREEN			27		// Green side sensor IO pin

//int lineRead[NUM_SENSORS];
unsigned char PIN_NUMS[] = {38, 42, 40, 44, 46, 48, 50, 52};
unsigned int rawValues[NUM_SENSORS];
unsigned int calValues[NUM_SENSORS];

QTRSensorsRC QTR(PIN_NUMS, NUM_SENSORS, TIMEOUT, EMITTER_PIN);

Line::Line()
{
	pattern = 0;
	prevPattern = 0;
}

void Line::fw() {
	#if LAPTOP_CONTROL
		Serial.print("Lin: ");
		Serial.print(VERSION);
		Serial.println();
	#else
		Serial2.print("Lin: ");
		Serial2.print(VERSION);
		Serial2.println();
	#endif
}

void Line::init() {
	#if VERBOSE_BOOT
		#if LAPTOP_CONTROL
			Serial.println("|_ Line init...");
		#else
			Serial2.println("|_ Line init...");
		#endif
	#endif
	
	pinMode(WHITE, INPUT);
	pinMode(GREEN, INPUT);

	#if CALIBRATION_ENABLED
		int i = 0;
		digitalWrite(13, HIGH);

		#if VERBOSE_BOOT
			#if LAPTOP_CONTROL
				Serial.println("|__ Sensor calibration starting...");
				for (i = 0; i < CAL_LOOPS; i++) // make the calibration take about 5 seconds
				{
					QTR.calibrate(); // 400 reads all sensors 10 times at 1500 us per read (i.e. ~15 ms per call)
				}

				#if LINE_DEBUG
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
				#endif		
				Serial.println("|__ Sensor calibration complete");
				digitalWrite(13, LOW);
			#else
				Serial2.println("|__ Sensor calibration starting...");
				for (i = 0; i < CAL_LOOPS; i++) // make the calibration take about 5 seconds
				{
					QTR.calibrate(); // 400 reads all sensors 10 times at 1500 us per read (i.e. ~15 ms per call)
				}

				if (debug >= 3) {
					// print the calibration minimum values measured when emitters were on
					Serial2.println("Calibrated Minimum: ");
					for (i = 0; i < NUM_SENSORS; i++)
					{
						Serial2.print(QTR.calibratedMinimumOn[i]);
						Serial2.print(' ');
					}
					Serial2.println();

					// print the calibration maximum values measured when emitters were on
					Serial2.println("Calibrated Maximum: ");
					for (i = 0; i < NUM_SENSORS; i++)
					{
						Serial2.print(QTR.calibratedMaximumOn[i]);
						Serial2.print(' ');
					}
					Serial2.println();
					}		
				Serial2.println("|__ Sensor calibration complete");
				digitalWrite(13, LOW);
			#endif
		#endif
	#else
		Serial.println("|__ Sensor calibration DISABLED");
	#endif
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

int Line::readPatternSide(int verbose, int debug) {
	if (verbose >= 3) Serial.println("->Determining Side Colour Pattern");

	if (digitalRead(WHITE) == 1 && digitalRead(GREEN) == 0)
	{
		if (debug >=3) Serial.println("Found Black - White Pattern");
		return 1;	// Black - White
	}
	else if (digitalRead(WHITE) == 0 && digitalRead(GREEN) == 1)
	{
		if (debug >=3) Serial.println("Found White - Black Pattern");
		return 2;	// White - Black
	}
	else if (digitalRead(WHITE) == 1 && digitalRead(GREEN) == 1)
	{
		if (debug >=3) Serial.println("Found White - White Pattern");
		return 3;	// White - White
	}
	else if (digitalRead(WHITE) == 0 && digitalRead(GREEN) == 0)
	{
		if (debug >=3) Serial.println("Found Black - Black Pattern");
		return 4;	// Black - Black
	}

}

int Line::tracking (int verbose, int debug) {
	position = 0;

	readArray(verbose, debug); // update array data on every line tracking request

	pattern = readPattern(verbose, debug);	// determine the colour pattern under the sensor array
	patternSide = readPatternSide(verbose, debug); //determine colour pattern on axle axis

	if (pattern == 1 || pattern == 2) {
		prevPattern = pattern;	// if we see either 'on line' pattern store as previous pattern seen
	}
	if (patternSide == 1 || patternSide == 2) {
		prevPatternSide = patternSide;	// if we see either 'on line' pattern store as previous pattern seen
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
	// for (int i = 0; i < NUM_SENSORS; i++)
	// {
	// Serial.print(lineRead[i]);
	// Serial.print(' ');
	// }
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

int Line::getPatternSide() {
	return patternSide;
}

int Line::getPrevPatternSide() {
	return prevPatternSide;
}


//----------------------------------------------------------//
// Two roads diverged in a wood and I - I took the one less //
// traveled by, and that has made all the difference.		//
// -Robert Frost											//
//----------------------------------------------------------//