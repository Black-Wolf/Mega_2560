#include "DTMF.h"
#include "Arduino.h"

#define version 0.1 //Software version

DTMF::DTMF() {

}

void DTMF::fw() {
	Serial.print("DTMF: ");
	Serial.print(version);
	Serial.println();
}

void DTMF:: init(int verbose) {
	if (verbose >= 2) {
		Serial.println("DTMF init...");
	}
}