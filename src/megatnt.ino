/*
	MEGATNT v1 rev 0 ~ release

	5G00BM43-3001-3002
	03.12.2014

	Antti E.K. Laine
	Anna-Lotta Luoma
	Katri Pitkänen

	laine.antti.e(at)gmail.com
	
	TODO:
		- better variable names
		- better menus
		- MOAR MUSIIK~
		- Polyphony (even if fake)
		- way of changing waveforms (wavetable shuffle)
		- faster LCD interface
		- rethink variable types
		- figure out WHAT THE CODE actually DOES
		- seperate sub functions into their own .c files with internal structure
		  (right now it's a big list of headers)
		  (it works but its very messy)
*/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "LiquidCrystal.h"

LiquidCrystal lcd(1,0,3,4,5,6);

#include "music.h"
#include "waves.h"
#include "beeps.h"
#include "led.h"
#include "glyphs.h"
#include "keypad.h"

#include "game.h"
#include "jukebox.h"

bool startup = true;


void setup() {

	/*
		Most of the audio code is based on Jon Thompson's article on makezine.com
		I've merely adopted his methods and molded them to suit my evil plans
		http://makezine.com/projects/make-35/advanced-arduino-sound-synthesis/
	*/
  
	lcd.begin(16,2);

	DDRD = DDRD | B10000000;            				// D7 to output mode
	DDRB = DDRB | B00000011;							// D8 and D9 to output mode
	/* --------- */
	cli();												// Disable interrupts

	TCCR1B  = (1 << CS10);          					// Set prescaler to full 16MHz
	TCCR1A |= (1 << COM1A1);        					// PWM pin to go low when TCNT1=OCR1A
	TCCR1A |= (1 << WGM10);         					// Put timer into 8-bit fast PWM mode
	TCCR1B |= (1 << WGM12);								// CTC Mode enable


	TCCR2A = 0;                     					// We need no options in control register A
	TCCR2B = (1 << CS21);           					// Set prescaler to divide by 8
	TIMSK2 = (1 << OCIE2A);         					// Set timer to call ISR when TCNT2 = OCRA2

														// This is for the External Clock Module
	EICRA |= (1 << ISC01) | (1 << ISC00);				// Rising edge trigger mode for INT0
	EIMSK |= (1 << INT0);								// Enable ISR call on INT0

	sei();                          					// Enable interrupts
	/* --------- */
}

void loop() {

	if (startup) {										// Startup logo and loop										
		load_intro_glyphs ();
		printIntroScreen();

		while (!enter) {
			checkForEnter();							// Wait for the user to press Enter (A)
		}

		play_song(POWERON, NO_LOOP);					// Play the intro music and flash LEDs

		led_on(GREEN);
		delay(80);
		led_off(GREEN);
		led_on(RED);
		lcd.noDisplay();
		delay(80);
		led_on(GREEN);
		led_off(RED);
		lcd.display();
		delay(80);
		led_off(GREEN);
		led_on(RED);
		lcd.noDisplay();
		delay(80);
		led_off(RED);
		lcd.display();
		delay(80);

		led_on(RED);
		led_on(GREEN);
		delay(700);
		led_off(RED);
		led_off(GREEN);

		load_game_glyphs();								// Replace the intro glyphs with game glyphs
														// We only have space for 8 custom glyhps
		startup = false;
	}

	short sel_;

	bool selected = false;
	enter = false;

	lcd.clear();
	lcd.setCursor(0,0);
	lcd.print("jBOX  PLAY  OPT1");						// Print our menu interaface on the LCD

	while (!selected) {									// Main menu loop
	
		int sensor = ADConvert(B0100);					// We use the potentiometer as a control for the cursor
		sel_ = map (sensor, 0, 1023, 0, 3);				// Map the ADC output from (0->1023) to (0->3)

		checkForEnter();

		switch (sel_) {									// We select the appropriate item based on the mapped ADC output
			case 0:
				updateScreen (0, sel_, true);			// Update the cursor location
				if (enter) {							// If the user presses enter we do the assigned function
					selected = true;					// This parameter lets us exit the main menu loop once we return from
					enterJukeBox();						// the subfunction. It's used to reprint the menu interface on the LCD
				}
				break;
			
			case 1:
				updateScreen (0, sel_ + 5, true);
				if (enter) {
					selected = true;
					difficultyMenu ();
				}
				break;
			
			case 2:
				updateScreen (0, sel_ + 10, true);
				if (enter) {
					select_beep ();
					selected = true;
					led_blink(RED);
					led_blink(GREEN);
				}
		}
	}
}

