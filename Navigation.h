//----------------------------------------------------------//
// File: Navigation.h										//
// Description: Navigation and locomotion abstraction class //
// Author: Jeremy Dalton									//
// Creation Date: 23/10/2012								//
//----------------------------------------------------------//

#include "RunningAverage.h"
#include "Drive.h"
#include "Line.h"
#include "Arduino.h"
#include "Config.h"

class Navigation {
public:
	Navigation();
	void fw();
	void init();
	bool forward(int verbose, int debug, int blocks, int targetDetect);
	bool backward(int verbose, int debug, int blocks);
	bool turnRight(int verbose, int debug);
	bool turnLeft(int verbose, int debug);
	bool targetEngage(int verbose, int debug);
	bool targetRetract(int verbose, int debug);

private:
	Drive drive;
	Line line;
	int blocksTravelled;
	int currPosition;
	int currPattern;
	int lastPattern;
	int inMotion;
};