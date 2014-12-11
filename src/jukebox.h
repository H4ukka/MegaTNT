/*
	JUKEBOX HEADER
*/

/*
	Want to play some "quality" music, eh?
*/

String selection[] = {
	"Back       ",
	"Megaman 2  ",
	"Max Payne 2",
	"Victory    ",
	"Mario      ",
	"Canon      "
};

/*
	This works excatly like the previous menus

	W just use the sensor value to choose which song
	title to print
*/

void enterJukeBox () {

	bool exitJuke = false;

	lcd.clear();

	while (!exitJuke) {

		bool selected = false;
		enter = false;

		short sel_;

		lcd.setCursor(0,0);
		lcd.print("MegaTNT  Jukebox");

		while (!selected) {

			int sensor = ADConvert(B0100);
			sel_ = map (sensor, 0, 1023, 0, 5);

			checkForEnter();

			lcd.setCursor(1,1);
			lcd.print(selection[sel_]);

			lcd.setCursor(0,1);
			lcd.print(sel_);

			switch (sel_) {
				case 0:
					if (enter) {
						selected = true;
						exitJuke = true;
					}
					break;

				case 1:
					if(enter) {
						selected = true;
						play_song(MEGAMAN, NO_LOOP);
					}
					break;
					
				case 2:
					if(enter) {
						selected = true;
						play_song(DEATH_MUSIC, NO_LOOP);
					}
					break;
					
				case 3:
					if(enter) {
						selected = true;
						play_song(WIN_MUSIC, NO_LOOP);
					}
					break;

				case 4:
					if(enter) {
						selected = true;
						play_song(MARIO, NO_LOOP);
					}
					break;

				case 5:
					if (enter) {
						selected = true;
						play_song(CANON, NO_LOOP);
					}		
			}
		}
	}
}