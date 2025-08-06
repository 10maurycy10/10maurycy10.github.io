"""
Creates markov files for babble.c using text in the files book1, book2 and book3.
"""

import json
import gzip

# One works well for short bodies of text. 
# Two needs ~10,000 words to work well.
# Three and above is not recommended
context_length = 2

def multigram(words):
	"""
	Join words with hyphens in reverse order
	"""
	return "-".join(words[::-1])	

def get_words(file):
	"""
	Extract words from a file, replacing periods with "END".
	"""
	print("Reading", file, "...")
	file = open(file)
	print("[*] Generating markov chain...")
	for line in file:
		for word in line.split(" "):
			has_period = '.' in word
			if ("-" in word): continue
			if ("'" in word): continue
			word = word.strip(";:.!?\n“”\"[]-()_0123456789 ")
			word = word.lower()
			if (len(word) > 0): yield word
			if has_period: yield "END"
	for i in range(context_length):	yield "END"
			
def make_chain(filename):
	chain = {}
	starters = {}

	def add_chunk(words):
		# Split into lists of size context_length
		prev = multigram(words[:-1])
		now = multigram(words[1:])

		# Record
		if prev not in chain: chain[prev] = {}
		if now not in chain[prev]: chain[prev][now] = 1
		else: chain[prev][now] += 1

		# If this is the start of a sentance, record it
		if prev.startswith("END"):
			if prev not in starters: starters[prev] = 1
			else: starters[prev] += 1

	# Read file
	try:			
		chunk = ["END"] * (context_length + 1)
		for word in get_words(filename):
			chunk = chunk[1:]
			chunk.append(word)
			add_chunk(chunk)

	except KeyboardInterrupt:
		print("^C detected, continuing...")

	print("[*] Chain size:", len(chain))

	if 'END' not in chain:
		chain["END"] = starters

	print("[*] Converting...")
	dump = ""
	for word1 in chain.keys():
		prob = []
		for word2 in chain[word1].keys():
			prob.append((word2, chain[word1][word2]))
		prob.sort(key=lambda point: point[1], reverse = True)
		sort = [point[0] for point in prob]
		if len(sort) > 30: sort = sort[:30]
		dump += word1 + " " + " ".join(sort) + "\n"

	return dump

open("chain1.txt", 'w').write(make_chain("book1.txt"))
open("chain2.txt", 'w').write(make_chain("book2.txt"))
open("chain3.txt", 'w').write(make_chain("book3.txt"))


