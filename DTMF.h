#include "Arduino.h"

//DTMF class for reciept of DTMF code
class DTMF {
public:
	DTMF();
	void fw();
	void init();
	void getChar();
};