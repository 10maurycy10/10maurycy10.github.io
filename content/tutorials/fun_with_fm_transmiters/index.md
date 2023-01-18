---
title: "Fun with tiny FM transmitters"
date: 2023-01-17T17:11:23-08:00
tags: ["wireless"]
draft: false
---

It is surprisingly easy to make a working FM transmitter with just one transistor and a handful of passive components.

![Schematic diagram of a simple FM transmitter](base.png)

This circuit runs from a single 9 volt battery.
The coil is 6 turns of solid core hook up wire wrapped around a 3mm form, and the antenna is 30-90cm of wire.
With a audio signal from a headphone output is connected between ground and the 100nF capacitor, the audio can be head on a nearby FM radio.
Another 100nF capacitor can be connected across the power supply/battery to improve performance.

The frequency should be adjusted to an empty channel by spreading out/compressing the coil.

# A simple bug

![Schematic diagram of transmitter with microphone](bug.png)

The addition of an electret microphone and use of a 9v battery for power turns allows this transmitter to transmit your voice, or any other conversations in a room.
(Note that using such a device to listen in on people without there consent in illegal in all of the US)

This wont pick up quite noises in the room, for that, an audio amplifier stage is required:

![Schematic diagram of a two transistor transmitter](bug2.png)

Both of these circuits can be built small enough to fit neatly on top of the battery.

# Fox hunting beeper

This is a two (three with the second RF stage describe later) transistor device, that transmits a beeping sound, functioning as a beacon.
The flashing LED can be replaced with a wire, this results in a constant tone.

![A schematic of a radio beacon](beacon.png)

# Improving frequency stability

These circuits work but the frequency changes a lot from something as small as moving your hand next to the antenna, and sometimes the oscillation breaks down entirely.
The solution is another RF stage (this has the added benefit of reducing radiated harmonics):

![A transmitter with with an RF stage for isolation and trf filter](trf.png)

Keep everything outside of the green box the same as whatever circuit you want to modify.

# Antenna

Ideally the antenna would be two identical wires, one one the output, and one on the circuits ground (0v line). 
In practice, it still performs well when one side is much sorter than the other. (even just the size of the circuit itself)
If the circuit is mounted in a metal box, take advantage of it by connecting it to the negative side of the battery.

# Legality

This section only applies to the US.
From my understanding of part 15 of the FCC rules, low power transmitters are allowed in the FM broadcast band (and quite a few other places) from 88 to 108 MHz.
As long as you don't go outside of that range (you can check with an oscilloscope or FM radio), or transmit with higher power than these circuits with the recommended antenna, you are not breaking the law.
The actual limit for signals with less than 200kHz bandwidth on the FM band is 250 µV/m @ 3 m
