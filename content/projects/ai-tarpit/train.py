import tqdm
import json
import gzip

# Read words from file
def get_words(file):
	print("Reading", file, "...")
	file = open(file)
	print("[*] Generating wordfreq table...")
	for line in tqdm.tqdm(file):
		for word in line.split(" "):
			has_period = '.' in word
			if ("-" in word): continue
			if ("'" in word): continue
			word = word.strip(";:,.!?\n“”\"[]-()_0123456789 ")
			word = word.lower()
			if (len(word) > 0): yield word
			if has_period: yield "END"
			
def make_chain(filename):
	# Construct markov chain
	chain = {}
	freq = {}

	def add_digram(word1, word2):
		if (word1 not in chain): chain[word1] = {}
		if (word2 not in chain[word1]): chain[word1][word2] = 0
		chain[word1][word2] += 1;
		if (word1 not in freq): freq[word1] = 0
		freq[word1] += 1;
	try:			
		last_word = "";
		for word in get_words(filename):
			if (last_word != ""): add_digram(last_word, word)
			last_word = word
	except KeyboardInterrupt:
		print("^C detected, continuing...")

	print("[*] Chain size:", len(chain))

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

open("chain1.txt", 'w').write(make_chain("book1"))
open("chain2.txt", 'w').write(make_chain("book2"))
open("chain3.txt", 'w').write(make_chain("book3"))


