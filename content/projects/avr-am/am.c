// A super simple AM transmitter, requiring an avr and a single PNP transistor.
//
// The microcontroler must be clocked at 1 MHz (or your desired transmit frequency), and have clock output enabled
// For an atmega328p (with an 8MHz internal oscillator), setting the lfuse byte to 0x22 is fine.
//
// Magnetic loop antennas are very effective. 
//
// (Avrdude: -U lfuse:w:0x22:m)
//
// External circuit:
//
// PB0/CLKO-----+
//              |
//           | /
// PD7 ------|(
//           | V
//              \
//               +---- To antenna
//               |
//               +-\/\/\/\--- GND
//                  optional resistor (300ohms) (increases modulation depth)
//
// Don't use this for a high power transmission, as the square wave output it 
// creates has a lot of harmonics, which will cause a lot of interference on other frequencies.

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define NPS 4
#define BEEP_LENGTH (1000/NPS)
#define PRESCALER 64

// Declared as volatile to stop the compiler from thinking these are unnecessary.
volatile int cycles = 500; // (scaled) cycles between toggles between toggles
volatile int on = 0;

// Timer over flow interrupt, called when timer reaches 65536 (2**16)
ISR (TIMER0_OVF_vect) {
	// If playing sound, toggle pin
	if (on)
		PORTD ^= (1 << 7);
	// set timer back for next toggle	
	TCNT0 = (65536 - (cycles));
}

// Play a tone
void beep(int freq, int ms) {
	int delay = (1000000 / (freq)) / 2; // Time wait between toggles, in us
	cycles = (int)(delay * (F_CPU / 1000000) / PRESCALER); // Time to wait, in timer cycles
	on = 1;
	for (int i = 0; i < ms; i++)
		_delay_ms(1);
	on = 0;
}

void note(int freq) {
	beep(freq, BEEP_LENGTH);
}

void initTimer0() {
	// Setup timer,  preschooler to 64
	// This is used to create the audio waveform
	TCCR0B = 0b11;
	TCNT0 = 0;
	TIMSK0 |= (1 << TOIE1);

}

int main() {
	// Set outputs
	DDRB=0xFF;
	DDRD=0xFF;
	initTimer0();
	sei();
	int counter = 0;
	while (1) {
		// A simple 2 tone station marker.
		note(1000);
		note(500);
		_delay_ms(250);
		counter ++;
	}
	return 0;
	
}
