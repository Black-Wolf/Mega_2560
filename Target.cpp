#include "Target.h"
#include "Arduino.h"

#define version 2.0 //Software version
#define bufferSize 20 //Size for ring buffers

int portA;
int portB;

RunningAverage ref1(bufferSize);
RunningAverage ref2(bufferSize);
RunningAverage read1(bufferSize);
RunningAverage read2(bufferSize);
StopWatch timer(StopWatch::MICROS);

Target::Target() {
	portA = B00000000;
	portB = B00000000;

	flushBuffers();

	pinMode(A2, INPUT);
}

void Target::fw() {
	Serial.print("Targ: ");
	Serial.print(version);
	Serial.println();
}

void Target::init() {
	#if VERBOSE_BOOT
		Serial.println("Target init...");
		Serial.println("|_ MCP23017 config...");
		GPIOinit();
		Serial.print("|_ Sample buffers length: ");
		Serial.println(bufferSize);
	#else
		GPIOinit();
	#endif
}

void Target::preheat() {
	// 10v source resistors to ground via 3M3 ground resistance
	// *** DO NOT de-select ground resistance ***
	#if TARGET_DEBUG
	Serial.println("Begining source resistance preheat...");
	#endif

	#if TARGET_DEBUG
	Serial.println("Source resistance prehat complete");
	#endif
}

