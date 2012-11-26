//----------------------------------------------//
// File: Mega_2560.ino                          //
// Description: System core for buggy mainboard //
// Author: Jeremy Dalton                        //
// Creation Date: 23/10/2012                    //
//----------------------------------------------//

#include "Navigation.h"
#include "DTMF.h"
#include "Target.h"

#define version 1.0		// Software version

#define verbose 3		// Verbose information output setting
#define debug 0			// Debug information output setting
#define development 0	// Testing mode for development purposes

int TCCReraser = 7;
int TCCRpre = 1;
int indexIn = 0;
int blocks = 0;
int frequency = 0;
char commandIn[7];
char charIn;
Navigation nav;
Target target;
DTMF dtmf;

void setup() {
	Serial.begin(9600);
	pinMode(13,OUTPUT);

	if (verbose >= 1) Serial.println("Buggy mainboard booting...");

	TCCR3B &= ~TCCReraser;		// Erase Timer 3 Pre-scale value
	TCCR4B &= ~TCCReraser;		// Erase Timer 4 Pre-scale value
	TCCR3B |= TCCRpre;			// Write 1 to Pre-scale. ~32Khz
	TCCR4B |= TCCRpre;			// Write 1 to Pre-scale. ~32Khz

	nav.init(verbose, debug);	// Initialise navigation class ready for use
	dtmf.init(verbose);			// Initialise DTMF class ready for use
	target.init(verbose);		// Initialise target analysis class ready for use

	if (verbose >= 1) Serial.println("-> Buggy mainboard ready <-");
}

void loop() {
	while (Serial.available() > 0) {
		charIn = Serial.read();

		if(debug >= 1) Serial.print(charIn);

		if(charIn == '#') {	// Command termination character
			switch (commandIn[0]) {
			case 'f':	// Go forward by xx blocks
				blocks = (10 * (commandIn[1] - 48)) + (commandIn[2] - 48);
				if(debug >= 1) Serial.println(blocks);

				if (nav.forward(verbose, debug, blocks) == true){
					Serial.println("D");
				}
				break;

			case 'b':	// Go back by xx blocks
				blocks = (10 * (commandIn[1] - 48)) + (commandIn[2] - 48);
				if(debug >= 1) Serial.println(blocks);

				if (nav.backward(verbose, debug, blocks) == true){
					Serial.println("D");
				}
				break;

			case 'l':	// Turn left

				if (nav.turnLeft(verbose, debug) == true){
					Serial.println("D");
				}
				break;

			case 'r':	// Turn right

				if (nav.turnRight(verbose, debug) == true){
					Serial.println("D");
				}
				break;

			case 'd':	// Recievie DTMF code
				if (development >= 1) Serial.println("*#*#*#*#A057C064D026B001D132D100A137*003*033*045*117*060*114*075*149#");

				// DTMF function goes here
				Serial.println("D");
				break;

			case 'a':	// Analyse target @ frequency
				if (development >= 1) Serial.println("FF4");

				frequency = (1000 * (commandIn[1] - 48)) + (100 * (commandIn[2] - 48)) + (10 * (commandIn[3] - 48)) + (commandIn[4] - 48);
				if(debug >= 1) Serial.println(frequency);

				// Analysis function goes in here
				Serial.println("D");
				break;

			case 'v':	// Perform victory roll

				for (int v = 0; v == 6; v++){
					nav.turnRight(verbose, debug);	// 2.5 full turns right
				}
				nav.forward(verbose,debug,1);		// forward 1 block
				for (int v = 0; v== 2; v++) {
					nav.turnLeft(verbose, debug);	// 0.5 full turns left
				}
				nav.forward(verbose, debug,1);		// forward 1 block back to start location

				Serial.println("D");
				break;

			case 'z':	// Print firmware revisions
				Serial.print("Bug: ");
				Serial.print(version);
				Serial.println();
				nav.fw();
				target.fw();
				dtmf.fw();
				Serial.println("D");
				break;

			default:	// Error case. Any non-valid command returns error
				Serial.println("Error!");
				for (int i = 0; i <= indexIn; i++) {
					Serial.print(commandIn[i]);
				}
				break;
			}
			indexIn = 0;
		}
		else {
			commandIn[indexIn] = charIn;
			indexIn++;
		}
	}
}