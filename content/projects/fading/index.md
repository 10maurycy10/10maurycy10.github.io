---
title: "Your TV can see through walls:"
date: 2025-08-02T1:00:00-00:00
tags: ["wireless"]
draft: false
summary: "</br>Exploiting selective fading to detect motion though walls with the simplest possible setup."
---

This is a spectogram of the signal from a local TV station, as seen by an an indoor dipole antenna:

<img src=/projects/fading/spectro1.jpg alt="A strong, unchanging signal">
<center style="color: gray"> ATSC digital TV signal. (Recorded using an Ettus USRP B200)</center>

Walking between the receiver and TV station causes a slight disturbance to the signal, but it's nothing to write home about:

![A strong signal with slight amplitude variations](/projects/fading/spectro2.jpg)
<center style="color: gray">Walking through the signal path</center>

Now, let's move the antenna a bit:

![A weak signal with irregular spectrum](/projects/fading/spectro3.jpg)
<center style="color: gray">Where'd my signal go?</center>

This is selective fading: The direct signal is being canceled out by a strong reflection. 
Because the phase of the reflection depends on frequency, different parts of the spectrum are affected differently.
Apart from being annoying if you're trying to watch TV, this condition is extremely sensitive to any nearby movement.

Here's what happened to the signal when I got up from my chair:

![An irregular signal brief variation](/projects/fading/spectro5.jpg)

... and here's me walking around in a neighboring room:

![An irregular signal with drastically varying spectrum](/projects/fading/spectro4.jpg)
<center style="color: gray">Stripes!</center>

The gaps between the stripes are caused by my reflection perfectly canceling other signals, which happens each time the path length changes by one wavelength.
Let's use this to figure out how fast I was walking:

The gap between each stripe is around 70 pixels, and the waterfall has an update rate of 150 Hz...

<center><b>70 pixels * 1 second/150 pixels = 0.47 seconds = 2.1 Hz</b></center>
<br>
<center><b>speed of light / 490 MHz * 2.1 Hz = 1.3 m/s = 4.6 km/h</b></center>

I was walking perpendicular to the direct signal path, so the speed can be used as is, without any adjustments for geometry. 

So this signal is able to detect motion and provide an estimated speed.
-- and it's not just TV that works for this: any wideband radio signal like WiFi or LTE will work. 
Those make worse demonstrations because they have a messy waterfall, but it's possible extract the [same data](https://en.wikipedia.org/wiki/Channel_state_information) with a bit of work.
