#include "Target.h"
#include "Arduino.h"

#define version 0.1 //Software version

Target::Target() {

}

void Target::fw() {
	Serial.print("Targ: ");
	Serial.print(version);
	Serial.println();
}

void Target:: init(int verbose) {
	if (verbose >= 2) {
		Serial.println("Target init...");
	}
}