void Target::analyse(char target, int direction){

	//----------------------------------------------------------//
	// Relay Channel	Purpose				Port A		Port B
	// 1				GND -> Purple		0x000000	
	// 2				ADC - > Green		00x00000	
	// 3				ADC -> Yellow		000x0000	
	// 4				ADC -> Purple		0000x000	
	// 5				10v -> Green		00000x00	
	// 6				10v -> Yellow		000000x0	
	// 7				10v -> Purple		0000000x	
	// 8				s_GND -> Pass / 3M3				0x000000
	// 9				s_10v -> R1 / Pass				00x00000
	// 10				s_10v -> R2 / Pass				000x0000
	// 11				s_10v -> R3 / R4				0000x000
	// 12				Changeover						00000x00
	// 13				GND -> Green					000000x0
	// 14				GND -> Yellow					0000000x
	//----------------------------------------------------------//

	switch(target) {
		//----------//
		// Target 1 //
		//----------//
	case '1':
		if (direction == 0) {
			//GPIOon(B00001100,B01010001);

			flushBuffers();

			for(int i=1 ; i<=bufferSize ;i++) {

				// P1->10v[R2 - 1K] GND->GND[low Z] P2->ADC
				GPIOon(B00001100,B01010001);
				delay(25);
				read1.addValue(analogRead(2)+9);
				GPIOoff(B00000000,B00000000);
				delay(25);

			#if TARGET_DEBUG
				Serial.println(i);
			#endif;
			}

			#if TARGET_DEBUG
			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("Component: ");
			Serial.print( zennerValue((ADCtoMV(read1.getAverage()) + 1000)) );
			Serial.println("V");
			#endif
			returnValue( ADCtoMV(read1.getAverage()), 6);
			returnValue( zennerValue((ADCtoMV(read1.getAverage()) + 1000)), 6);
		}
		else {
			//GPIOon(B00001100,B01010101);
			flushBuffers();

			for(int i=1 ; i<=bufferSize ;i++) {

				// P1->10v[R2 - 1K] GND->GND[low Z] P2->ADC
				GPIOon(B00001100,B01010101);
				delay(25);
				read1.addValue(analogRead(2)+9);
				GPIOoff(B00000000,B00000000);
				delay(25);

			#if TARGET_DEBUG
				Serial.println(i);
			#endif;
			}

			#if TARGET_DEBUG
			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("Component: ");
			Serial.print( zennerValue((ADCtoMV(read1.getAverage()) + 1000)) );
			Serial.println("V");
			#endif
			returnValue( ADCtoMV(read1.getAverage()), 6);
			returnValue( zennerValue((ADCtoMV(read1.getAverage()) + 1000)), 6);
		}
		break;

		//----------//
		// Target 2 //
		//----------//
	case '2':
		if (direction == 0) {
			flushBuffers();

			for(int i=1 ; i<=bufferSize ;i++) {
				// P1->10v[R2 - 1K] GND-> OPEN P2->GND[low Z] ADC->P1
				GPIOon(B01100100, B01010000);
				delay(25);
				read1.addValue(analogRead(2)+16);
				delay(25);
				GPIOoff(B00000000,B00000000);
				delay(25);

				// P1->OPEN GND->10v[R2 - 1K] ADC->P2 P2->GND [low Z]
				GPIOon(B01010010, B01010000);
				delay(25);
				read2.addValue(analogRead(2)+16);
				delay(25);
				GPIOoff(B00000000,B00000000);
				delay(25);

			#if TARGET_DEBUG
				Serial.println(i);
			#endif;
			}

			#if TARGET_DEBUG
			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("Ra: ");
			Serial.print( TRmaths(ADCtoMV(read1.getAverage())) );
			Serial.println(" Ohms");

			Serial.print("Ra E24: ");
			Serial.print( resistorValue( TRmaths(ADCtoMV(read1.getAverage())) ) );
			Serial.println(" Ohms");

			Serial.print("Reading 2: ");
			Serial.print(ADCtoMV(read2.getAverage()));
			Serial.println("mV");

			Serial.print("Rb: ");
			Serial.print( TRmaths(ADCtoMV(read2.getAverage())) );
			Serial.println(" Ohms");

			Serial.print("Rb E24: ");
			Serial.print( resistorValue( TRmaths(ADCtoMV(read2.getAverage() ) ) ) );
			Serial.println(" Ohms");
			#endif
			returnValue( resistorValue(TRmaths(ADCtoMV(read1.getAverage()))), 6);
			returnValue( resistorValue(TRmaths(ADCtoMV(read2.getAverage()))), 6);
			returnValue( TRmaths(ADCtoMV(read1.getAverage())), 6);
			returnValue( TRmaths(ADCtoMV(read2.getAverage())), 6);
		}
		else {
			flushBuffers();

			for(int i=1 ; i<=bufferSize ;i++) {
				// P1->10v[R2 - 1K] GND-> OPEN P2->GND[low Z] ADC->P1
				GPIOon(B01100100, B01010100);
				delay(25);
				read1.addValue(analogRead(2)+16);
				delay(25);
				GPIOoff(B00000000,B00000000);
				delay(25);

				// P1->OPEN GND->10v[R2 - 1K] ADC->P2 P2->GND [low Z]
				GPIOon(B01010010, B01010100);
				delay(25);
				read2.addValue(analogRead(2)+16);
				delay(25);
				GPIOoff(B00000000,B00000000);
				delay(25);

			#if TARGET_DEBUG
				Serial.println(i);
			#endif;
			}

			#if TARGET_DEBUG
			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("Ra: ");
			Serial.print( TRmaths(ADCtoMV(read1.getAverage())) );
			Serial.println(" Ohms");

			Serial.print("Ra E24: ");
			Serial.print( resistorValue( TRmaths(ADCtoMV(read1.getAverage())) ) );
			Serial.println(" Ohms");

			Serial.print("Reading 2: ");
			Serial.print(ADCtoMV(read2.getAverage()));
			Serial.println("mV");

			Serial.print("Rb: ");
			Serial.print( TRmaths(ADCtoMV(read2.getAverage())) );
			Serial.println(" Ohms");

			Serial.print("Rb E24: ");
			Serial.print( resistorValue( TRmaths(ADCtoMV(read2.getAverage() ) ) ) );
			Serial.println(" Ohms");
			#endif
			returnValue( resistorValue(TRmaths(ADCtoMV(read1.getAverage()))), 6);
			returnValue( resistorValue(TRmaths(ADCtoMV(read2.getAverage()))), 6);
			returnValue( TRmaths(ADCtoMV(read1.getAverage())), 6);
			returnValue( TRmaths(ADCtoMV(read2.getAverage())), 6);
		}
		break;

		//----------//
		// Target 3 //
		//----------//
	case '3':
		if (direction == 0) {
			flushBuffers();

			for(int i=1 ; i<=bufferSize ;i++) {
				// Reference voltage
				// P2->10v[R2 - 1K] + ADC GND-> GND[low Z] P1->OPEN
				GPIOon(B00001001,B01010001);
				delay(50);
				ref1.addValue(analogRead(2));
				delay(50);
				GPIOoff(B00000000,B00000000);
				delay(50);

				//First Measurement
				// P2->10v[R2 - 1K] GND->GND[low Z] P1->ADC
				GPIOon(B00100001,B01010001);
				delay(50);
				read1.addValue(analogRead(2)+6);
				delay(50);
				GPIOoff(B00000000,B00000000);
				delay(50);

				// Reference voltage 2
				// P2->10v[R2 - 1K] + ADC GND->OPEN P1->GND[low Z]
				GPIOon(B00001001,B01010010);
				delay(50);
				ref2.addValue(analogRead(2));
				delay(50);
				GPIOoff(B00000000,B00000000);
				delay(50);

				// Second Measurement
				// P2->10v[R2 - 1K] GND->ADC P1->GND[low Z]
				GPIOon(B00010001,B01010010);
				delay(50);
				read2.addValue(analogRead(2)+6);
				delay(50);
				GPIOoff(B00000000,B00000000);
				delay(50);

			#if TARGET_DEBUG
				Serial.println(i);
			#endif
			}

			#if TARGET_DEBUG
			Serial.print("Ref 1: ");
			Serial.print(ADCtoMV(ref1.getAverage()));
			Serial.println("mV");

			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("Ra: ");
			Serial.print( T3maths(ADCtoMV(read1.getAverage()), ADCtoMV(ref1.getAverage()) ) );
			Serial.println(" Ohms");

			Serial.print("Ra E24: ");
			Serial.print( resistorValue( T3maths(ADCtoMV(read1.getAverage()),ADCtoMV(ref1.getAverage())) ) );
			Serial.println(" Ohms");

			Serial.print("Ref 2: ");
			Serial.print(ADCtoMV(ref2.getAverage()));
			Serial.println("mV");

			Serial.print("Reading 2: ");
			Serial.print(ADCtoMV(read2.getAverage()));
			Serial.println("mV");

			Serial.print("Rb: ");
			Serial.print( T3maths(ADCtoMV(read2.getAverage()),ADCtoMV(ref2.getAverage())) );
			Serial.println(" Ohms");

			Serial.print("Rb E24: ");
			Serial.print( resistorValue( T3maths(ADCtoMV(read2.getAverage()),ADCtoMV(ref2.getAverage())) ) );
			Serial.println(" Ohms");
			#endif
			returnValue( resistorValue( T3maths( ADCtoMV( read1.getAverage() ) , ADCtoMV( ref1.getAverage() ) ) ), 6);
			returnValue( resistorValue( T3maths( ADCtoMV( read2.getAverage() ) , ADCtoMV( ref2.getAverage() ) ) ), 6);
			returnValue( T3maths( ADCtoMV( read1.getAverage() ) , ADCtoMV( ref1.getAverage() ) ), 6);
			returnValue( T3maths( ADCtoMV( read2.getAverage() ) , ADCtoMV( ref2.getAverage() ) ), 6);
		}
		else {
			flushBuffers();

			for(int i=1 ; i<=bufferSize ;i++) {
				// Reference voltage
				// P2->10v[R2 - 1K] + ADC GND-> GND[low Z] P1->OPEN
				GPIOon(B00001001,B01010101);
				delay(50);
				ref1.addValue(analogRead(2));
				delay(50);
				GPIOoff(B00000000,B00000000);
				delay(50);

				//First Measurement
				// P2->10v[R2 - 1K] GND->GND[low Z] P1->ADC
				GPIOon(B00100001,B01010101);
				delay(50);
				read1.addValue(analogRead(2)+6);
				delay(50);
				GPIOoff(B00000000,B00000000);
				delay(50);

				// Reference voltage 2
				// P2->10v[R2 - 1K] + ADC GND->OPEN P1->GND[low Z]
				GPIOon(B00001001,B01010110);
				delay(50);
				ref2.addValue(analogRead(2));
				delay(50);
				GPIOoff(B00000000,B00000000);
				delay(50);

				// Second Measurement
				// P2->10v[R2 - 1K] GND->ADC P1->GND[low Z]
				GPIOon(B00010001,B01010110);
				delay(50);
				read2.addValue(analogRead(2)+6);
				delay(50);
				GPIOoff(B00000000,B00000000);
				delay(50);

			#if TARGET_DEBUG
				Serial.println(i);
			#endif
			}

			#if TARGET_DEBUG
			Serial.print("Ref 1: ");
			Serial.print(ADCtoMV(ref1.getAverage()));
			Serial.println("mV");

			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("Ra: ");
			Serial.print( T3maths(ADCtoMV(read1.getAverage()), ADCtoMV(ref1.getAverage()) ) );
			Serial.println(" Ohms");

			Serial.print("Ra E24: ");
			Serial.print( resistorValue( T3maths(ADCtoMV(read1.getAverage()),ADCtoMV(ref1.getAverage())) ) );
			Serial.println(" Ohms");

			Serial.print("Ref 2: ");
			Serial.print(ADCtoMV(ref2.getAverage()));
			Serial.println("mV");

			Serial.print("Reading 2: ");
			Serial.print(ADCtoMV(read2.getAverage()));
			Serial.println("mV");

			Serial.print("Rb: ");
			Serial.print( T3maths(ADCtoMV(read2.getAverage()),ADCtoMV(ref2.getAverage())) );
			Serial.println(" Ohms");

			Serial.print("Rb E24: ");
			Serial.print( resistorValue( T3maths(ADCtoMV(read2.getAverage()),ADCtoMV(ref2.getAverage())) ) );
			Serial.println(" Ohms");
			#endif
			returnValue( resistorValue( T3maths( ADCtoMV( read1.getAverage() ) , ADCtoMV( ref1.getAverage() ) ) ), 6);
			returnValue( resistorValue( T3maths( ADCtoMV( read2.getAverage() ) , ADCtoMV( ref2.getAverage() ) ) ), 6);
			returnValue( T3maths( ADCtoMV( read1.getAverage() ) , ADCtoMV( ref1.getAverage() ) ), 6);
			returnValue( T3maths( ADCtoMV( read2.getAverage() ) , ADCtoMV( ref2.getAverage() ) ), 6);
		}
		break;

		//----------//
		// Target 4 //
		//----------//
	case '4':
		if (direction == 0) {
			flushBuffers();

			for(int i=1 ; i<=bufferSize ;i++) {
				//First Measurement
				// P1->10v[R2 - 1K] + ADC GND->GND[low Z] P2->OPEN
				GPIOon(B00001100,B01010001);
				delay(50);
				read1.addValue(analogRead(2) + 17);
				GPIOoff(B00000000,B00000000);
				delay(50);

				//Second Measurment
				//P1->OPEN GND->GND[low Z] P2->10v[R2 - 1K] + ADC
				GPIOon(B00100001,B01010001);
				delay(50);
				read2.addValue(analogRead(2) + 17);
				GPIOoff(B00000000,B00000000);
				delay(50);

			#if TARGET_DEBUG
				Serial.println(i);
			#endif
			}

			#if TARGTE_DEBUG
			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("Ra: ");
			Serial.print( T4maths(ADCtoMV(read1.getAverage())) );
			Serial.println(" Ohms");

			Serial.print("Ra E24: ");
			Serial.print( resistorValue( T4maths(ADCtoMV(read1.getAverage())) ) );
			Serial.println(" Ohms");

			Serial.print("Reading 2: ");
			Serial.print(ADCtoMV(read2.getAverage()));
			Serial.println("mV");

			Serial.print("Rb: ");
			Serial.print( T4maths( ADCtoMV(read2.getAverage()) ) );
			Serial.println("Ohms");

			Serial.print("Rb E24: ");
			Serial.print( resistorValue( T4maths(ADCtoMV(read2.getAverage())) ) );
			Serial.println(" Ohms");
			#endif
			returnValue( resistorValue(T4maths(ADCtoMV(read1.getAverage()))), 6);
			returnValue( resistorValue(T4maths(ADCtoMV(read2.getAverage()))), 6);
			returnValue( T4maths(ADCtoMV(read1.getAverage())), 6);
			returnValue( T4maths(ADCtoMV(read2.getAverage())), 6);
		}
		else {
			flushBuffers();

			for(int i=1 ; i<=bufferSize ;i++) {
				//First Measurement
				// P1->10v[R2 - 1K] + ADC GND->GND[low Z] P2->OPEN
				GPIOon(B00001100,B01010101);
				delay(50);
				read1.addValue(analogRead(2) + 17);
				GPIOoff(B00000000,B00000000);
				delay(50);

				//Second Measurment
				//P1->OPEN GND->GND[low Z] P2->10v[R2 - 1K] + ADC
				GPIOon(B00100001,B01010101);
				delay(50);
				read2.addValue(analogRead(2) + 17);
				GPIOoff(B00000000,B00000000);
				delay(50);

			#if TARGET_DEBUG
				Serial.println(i);
			#endif
			}

			#if TARGTE_DEBUG
			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("Ra: ");
			Serial.print( T4maths(ADCtoMV(read1.getAverage())) );
			Serial.println(" Ohms");

			Serial.print("Ra E24: ");
			Serial.print( resistorValue( T4maths(ADCtoMV(read1.getAverage())) ) );
			Serial.println(" Ohms");

			Serial.print("Reading 2: ");
			Serial.print(ADCtoMV(read2.getAverage()));
			Serial.println("mV");

			Serial.print("Rb: ");
			Serial.print( T4maths( ADCtoMV(read2.getAverage()) ) );
			Serial.println("Ohms");

			Serial.print("Rb E24: ");
			Serial.print( resistorValue( T4maths(ADCtoMV(read2.getAverage())) ) );
			Serial.println(" Ohms");
			#endif
			returnValue( resistorValue(T4maths(ADCtoMV(read1.getAverage()))), 6);
			returnValue( resistorValue(T4maths(ADCtoMV(read2.getAverage()))), 6);
			returnValue( T4maths(ADCtoMV(read1.getAverage())), 6);
			returnValue( T4maths(ADCtoMV(read2.getAverage())), 6);
		}
		break;

		//----------//
		// Target 5 //
		//----------//
	case'5':
		if (direction == 0) {
			long temp = 0;

			//-> measure resistance
			flushBuffers();
			for(int i=1 ; i<=bufferSize ;i++) {

				// P1->OPEN GND->GND[low Z] p2->10v[R2 - 1K] + ADC
				GPIOon(B00001001,B01010001);
				delay(100);
				read1.addValue(analogRead(2) + 15);
				GPIOoff(B00000000,B00000000);
				delay(50);


				//-> discharge capacitor
				// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
				GPIOon(B01000000,B01000011);
				delay(100);
				GPIOoff(B00000000,B00000000);
				delay(50);

				// -> charge capacitor via 10v[R4 - 3M3] and start timer
				// P1->10v[R4 - 1M] + ADC GND->OPEN P2->GND[low Z]
				GPIOon(B01100100,B01000000);
				timer.start();

				// -> time taken to reach 63% of 10v [6.3V - 645]
				while(temp <= 645){
					temp = analogRead(2);
				}

				timer.stop();
				read2.addValue(timer.elapsed());
				ref2.addValue(temp);

				timer.reset();
				temp = 0;

				//-> discharge capacitor
				// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
				GPIOon(B01000000,B01000011);
				delay(200);
				GPIOoff(B00000000,B00000000);
				delay(50);

				#if TARGET_DEBUG
				Serial.println(i);
				#endif
			}

			#if TARGET_DEBUG
			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("R: ");
			Serial.print( TRmaths( ADCtoMV(read1.getAverage()) ) );
			Serial.println("Ohms");

			Serial.print("R E24: ");
			Serial.print( resistorValue( TRmaths(ADCtoMV(read1.getAverage())) ) );
			Serial.println(" Ohms");

			Serial.print("C Actual V: ");
			Serial.print(ADCtoMV(ref2.getAverage()));
			Serial.println("mV");

			Serial.print("Charge time: ");
			Serial.print(read2.getAverage());
			Serial.println("uS");

			Serial.print("C: ");
			Serial.print( TCmaths(read2.getAverage()) );
			Serial.println(" nF");

			Serial.print("C E12: ");
			Serial.print( capValue( TCmaths(read2.getAverage()) ) );
			Serial.println(" nF");
			#endif
			returnValue( TCmaths(read2.getAverage()), 6);
			returnValue( TRmaths(ADCtoMV(read1.getAverage())), 6);
			returnValue( capValue(TCmaths(read2.getAverage())), 6);
			returnValue( resistorValue(TRmaths(ADCtoMV(read1.getAverage()))), 6);
		}
		else {
			if (direction == 0) {
				long temp = 0;

				//-> measure resistance
				flushBuffers();
				for(int i=1 ; i<=bufferSize ;i++) {

					// P1->OPEN GND->GND[low Z] p2->10v[R2 - 1K] + ADC
					GPIOon(B00001001,B01010101);
					delay(100);
					read1.addValue(analogRead(2) + 15);
					GPIOoff(B00000000,B00000000);
					delay(50);


					//-> discharge capacitor
					// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
					GPIOon(B01000000,B01000111);
					delay(100);
					GPIOoff(B00000000,B00000000);
					delay(50);

					// -> charge capacitor via 10v[R4 - 3M3] and start timer
					// P1->10v[R4 - 1M] + ADC GND->OPEN P2->GND[low Z]
					GPIOon(B01100100,B01000100);
					timer.start();

					// -> time taken to reach 63% of 10v [6.3V - 645]
					while(temp <= 645){
						temp = analogRead(2);
					}

					timer.stop();
					read2.addValue(timer.elapsed());
					ref2.addValue(temp);

					timer.reset();
					temp = 0;

					//-> discharge capacitor
					// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
					GPIOon(B01000000,B01000111);
					delay(200);
					GPIOoff(B00000000,B00000000);
					delay(50);

					#if TARGET_DEBUG
					Serial.println(i);
					#endif
				}

				#if TARGET_DEBUG
				Serial.print("Reading 1: ");
				Serial.print(ADCtoMV(read1.getAverage()));
				Serial.println("mV");

				Serial.print("R: ");
				Serial.print( TRmaths( ADCtoMV(read1.getAverage()) ) );
				Serial.println("Ohms");

				Serial.print("R E24: ");
				Serial.print( resistorValue( TRmaths(ADCtoMV(read1.getAverage())) ) );
				Serial.println(" Ohms");

				Serial.print("C Actual V: ");
				Serial.print(ADCtoMV(ref2.getAverage()));
				Serial.println("mV");

				Serial.print("Charge time: ");
				Serial.print(read2.getAverage());
				Serial.println("uS");

				Serial.print("C: ");
				Serial.print( TCmaths(read2.getAverage()) );
				Serial.println(" nF");

				Serial.print("C E12: ");
				Serial.print( capValue( TCmaths(read2.getAverage()) ) );
				Serial.println(" nF");
				#endif
				returnValue( TCmaths(read2.getAverage()), 6);
				returnValue( TRmaths(ADCtoMV(read1.getAverage())), 6);
				returnValue( capValue(TCmaths(read2.getAverage())), 6);
				returnValue( resistorValue(TRmaths(ADCtoMV(read1.getAverage()))), 6);
			}
			break;

			//----------//
			// Target 6 //
			//----------//
	case '6':
		if (direction == 0) {
			long temp = 0;

			flushBuffers();
			for(int i=1 ; i<=bufferSize ;i++) {
				//-> measure resistance
				// P1->10v[R2 - 1K] + ADC GND->OPEN p2->GND[low Z]
				GPIOon(B01100100,B01010000);
				delay(100);
				read1.addValue(analogRead(2)+17);
				GPIOoff(B00000000,B00000000);
				delay(50);


				//-> discharge capacitor
				// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
				GPIOon(B01000000,B01000011);
				delay(200);
				GPIOoff(B00000000,B00000000);
				delay(50);

				// -> charge capacitor via 10v[R4 - 1M] and start timer
				// P1->OPEN GND->GND[low Z] P2->10v[R4 - 1M] + ADC
				GPIOon(B00001001,B01000001);
				timer.start();

				// -> time taken to reach 63% of 10v [6.3V - 645]
				while(temp <= 645){
					temp = analogRead(2);
				}

				timer.stop();
				read2.addValue(timer.elapsed());
				ref2.addValue(temp);

				timer.reset();
				temp = 0;

				//-> discharge capacitor
				// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
				GPIOon(B01000000,B01000011);
				delay(200);
				GPIOoff(B00000000,B00000000);
				delay(50);

				#if TARGET_DEBUG
				Serial.println(i);
				#endif

			}

			#if TARGET_DEBUG
			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("R: ");
			Serial.print( TRmaths( ADCtoMV(read1.getAverage()) ) );
			Serial.println("Ohms");

			Serial.print("R E24: ");
			Serial.print( resistorValue( TRmaths(ADCtoMV(read1.getAverage())) ) );
			Serial.println(" Ohms");

			Serial.print("C Actual V: ");
			Serial.print(ADCtoMV(ref2.getAverage()));
			Serial.println("mV");

			Serial.print("Charge time: ");
			Serial.print(read2.getAverage());
			Serial.println("uS");

			Serial.print("C: ");
			Serial.print( TCmaths(read2.getAverage())*0.967 );
			Serial.println(" nF");

			Serial.print("C E12: ");
			Serial.print( capValue( TCmaths(read2.getAverage())*0.967 ) );
			Serial.println(" nF");
			#endif
			returnValue( TCmaths(read2.getAverage())*0.967, 6);
			returnValue( TRmaths(ADCtoMV(read1.getAverage())), 6);
			returnValue( capValue(TCmaths(read2.getAverage())*0.967), 6);
			returnValue( resistorValue(TRmaths(ADCtoMV(read1.getAverage()))), 6);
		}
		else {
			long temp = 0;

			flushBuffers();
			for(int i=1 ; i<=bufferSize ;i++) {
				//-> measure resistance
				// P1->10v[R2 - 1K] + ADC GND->OPEN p2->GND[low Z]
				GPIOon(B01100100,B01010100);
				delay(100);
				read1.addValue(analogRead(2)+17);
				GPIOoff(B00000000,B00000000);
				delay(50);


				//-> discharge capacitor
				// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
				GPIOon(B01000000,B01000111);
				delay(200);
				GPIOoff(B00000000,B00000000);
				delay(50);

				// -> charge capacitor via 10v[R4 - 1M] and start timer
				// P1->OPEN GND->GND[low Z] P2->10v[R4 - 1M] + ADC
				GPIOon(B00001001,B01000101);
				timer.start();

				// -> time taken to reach 63% of 10v [6.3V - 645]
				while(temp <= 645){
					temp = analogRead(2);
				}

				timer.stop();
				read2.addValue(timer.elapsed());
				ref2.addValue(temp);

				timer.reset();
				temp = 0;

				//-> discharge capacitor
				// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
				GPIOon(B01000000,B01000111);
				delay(200);
				GPIOoff(B00000000,B00000000);
				delay(50);

			#if TARGET_DEBUG
				Serial.println(i);
			#endif
			}

			#if TARGET_DEBUG
			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("R: ");
			Serial.print( TRmaths( ADCtoMV(read1.getAverage()) ) );
			Serial.println("Ohms");

			Serial.print("R E24: ");
			Serial.print( resistorValue( TRmaths(ADCtoMV(read1.getAverage())) ) );
			Serial.println(" Ohms");

			Serial.print("C Actual V: ");
			Serial.print(ADCtoMV(ref2.getAverage()));
			Serial.println("mV");

			Serial.print("Charge time: ");
			Serial.print(read2.getAverage());
			Serial.println("uS");

			Serial.print("C: ");
			Serial.print( TCmaths(read2.getAverage())*0.967 );
			Serial.println(" nF");

			Serial.print("C E12: ");
			Serial.print( capValue( TCmaths(read2.getAverage())*0.967 ) );
			Serial.println(" nF");
			#endif
			returnValue( TCmaths(read2.getAverage())*0.967, 6);
			returnValue( TRmaths(ADCtoMV(read1.getAverage())), 6);
			returnValue( capValue(TCmaths(read2.getAverage())*0.967), 6);
			returnValue( resistorValue(TRmaths(ADCtoMV(read1.getAverage()))), 6);
		}
		break;

		//----------//
		// Target 7 //
		//----------//
	case '7':
		if (direction == 0) {
			long temp = 0;

			flushBuffers();

			for(int i=1 ; i<=bufferSize ;i++) {
				//-> measure resistance
				// P1->OPEN GND->GND[low Z] p2->10v[R1 - 80R] + ADC
				GPIOon(B00001001,B01100001);
				delay(100);
				read1.addValue(analogRead(2)+15);
				GPIOoff(B00000000,B00000000);
				delay(50);

				//-> discharge capacitor
				// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
				GPIOon(B01000000,B01000011);
				delay(200);
				GPIOoff(B00000000,B00000000);
				delay(50);

				// -> charge capacitor via 10v[R4 - 3M3] and start timer
				// P1->GND[low Z] GND->OPEN P2->10v[R4 - 1M] + ADC
				GPIOon(B00001001,B01000010);
				timer.start();

				// -> time taken to reach 63% of 10v [6.3V - 645]
				while(temp <= 645){
					temp = analogRead(2);
				}

				timer.stop();
				read2.addValue(timer.elapsed());
				ref2.addValue(temp);

				timer.reset();
				temp = 0;

				//-> discharge capacitor
				// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
				GPIOon(B01000000,B01000011);
				delay(200);
				GPIOoff(B00000000,B00000000);
				delay(50);

			#if TARGET_DEBUG
				Serial.println(i);
			#endif

			}
			#if TARGET_DEBUG
			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("R: ");
			Serial.print( T7Rmaths( ADCtoMV(read1.getAverage()) ) );
			Serial.println("Ohms");

			Serial.print("R E24: ");
			Serial.print( resistorValue( T7Rmaths(ADCtoMV(read1.getAverage())) ) );
			Serial.println(" Ohms");

			Serial.print("C Actual V: ");
			Serial.print(ADCtoMV(ref2.getAverage()));
			Serial.println("mV");

			Serial.print("Charge time: ");
			Serial.print(read2.getAverage());
			Serial.println("uS");

			Serial.print("C: ");
			Serial.print( TCmaths(read2.getAverage()) );
			Serial.println(" nF");

			Serial.print("C E12: ");
			Serial.print( capValue( TCmaths(read2.getAverage()) ) );
			Serial.println(" nF");
			#endif
			returnValue( TCmaths(read2.getAverage()), 6);
			returnValue( T7Rmaths(ADCtoMV(read1.getAverage())), 6);
			returnValue( capValue(TCmaths(read2.getAverage())), 6);
			returnValue( resistorValue(T7Rmaths(ADCtoMV(read1.getAverage()))), 6);
		}
		else {
			long temp = 0;
			flushBuffers();

			for(int i=1 ; i<=bufferSize ;i++) {
				//-> measure resistance
				// P1->OPEN GND->GND[low Z] p2->10v[R1 - 80R] + ADC
				GPIOon(B00001001,B01100001);
				delay(100);
				read1.addValue(analogRead(2)+15);
				GPIOoff(B00000000,B00000000);
				delay(50);

				//-> discharge capacitor
				// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
				GPIOon(B01000000,B01000011);
				delay(200);
				GPIOoff(B00000000,B00000000);
				delay(50);

				// -> charge capacitor via 10v[R4 - 3M3] and start timer
				// P1->GND[low Z] GND->OPEN P2->10v[R4 - 1M] + ADC
				GPIOon(B00001001,B01000010);
				timer.start();

				// -> time taken to reach 63% of 10v [6.3V - 645]
				while(temp <= 645){
					temp = analogRead(2);
				}

				timer.stop();
				read2.addValue(timer.elapsed());
				ref2.addValue(temp);

				timer.reset();
				temp = 0;

				//-> discharge capacitor
				// P1->GND[low Z] GND->GND[low Z] P2->GND[low Z]
				GPIOon(B01000000,B01000011);
				delay(200);
				GPIOoff(B00000000,B00000000);
				delay(50);

				#if TARGET_DEBUG
				Serial.println(i);
				#endif
			}
#if TARGET_DEBUG
			Serial.print("Reading 1: ");
			Serial.print(ADCtoMV(read1.getAverage()));
			Serial.println("mV");

			Serial.print("R: ");
			Serial.print( T7Rmaths( ADCtoMV(read1.getAverage()) ) );
			Serial.println("Ohms");

			Serial.print("R E24: ");
			Serial.print( resistorValue( T7Rmaths(ADCtoMV(read1.getAverage())) ) );
			Serial.println(" Ohms");

			Serial.print("C Actual V: ");
			Serial.print(ADCtoMV(ref2.getAverage()));
			Serial.println("mV");

			Serial.print("Charge time: ");
			Serial.print(read2.getAverage());
			Serial.println("uS");

			Serial.print("C: ");
			Serial.print( TCmaths(read2.getAverage()) );
			Serial.println(" nF");

			Serial.print("C E12: ");
			Serial.print( capValue( TCmaths(read2.getAverage()) ) );
			Serial.println(" nF");
#endif
			returnValue( TCmaths(read2.getAverage()), 6);
			returnValue( T7Rmaths(ADCtoMV(read1.getAverage())), 6);
			returnValue( capValue(TCmaths(read2.getAverage())), 6);
			returnValue( resistorValue(T7Rmaths(ADCtoMV(read1.getAverage()))), 6);
		}
		break;

	default:
		GPIOoff(B00000000,B00000000);
		break;
		}
	}
}

