/*
	GAME HEADER
*/

#define DIGITS 							6 			// This defined the max number of digits in the secret code
#define ANSWER_MAX_LENGTH 				5 			// Max number of digits in the user's answer
#define MAX_LIFE 						5 			// Users max life; 5 is equal to 2 hearts

short time_limit, time_left, life;					// Time based variables for the game's timelimit
int last_;											// Stores the last count of millis() when the countdown was updated

bool timeout, dead, victory;						// Game flags

/*
	I think you just  lost the game!

	Make some noise, flash the red LED, flash the LCD with a checker pattern
	( low budget explosions ) and play some sad music
*/

void defeat () {

	sound = true;
	OCR2A = 180;
	fillWithChecker();
	led_on(RED);

	delay (100);
	sound = false;
	lcd.clear();
	led_off(RED);
	delay(20);

	sound = true;
	fillWithChecker();
	led_on(RED);
	delay (150);
	sound = false;
	lcd.clear();
	led_off(RED);

	lcd.setCursor(0, 0);
	lcd.print("   GAME  OVER   ");

	led_on(RED);
	play_song(DEATH_MUSIC, NO_LOOP);
	delay(7000);
	led_off(RED);
}

/*
	Print the secret code to the LCD
	Digits which the user has not unlocked are shown as asterisks (*)
	For each digit which they've unlocked we flash the green LED,
	flash the LCD off and on and make a beep
*/

void printDisplayCode (char display_code[]) {
	
	lcd.clear();
	lcd.setCursor(6, 0);
	lcd.print ("CODE");
	lcd.setCursor(5, 1);

	for (short i = 0; i < DIGITS; ++i) {
		lcd.print(display_code[i]);

		if (display_code[i] != '*') {
			delay(150);
			sound = true;
			OCR2A = 13;
			led_on(GREEN);
			lcd.noDisplay();
			delay(150);
			sound = false;
			led_off(GREEN);
			lcd.display();
		}
	}
}

/*
	Technically not a part of the game
	This just prints the startup screen to the LCD
*/

void printIntroScreen () {
	
	lcd.setCursor(6,0);

	lcd.write(byte(0));								// This
	lcd.write(byte(2));								// is
	lcd.write(byte(1));								// our
	lcd.write(byte(3));								// logo

	lcd.setCursor(4,1);
	lcd.print("Press A");
}

/*
	Yay you won the game!
*/

void printVictoryScreen () {

	lcd.clear();
	lcd.setCursor(2, 0);
	lcd.print("BOMB DEFUSED");
}

/*
	A countdown from 3 -> 1 before each new puzzle
	We beep everytime the counter goes down by one
*/

void puzzleCountDown () {

	lcd.clear();

	lcd.setCursor(0,0);
	lcd.print("Next puzzle in ");

	for (short i = 3; i > 0; i--) {
		lcd.setCursor(15,0);
		lcd.print(i);
		beep();
		delay(1000);								// Using a delay is the best option but it
	}												// Works for a quick countdown

}

/*
	While the user is solving the puzzle we need a way of updating the countdown timer every second

	last_ stores the value of millis() from last time we updated the counter. At the start of the game
	this is set to -1 so it's guaranteed to update.
	time_left stores the number of seconds the user has remaining to solve the puzzle
	timeout is a flag that tells the main loop the user ran out of time
*/

void updateTime () {

	if(last_ != (millis() / 1000)) {				// Check if the current time different from last time we updated the counter
		last_ = millis() / 1000;					// If so; store it
		time_left--;								// Decrease the remaining time by one

		lcd.setCursor(14,0);						// Set the cursor to the location of the countdown
		if (time_left < 10) {						// If the counter is less than ten; print a space before printing the
			lcd.print(' ');							// actual count; this prevents ghosting from previous counts
		}

		lcd.print(time_left);						// Print the count

		if(time_left == 0) {						// Check if we should raise the timeout flag
			timeout = true;
		}
	}
}

/*
	Super dirty way of updating the heart glyphs based on
	how much "life" the user has left

	byte(4) = full heart
	byte(5) = empty heart
	byte(6)	= half heart

	This also checks if the user has ran out of life and
	should be declared dead
*/

void updateLife () {

	lcd.setCursor(0,0);

	// This is retarded

	switch (life) {
		case 0: dead = true; break;
		case 1: lcd.write(byte(5)); lcd.write(byte(5)); break;
		case 2: lcd.write(byte(6)); lcd.write(byte(5)); break;
		case 3: lcd.write(byte(4)); lcd.write(byte(5)); break;
		case 4: lcd.write(byte(4)); lcd.write(byte(6)); break;
		case 5: lcd.write(byte(4)); lcd.write(byte(4)); break;
		default: lcd.write(byte(4)); lcd.write(byte(4));
	}
}

/*
	This is the core of the game

	Each new puzzle is given the current difficulty setting which it
	uses to adjust the range of generated random numbers

	If the user solves the puzzle correctly witohut dying and within the 
	time limit we return true; else they've failed and we return false
*/

