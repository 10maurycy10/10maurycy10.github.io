---
title: "Modifying a radiation meter for (radioactive) rock collecting:"
tags: ["radiation", "electronics"]
date: "2025-09-18"
---

The Ludlum Model 3 is quite a nice radiation meter, as long as you like analog displays and don't mind it weighing one and a half kilograms:

![An old style survey meter, designed for an external probe.](/projects/ludlum3/meter1.jpg)

These can be found used for a reasonable price, are easy to fix, nearly indestructible, and have an adjustable HV power supply that will drive just about any Geiger tube or scintillation detector you can throw at it. 

Geiger tubes are mostly sensitive to alpha and beta particles:
great for detecting surface contamination and measuring weakly active specimens but nearly useless for finding them:
All it takes is a bit of dirt on top of the rock it and the counter will detect nothing. 

Scintillation counters are super super sensitive to gamma radiation, which punches right though dirt and rock. 
However, because of this, they pick up background of several hundred counts per second, making the audio clicks completely useless.
Without audio, you have to constantly look at the meter face, guaranteeing that you trip on something. 

Using the meter for rock hounding is certainly possible, but it's not fun.

The most obvious solution is to divide down the clicks with a counter.
While this is a huge improvement over the raw output, it's not perfect:
All the background radiation picked up by a scintillator makes weaker signals imperceptible.

Would you notice if one click came a few milliseconds earlier then the last?

I certainly can't, but a computer has no problem:

![Schematic diagram showing how to connect a microcontroller to replace the audio section of a Ludlum meter.](/projects/ludlum3/tone.png)
<center style="color: gray">Desecrating an analog meter.</center>

Source code: [tone.c](/projects/ludlum3/tone.c) <br>
Prebuilt binary: [tone.elf](/projects/ludlum3/tone.elf)

The microcontroller measures the current count rate, subtracts a background reading and converts the difference into an audible tone.

On my meter, I got switch controlled power from the reset button and the event signal from pin 3 of the CD4093BE IC.
Because the CD4093BE runs at a high voltage then the battery, I added a 22k resistor to protect the MCU.
The Ludlum Model 3 has had multiple redesigns, your meter will probably be different: 
find an appropriate manual or poke around with an oscilloscope.

<div style="border: 1px yellow solid; padding: .5em;">
If you don't have a UPDI programmer, you can improvise one using a USB to UART adapter: 
Connect a 1k resistor between the adapter's Tx and Rx pins, and connect the Rx side of the resistor to the UPDI pin. 
<br><br>
Also, if you use an Atmel ICE programmer, you might need to update the firmware to avoid an unhelpful "initilization failed" error.
</div>

Here's what the circuit looks like wired into my meter:

![The same meter, opened up and with some extra wires.](/projects/ludlum3/meter2.jpg)

The microcontroller is wrapped in tape to avoid shorts, and fits in the gap between the board and case when the meter is closed.
I made sure to keep wires away from the high voltage section: a stray arc could easily destroy the microcontroller. 
After taking the photo, I tapped the microcontroller to the board so it doesn't get trapped under one of the transformers or other large parts.

Here's the circuit detecting some weakly active granite, that measures ~10% above background: (10 CPS on my detector, and around 0.5 CPS a Radiacode 102)

<video width='100%' controls>
<source type=video/mp4 src=rock.mp4>
</video>
<center style="color: gray">Fairly quiet audio, you might need to turn up your volume.</center>

This is a good demonstration of the squelch and background subtraction:
A ten percent increase over background would normally be imperceptible.
In the field, an increase like this can indicate a deeply buried specimen that would otherwise be missed.

The same circuit should also work in other meters, but you'll be on your own have to find needed signals:
The circuit needs power, ground and a digital event signal.
You might need to adjust some parameters in the code depending on your scintillator's sensitivity.

Because it relies on making fast and accurate measurements, I wouldn't recommend it for small detectors like the Radiacode, Raysid, or Better Geiger:
Anything with a background rate below 30 counts/second is unlikely to work well. 

If you want something cheap, look around for used scintillators:
The 1.25\*1.25\*3 inch CsI(Tl) detector in the video cost me $60 delivered, and it's got a crystal 76 times bigger then the Radiacode 102's.

## Usage instructions:

When powered on, the meter will beep once, take a three second calibration measurement, and then beep a second time to indicate that it is ready to use.
During operation, it takes a one second sliding-window average and outputs a tone depending on the measured count rate.
The audio is turned off if the count rate is not significantly above the background measurement, which preserves power and makes hot spots easier to notice. 

If the sound activates, simply go towards the direction in which the pitch increases until you find the source. 

## Troubleshooting:

If ...

... No audio is produced:
Try disconnecting the "Audio switch" line. 
If this fixes it, you wired it to the wrong side of the switch.
Otherwise, check that the microcontroller has power, has been programmed, and that the speaker amplifier is wired correctly.

... The meter beeps 10 times after calibration:
No counts were detected during calibration. 
Make sure the probe is connected and working, and check the wiring. 

... The meter makes noise even with no radiation source:
This happens if the meter is taken to an area with more background radiation then during calibration.
Recallibrate the background by turning it off, and then on again. 

... The meter is beeping:
This happens if the reading is below the callibrated background, usually because the meter was taken to a different area. 
Reset the background by turning it off, and then on again. 

... The meter randomly screams and pegs the needle: 
This is usually caused by a bad connection somewhere, most commonly in the cable. 