void Target::returnValue (float value, byte width) {

	float tempFloat = (float)value * 100;
	long temp = (long)tempFloat;
	long currentMax = 10;

	for(byte i=1; i< width; i++) {
		if (temp < currentMax) {
#if LAPTOP_CONTROL
			Serial.print("0");
#else
			Serial2.print("0");
#endif
		}
		currentMax *= 10;
	}
#if LAPTOP_CONTROL
	Serial.println(temp);
#else
	Serial2.println(temp);
#endif
}

float Target::capValue(float reading){
	float components[] = {3.901,4.701,5.601,6.801,8.201,10.001,12.001,15.001,18.001,22.001,27.001,33.001,39.001,47.001,56.001,68.001,82.001,100.001,120.001};
	float closest = components[0];

	for ( int i = 0; i < 19; ++i ) {
		if ( abs( components[ i ] - reading ) < abs( closest - reading ) ) closest = components[i];
	}
	return closest;
}

long Target::resistorValue(float reading) {
	float components[] = {10,11,12,13,15,16,18,20,22,24,29,30,33,36,39,43,47,51,56,62,68,75,82,91,100,110,120,130,150,160,180,200,220,240,270,300
		,330,360,390,430,470,510,560,620,680,750,820,910,1000,1100,1200,1300,1500,1600,1800,2000,2200,2400,2700,3000,3300,3600,3900
		,4300,4700,5100,5600,6200,6800,7500,8200,9100};
	float closest = components[0];

	for ( int i = 0; i < 73; ++i ) {
		if ( abs( components[ i ] - reading ) < abs( closest - reading ) ) closest = components[i];
	}
	return closest;
}

