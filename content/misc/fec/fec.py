from matplotlib import pyplot
import random
import math

msg = [-1,1,1,-1,1, 1,1,1,-1,-1]
print("Message length", len(msg))

sample_rate = 200

"""
Utility functions
"""

def hamming_distance(a, b):
	"""
	Compute the difference between two bit sequences
	"""
	dist = 0
	for (x, y) in zip(a, b):
		if (x != y): dist += 1
	return dist

def transmit(data, samples_per_sym):
	"""
	Transmit data with no correction
	"""
	signal = []
	for v in data:
		signal += [v]*samples_per_sym
	return signal

def rx(data, samples_per_sym):
	"""
	Average symbols
	"""
	avged = []
	for i in range(len(data)//samples_per_sym):
		chunk = data[samples_per_sym*i:samples_per_sym*(i+1)]
		avg = sum(chunk)/len(chunk)
		avged += [avg] * samples_per_sym
	return avged
		

def noise(data):
	"""
	Add a lot of noise to a signal
	"""
	return [d + random.random() * 30-15 for d in data]

def plot(sig, name, file):
	"""
	Generate a plot and save it
	"""
	pyplot.figure(figsize=(10, 3))
	pyplot.plot([x/sample_rate for x in range(len(sig))], sig)
	pyplot.xlabel("Time (seconds)")
	pyplot.ylabel("Volts")
	pyplot.title(name)
	pyplot.tight_layout()
	pyplot.savefig(f"{file}.png", dpi=300)
#	pyplot.show()

def test():
	"""
	Try to send a message with no error correction, returns the amount of errors
	"""
	coded = transmit(msg, sample_rate)
	coded = noise(coded)
	averaged = rx(coded, sample_rate)
	data = [1 if d > 0 else -1 for d in averaged[::sample_rate]]
	return hamming_distance(data, msg)

def run(fn):
	"""
	Runs a test function over and over
	"""
	n = 10000
	d = [fn() for s in range(n)]
	d = [e for e in d if e > 0]
	print(f"* Failed in {len(d)/n*100}% of {n} tries.")

print("Trying no error correction...")
run(test)

def test_code(code):
	worst = None
	for i in range(len(code)):
		for e in range(i):
			dist = hamming_distance(code[i], code[e])
			if not worst or dist < worst: worst = dist
	return worst

def generate_code(bits, syms, limit):
	"""
	Generate a random codebook with a message of `bits` bits and `syms` symbols per codeword.
	Limit is the smallest allowed hamming difference between 2 codes.
	"""
	code = []
	for i in range(2**bits):
		while True: # Try to generate codewords until one statiflies the distance limit
			codeword = [random.choice([-1, 1]) for i in range(syms)]
			valid = True 
			for old in code:
				if hamming_distance(codeword, old) < limit:
					valid = False
			if valid:
				code.append(codeword)
				break;
	return code

def threshold(data, samples_per_symbol):
	return [1 if d > 0 else -1 for d in data[::samples_per_symbol]]

def decode(code, data, samp):
	data = threshold(data, samp)
	best = None
	best_msg = None
	for (msg, codeword) in enumerate(code):
		dist = hamming_distance(codeword, data)
		if not best or dist < best:
			best = dist
			best_msg = msg
	return best_msg

def decode_fuzzy(code, data, samp):
	best = None
	best_msg = None
	for (msg, codeword) in enumerate(code):
		correlation = 0
		for (a, b) in zip(data[::samp], codeword):
			correlation += (a * b)
		if not best or correlation > best:
			best = correlation
			best_msg = msg
	return best_msg

print("Generating code...")
random.seed(1)
code = generate_code(10, 100, 32)

def test2():
	coded = code[0b0110111100]
	coded = transmit(coded, sample_rate//10)
	coded = noise(coded)
	averaged = rx(coded, sample_rate//10)
	rxed = decode(code, averaged, sample_rate//10)
	return abs(rxed - 444)

def test2_fuzzy():
	coded = code[0b0110111100]
	coded = transmit(coded, sample_rate//10)
	coded = noise(coded)
	averaged = rx(coded, sample_rate//10)
	rxed = decode_fuzzy(code, averaged, sample_rate//10)
	return abs(rxed - 444)

print("Trying 100 symbol EC...")
run(test2)
print("Trying 100 symbol EC - Fuzzy RX...")
run(test2_fuzzy)

print("Genrating example plots...")
print("* Baseband TX...")
coded = transmit(msg, sample_rate)
plot(coded, "Baseband transmission, 10 bits", "data")
print("* Baseband RX...")
random.seed(1)
coded = noise(coded)
plot(coded, "Baseband reception, 10 bits", "noisy")
print("* Filtered...")
averaged = rx(coded, sample_rate)
plot(averaged, "Averaged reception, 10 bits", "avg")
print("* Data...")
plot(threshold(averaged, 1), "Recovered data, 10 bits", "rx")

print("* Baseband TX with EC...")
coded = code[0b0110111100]
coded = transmit(coded, sample_rate//10)
plot(coded, "Baseband transmission, 100 symbols", "data2")
print("* Baseband RX with EC...")
random.seed(2)
coded = noise(coded)
plot(coded, "Baseband reception, 100 symbols", "noise2")
print("* Baseband filtered RX with EC...")
averaged = rx(coded, sample_rate//10)
plot(averaged, "Averaged reception, 100 symbols", "avg2")
plot(threshold(averaged, 1), "Recovered codeword, 100 symbols", "rx2")
print("* Attempting hard decode...")
print("* Got ",decode(code, averaged, sample_rate//10))

thold = threshold(averaged, sample_rate//10)
print(hamming_distance(code[0b0110111100], thold))

print("* Attempting soft decode...")
# Cross corelation plot
corr = []
for (msg, codeword) in enumerate(code):
	correlation = 0
	for (a, b) in zip(averaged[::sample_rate//10], codeword):
		correlation += (a * b)
	corr.append(correlation/len(codeword))

print("* Plotting correlation...")
pyplot.figure(figsize=(10, 3))
pyplot.plot(corr)
pyplot.xlabel("Codeword")
pyplot.ylabel("Correlation")
pyplot.title("Correlation between a noisy signal and each codeword")
pyplot.tight_layout()
pyplot.savefig(f"corr.png", dpi=300)
