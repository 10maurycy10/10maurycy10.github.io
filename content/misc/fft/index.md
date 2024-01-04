---
title: "The (Cooley–Tukey) Fast Fourier Transform"
date: 2023-12-27T13:24:54-08:00
tags: ["programming"]
draft: false
---

The discrete Fourier transform breaks down a signal represented as a list of samples into its constituent frequencies.
That is, it converts the signal to a sum of sine and cosine waves.

It helps to use complex numbers, which for this purpose are just points in 2d space.
Multiplying one complex number by another is the same as rotating one point the angle of another.
For this, Euler's formula, `e**(i*θ)` = `cos(θ) + i*sin(θ)`, comes in handy as an easy way to create complex numbers along a unit circle at a given angle.
As a result, `x * e**(i*θ)` is  `x` rotated by `θ`.

The simplest way to implement a Fourier transform is to take every sample, multiply (rotate) it by a point moving along a circle at a given frequency and sum the result.
Any components at different frequencies will spend an equal amount of time in sync and out of sync with point, and they will cancel out in the sum.
However, a component at the same frequency will stay in sync and effect the overall sum.
This whole process then has to be repeated for every frequency of interest.

A really nice visual analogy is taking the samples and wrapping them around the origin, and then taking the center-of-mass of the result.
You can play around with this with [any program that can plot functions in polar coordinates](https://www.desmos.com/calculator/u5acrvmhys) by plotting `r=sin(a*θ)`.
For any value of `a` other then 1, the graph will spend an equal amount of time on all sides of the origin.

Implementing a Fourier transform requires deciding on what frequency bins to use, the standard method is to use a number equal to the number of samples, evenly distributed from 0 to one full revolution per sample.
This provides the maximum amount of detail available from a given number of samples: [^shannon]

```py
import cmath # Boring python stuff
from cmath import pi, e
i = cmath.sqrt(-1)

def fourier(samples):
	transform = [] # Buffer for results
	for bin_no in range(len(samples)):
		# Evenly spaced freqiencies, in radians/sample
		freq = 2*pi / len(samples) * bin_no
		# Total up the rotated samples
		acc = 0
		for j in range(len(samples)):
			# Rotate the sample by the content 
			acc += samples[j] * e**(i*freq*j)
		transform.append(acc)
	return transform
```
If you play around with this code, make sure to use the magnitude of resulting complex number if you want the amplitude.
In Python, this can be done using the `abs()` function.

This straightforwards implementation works, but is has a problem;
At a minimum (assuming all powers of e are precomputed), it requires the number of samples *squared* complex multiplications and additions.
This gets slow quickly, for 1024 samples, it takes over a million multiplications, and for 44100 samples (1 second of audio) it takes almost 2 billion.
Keep in mind that a complex multiplication is actually 4 normal multiplications and 2 normal additions.

This can be improved splitting a Fourier transform into two[^radix] smaller transforms, one on the even and another on the odd numbered samples.
The outputs can be combined by adding up the result for each bin from both sub-transforms, with the odd ones rotated a bit to account for the time offset between even and odd samples.
The rotation needed is different for each frequency, but always increases by a fixed amount between bins.

On it's own, this would give half as many bins as the full sized transform, but this can easily be fixed.
The second half of the bins correspond to the same frequencies as the lower half, but with an additional 180 degree rotation between every sample.
To get them, the odd FFT's output can be negated (the same as rotating by 180 degrees) before adding it to the even FFT's output:

```py
# Boring python stuff
import cmath
from cmath import pi, e
i = cmath.sqrt(-1)

def fft2(samples):
	"""
	Compute the Fourier transform of the input, requires the length to be a power of 2.
	Pad the input with zeros before calling if it is not already a power of 2.
	"""
	# Trivial base case
	if len(samples) == 1: return samples
	# Compute sub FFTs
	even = fft2(samples[0::2])
	odd = fft2(samples[1::2])
	# Rotation used for combining the sub FFTs
	rot = e**(2*pi*i/n) # Extra rotation per bin
	c_rot = 1 # Current rotation between even and odd
	# Combine them
	combined = [0] * len(samples)
	for j in range(len(samples)//2):
		p = even[j]
		q = odd[j] * c_rot
		combined[j] = p + q
		combined[j + len(samples)//2] = p - q # Negation is a 180 degree rotation
		# Increase the rotation for the next bin
		c_rot *= rot
	return combined
```

If the input is a power of 2, this repeated division by 2 will go all they way down to a single sample.
A single sample Fourier transform only has a single frequency, 0, and a single sample, so no rotation has to be done, the function just returns the sample.

Because the time taken by a transform is proportional to the number of inputs squared, the half sized transforms don't take half the time, but a quarter of the time.
Furthermore, those smaller transforms can be split again, and again, and again.
As a result, this algorithm only does `2 * n * log2(n)` complex multiplications, of which half could be precomputed. 
On 44100 samples (one second of audio), the minimum number of multiplications is just 68 thousand, almost 3 thousand times better then the brute force method.

<!--
- Example

Just to make sure this works, lets walk trough a simple FFT:

```py
fft2([1, 2, 1, 3])
```

The two sub FFTs are:

```py
fft2([1, 1])
fft2([2, 3])
```

each of these FFTs has a rotation of 1 (zero degrees) and -1 (180 degrees). 

```py
fft2([1, 1]) -> [1+1=2, 1+1=2]
fft2([2, 3]) -> [2+3=5, 2+3=]
```
-->

An inverse FFT would allow quickly hopping between time and frequency depending on what is the most convenient for the problem at hand.
Fortunatly, this is quite an easy change to the current FFT function, and just requires negating the angle of rotation.
Additionaly, the output has to be divided by the number of samples because of how an FFT totals up frequency components in each sample, but this is easest to do outside of the recursive function:

```py
# Boring python stuff
import cmath
from cmath import pi, e
i = cmath.sqrt(-1)

def ifft2(samples):
	"""
	Compute the Inverse Fourier transform of the input, requires the length to be a power of 2.
	The outputs will be scaled by the number of samples, and should be divided by them
	"""
	# Trivial base case
	if len(samples) == 1: return samples
	# Compute sub IFFTs
	even = ifft2(samples[0::2])
	odd = ifft2(samples[1::2])
	# Rotation used for combining the sub IFFTs
	# Changed from a normal FFT
	rot = e**(-2*pi*i/n) # Extra rotation per bin
	c_rot = 1 # Current rotation between even and odd
	# Combine them
	combined = [0] * len(samples)
	for j in range(len(samples)//2):
		p = even[j]
		q = odd[j] * c_rot
		combined[j] = p + q
		combined[j + len(samples)//2] = p - q # Negation is a 180 degree rotation
		# Increase the rotation for the next bin
		c_rot *= rot
	return combined

freq = [...] # Placeholder for real data
time = ifft2(freq) # Compute the IDFT of freq
time = [sample/len(freq) for sample in time] # Scale it.
```

[^shannon]:
	Going up to one full revolution per sample might seem pointless, given the Nyquist–Shannon sampling theorem, but this is actually useful in some cases.
	Rotating nearly one full revolution is equivelent to rotating a small amount backwards, a *negative* frequency.
	Negative frequencies don't make much sense if you look at a sine wave, which is only half of the circle.
	If you have both the sine and cosine, real and imaginary components, like from a IQ mixer, this can be useful.
	The first half of the transform is from 0 to half the smaple rate, and the next half is from half the sample rate to 0, but negative.
	For real number inputs, the second half of the FFT can simply be discarded.

[^radix]:
	Two is the most common number to use, because it is the smallest one that works and because programmers like powers of two.
	The algorithm itself can be generalized to other small numbers, and this can reduce the need to pad the samples with zeros before the FFT, speeding it up.
