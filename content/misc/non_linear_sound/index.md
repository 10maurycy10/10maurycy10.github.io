---
title: "Weekend project: Nonlinear sound propagation"
date: 2024-09-01T18:21:35-06:00
tags: []
draft: false
---

Sound normally behaves linearly, a tone at one frequency won't create or effect one at another.
But this isn't always the case if the sound is loud enough.

As a quick experiment, I took an ultrasonic distance sensor, and connected a function generator directly to the sensor's transducer.
I also connected another transducer to my oscilloscope as an ultrasound detector.

This is the received signal after giving the transmitter 40 kHz at 20 volts:

![](rx_cw.png)

The sound is clearly there, but also completely inaudible;
40 kHz is well above the range of human hearing.
Next, I introduced some amplitude modulation at 1 kHz:

![](rx_mod.png)

Despite the carrier ultrasound being inaudible, I was able to hear a 1 kHz tone.
Interestingly, it sounded like it was coming from the wall the transducer was pointed at.

To confirm the demodulation was not caused by the transducer itself, I pointed the ultrasound beam out an open door.
Doing this almost completely eliminated the sound, but it returned once the door was closed.
The effect worked even better when sending a 100 Hz square wave, going from complete silence to loud buzzing when the door was closed.

So how does the ultrasound create sound at a 400 times lower frequency?

Sound is a pressure wave, and changing the pressure of a gas also changes its temperature.
This temperature variation slightly changes the local speed of sound.
The nonlinear behavior allows the different frequency components to interact, creating a [low frequency tone](https://en.wikipedia.org/wiki/Heterodyne). 

This is a very roundabout way to make sound, but the result is far more directional then could be achieved directly.
Even my small ~1.5 cm transducer has pronounced directivity, unlike a conventional speaker of comparable size.
A large transducer (or phased array) could produce a beam only a couple of degrees wide.

The main downside is limited bandwidth: lower frequencies take more ultrasound to produce, and the higher frequencies are limited by transducer bandwidth.
This is my function generators best attempt at sending a square wave:

![](rx_sq.png)

This looks somewhat worse then it actually is, because the sound is being measured by a second similarly bandwidth limited transducer.

A workaround could be to amplify and overmodulate higher frequencies, or to simply use a higher frequency transducer, with a correspondingly higher bandwidth.

Another effect I noticed is that the transducers blow a small amount of air.
There are probably some cool (pun intended) applications for a completely silent fan with no moving parts.
