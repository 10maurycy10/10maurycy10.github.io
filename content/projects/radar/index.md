---
title: "Building an FMCW radar"
date: 2024-07-23T15:41:05-10:00
tags: ["electronics", "wireless"]
draft: true
math: true
---

In principle, a radar is very simple, it just send out a radio wave and listens for reflections.
By measuring the time it takes for each reflection to arive, the radar can determine how far away an object is.

Of course, things are never that easy.
The main problem is that the speed of light is fast, very fast.

Light takes 3 nanoseconds to travel a meter, so for 1 meter resulution, it also needs to sample the recieved power at over 150 megahertz.
Of course this problems can be solved by throwing money at them, but much like [with oscilloscopes](/projects/fast_scope/), there is a way around them.

# The easy way:

In FMCW radar, the transmitter constantly sends out a linear chirp signal, and the recieve side mixes (multiples) the reflected signal with the original chirp tapped of the transmitter:

![](fmcw.svg)

The reflections will be delayed, and will be at any instant a different frequency then the undelayed chirp.
This creates a relativly low frequency signal after the mixer, which can be easly aplified and digitized. 
With FMCW, the resolution is limited by the RF bandwith, not the IF bandwith, so a cheap audio ADC can still acheave 1 meter resolution.

# Circuit design:

The chirp has to be very linear, or else the IF frequency will not be constant, smearing out the FFT peaks.
Unfortuanly, most microwave VCOs are quite non linear:

![Plot of tuning voltage and frequcny for a typical VCO](vco.png)

This won't matter too much at close ranges, but will ruin the resulution at beyond 50 meters or so.

This can be solved using a PLL, like the ADF4158 to constantly monitor and adjust the frequecy against a reference, like a crystal oscillator:

- PLL radar diagram

The ADF4158 in particular has a nice feature where it can automaticly sweep the output frequency, leaving the microcontroller free to do other things.
As a bonus, it ensures the output is exactly 5.8 GHz, to avoid stepping on anyone's toes.

Because the IF frequencies will be very low, a low flicker noise mixer will be best.
I setlled on a active BJT mixer to minize the drive requirements, allowing it to be driven by a simpler microstrip coupler rather then a power divider.

Becuase recieved signal power is inversely proportional to the distance to the *forth* power, the signal from the mixer has a huge dynamic range.
This has to be squashed down before being digitized, which can be done with a high pass filter with a slope of around 40 dB/decade:

This filter also has a low pass response above 65 kHz to prevent aliasing.

The downside of doing this is that it ends up attenuating returns at close range, but these tend to be quite strong to start with.
I wasn't trying to detect insects at 10 centimeters anyways.

# Board desgin

Unfortunatly most microwave parts are only avalable in leadless packages, making hand soldering impossible.

- Board
- Antennas

# Math for nerds:

*This section is quite technical, and only important if you want to design your own radar, feal free to skip it.*

Before building the radar, I wanted to get an idea of how good it could possibly work, so that I could see how close the real radar got.

Just throwing some numbers out their, I picked a frequcy of 5.8 GHz, right at the center of a 150 MHz ISM band.
It should be high enough to allow good resolution and small antennas, but low enough to preserve my wallet and sanity.

The range resolution of a radar depends only on the signal bandwith, a bandwith of 150 MHz gives a resolultion of 1 meter:

$$ \text{Resolution} = $$
$$ \frac{\text{Speed of light}}{2\times\text{Bandwith}} = $$
$$ \frac{3 \times 10^8 \text{ m/s}}{2 \times 150 \text{ MHz}} = $$
$$ 1\text{ meter}$$ 

A chirp rate of 100 Hz (10 ms each) gives a ramp rate of 15 GHz/second, which is quite manangable.
Dividing that by half the speed of light gives the ratio between IF frequcy and range, in this case 100 Hz/m:

$$ \text{IF range freqency ratio} = $$
$$ \frac{\text{Bandwith}\times 2}{\text{Sweep time}\times\text{Speed of light}} = $$
$$ \frac{150\text{ MHz}}{10\text{ ms}} \times (3\times 10^8)^{-1} = $$
$$ 100\text{ Hz/m} $$

Using a 130 ksample per second second ADC, the highest frequency that can be digitized is 65 kHz, which corisponds to a range of 650 meters:

$$ \text{Max range} = $$
$$ \frac{\text{Sample rate}\times.5}{\text{Range frequency ratio}} = $$
$$ \frac{130 \text{ kHz} * .5}{100\text{ Hz/m}} = $$
$$ 650\text{ m} $$

So far the radar looks quite good, with a max range of 650 meters and resolution of 1 meter, but this math ignores noise.

# Noise & Link budget:

Thermal noise is the unavoidable noise from heat jostling around charged particles, and mostly just depends on the measurement bandwith.
Here this is the bandwith of a single FFT bin, which is one over the FFT length, which in my radar is the same as the chirp rate, 100 Hz.

