//------------------------------------------------------//
// File: Line.h											//
// Description: Line following sensor abstraction class //
// Author: Jeremy Dalton								//
// Creation Date: 23/10/2012							//
//------------------------------------------------------//

#include "Arduino.h"
#include "QTRSensors.h"


class Line {
public:
	Line();
	void fw();
	void init(int verbose, int debug);
	void readArray(int verbose, int debug);
	int readPattern(int verbose, int debug);
	int tracking(int verbose, int debug);

private:
	int pattern;
	int position;
	int prevPattern;
};