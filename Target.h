#include "RunningAverage.h"
#include "StopWatch.h"
#include "Wire.h"
#include "Arduino.h"
#include "Config.h"

class Target {
public:
	Target();
	void fw();
	void init();
	void analyse(char target, int direction);

private:
	float T3maths (long reading, long reference);
	float T4maths (long reading);
	float T7Rmaths (long reading);
	float TRmaths (long reading);
	float TCmaths (long time);

	void returnValue(float value, byte width);
	float capValue(float reading);
	long resistorValue(float reading);
	float zennerValue(long reading);
	long ADCtoMV(long ADCval);

	void preheat();
	void flushBuffers();

	void GPIOinit();
	void GPIOtest();
	void GPIOon(int maskA, int maskB);
	void GPIOoff(int maskA, int maskB);
};