$$ \text{Thermal noise} = $$
$$ \text{Boltzmann constant} \times \text{Temperature} \times \text{Chirp rate}^{-1} =$$
$$ 1.38 \times 10^{-23} \text{ J/K} \times 290\text{ K} \times \frac{1}{10 ms} = $$
$$ 4 \times 10^{-19}\text{ W} = -154\text{ dBm} $$

The reciever amplfier will also add some noise, expressed as noise figure, around 6 dB is typical.
It's also not enough for a return to be at the same level as the noise, it has to be significant stronger, let's say by 20 dB:

$$ \text{Minium Rx Power} = $$
$$ \text{Thermal noise power (dBm)} + \text{Noise figure} + \text{Needed SNR} = $$
$$ -158\text{ dBm} + 6\text{ dB} + 20\text{ dB} = $$
$$ -128 \text{ dBm}  $$

This minium power, along with a transmit power (20 dBm) and antenna gain (10 dB), and the radar cross section of the target (say, a human) can be used to calcuate the detection range:

$$ \text{Max range} = $$
$$ \sqrt[4]{\frac{\text{Tx power}\times\text{Antenna gain}^2\times\text{Wave length}^2\times\text{Target cross section}}{\text{Minium Rx power}\times(4\pi)^3}} = $$
$$ \sqrt[4]{\frac{100 \text{ mW}\times10^2\times5.3\text{ cm}^2\times1\text{m}^2}{10^{-12.8}\text{ mW}\times(4\pi)^3}} = $$
$$ 540 \text{ m} $$

The radar will have a harder time detecting smaller objects, a typical bird (RCS: 0.01 square meter) could only be detected 170 meters out.

This can be improved with better antennas, using 40 dB parabolic antennas would allow a human could be detected 17 kilometers out, and a bird at 5 kilometers out.
At these distances the radar would need a slower chirp or faster ADC to avoid hitting the nyquist at least with a faster ADC or slower chirp)[^justforfun]

Of course this is the theoretical maxium, assuiming just unavoidable thermal noise. 
In practice, antenna loss, reciever overload, clock jitter and other effects will limit performance.[^pulse ]

At this point I had a set of general parameters that should give a good comprimize between cost, complexity and performace: 20 dBm transmit power, 150 MHz bandwith @ 5.8 GHz, 100 ms chirp, 65 kHz IF and 10 dB antenna gain.
For designing the reciever, I need a representive recieved power, and I picked the return from a typical person at 100 meters:

$$ \text{Recieved power} = $$
$$ \frac{\text{Tx power}\times\text{Antenna gain}^2\times\text{Wave length}^2\times\text{Target cross section}}{\text{Range}^4\times(4\pi)^3} = $$
$$ \frac{100 \text{ mW}\times10^2\times5.3\text{ cm}^2\times1\text{m}^2}{(100\text{ m})^4\times(4\pi)^3} \approxeq $$
$$ 10^{-10} = -100\text{ dBm} $$

A peak to peak 1 volt into the ADC is 0.7 volts RMS, which is around 10 mW into a 50 ohm load, or 10 dBm.
1 volt is quite a bit, so I decided to make this "typical" return 30 dB down, or -20 dBm, which requires a total gain of 80 dB.
More RF gain will decrease the effect of flicker noise in the mixer and IF, but will make the reciever easier to saturate.

I fairly arbitrarly split this into 30 dB of RF and 50 dB of IF gain (@ 10 kHz), which is finaly enough to spec out the whole system:

VCO output: 2 dBm, a resisive splitter provices -4 dBm into the PLL's feedback, and the same into the PA.
The F1490 PA has 34 dB of gain, so needs a -14 dBm drive, which can be generated with a -10 dB attenuator.
A -16 dB coupler off the output of the PA provices 4 dBm, which is enough to drive an active mixer like the LTC5510.
The mixer's RF input needs 30 dB of gain, which can be provided by two 15 dB LNA, like the BGU7258.
The final 50 dB of power gain (25 dB voltage gain) will be provided by an op-amp active filter, which also provides range compensation and anti-aliasing.

There will be a few dB of loss here and there, so I added an additional LNA, which can be switched on if needed, and kept in the "Bypass" mode if not.

I also calculated the thermal noise refered to the ADC, at the highest gain frequncy at the compensation filter:

$$ \text{Noise floor at ADC (dBm)} = $$
$$ \text{Thermal noise power (dBm)} + \text{RF gain (dB)} + \text{Maxium IF gain (dB)} = $$
$$ -158\text{ dBm} + 30\text{ dB} + 80\text{ dB} = $$
$$ -48\text{ dBm} $$

The 12 bit ADC has a dynamic range of 70 dB, so at a range of above 100 meters, the radar is limited by noise, not the ADC.
A close range signals are quite strong, so quantization errors shouldn't be quite as big of a problem.

[^resolution]: This formula actually works for any radar, not just FMCW designs. 
