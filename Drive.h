//----------------------------------------------//
// File: Drive.h								//
// Description: Motor control abstraction class //
// Author: Jeremy Dalton                        //
// Creation Date: 23/10/2012                    //
//----------------------------------------------//

#include "Arduino.h"
#include "Config.h"

class Drive {
public:
	Drive();
	void init();
	void fw();
	void right(int verbose, int debug, int speed);
	void left(int verbose, int debug, int speed);
	void forward(int verbose, int debug, int motor, int speed);
	void backward(int verbose, int debug, int motor, int speed);
	void stop(int verbose, int debug, int motor);

private:
	void rampUp(int debug, int maxSpeed, int motor);
	void rampDown(int debug, int minSpeed, int motor);

	int RMspe;	//current Right Motor Speed
	int RMdir;	//current Right Motor direction
	int RMpre;	//current Right Motor preset
	int LMspe;	//current Left Motor Speed
	int LMdir;	//current Left Motor direction
	int LMpre;	//current Left Motor preset
	int BMspe;	//current Both Motor Speed
};