/*
	This fucntion updates the cursor while we're in a menu
	s_ is the base point of the cursor, s is how much we
	should offset the cursor from the base and multi
	control if it's a wide cursor or not

	Also it's not necessary to update the cursor if it hasn't moved
	olds_ takes care of that
*/

void updateScreen (short s_, short s, bool multi) {

	static short olds_ = -1;

	if (s != olds_) {									// If the new cursor offset is not equal to the previous one
														// the cursor has moved, so we update it's location on the LCD
		lcd.setCursor(s_ + olds_, 1);					// Set the LCD cursor to the start of the old menu cursor

		if (multi) {									// Erase the old cursor, multi tells us if it's a wide one or not
			lcd.print("    ");
		}else{
			lcd.print(' ');
		}

		lcd.setCursor(s_ + s, 1);						// Set the LCD cursor to the start of the new menu cursor	

		if (multi) {									// Print the new cursor; byte(2) is an underline and byte(1)
			lcd.write(byte(2));							// is an arrow which is used in the difficulty menu
			lcd.write(byte(2));
			lcd.write(byte(2));
			lcd.write(byte(2));
		}else{
			lcd.write(byte(1));
		}

		beep ();										// Make some noise so the user know we did something

		olds_ = s;										// Replace the previous cursor's offset with the new one
	}													// when we comeback again we check against this value to see
}														// if the cursor should be moved

/*
	Horrible, just utterly terrible menu
	(But it works)

	Works excatly like the main menu
	it just calls different functons when the user
	selects something
*/

void difficultyMenu () {

	short sel_;

	bool exit_ = false;
	enter = false;

	select_beep ();

	lcd.setCursor(0,0);
	lcd.print("Select      Q123");
	lcd.setCursor(0,1);
	lcd.print("Difficulty");

	while (!exit_) {
		int sensor = ADConvert(B0100);
		sel_ = map (sensor, 0, 1023, 0, 4);

		checkForEnter();

		switch (sel_) {
			case 0:
				updateScreen (12, sel_, false);
				if (enter) {
					select_beep ();
					exit_ = true;
					lcd.clear();
				}
				break;

			case 1:
				updateScreen (12, sel_, false);
				if (enter) {
					startGame(1);
					exit_ = true;
				}
				break;

			case 2:
				updateScreen (12, sel_, false);
				if (enter) {
					startGame(2);
					exit_ = true;
				}
				break;

			case 3:
				updateScreen (12, sel_, false);
				if (enter) {
					startGame(3);
					exit_ = true;
				}
		}
	}
}

/* --------- */

/* 
	Iterrupt Service Routine called when PIN0 (External Timer) goes high
	This happens at about 10ms interval

	< Nice description goes here >

	This is pure magic, surely.
*/

ISR(INT0_vect, ISR_NOBLOCK) {

	if (music_enabled) {

		if (c_ <= 0) {
			if (!pause) {
				sound = true;
				OCR2A = pgm_read_word_near(note_ptr + PITCH_OFFSET);
				c_ = pgm_read_word_near(note_ptr);

				if (pgm_read_word_near(note_ptr + PAUSE_OFFSET) != 0) {
					pause = true;
				}else{
					pause = false;
					note_ptr += NOTE_STEPPING;
				}
			}else{
				sound = false;
				c_ = pgm_read_word_near(note_ptr + PAUSE_OFFSET);
				pause = false;
				note_ptr += NOTE_STEPPING;
			}
		}	
		if (note_ptr - start_ptr > song_length) {
			if (loop_enabled) {
				note_ptr = start_ptr;
				c_ = 0;
			}else{
				sound = false;
				music_enabled = false;
			}
		}else{
			c_--;
		}
	}
}

/*
	This Interrupt Service Routine is used to load the next sample from the currently used wavetable
	We have some magical assembly calls to compensate the time the processor takes to jump (hoops!)
	"NOP;NOP" just means "do nothing for 2 cycles"

	TCNT2 has to be adjusted at the end; otherwise we're going to sound terrible
	'6' seems to be the right magical number for compensating the time we spent in the ISR call

	The index variable stores the index value of the current sample in the wavetable
	During each ISR call we increment it by 1 to laod the next sample
	When the index value reaches 255 it will overflow back to 0 since it's defined as a byte
*/

ISR(TIMER2_COMPA_vect) {

	static byte index = 0;
	
	if (sound) {
		OCR1AL = pgm_read_byte(&WAVES[6][index++]);
		asm("NOP;NOP");
		TCNT2 = 6;
	}
}