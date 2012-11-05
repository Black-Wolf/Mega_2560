//----------------------------------------------//
// File: Drive.h								//
// Description: Motor control abstraction class //
// Author: Jeremy Dalton                        //
// Creation Date: 23/10/2012                    //
//----------------------------------------------//

#include "Arduino.h"

class Drive {
public:
	Drive();
	void init(int verbose);
	void fw();
	void forward(int debug, int motor, int ramp);
	void backward(int debug, int motor, int ramp);

private:
	void rampUp(int maxSpeed, int motor);
	void rampDown(int minSpeed, int motor);

	int RMspe;	//current Right Motor Speed
	int RMdir;	//current Right Motor direction
	int RMpre;	//current Right Motor preset
	int LMspe;	//current Left Motor Speed
	int LMdir;	//current Left Motor direction
	int LMpre;	//current Left Motor preset
	int BMspe;	//current Both Motor Speed
	int rampArray[14]; //motor speed ramping array
};