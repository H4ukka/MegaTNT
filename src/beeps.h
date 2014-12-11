/*
	Honestly these are pretty stupid
	Using delay() is terrible practice
*/

/*
	Functions for creating all sorts of beeps and boops
	Do not call n_() directly unless you know what values to put in
	(it'll make annoying noises at ye!)
*/

//	This controls if we should be playing a sound
bool sound = false;

/*
	This generates a sound of length t with a pause of p_
	Pitch is defined by p; values of p corresponding to standard 
	musical notation can be found OCR2A_frequency_table.pdf

	More info on OCR2A can be found in the music.h header
*/

void n_ (int t, int p, int p_) {

	sound = true;

	OCR2A = p;
	delay (t);
	sound = false;
	delay (p_);
}

void beep () {

	n_ (50, 20, 0);
}

void double_beep () {

	n_ (50, 28, 250);
	n_ (50, 28, 0);
}

void deep_beeps () {

	n_ (126, 40, 21);
	n_ (126, 53, 42);
	n_ (210, 89, 0);
}

void select_beep () {
	
	n_ (42, 20, 21);
	n_ (42, 18, 21);
	n_ (42, 15, 0);
}