float Target::zennerValue(long reading){
	float components[] = {3.001,3.601,4.301,5.101,7.501};
	float temp = ( (float)reading / (float)1000 );
	float closest = components[0];

	for ( int i = 0; i < 5; ++i ) {
		if ( abs( components[ i ] - temp ) < abs( closest - temp ) ) closest = components[i];
	}
	return closest;
}

float Target::T3maths(long reading, long reference){
	float refvolts = ( (float)reference / (float)1000 );
	float volts = ( (float)reading / (float)1000 );

	float temp = ( (refvolts/volts)*1000 ) - 1000;

	return temp;
}

float Target::T4maths(long reading){

	float volts = ( (float)reading / (float)1000 );
	float temp = (12000/volts) - (974 + 1200);

	return temp;
}

float Target::T7Rmaths(long reading){
	float volts = ( (float)reading / (float)1000 );
	float temp = (float)81.32 / ( (10/volts) - 1 );

	return temp;
}

float Target::TRmaths(long reading){
	float volts = ( (float)reading / (float)1000 );
	float temp = 974 / ( (10/volts) - 1 );

	return temp;
}

float Target::TCmaths(long time){
	float temp = (float)0.9225 * ((float)((time*0.985) - 4016.4) / (float)1000);

	return temp;
}

void Target::flushBuffers(){
	#if TARGET_DEBUG
	Serial.println("Clearing Target buffers...");
	ref1.clear();
	read1.clear();
	ref2.clear();
	read2.clear();
	timer.reset();
	Serial.println("Target buffers clear");
	#else
	ref1.clear();
	read1.clear();
	ref2.clear();
	read2.clear();
	timer.reset();
	#endif
}

