/*
	MUSIC HEADER

	This file contains music definitions, sound controls and playback functions for the game
	Each song is stored as a seperate <short> array in program memory (PROGMEM); and is made
	up by a collection of notes (like real music). Each note is defined by 3 data fields -
	[length] [pitch] [pause]. Length and pause are defined as multiples of ECLK which runs
	at roughly 85 Hz. The actual length of a note can be calculated as x = n * (1/a) ;
	where n is the defined length of the note in the array and a is the clock speed of ECLK.
	Pause is the time after a note when no sound is played. Its real length can be 
	calculated with the same formula,  x = n * (1/a).

	Pitch is a compare match control value for TIMER2 (OCR2A). It controls how often TIMER2
	triggers a compare match interrupt (TIMER2_COMPA_vect); a higher value results in a
	lower pitch beign generated.

	BUGS:
		- main game music doesn't loop correctly. Seems like it loops fine but every other
		  loop is silent. It works fine outside the game loop. 
		  FIXED: added c_ = 0; upon looping.
		- Odd low tone being generated after a song finishes. Doesn't happen with all songs.
*/

#define PITCH_OFFSET 			1 			// Offsets to the note pointer for pitch and pause
#define PAUSE_OFFSET 			2 

#define NOTE_STEPPING 			3 			// There are 3 data fields per note; stepping to the next note requires 3 moves

#define LOOP 					true		// Makes 'play_song ()' function calls a bit better visually
#define NO_LOOP 				false

#define GAME_MUSIC 				0 			// Lengths and identifiers for our little tunes
#define GAME_MUSIC_LENGTH 		84 
#define WIN_MUSIC 				1
#define WIN_MUSIC_LENGTH 		111
#define MEGAMAN 				2
#define MEGAMAN_LENGTH 			450
#define DEATH_MUSIC				3
#define DEATH_MUSIC_LENGTH		30
#define CANON 					4
#define CANON_LENGTH 			339
#define MARIO 					5
#define MARIO_LENGTH			72
#define POWERON 				6
#define POWERON_LENGTH			9

const short *note_ptr;						// Pointer to the current note we're playing
const short *start_ptr;						// Pointer to the start address so we can loop easily

short song_length 			=	0;			// Stores the length of the song in number of notes

volatile short c_ 			=	0;			// Ticks at the rate of ECLK, down

bool pause 					=	false;		// Indicates that a note pause is happening
bool music_enabled 			=	false;		// Are we playing a jam ?
bool loop_enabled 			=	false;		// Should we loop at the end ?

/*
	MUSIC! *yay*
	Note and pause lengths are defined as multiples of the external clock period (ECLK), which is about 10ms
	so if you wish to use delay() for timing multiply the lengths by 10
*/

/*
	Main game music (looping)
	This plays when we're solving a puzzle
*/

const short PROGMEM game[GAME_MUSIC_LENGTH ] = {
	10, 80, 1,
	10, 80, 1,
	10, 80, 1,
	10, 80, 1,
	10, 80, 1,
	20, 106, 1,
	20, 100, 1,
	10, 100, 1,
	20, 50, 1,
	10, 100, 1,
	10, 100, 1,
	20, 50, 1,
	10, 119, 1,
	10, 119, 1,
	10, 60, 1,
	10, 119, 1,
	10, 106, 1,
	10, 106, 1,
	10, 53, 1,
	10, 106, 1,
	10, 100, 1,
	10, 100, 1,
	10, 50, 1,
	10, 100, 1,
	10, 89, 1,
	10, 89, 1,
	10, 45, 1,
	10, 89, 1
};

