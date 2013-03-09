//----------------------------------------------//
// File: Mega_2560.ino                          //
// Description: System core for buggy mainboard //
// Author: Jeremy Dalton                        //
// Creation Date: 23/10/2012                    //
//----------------------------------------------//

#include "Wire.h"
#include "Navigation.h"
#include "DTMF.h"
#include "Target.h"
#include "Beacon.h"
#include "Arduino.h"
#include "Config.h"

#define version 1.5		// Software version

#define verbose 2		// Verbose information output setting
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
Beacon beacon;

void setup() {
	Serial.begin(115200);
	Serial2.begin(115200);
	pinMode(13,OUTPUT);

	if (verbose >= 1) Serial.println("Buggy mainboard booting...");
	if (verbose >= 1) Serial2.println("Buggy mainboard booting...");

	TCCR3B &= ~TCCReraser;		// Erase Timer 3 Pre-scale value
	TCCR4B &= ~TCCReraser;		// Erase Timer 4 Pre-scale value
	TCCR3B |= TCCRpre;			// Write 1 to Pre-scale. ~32Khz
	TCCR4B |= TCCRpre;			// Write 1 to Pre-scale. ~32Khz

	nav.init();	// Initialise navigation class ready for use
	dtmf.init();			// Initialise DTMF class ready for use
	target.init();		// Initialise target analysis class ready for use
	beacon.init();		// Initialise beacon control class ready for use

	if (verbose >= 1) Serial.println("-> Buggy mainboard ready <-");
	if (verbose >= 1) Serial2.println("-> Buggy mainboard ready <-");
}

void loop() {
	#if LAPTOP_CONTROL
		while (Serial.available() > 0) {
			charIn = Serial.read();
	#else
		while (Serial2.available() > 0) {
			charIn = Serial2.read();
	#endif

	#if MAIN_DEBUG 
		Serial.print(charIn);
	#endif

		if(charIn == '#') {	// Command termination character
			switch (commandIn[0]) {
			case 'f':	// Go forward by xx blocks
				blocks = (10 * (commandIn[1] - 48)) + (commandIn[2] - 48);
				#if MAIN_DEBUG  
					Serial.println(blocks);
				#endif

				#if MOVEMENT_ENABLED
					beacon.active();
					if (nav.forward(verbose, debug, blocks) == true){
						#if LAPTOP_CONTROL 
							Serial.println("D");
						#else 
							Serial2.println("D");
						#endif
					beacon.pulse();
					}
				#else
					#if LAPTOP_CONTROL 
						Serial.println("D");
					#else 
						Serial2.println("D");
					#endif
				#endif
				break;

			case 'b':	// Go back by xx blocks
				blocks = (10 * (commandIn[1] - 48)) + (commandIn[2] - 48);
				#if MAIN_DEBUG  
					Serial.println(blocks);
				#endif

				#if MOVEMENT_ENABLED
					beacon.active();
					if (nav.backward(verbose, debug, blocks) == true){
						#if LAPTOP_CONTROL 
							Serial.println("D");
						#else 
							Serial2.println("D");
						#endif
					beacon.pulse();
					}
				#else
					#if LAPTOP_CONTROL 
						Serial.println("D");
					#else 
						Serial2.println("D");
					#endif
				#endif
				break;

			case 'l':	// Turn left
				#if MOVEMENT_ENABLED
					beacon.left();
					if (nav.turnLeft(verbose, debug) == true){
						#if LAPTOP_CONTROL 
							Serial.println("D");
						#else 
							Serial2.println("D");
						#endif
					beacon.pulse();
					}
				#else
					#if LAPTOP_CONTROL 
						Serial.println("D");
					#else 
						Serial2.println("D");
					#endif
				#endif
				break;

			case 'r':	// Turn right
				#if MOVEMENT_ENABLED
					beacon.right();
					if (nav.turnRight(verbose, debug) == true){
						#if LAPTOP_CONTROL 
							Serial.println("D");
						#else 
							Serial2.println("D");
						#endif
					beacon.pulse();
					}
				#else
					#if LAPTOP_CONTROL 
						Serial.println("D");
					#else 
						Serial2.println("D");
					#endif
				#endif
				break;

			case 'd':	// Initiate DTMF code
				if (development >= 1) Serial1.println("*#*#*#*#A057C064D026B001D132D100A137*003*033*045*117*060*114*075*149#");
				beacon.active();

				attachInterrupt(4, callGetChar, RISING);
				break;

			case 't':	// Terminate DTMF reception
				detachInterrupt(4);
				#if LAPTOP_CONTROL 
					Serial.println("D");
				#else 
					Serial2.println("D");
				#endif
				beacon.pulse();
				break;

			case 'a':	// Analyse target
				beacon.active();

				#if MOVEMENT_ENABLED
					nav.targetEngage(verbose, debug);
				#endif

				target.analyse(commandIn[1],0);

				#if MOVEMENT_ENABLED
					nav.targetRetract(verbose, debug);
				#endif

				#if LAPTOP_CONTROL 
					Serial.println("D");
				#else 
					Serial2.println("D");
				#endif
				beacon.pulse();
				break;

			case 'q':	// Analyse target from reverse direction);
				beacon.active();

				#if MOVEMENT_ENABLED
					nav.targetEngage(verbose, debug);
				#endif

				target.analyse(commandIn[1],1);

				#if MOVEMENT_ENABLED
					nav.targetRetract(verbose, debug);
				#endif

				#if LAPTOP_CONTROL 
					Serial.println("D");
				#else 
					Serial2.println("D");
				#endif
				beacon.pulse();
				break;

			case 'v':	// Perform victory roll
				#if MOVEMENT_ENABLED
					beacon.active();
					for (int v = 0; v < 6; v++){
						nav.turnRight(verbose, debug);	// 2.5 full turns right
					}
					nav.forward(verbose,debug,1);		// forward 1 block
					for (int v = 0; v < 2; v++) {
						nav.turnLeft(verbose, debug);	// 0.5 full turns left
					}
					nav.forward(verbose, debug,1);		// forward 1 block back to start location
				
					#if LAPTOP_CONTROL 
						Serial.println("D");
					#else 
						Serial2.println("D");
					#endif

					beacon.pulse();
				#else
					#if LAPTOP_CONTROL 
						Serial.println("D");
					#else 
						Serial2.println("D");
					#endif
				#endif
				break;

			case 'z':	// Print firmware revisions
				beacon.active();
				Serial.print("Bug: ");
				Serial.print(version);
				Serial.println();
				nav.fw();
				target.fw();
				dtmf.fw();
				beacon.fw();
				#if LAPTOP_CONTROL
					Serial.println("D");
				#else 
					Serial2.println("D");
				#endif
				beacon.pulse();
				break;

			default:	// Error case. Any non-valid command returns error
				#if LAPTOP_CONTROL 
					Serial.println("Error!");
					for (int i = 0; i <= indexIn; i++) {
						Serial.print(commandIn[i]);
					}
					Serial.println();
				#else
					Serial2.println("Error!");
					for (int i = 0; i <= indexIn; i++) {
						Serial2.print(commandIn[i]);
					}
					Serial2.println();
				#endif
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

void callGetChar() {
	dtmf.getChar();
}