long Target::ADCtoMV(long ADCval){
	long temp = 2*((ADCval*5000)/1024);
	return temp;
}

void Target::GPIOinit() {
	// This setup is for the port expander
	Wire.begin(); // join i2c bus (address optional for master)

	// Configuring IODIRA, setting to outputs (Port expander)
	Wire.beginTransmission(0x20);
	Wire.write(byte(0x00));            // Offset for IODIRA  
	Wire.write(byte(0x00));             // Data byte going into IODIRA  
	Wire.endTransmission();
	#if VERBOSE_BOOT
	Serial.println("|__ GPIOA -> Output");
	#endif

	// Configuring IODIRB, Setting to outputs (Port expansder)
	Wire.beginTransmission(0x20);
	Wire.write(byte(0x01));            // Offset for IODIRB
	Wire.write(byte(0x00));             // Data byte going into IODIRB  
	Wire.endTransmission();
	#if VERBOSE_BOOT
	Serial.println("|__ GPIOB -> Output");
	#endif

	// Intilly lets say it is all off
	portA = B00000000;
	portB = B00000000;
}

void Target::GPIOtest() {
	GPIOoff(B00000000,B00000000);
	delay(200);

	GPIOon(B00000000,B00000000);
	delay(200);
	GPIOon(B11000000,B00000000);
	delay(200);
	GPIOon(B11100000,B00000000);
	delay(200);
	GPIOon(B11110000,B00000000);
	delay(200);
	GPIOon(B11111000,B00000000);
	delay(200);
	GPIOon(B11111100,B00000000);
	delay(200);
	GPIOon(B11111110,B00000000);
	delay(200);
	GPIOon(B11111111,B00000000);
	delay(200);

	GPIOoff(B00000000,B00000000);
	delay(200);

	GPIOon(B00000000,B00000000);
	delay(200);
	GPIOon(B00000000,B11000000);
	delay(200);
	GPIOon(B00000000,B11100000);
	delay(200);
	GPIOon(B00000000,B11110000);
	delay(200);
	GPIOon(B00000000,B11111000);
	delay(200);
	GPIOon(B00000000,B11111100);
	delay(200);
	GPIOon(B00000000,B11111110);
	delay(200);
	GPIOon(B00000000,B11111111);
	delay(200);

	GPIOoff(B00000000,B00000000);
}

