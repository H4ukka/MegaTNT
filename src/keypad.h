/*
	KEYPAD HEADER
*/

/*
	A bunch of functions for reading input from the keypad via ADC
*/

char keys[] = { '1', '2', '3', 'A', 			// ASCII character representations of the keypad keys
				'4', '5', '6', 'B', 
				'7', '8', '9', 'C', 
				'*', '0', '#', 'D' 
			};

short keys_num[] = { 1, 2, 3, 10, 				// Numerical representations of the keypad keys
					 4, 5, 6, 0, 				// Keys *, #, B, C and D are basically ignored
					 7, 8, 9, 0, 				// - they all map to 0; 10 = Enter = A
					 0, 0, 0, 0 
			};

short key;										// Stores the value of the key which has been pressed 
bool key_lockout = false;						// Set to true when a key is pressed; remains true until all keys are released
bool enter = false;								// A global flag for letting EVERYONE know someone pressed the 'A' key

/*
	Analog to Digital conversion function; you give it a channel code as a nibble
	and it'll spit out a value between (0 -> 1023) representing that channels current
	voltage level

	ATMega328 ADC pins:

	id | nibble
	---|-------
	A0 | B0000
	A1 | B0001
	A2 | B0010
	A3 | B0011
	A4 | B0100
	A5 | B0101
	A6 | B0110
	A7 | B0111
*/

int ADConvert (byte channel) {

	DIDR0 = 0xFF;								// Disable all digital input pins on the ADC
	ADMUX = 0x40 | channel;       				// 0x40 = Analog Comparator Multiplexer Enable (ACME)
												// we OR the channel nibble to select the wanted channel via ADMUX (p. 242 ds.)
	ADCSRA = 0xC7;								// Enable ADC and start the conversion process; set the prescaler factor to 128

	while ( (ADCSRA & 0x40) != 0);				// When the conversion is done the ADC will set the ADSC bit to 0; until then we wait

	return ADC;									// Once we're done return the contents of the ADC register
}

/*
	Gets the value of a key if a key is pressed
	else this returns a -1 signaling no key is
	pressed
*/

short getKeypad(){

	short ret = -1;								// Our default return value

	if (ADConvert(B0011) == 0) {				// If nothing is pressed the ADC will read 0; we reset the lockout flag and return -1
		key_lockout = false;
	}else if (!key_lockout){					// The ADC was not 0; so a key is beign pressed! (or noise); if a key has
		delay(20);								// Dirty debounce; wait 20 ms before reading the key's value

		/*
			THE MAGIC

			more on this will be written later; it will include an excel chart for sure!
		*/

		ret = 15 - (log((ADConvert(B0011)-183.9)/58.24)/0.1623) + 0.5;
		key_lockout = true;
	}

	return ret;
}

/*
	This merely sets the global enter flag if A is pressed while
	this function is called
*/

void checkForEnter () {

	key = getKeypad();

	if (key != -1) {
		if (keys[key] == 'A') {
			enter = true;
		}
	}
}

/*
	This gets numerical input from the keypad
	Returns -1 if no keys were pressed
*/

short getInput () {

	key = getKeypad();

	if (key != -1) {
		return keys_num[key];
	}
	return -1;
}