const short PROGMEM canon[CANON_LENGTH] = {
	24, 36, 0,
	12, 42, 0,
	12, 40, 0,
	24, 36, 0,
	12, 42, 0,
	12, 40, 0,
	12, 36, 0,
	12, 71, 0,
	12, 63, 0,
	12, 56, 0,
	12, 53, 0,
	12, 47, 0,
	12, 42, 0,
	12, 40, 0,
	24, 42, 0,
	12, 53, 0,
	12, 47, 0,
	24, 42, 0,
	12, 84, 0,
	12, 80, 0,
	12, 71, 0,
	12, 63, 0,
	12, 71, 0,
	12, 80, 0,
	12, 71, 0,
	12, 53, 0,
	12, 56, 0,
	12, 53, 0,
	24, 80, 0,
	12, 63, 0,
	12, 71, 0,
	24, 80, 0,
	12, 84, 0,
	12, 95, 0,
	12, 84, 0,
	12, 95, 0,
	12, 106, 0,
	12, 95, 0,
	12, 84, 0,
	12, 80, 0,
	12, 71, 0,
	12, 63, 0,
	24, 80, 0,
	12, 63, 0,
	12, 71, 0,
	24, 63, 0,
	12, 56, 0,
	12, 53, 0,
	12, 71, 0,
	12, 63, 0,
	12, 56, 0,
	12, 53, 0,
	12, 47, 0,
	12, 42, 0,
	12, 40, 0,
	12, 36, 0,
	24, 42, 0,
	12, 53, 0,
	12, 47, 0,
	24, 42, 0,
	12, 47, 0,
	12, 53, 0,
	12, 47, 0,
	12, 56, 0,
	12, 53, 0,
	12, 47, 0,
	12, 42, 0,
	12, 47, 0,
	12, 53, 0,
	12, 56, 0,
	24, 53, 0,
	12, 63, 0,
	12, 56, 0,
	24, 53, 0,
	12, 106, 0,
	12, 95, 0,
	12, 84, 0,
	12, 80, 0,
	12, 84, 0,
	12, 95, 0,
	12, 84, 0,
	12, 71, 0,
	12, 80, 0,
	12, 71, 0,
	24, 63, 0,
	12, 53, 0,
	12, 56, 0,
	24, 63, 0,
	12, 71, 0,
	12, 80, 0,
	12, 71, 0,
	12, 80, 0,
	12, 84, 0,
	12, 80, 0,
	12, 71, 0,
	12, 63, 0,
	12, 56, 0,
	12, 53, 0,
	24, 63, 0,
	12, 53, 0,
	12, 56, 0,
	24, 53, 0,
	12, 56, 0,
	12, 63, 0,
	12, 56, 0,
	12, 53, 0,
	12, 47, 0,
	12, 53, 0,
	12, 56, 0,
	12, 53, 0,
	12, 63, 0,
	12, 56, 0,
	48, 53, 1
};

/*
	Game victory music (not looping)
	This plays when we've won
*/

const short PROGMEM win[WIN_MUSIC_LENGTH] = {
	6, 22, 0,
	6, 24, 0,
	6, 30, 0,
	6, 36, 0,
	6, 40, 0,
	6, 45, 0,
	6, 45, 0,
	6, 45, 0,
	6, 40, 0,
	6, 36, 0,
	6, 34, 0,
	6, 30, 0,
	6, 27, 0,
	6, 24, 0,
	12, 22, 0,
	6, 22, 0,
	6, 24, 0,
	6, 30, 0,
	6, 36, 0,
	6, 40, 0,
	6, 45, 0,
	6, 45, 0,
	6, 45, 0,
	6, 40, 0,
	6, 36, 0,
	6, 34, 0,
	6, 30, 0,
	6, 27, 0,
	6, 24,0 ,
	12, 22, 0,
	24, 28, 6,
	10, 28, 2,
	30, 28, 0,
	24, 25, 12,
	24, 28, 12,
	48, 25, 0,
	120, 22, 1
};

/*
	Start of the Max Payne 2: Fall of Max Payne theme
	Suits the occasion
*/

const short PROGMEM death[DEATH_MUSIC_LENGTH] = {
	57, 80, 0,
	26, 100, 0,
	83, 127, 0,
	41, 151, 0,
	41, 159, 0,
	62, 106, 0,
	62, 159, 0,
	62, 106, 0,
	62, 100, 0,
	109, 119, 1
};

/*
	Run you little blueman. Run!
*/

