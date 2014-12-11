/*
	GLYPH HEADER
*/

/*
	These byte arrays contain some custom glyphs for the LCD display
*/

byte smiley[8] = {
	B00000,
	B10001,
	B00000,
	B00000,
	B10001,
	B01110,
	B00000,
	B00000
};

byte arrow[8] = {
	B00000,
	B00100,
	B01110,
	B10101,
	B00100,
	B00100,
	B00100,
	B00000
};

byte underline[8] = {
	B11111,
	B11111,
	B00000,
	B00000,
	B00000,
	B00000,
	B00000,
	B00000
};

byte checker[8] = {
	B10101,
	B01010,
	B10101,
	B01010,
	B10101,
	B01010,
	B10101,
	B01010
};

byte heart[8] = {
	B00000,
	B11011,
	B11111,
	B11111,
	B01110,
	B00100,
	B00000,
	B00000
};

byte heart_empty[8] = {
	B00000,
	B11011,
	B10101,
	B10001,
	B01010,
	B00100,
	B00000,
	B00000
};

byte heart_half[8] = {
	B00000,
	B11011,
	B11101,
	B11001,
	B01010,
	B00100,
	B00000,
	B00000
};

byte tnt_t0[8] = {
	B11111,
	B10000,
	B10111,
	B10010,
	B10010,
	B10010,
	B10000,
	B11111
};

byte tnt_t1[8] = {
	B11111,
	B00001,
	B11101,
	B01001,
	B01001,
	B01001,
	B00001,
	B11111
};


byte tnt_n[8] = {
	B11111,
	B00000,
	B11001,
	B11101,
	B10111,
	B10011,
	B00000,
	B11111
};

byte tnt_fuse[8] = {
	B00000,
	B00000,
	B00000,
	B11001,
	B00110,
	B00000,
	B00000,
	B00000
};

/*
	This fills the LCD screen with a checker pattern
*/

void fillWithChecker () {

	lcd.clear();

	for (int i = 0; i < 2; i++) {
		lcd.setCursor(0, i);
		for (int j = 0; j < 16; j++) {
		  lcd.write(byte(3));
		}
	}
}

/*
	Functions for loading custom glyph sets
*/

void load_game_glyphs () {
	lcd.createChar(0, smiley);
	lcd.createChar(1, arrow);
	lcd.createChar(2, underline);
	lcd.createChar(3, checker);
	lcd.createChar(4, heart);
	lcd.createChar(5, heart_empty);
	lcd.createChar(6, heart_half);
}

void load_intro_glyphs () {
	lcd.createChar(0, tnt_t0);
	lcd.createChar(1, tnt_t1);
	lcd.createChar(2, tnt_n);
	lcd.createChar(3, tnt_fuse);
}