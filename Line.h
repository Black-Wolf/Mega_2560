//------------------------------------------------------//
// File: Line.h											//
// Description: Line following sensor abstraction class //
// Author: Jeremy Dalton								//
// Creation Date: 23/10/2012							//
//------------------------------------------------------//

#include "QTRSensors.h"
#include "Arduino.h"
#include "Config.h"

class Line {
public:
Line();
void fw();
void init();
void readArray(int verbose, int debug);
int readPattern(int verbose, int debug);
int readPatternSide(int verbose, int debug);
int tracking(int verbose, int debug);
int getPattern();
int getPrevPattern();
int getPatternSide();
int getPrevPatternSide();

private:
int pattern;
int position;
int prevPattern;
int patternSide;
int prevPatternSide;
};