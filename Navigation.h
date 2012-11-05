//----------------------------------------------------------//
// File: Navigation.h										//
// Description: Navigation and locomotion abstraction class //
// Author: Jeremy Dalton									//
// Creation Date: 23/10/2012								//
//----------------------------------------------------------//

#include "Drive.h"
#include "Line.h"
#include "Arduino.h"

class Navigation {
public:
	Navigation();
	void fw();
	void init(int verbose);
	bool forward(int verbose, int debug, int blocks);
	bool backward(int verbose, int debug, int blocks);
	bool turnRight(int verbose, int debug);
	bool turnLeft(int verbose, int debug);

private:
	Drive drive;
	Line line;
	int blocks;
};