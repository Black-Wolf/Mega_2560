//----------------------------------------------------------//
// File: Beacon.h											//
// Description: Control class for the beacon, tidies main	//
// Author: Jeremy Dalton									//
// Creation Date: 05/02/2013								//
//----------------------------------------------------------//

#include "Arduino.h"
#include "Config.h"

class Beacon {
public:
	Beacon();
	void fw();
	void init();
	void pulse();
	void right();
	void left();
	void active();
};