const short PROGMEM megaman[MEGAMAN_LENGTH] = {
	12, 56, 4,
	4, 47, 4,
	4, 47, 4,
	12, 47, 4,
	4, 47, 4,
	4, 47, 4,
	16, 47, 4,
	16, 56, 16,
	4, 56, 4,
	4, 56, 4,
	12, 47, 4,
	4, 47, 4,
	4, 47, 4,
	12, 47, 4,
	12, 56, 21,
	12, 38, 4,
	12, 42, 4,
	12, 38, 21,
	4, 47, 4,
	4, 47, 4,
	12, 47, 4,
	4, 47, 4,
	4, 47, 4,
	12, 47, 4,
	12, 56, 21,
	12, 38, 21,
	12, 42, 21,
	12, 47, 21,
	12, 42, 63,
	4, 42, 4,
	4, 42, 4,
	12, 42, 4,
	4, 42, 4,
	4, 42, 4,
	12, 42, 4,
	12, 50, 21,
	12, 38, 21,
	12, 42, 21,
	12, 47, 21,
	12, 50, 21,
	12, 56, 21,
	12, 56, 4,
	12, 38, 4,
	12, 32, 4,
	42, 34, 8,
	12, 56, 21,
	12, 56, 4,
	12, 38, 4,
	12, 32, 4,
	33, 34, 0,
	12, 25, 4,
	12, 24, 21,
	4, 47, 4,
	4, 47, 4,
	12, 47, 4,
	4, 47, 4,
	4, 47, 4,
	12, 47, 4,
	16, 56, 21,
	4, 56, 4,
	4, 56, 4,
	12, 47, 4,
	4, 47, 4,
	4, 47, 4,
	12, 47, 4,
	12, 56, 21,
	12, 38, 4,
	12, 42, 4,
	12, 38, 21,
	4, 47, 4,
	4, 47, 4,
	12, 47, 4,
	4, 47, 4,
	4, 47, 4,
	12, 47, 4,
	12, 56, 21,
	12, 38, 21,
	12, 42, 21,
	12, 47, 21,
	12, 42, 63,
	4, 42, 4,
	4, 42, 4,
	12, 42, 4,
	4, 42, 4,
	4, 42, 4,
	12, 42, 4,
	12, 50, 21,
	12, 38, 21,
	12, 42, 21,
	12, 47, 21,
	12, 50, 21,
	12, 56, 21,
	12, 56, 4,
	12, 38, 4,
	12, 32, 4,
	42, 34, 4,
	12, 56, 21,
	12, 56, 4,
	12, 38, 4,
	12, 32, 4,
	33, 34, 0,
	12, 32, 4,
	96, 28, 21,
	12, 32, 4,
	29, 24, 4,
	29, 28, 4,
	29, 32, 4,
	29, 28, 4,
	71, 32, 12,
	12, 32, 4,
	46, 28, 4,
	12, 38, 4,
	12, 36, 4,
	12, 38, 4,
	12, 47, 21,
	12, 47, 4,
	12, 38, 4,
	12, 32, 4,
	79, 28, 21,
	12, 32, 4,
	29, 24, 4,
	29, 28, 4,
	29, 32, 4,
	29, 28, 4,
	79, 32, 4,
	12, 32, 4,
	12, 38, 4,
	12, 32, 4,
	12, 30, 21,
	12, 30, 4,
	12, 30, 4,
	12, 25, 4,
	93, 19, 4,
	80, 75, 21,
	12, 84, 4,
	29, 63, 4,
	29, 71, 4,
	29, 75, 4,
	29, 71, 4,
	96, 75, 21,
	12, 84, 4,
	29, 63, 4,
	29, 71, 4,
	29, 75, 4,
	29, 71, 4,
	71, 95, 12,
	12, 95, 4,
	12, 84, 4,
	12, 75, 4,
	130, 100, 1
};

/*
	A must have, perhaps?
*/

const short PROGMEM mario[MARIO_LENGTH] = {
	6, 24, 6,
	6, 24, 18,
	6, 24, 18,
	6, 30, 6,
	6, 24, 18,
	6, 20, 42,
	6, 40, 42,
	6, 30, 30,
	6, 40, 30,
	6, 47, 30,
	6, 36, 18,
	6, 32, 18,
	6, 34, 6,
	6, 36, 18,
	6, 40, 12,
	6, 24, 8,
	6, 20, 10,
	6, 18, 18,
	6, 22, 6,
	6, 20, 18,
	6, 24, 18,
	6, 30, 6,
	6, 27, 6,
	6, 32, 30

};

const short PROGMEM poweron[POWERON_LENGTH] = {
	12, 24, 12,
	12, 24, 4,
	30, 20, 1
};

/*
	This is used select which song should be played
	The second parameter (looping) controls if the
	song should be played in a loop or not

	Please, only use the #defined names
*/

void play_song (short song, bool looping) {

	music_enabled = true;
	loop_enabled = looping;

	switch (song) {
		case GAME_MUSIC:
			start_ptr = &game[0];
			song_length = GAME_MUSIC_LENGTH;
			break;

		case WIN_MUSIC:
			start_ptr = &win[0];
			song_length = WIN_MUSIC_LENGTH;
			break;

		case MEGAMAN:
			start_ptr = &megaman[0];
			song_length = MEGAMAN_LENGTH;
			break;

		case DEATH_MUSIC:
			start_ptr = &death[0];
			song_length = DEATH_MUSIC_LENGTH;
			break;

		case CANON:
			start_ptr = &canon[0];
			song_length = CANON_LENGTH;
			break;

		case MARIO:
			start_ptr = &mario[0];
			song_length = MARIO_LENGTH;
			break;

		case POWERON:
			start_ptr = &poweron[0];
			song_length = POWERON_LENGTH;
			break;
	}

	note_ptr = start_ptr;
}

/*
	Quick function for stopping playback
*/

void stop_music () {

	music_enabled = false;
}