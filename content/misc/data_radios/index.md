---
title: "Long range links with tiny radio modules"
date: 2024-03-30T14:08:05-06:00
tags: ["wireless", "programming"]
draft: false
---

WiFi and Bluetooth capable microcontrollers are very popular for hobby projects, but at the best of times offer a range of around a hundred meters.
Indoors, with with walls and other obstacles, the usable range is often less then 20 meters.

A radio using a much lower data rate and frequency can achieve many kilometers of range[^range].
The low data rate allows even a very weak signal to be received, and lower frequencies are much better at going through walls, trees and other objects.

Most of the time the data being sent is just a few bytes, such as a sensor reading, button press, GPS fix, battery level, or similar so low speeds hardly matter.

# The radio itself:

I used the RFM69HCW modules, because it includes a fairly capable transceiver and all supporting components on a small 16 by 16 mm board.
All the annoying parts like clock recovery and byte synchronization are built into the module, making them very easy to use.

Like most lower frequency radio modules, the RFM69HCW does not have a built in antenna, fortunately one of the best (omni-directional) antennas is super simple:
the monopole, just a straight length of wire, cut to a 1/4 wavelength.
For the 434/433 MHz, a 1/4 wavelength is 17.2 centimeters and for 915 MHz it is 8.2 centimeters.

Smaller antennas are like loaded monopoles and chip antennas do exist, but they are less efficient, and waste some (or most) of the signal as heat.

A monopole relies on the rest of the device to as a ground plane, if the device is small, performance can be improved by adding a 1/4 wavelength ground wire in the opposite direction as the antenna.
This wire should ideally be attached to one of the ground pins on the module.

# Talking to the radio:

An Arduino library for the module does exist, but it is limited to a small number of microcontrollers.
In any case, writing a driver for it is not difficult, and all the information needed is in the [datasheet](https://www.mouser.com/datasheet/2/813/RFM69HCW_V1_1-2490219.pdf).

All of the communication is done over SPI[^spi], which quite a simple protocol.
Most microcontrollers have built in SPI hardware, but it is quite easy to implement from scratch:

```c
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// SPI Driver, for an AVR style API, assumes radio module is on PORTD

// Pins in PORTD
#define MISO    0b0001
#define MOSI    0b0010
#define SCK     0b0100
#define CS      0b1000

uint8_t spi_xfer(uint8_t tx) {
	uint8_t rx = 0;
	for (uint8_t i = 0; i < 8; i++) { // Send each bit
		// Shift data in
		_delay_us(5);
		rx = rx << 1;
		if (PIND & MISO) rx |= 1;
		// Shift out
		if ((tx << i) & 0b10000000) {
			PORTD = MOSI;
		} else {
			PORTD = 0;
		}
		// Strobe clock
		_delay_us(5);
		PORTD |= SCK;
		_delay_us(5);
		PORTD &= !SCK;
	}
	return rx;
}
```

The registers on the module can be modified by sending a 1, a 7 bit address and then sending the desired value.
Reads are done much the same except the first bit is a zero, and after the address the MCU sends dummy data while storing the response back from the module.
A small catch is that the chip must be deselected by pulling Chip Select high between accesses.

From here, the datasheet provides a large number of knobs to tinker with, but only a handful need to be set to use the radio.

To transmit, fill the buffer (at address 0) with data, starting with a length byte and switch the radio into transmit mode until it indicates it is done with the `PacketSent` flag.
to receive, switch the radio into receive mode, wait for the `PayloadReady` flag and read out the data from the buffer.

```c
// Register operations

void radio_write(uint8_t addr, uint8_t data) {
	PORTD = CS;
	_delay_us(5);
	spi_xfer((1 << 7) | addr);
	spi_xfer(data);
	PORTD = CS;
	_delay_us(5);
}

uint8_t radio_read(uint8_t addr) {
	PORTD = CS;
	_delay_us(5);
	spi_xfer(addr);
	uint8_t rx = spi_xfer(0);
	PORTD = CS;
	_delay_us(5);
	return rx;
}
```

```c
// Utility functions

// All these magic values came from the datasheet, it
// would be a good idea to define macros or constants
// for them to make errors easier to notice.

// Set frequency in Hz
void set_frequency(uint32_t target) { 
	uint32_t setting = target / 61.03515625;
	radio_write(0x07, setting >> 16);
	radio_write(0x08, setting >> 8);
	radio_write(0x09, setting);
}

// Load sane default settings
void configure_radio() {
	// PA Settings: -14 dBm + 15 = +3 dBm = 2 mW output
	// 2 mW is enough for most cases.
	radio_write(0x11, 0b01100000 | 14);
	// Packet Mode: Variable length + CRC
	radio_write(0x37, 1 << 7 | 1 << 4);
	// Sync Word: 0x01, 8 times
	for (uint8_t i = 0; i < 8; i++) radio_write(0x2F+i, 1);
	// FIFO Threshold: Transmit any data in buffer
	radio_write(0x3C, 0xff);
}

// Switch operating mode and wait for radio to be ready
void switch_mode(uint8_t mode) {
	radio_write(0x1, mode);
	// Wait for radio to switch modes
	while (~radio_read(0x27) & (1<<7));
}
```

Transmitting:

```c
int main() {
	// Setup microcontroler.
	DDRD = 0b1110;
	PORTD = CS;
	_delay_ms(10);
	// Radio setup.
	configure_radio();
	set_frequency(434000000); // Placeholder frequency
	// Transmit a packet every second.
	while (1) {
		// Length of packet, includes length byte.
		// Max: 66 bytes.
		radio_write(0x0, 5);
		// Placeholder data.
		radio_write(0x0, 0xde);
		radio_write(0x0, 0xad);
		radio_write(0x0, 0xbe);
		radio_write(0x0, 0xaf);
		// Switch to transmit mode.
		switch_mode(3 << 2);
		// Wait for completion...
		while (~radio_read(0x28) & 1 << 3 ) ;
		// Back to standby.
		switch_mode(1 << 2);
		// Wait.
		_delay_ms(1000);
	}
}
```

Receiving:

```c
int main() {
	// Setup microcontroler.
	DDRD = 0b1110;
	PORTD = CS;
	_delay_ms(10);
	// Radio setup.
	configure_radio();
	set_frequency(434000000); // Placeholder frequency
	switch_mode(4 << 2); // RX Mode
	while (1) {
		// Wait for data
		while (~radio_read(0x28) & (1<<2));
		// Switch to standby
		switch_mode(1 << 2);
		// Read out the data, should probobly do something with it
		uint8_t length = radio_read(0x0);
		radio_read(0x0); // reads byte 1 of payload
		radio_read(0x0); // reads byte 2 of payload
		// Back to RX, clears buffer
		switch_mode(4 << 2);
	}
}
```

These radios don't have built in acknowledgments or retransmission, these will have to be implemented if the transmitter needs to know if the message was received.
If the receiver need to know if the link works, the transmitteri can periodicaly send heartbeat packet, if one has not been received for a while, the receiver knows the the link is interrupted. 


[^range]: In theory, the RFM69HCW module has enough link budget to communicate from low earth orbit to ground (>700 km), but in most practical use cases the range is limited by local terrain; The signal can't go trough a mountain, or go past the horizon. 

[^spi]: SPI is also used for a bunch of other things, like flash chips, SD cards, sensors, displays, and WiFi modules, so this code can be adapted for those with trivial modifications.
