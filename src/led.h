#define RED 						8		// LED pin defenitions
#define GREEN 						7

#define RED_LED_PORT_PIN			0x01	// Output port pins for LEDs
#define GREEN_LED_PROT_PIN			0x80

/*
	Turn a LED on
	Done by writing a 1 to the PORT register that corresponds to the chosen LED's pin

	We OR the LED's port pin to the current state of the port (A = A or B)
*/

void led_on (byte pin) {

	switch (pin) {
		case RED: 		PORTB = PORTB | RED_LED_PORT_PIN; break;
		case GREEN: 	PORTD = PORTD | GREEN_LED_PROT_PIN;
	}
}

/*
	Turn a LED off
	Done by writing a 0 to the PORT register that corresponds to the chosen LED's pin

	We XOR the LED's port pin to the current state of the port (A = A xor B)
	If the register contains a 0 xoring it with a 1 results in a 0
	If the register contains a 1 xoring it with a 1 results in a 0
	Thus we only affect the LED's register value
*/

void led_off (byte pin) {

	switch (pin) {
		case RED: 		PORTB = PORTB ^ RED_LED_PORT_PIN; break;
		case GREEN: 	PORTD = PORTD ^ GREEN_LED_PROT_PIN;
	}
}

/*
	Double blink the chosen LED
	Using delays is bad practice

	Find a better sollution later...
*/

void led_blink (byte pin) {

	led_on(pin);
	delay(100);
	led_off(pin);
	delay(50);
	led_on(pin);
	delay(100);
	led_off(pin);
	delay(100);

}