void Target::GPIOon(int maskA, int maskB) {
	int portA_new;
	int portB_new;

	// Work out what to put on each port
	portA_new = maskA | portA;
	portB_new = maskB | portB;

	// Now send this inf to the port expander
	Wire.beginTransmission(0x20);      // Open the transmission portal
	Wire.write(byte(0x12));            // Offset for GPIOA
	Wire.write(byte(portA_new));       // Data byte going into GPIOA (Then it auto increments)
	Wire.write(byte(portB_new));       // Data going into GPIOB
	Wire.endTransmission();            // Close the hole trough the underworld

	// Make sure to update our knollage of what the ports currently are set to
	portA = portA_new;
	portB = portB_new;
}

void Target::GPIOoff(int maskA, int maskB) {
	int portA_new;
	int portB_new;

	// Work out what to put on each port
	portA_new = maskA & portA;
	portB_new = maskB & portB;

	// Now send this inf to the port expander
	Wire.beginTransmission(0x20);      // Open the transmission portal
	Wire.write(byte(0x12));            // Offset for GPIOA
	Wire.write(byte(portA_new));       // Data byte going into GPIOA (Then it auto increments)
	Wire.write(byte(portB_new));       // Data going into GPIOB
	Wire.endTransmission();            // Close the hole trough the underworld

	// Make sure to update our knollage of what the ports currently are set to
	portA = portA_new;
	portB = portB_new;
}