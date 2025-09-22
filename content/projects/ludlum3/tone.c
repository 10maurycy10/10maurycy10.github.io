// A MCU program to produce usable audio from a scintillation detectors. 
// Intended to be used for modifing existing counters, but this could build a 
// detector around it. 
//
// When powered up, it will make a measurement of the background radiation.
// It is important to do this far from any radiation sources.
//
// Once this callibration is complete, it will monitor the current count rate,
// and if it rises more then one standard deviation above background, it will
// emit a tone with a frequency that increase with the observed count rate.
//
// This turns what would otherwise be a imperceptible increase in count rate into
// a very clear indication, allowing you to make the best use of your scintillator's
// sensitivity. 
//
// The speaker will produce a 440 Hz beep at the start of callibration. Once 
// callibration is finished, it will produce either a 880 Hz beep if everything
// looks good, or 10x 1000 Hz beeps if no counts were seen during callibration.
//
// The detector will repeatedly beep when the measured rate is below background.
// If this happens, you shoud probobly recallibrate the background.
//
// Pinout (AVR128DA28):
// 	6  (PD0): Logic level event input. All meters I've seen have this exposed
//                somewhere on the PCB, often with a convenient test point. 
// 	7  (PD1): Audio enable line. When pulled low, the MCU won't output any
//                sound.
// 	22 (PA6): Audio output. Feed into headphones or an amplifier and speaker. 
//
// Current draw is around 1 mA, but a loud speaker will draw a lot more.

// Length of each sample. 100ms should be fine for most meters. 
#define INTEGRATION_MS 100
// How many samples are averaged to produce a tone.
// This is what I recomend adjusting to set sensitivity.
// A value that is too small results is an agressive squelch 
// and jumpy audio. A value that is too big results in a slow
// response. 
#define AVG_LEN        10
// Number of samples to take during callibration. 
// Increase this if your detector has a low count rate or 
// bakcground subtraction is unreliable. Decrease it if 
// callibration takes too long. 
#define CAL_OVERSAMP   30
// Frequency range used for tones, in Hz.
// Set to taste. 40 Hz is the lowest frequency the MCU can generate.
#define MAX_F 5000  
#define MIN_F 40
// Count rate over background that yields a tone of MAX_F.
// A value that's too big will cause the tones to not change by 
// much, a value that's too small will cause it to max out easly. 
#define MAX_CPS 5000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

#define PORTA_SPEAKER (1 << 6)

// My makefile sets this, but doing it here will make it easier to build by hand.
#ifndef F_CPU
#define F_CPU 4'000'000
#endif

// Callibrated background, measured at startup.
float background; 

// Sliding window averaging 
uint32_t buffer[AVG_LEN];
int buffer_head;
uint32_t running_total;

void avg_init() {
	for (int i = 0; i < AVG_LEN; i++) buffer[i] = 0;
	buffer_head = 0;
	running_total = 0;
}

float avg_feed(uint32_t new_value) {
	// Remove old value from total
	running_total -= buffer[buffer_head];
	// Add new one, updating the buffer
	buffer[buffer_head] = new_value;
	running_total += new_value;
	// Advance buffer, rolling around if needed
	buffer_head++;
	buffer_head %= AVG_LEN;
	return (float)running_total / AVG_LEN;
}

uint8_t audio_on;
float long_avg = 10; // Used for under-background alarm.

int32_t map_frequency(int32_t raw_count) {
	// Background subtraction. 
	float rate = avg_feed(raw_count) - background;
	// Standard deviation assuming shot noise.
	float sigma = sqrt(background * AVG_LEN)/AVG_LEN;
	
	// Under background warning
	long_avg *= 0.98;
	long_avg += rate * 0.02;
	if (long_avg < -1.5*sigma) return -1;
	
	// Squelch, stops sound if the count rate is not significantly above background.
	if (rate <= 1.0*sigma) audio_on = 0;
	if (rate >  3.5*sigma) audio_on = 1;
	if (!audio_on) return 0;

	// Scaling to max count rate
	float max_events = MAX_CPS / 1000 * INTEGRATION_MS;
	float normalized = rate / max_events;
	
	// Linear frequency interpolation
	float frequency = MIN_F*(1-normalized) + MAX_F*normalized;
	if (frequency > MAX_F) frequency = MAX_F;
	if (frequency < MIN_F) frequency = 0;
	return frequency;
}



// Pulse counting interupts
volatile uint32_t counts;
ISR(PORTD_PORT_vect) {
	// Record event
	counts += 1;
	// Clear interupt flags:
	// this stops the MCU from trigging the interupt continusly.
	PORTD.INTFLAGS = 1; 
}

// Sound playback using timer interrupts from TCA0 (20 Hz to >20 kHz),
// handles the audio on/off switch.
ISR(TCA0_OVF_vect) {
	// Toggle speaker
	if (PORTD.IN & 0b10) {
		PORTA.OUT ^= PORTA_SPEAKER;
	} else {
		PORTA.OUTCLR = PORTA_SPEAKER;
	}
	// Clear interupt
	TCA0.SINGLE.INTFLAGS = 1; 
}

// Sets the timer to generate a tone at the specified frequency
// Passing zero turns off the sound. 
// -1 Will cause a beep, which is used for the below-background warning. 
void play_tone(int32_t frequency) {
	if (frequency == -1) {
		play_tone(2000);
		_delay_ms(50);
		play_tone(0);
	} else if (frequency == 0) {
		TCA0.SINGLE.CTRLA = 0;
		PORTA.OUTCLR = PORTA_SPEAKER;
	} else {
		uint32_t cycles = 2'000'000 / frequency;
		TCA0.SINGLE.PER = cycles;
		TCA0.SINGLE.CTRLA = 1;	
	}
}

int main() {
	// Trigger interupts on rising edges of input
	PORTD.PIN0CTRL = 0x2; 
	// Pull up so audio is enabled if the switch is not connected.
	PORTD.PIN1CTRL = (1 << 3);

	// Speaker setup...
	PORTA.DIRSET = PORTA_SPEAKER;	
	TCA0.SINGLE.INTCTRL = 1;    // Enable overflow interupt
	TCA0.SINGLE.PER = 0;	

	sei();
	
	// Wait for meter to stabilize.
	_delay_ms(1000);

	// Startup chime, part one ...
	_delay_ms(200);
	play_tone(440);
	_delay_ms(100);
	play_tone(0);

	// Callibration...
	counts = 0;
	_delay_ms(INTEGRATION_MS * CAL_OVERSAMP);
	background = counts; 
	background /= CAL_OVERSAMP;
	
	if (background == 0) {
		// Loss-of-count warning...
		for (int i = 0; i < 10; i++) {
			play_tone(1000);
			_delay_ms(100);
			play_tone(0);
			_delay_ms(100);
		}
	} else {
		// Startup chime, part two...
		play_tone(440*2);
		_delay_ms(100);
		play_tone(0);
	}


	// Start counting!	
	avg_init();
	while (1) {
		counts = 0;
		_delay_ms(INTEGRATION_MS);
		play_tone(map_frequency(counts));
	}
}
