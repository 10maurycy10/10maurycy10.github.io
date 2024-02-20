from matplotlib import pyplot
import struct
from collections import deque
from scipy.signal import find_peaks

# Raw audio data, Mono, 16 bit litle endian signed
data = 'xray-raw-16-44100.dat/xray2m.raw' 

# Read in raw data
samples = []
with open(data, "rb") as f:
	while True:
		data = f.read(2)
		if len(data) != 2: break
		samples.append(-struct.unpack("<h", data)[0]) # Invert because pulses are negative
print("Read",len(samples),"samples")

# Find all the peaks
peaks = find_peaks(samples, height=0, distance=5, prominence=100)
print (peaks[1]['peak_heights'])

# Plot them
fig, ax = pyplot.subplots()
ax.hist([h/20000*10 for h in peaks[1]['peak_heights'] if h > 7000], bins=100)
ax.plot([8.638, 9.572], [80, 80], 'x')
ax.set_title("Tritium bremsstrahlung spectrum, acquired over 2 minutes")
ax.set_ylabel("Counts (arb)")
ax.set_xlabel("Energy (keV)")
pyplot.show()