bool newPuzzle(short difficulty) {

	short op_a, op_b, operator_, max_, ops_;		// Declarations for the puzzle elements
	int answer;										// This stores the correct answer

	char display_op;								// This is printed to the LCD showing the user
													// which type of a problem it is (+, - or x)
	switch (difficulty) {							// The difficulty setting is used here to adjust 
	    case 1:										// the max limit for operand_a and operand_b
	    	max_ = 20;
	    	ops_ = 2;								// This limits the problem to only be either + or -
	    	break;

	    case 2:
	    	max_ = 60;
	    	ops_ = 2;
	    	break;

	    case 3:
	    	max_ = 100;
	    	ops_ = 3;
	}

	do {											// Generate a problem based on the limits
		op_a = random(1, max_);						// Generate operand_a
		op_b = random(1, max_);						// Generate operand_b
		
		operator_ = random(0, ops_);				// Generate a random operator 0 = +, 1 = -, 2 = x
		
		switch (operator_) {
			case 0: answer = op_a + op_b; display_op = '+'; break;
			case 1: answer = op_a - op_b; display_op = '-'; break;
			case 2: answer = op_a * op_b; display_op = '*';
		}
	}while(answer < 1);								// If the answer is negative we generate a new problem

	int user_answer = 0;							// The user's answer

	short p_ = 0;									// This keeps track of the number of digits the user has 
													// entered
	short inp;										// Input from the keypad

	lcd.clear();

	play_song (GAME_MUSIC, LOOP);					// Start the game music, looping

	while (!timeout && !dead) {						// While we're not out of time or dead get input from the user

		updateTime();								// Update the countdown timer						
		updateLife();								// Update user's hearts (life)

		inp = getInput();							// Capture user's input

		if (inp == 10){								// 10 = A = Enter press
			if (user_answer == answer) {			// If the user's answer is correct; stop playbakc and exit with true 
				stop_music ();
				return true;
			}else{									// Else substract life by one and make some noise while flashing the
				life--;								// red LED
				user_answer = 0;
				p_ = 0;

				OCR2A = 82;

				sound = true;
				led_on(RED);

				delay(50);
				sound = false;
				led_off(RED);

				delay(50);
				sound = true;
				led_on(RED);

				delay(50);
				sound = false;
				led_off(RED);


				lcd.clear();
			}
		}else if (inp != -1) {						// If the user didn't give us an enter press we assume they want to add more
			if (p_ < ANSWER_MAX_LENGTH) {			// digits to the asnwer
				user_answer = (user_answer * 10) + inp;
				p_++;
			}
		}

		lcd.setCursor(0,1);							// Move the LCD cursor and pint the problem

		lcd.print(op_a);

		lcd.print(' ');
		lcd.print(display_op);
		lcd.print(' ');

		lcd.print(op_b);
		lcd.print(" = ");

		if (user_answer != 0) {
			lcd.print(user_answer);					// If the user has entered something; print it. 0 is not printed
		}
	}

	stop_music ();									// Either we're out of time or dead; stop the music and exit with false
	return false;
}

/*
	Main game loop
*/

void startGame (short difficulty) {

	randomSeed(ADConvert(B0000));					// This is a quick way of seeding the RNG		
													// we read a value from an unconnected ADC pin
	short progress = 0;								// Progress keeps track of how many digits the user has unlocked

	short correct_code[DIGITS];						// Table for storing the secret code
	char display_code[DIGITS];						// This table is printed to the screen; Digits are
													// moved to this table when they've been unlocked
													// replacing the asterisks
	for (short i = 0; i < DIGITS; i++){				// Fill the display table with asterisks
		display_code[i] = '*';
	}

	for (short i = 0; i < DIGITS; i++){				// Generate a random secret code
		correct_code[i] = random(0, 10);
	}

	switch (difficulty) {							// Adjust timelimit and max life based on the difficulty setting
	    case 1:
				life = MAX_LIFE;
				time_limit = 45;
	      break;

	    case 2:
	    	life = 3;
				time_limit = 30;
	      break;

	    case 3:
	    	life = 3;
				time_limit = 15;
	}

	victory = false;								// Reset flags
	timeout = false;
	dead = false;


	select_beep ();

	lcd.clear();

	lcd.setCursor(0, 0);
	lcd.print("GAME STARING");
	lcd.setCursor (0, 1);
	lcd.print("Difficulty: ");
	lcd.print(difficulty);

	delay (2000);

	while (!victory) {								// While the victory flag is not set; print the display code
													// ( unlocked digits )
		printDisplayCode(display_code);
		delay (3000);
		puzzleCountDown ();			

		time_left = time_limit;						// Reset remaining time between each new puzzle
		last_ = -1;

		if (newPuzzle(difficulty)){					// Generate a new puzzle; if the user answers right; unlock a digit
			display_code[progress] = '0' + correct_code[progress];

			if (progress < DIGITS - 1) {			// Check if all digits have been unlocked; if so we're victorius!
				progress++;							// If not; jump to the next digit
			}else{
				victory = true;
			}

		}else{
			break;									// if the user died or ran out of time we break out of the main game loop
		}
	}

	if (victory) {									// The main loop exited; did we win?
		printDisplayCode(display_code);				// Flash the green led, play some happy music and display the secret code fully
		play_song(WIN_MUSIC, NO_LOOP);

		led_on(GREEN);
		printVictoryScreen();
		delay(6000);
		led_off(GREEN);
	}else{
		defeat();									// Seems we didn't win :(
	}
}