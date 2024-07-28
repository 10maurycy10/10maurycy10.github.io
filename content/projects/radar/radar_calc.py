# FMCW radar design calculator, allows quickly checking fundimental front end parameters:
#
# Current setup: 5.8 GHz center, 150 bandwith, 10 ms chirp, 130 ksample/s AVR ADC.
#	20 dBm + 13 dB horn antenna : 
# 		650 m absolute maxium range
#		86 m bird detection
#		220 m human dtection
# 
# Long range setup: 5.8 GHz center, 150 bandwith, 500 ms chirp, 130 ksample/s AVR ADC:
#	20 dBm + 40 dB dish antenna: 
# 		32 m absolute maxium range
#		14 km bird detection
#		32 km human dtection

# 5.8 GHZ ism band, Within range of the HMC431LP4 + ADF4158
fcenter = 5800 * 1000*1000 # Hertz
bandwith = 150 * 1000*1000 # Hertz
sweep_time = 10 / 1000 # seconds
adc_samp_rate = 130 * 1000 # Hertz

tx_pwr = 20 # dBm
usable_sn = 10 # dB
noise_figure = 2 # dB

# dB
ant_gain = 10 # Decent horn antennas
#ant_gain = 40 # Large dish

# Radar cross sction for range calculations
rcs = 1 # m^2 # ~human
#rcs = .01 # m^2 # ~bird

# Helpful function to display large and small numbers
import math
prefixes = ["a", "f", "p", "n", "μ", "m", "", "k", "M", "G", "T", "P", "Y"]
def format_si(n, unit):
	exp = math.floor(math.log10(n))
	prefix_no = (exp//3)
	prefix_no = min(prefix_no, len(prefixes)//2 - 1)
	prefix_no = max(prefix_no, -len(prefixes)//2 + 1)
	prefixed_unit = prefixes[prefix_no + len(prefixes)//2] + unit
	return f"{(n * 10**(-3 * prefix_no)):.2f} {prefixed_unit}"

print("FMCW radar parameter calculator:")
print(f"\t F_center: {format_si(fcenter,'Hz')}, Bandwith: {format_si(bandwith, 'Hz')}, Sweep time: {format_si(sweep_time, 's')}, Samp rate: {format_si(adc_samp_rate, 'Hz')}")
print(f"")

# Frequency range, for checking Tx components and regulations
# Determined by center frequency and sweep bandwith
print(f"Sweep start: {format_si(fcenter - bandwith/2, 'Hz')}")
print(f"Sweep stop: {format_si(fcenter + bandwith/2, 'Hz')}")

print(f"")
print("Limits from chirp and bandwith:")
# Calculate the distance where a return will arive entirely in the next chirp, making unambigus distance measurement impossible.
# This does not apply to pulsed FM radars.
# Determined only by chirp lenght.
chirp_max_dist = sweep_time * (3*10**8)/2
print(f"\tMax unambiguous range: {format_si(chirp_max_dist, 'm')}")
# Calculate the longest time and distance for a return to still be within the IF bandwith.
# In most cases this will be the limiting factor for the radar's detection distance. (apart from noise floor)
# Determined by ADC sample rate, chirp time and bandwith
sweep_rate = bandwith / sweep_time # Hz/s
fmax = adc_samp_rate / 2 # Hertz
trip_max = fmax / sweep_rate # S, longest round trip that will resutlt in a beat within the adc's nyquist limit
print(f"\tLongest round-trip time in IF bandwith: {format_si(trip_max, 's')}")
dist_max = (trip_max * 3*10**8) / 2
print(f"\tLongest observable distance in IF bandwith: {format_si(dist_max, 'm')}")

# Information relevent to the IF and digital side
print("")
print("Baseband information:")
# The conversion factor between range and frequency/FFT bin, ignoring doppler shits.
# Determined by ADC sample rate, chirp time.
f_range =  sweep_rate / (3*10**8) * 2 # Hz/m
print(f"\tRange to freq ratio: {format_si(f_range, 'Hz/m')}")
print(f"\tFFT bin to range: {format_si((1/sweep_time)/f_range, 'm')}")
# Number of samples per sweep, determines the digital bandwith needed for a given update rate.
# Determined by ADC sample rate, chirp time.
samp_no = adc_samp_rate * sweep_time
print(f"\tSampling density: {samp_no} samples/sweep")

print("")
print("Overall front end parameters:")

# The best cutoff frequecy for the IF low pass filter, assuming a brick-wall response.
# The real cutoff might have to be lower with practical filter designs
# Determined by ADC sample rate.
print(f"\tIdeal IF antialias filter: {format_si(adc_samp_rate/2, 'Hz')}")
# Usable range as limited by the electronic's, ignoring antenna and noise limitations
# Determined by half the "Max unambig distance"  or the "Longest observable distance in IF bandwith", which ever is smaller
max_range = min(dist_max, chirp_max_dist / 2)
print(f"\tUsable range (excluding noise): {format_si(max_range, 'm')}")
# The minium distnace objects have to be to seperate, same as a signel FFT bin.
# Determined by the sweep bandwith
print(f"\tRange resolution: {format_si((3*10**8)/(2*bandwith), 'm')}")
# Fractional bandwith of chirp singal, relevent to antenna design
print(f"\tFractional bandwith: {(bandwith/fcenter*100):.2f}%")

print("")
print("Link budget:")

def from_db(n):
	return 10**(n/10)

def to_db(n):
	return math.log10(n)*10

# Thermal noise in an FFT bin
boltzman = 1.38064852*(10**-23) 
noise_floor = boltzman * 290 * (1/sweep_time) * from_db(noise_figure) * from_db(usable_sn)
print(f"\tSignal power needed to rise {usable_sn} dB over thermal: {to_db(noise_floor*1000):.0f} dBm")

# Link budget fron noise, tx power and antenna gain
l_budget = tx_pwr - noise_floor + ant_gain*2 # dB
print(f"\tMaxium path loss: {l_budget} dB")
print(f"\tMaxium one-way path loss: {l_budget//2} dB")

wavelength = 3*10**8 / fcenter # m

# Standard radar equation
eirp_len_rcs = (from_db(tx_pwr)/1000 * from_db(ant_gain)**2 * wavelength**2 * rcs) # W * m**4
range4 = eirp_len_rcs / (noise_floor * (4 * math.pi)**3) 
range_max = range4**0.25
print(f"\tMax detection distance for {rcs} m^2 target: {format_si(range_max, 'm')}")


