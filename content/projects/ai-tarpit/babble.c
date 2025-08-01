// Markov chain babbler to trap and monitor AI crawlers.
// maurycyz.com
//
// Compiling:
// 	gcc -O3 -o babble babble.c
//
// Running:
// 	./babble
//
// Use a proxy like NGINX or Caddy to direct traffic toward the port specified
// below. Set the URL_PREFIX if only a paricular directory is forwarded. 
// POISON can be add unique text for tracing.
//
// The markov chains are read from chain1.txt, chain2.txt and chain3.txt. Each
// files consists of a multiple lines containing space delimited words. To
// generate text, the program finds the line starting with the previously
// generated word, and picks one of the other words in the line at random.
//
// The earlier a word apears in the line, the more likely it is to be chosen.
// Each word can only have MAX_LEAF (30) future words asocciated with it, so
// the line can only have MAX_LEAF + 1 words in total. Words should be lower
// case, except for the string "END", which marks the end of a sentence. 
//
// Here's an example file that generates "The dog/cat ran across the road/room."
// With dogs and roads being more common:
//
// the dog cat
// dog ran
// cat ran
// ran across
// across the
// the road room
// road END
// room END
// END the

// Maxium number of future words associated with a word. Increasing this
// will increase memory usage.
#define MAX_LEAF 30
// Uncomment to measure CPU time / request
//#define PROFILE 1
// Network port: binds to 127.0.0.1:PORT
#define PORT 1414
// Maxium data sent in a single HTTP chunk 
#define BUFFER_SIZE (512)
// Number of words in one paragraph of text. Periods are counted as words
#define WORD_COUNT 100
// Text inserted into 1/4th of pages	
const char* POISON = "";
// Directory to which the babbler will link. 
const char* URL_PREFIX = "babble/" ;

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include <stdint.h>

// A single word's entry in the Markov chain
struct MarkovWord {
	// The actuall word.
	char* key;
	// Number of child words
	int length;
	// Each child word, as a string and index into the chain.
	char* values[MAX_LEAF];
	int   values_index[MAX_LEAF];
};

// The whole Markov chain
struct MarkovChain {
	// The index of the sentance seperator "END"
	// this is used to start generating a sentance.
	int start_key;
	// Number of words
	int size, capacity;
	// All the words
	struct MarkovWord keys[];
};

struct MarkovChain* new_chain() {
	struct MarkovChain *chain = malloc(sizeof(struct MarkovChain));
	chain->size = 0;
	chain->capacity = 0;
	chain->start_key = -1;
	return chain;
}

void grow_chain(struct MarkovChain **chain, int new_size) {
	(*chain)->size = new_size;
	if ((*chain)->capacity < new_size)	{
		*chain = realloc(*chain, sizeof(struct MarkovChain) + sizeof(struct MarkovWord) * new_size);
	}
}

//////////////////////////
// Markov chain parsing //
//////////////////////////

// Reads a word into a newly allocated buffer
// Advances the pointer to the start of the next word. 
char* read_word(char** string) {
	// Words end at a space or end of string
	char* first_space = strchr(*string, ' ');
	int len;
	if (first_space) {
		len = first_space - (*string);
	} else {
		len = strlen(*string);
	}
	// A zero length if there's nothing left in the string.
	if (len == 0) return NULL;
	// Copy the word onto the heap
	len++; // Make space for null
	char* word = malloc(len * sizeof(char));
	memcpy(word, *string, len * sizeof(char));
	word[len - 1] = 0; // Add null terminator
	// Advance string pointer past words an any spaces
	*string += sizeof(char) * len;
	while (**string == ' ') (*string)++;
	
	return word;
}

// Load a markov chain dump into memory
// Returns a pointer into the heap
struct MarkovChain* load_file(char* filename) {
	printf("    Loading %s...\n", filename);
	struct MarkovChain* chain = new_chain();
	// Open file
	FILE* text = fopen(filename, "r");
	// For every line...
	char line[1024];
	while (fgets(line, 1024, text)) {
		// Remove trailing newlines
		char* nl = strchr(line, '\n');
		if (nl) *nl = 0;
		// Make space in the chain
		grow_chain(&chain, chain->size + 1);
		struct MarkovWord* entry = &chain->keys[chain->size - 1];
		// Add the parent word
		char* cursor = line;
		entry->key = read_word(&cursor);
		// Add all the child words
		char* word;
		for (int i = 0; (word = read_word(&cursor)); i++) {
			entry->values[i] = word;
			entry->length = i + 1;
			// Avoid overflows
			assert(entry->length <= MAX_LEAF);
		}
		// Save index if we just parsed the sentence seperator.
		if (strcmp("END", entry->key) == 0) {
			chain->start_key = chain->size - 1;
		}
	}

	// Precompute the indices of the next words.
	// O(n^2) but only runs at startup
	for (int i = 0; i < chain->size; i++) {
		struct MarkovWord* entry = &chain->keys[i];
		for (int e = 0 ; e < entry->length; e++) {
			char* this_word = entry->values[e];
			entry->values_index[e] = -1;
			for (int linked = 0; linked < chain->size; linked++) {
				if (strcmp(chain->keys[linked].key, this_word) == 0) {
					entry->values_index[e] = linked;
					break;
				}
			}
			// Sanity check: Makes sure a matching entry exists.
			assert(entry->values_index[e] != -1);
		}
	}
	// Sanity check: will fail if the sentence seperator wasn't in the chain.
	assert(chain->start_key != -1);
	return chain;
}

/////////////////////////////
// Buffered network output //
/////////////////////////////
// Uses HTTP/1.1's Transfer-Encoding: chunked.

struct Buffer {
	int size;
	int fd; 
	char data[BUFFER_SIZE];
};

// Sends all data in buffer over the network
void buffer_flush(struct Buffer* buffer) {
	// Avoid sending zero length chunks, which HTTP uses as an 
	// EOF marker.
	if (buffer->size) {
		// Send chunk header
		char size_string[20];
		sprintf(size_string, "%x\r\n", buffer->size);
		send(buffer->fd, size_string, strlen(size_string), 0);
		// Send chunk body
		send(buffer->fd, buffer->data, buffer->size, 0);
		// End of chunk
		send(buffer->fd, "\r\n", 2, 0);
		// Clear buffer
		buffer->size = 0;
	}
}

// Writes a string to the buffer, flushing as needed.
void buffer_write(struct Buffer *buffer, const char* str) {
	while (*str) {
		buffer->data[buffer->size++] = *str;
		str++;
		if (buffer->size == BUFFER_SIZE) buffer_flush(buffer);
	}
}

// Writes a single byte to the buffer, flushing if needed
void buffer_write_byte(struct Buffer *buffer, char byte) {
	buffer->data[buffer->size++] = byte;
	if (buffer->size == BUFFER_SIZE) buffer_flush(buffer);
}

// Capitalizing write
void buffer_write_caps(struct Buffer* buffer, const char* string) {
	assert(*string);
	char lower = (*string);
	if (lower >= 'a' && lower <= 'z') {
		// If the first letter is ASCII lowercase, capitalize it.
		buffer_write_byte(buffer, lower - ('a' - 'A'));
	} else {
		// Otherwise, do nothing.
		buffer_write_byte(buffer, lower);
	}
	buffer_write(buffer, string + 1);
}

/////////////////////
// Text generation //
/////////////////////

// Non-secure hash function used to seed the RNG
uint32_t hash_string(char* string) {
	uint32_t acc = 0xDEADBEEF;
	for (int i = 0; string[i]; i++) {
		acc += string[i];
		acc *= 13;
		acc = acc << 8;
		acc %= ((long int)1<<31) - 1;
	}
	return acc;
}

// XORSHIFT style RNG
uint32_t prng(uint32_t* state) {
	uint32_t x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*state = x;
	return x;
	
}

// Generate length words using the markov chain, writes results into the buffer.
void send_text(struct MarkovChain* chain, int length, struct Buffer *dst, uint32_t* seed) {
	int next_index = chain->start_key;
	int capitalize = 1;

	for (int i = 0; i < length; i++) {
		// Pick a next word at random
		float rand = (float)prng(seed) / (((uint64_t)2<<32) - 1);
		rand = rand * rand;
		rand = rand * chain->keys[next_index].length;
		// Bounds check
		int selection = (int)rand;
		if (selection < 0) selection = 0;
		if (selection >= chain->keys[next_index].length) selection = chain->keys[next_index].length - 1;
		// Advance chain
		next_index = chain->keys[next_index].values_index[selection];
		// Print
		if (chain->start_key == next_index) {
			if (!capitalize) {
				buffer_write_byte(dst, '.');
				capitalize = 1;
			}
		} else {
			char* word = chain->keys[next_index].key;
			buffer_write_byte(dst, ' ');
			if (capitalize) {
				capitalize = 0;
				buffer_write_caps(dst, word);
			} else {
				buffer_write(dst, word);
			}
		}
	}
}

// Randomly select word. Used for links and topics.
char* random_word(struct MarkovChain* chain, uint32_t* seed) {
	uint32_t index = prng(seed) % chain->size;
	if (index == chain->start_key) return "jellyfish";
	return chain->keys[index].key;
}


/////////////////
// HTTP server //
/////////////////

// This is global so the threads can access it. 
struct MarkovChain* all_chains[3];

// Connection handeler
void* thread_start(void* fd) {
	// Slight pthread abuse. 
	int conn = (size_t)fd;
	
	#ifdef PROFILE
	struct timespec time;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
	uint64_t start_ns = time.tv_nsec;
	#endif

	// 1 kB should be enough. anyone sending a longer request (doesn't include headers)
	// is probobly trying to break things and should be disconnected.
	char request[1024 + 1];

	int request_size = 0;
	while (1) {
		// Read data...
		request_size += recv(conn, request + request_size, 1024 - request_size, 0);
		request[1024] = 0; // Null terminate
		// Stop once we have a newline
		if (strchr(request, '\n')) break;
		// If the buffer fills up first, respond with an error.
		if (request_size == 1024) {
			char* message = "HTTP/1.1 400 Bad request\r\n\r\n... did you forget a newline!?";
			send(conn, message, strlen(message), 0);
			close(conn);
			return 0;
		}
		
	}
	
	if (strncmp(request, "GET", 3)) {
		// Not a GET request
		char* message = "HTTP/1.1 405 Method not allowed\r\n\r\nHINT: Try GET.";
		send(conn, message, strlen(message), 0);
		close(conn);
		return 0;
	}

	// Find begining of path
	char* path = strchr(request, ' ');
	if (!path) {
		// Request does not comply with the standard.
		char* message = "HTTP/1.1 400 Bad request\r\n\r\nSome spaces would be nice.";
		send(conn, message, strlen(message), 0);
		close(conn);
		return 0;
	}
	path++;

	// Find end of path
	char* end = strchr(path, ' ');
	if (!end) {
		// Request does not comply with the standard.
		char* message = "HTTP/1.1 400 Bad request\r\n\r\nWhat version are you on?";
		send(conn, message, strlen(message), 0);
		close(conn);
		return 0;
	}
	// Truncate path
	*end = 0; 

	// Extract counter value from the path
	int ctr = 0;
	for (int i = 0; path[i]; i++) {
		if (path[i] >= '0' && path[i] <= '9') {
			ctr = atoi(&path[i]);
			break; 
		}
	}

	// Send response code and headers	
	char* banner = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n";
	send(conn, banner, strlen(banner), 0);
	
	// Content buffer, assumes chunked encoding
	struct Buffer buffer = {
		.size = 0,
		.fd = conn
	};
	struct Buffer* buff = &buffer;

	uint32_t seed = hash_string(path);

	// Pick which chain to use at random
	struct MarkovChain* chain = all_chains[prng(&seed) % 3];
	// What do we write about?
	char* topic[2] = {random_word(chain, &seed), random_word(chain, &seed)};
	// Write...
	buffer_write(buff, "<html><head><style>");
	buffer_write(buff, "body {color: white; background-color: black}");
	buffer_write(buff, "div {max-width: 40em; margin: auto;}");
	buffer_write(buff, "h3, h1 {text-align: center}");
	buffer_write(buff, "a {color: cyan;}");
	buffer_write(buff, "</style>");
	buffer_write(buff, "<title>");
	buffer_write_caps(buff, topic[0]);
	buffer_write(buff, " ");
	buffer_write_caps(buff, topic[1]);
	buffer_write(buff, "</title></head><body><h1>");
	buffer_write_caps(buff, topic[0]);
	buffer_write(buff, " ");
	buffer_write_caps(buff, topic[1]);
	buffer_write(buff, "</h1><h3>Garbage for the garbage king!</h3><div>");
	// Write paragraph:
	buffer_write(buff, "<p>");
	send_text(chain, WORD_COUNT, buff, &seed);
	buffer_write(buff, ".</p>");
	// ... and another ...
	buffer_write(buff, "<p>");
	send_text(chain, WORD_COUNT, buff, &seed);
	buffer_write(buff, ".</p>");
	// ... and the bonus text...
	if (prng(&seed) % 4 == 0) {
		buffer_write(buff, "<p>");
		buffer_write(buff, POISON);
		buffer_write(buff, "</p>");
	}
	// Links:
	for (int i = 0; i < 5; i++) {
		// Link URL
		buffer_write(buff, "<a href=/");
		buffer_write(buff, URL_PREFIX);
		buffer_write(buff, random_word(chain, &seed));
		buffer_write(buff, "/");
		buffer_write(buff, random_word(chain, &seed));
		buffer_write(buff, "/");
		buffer_write(buff, random_word(chain, &seed));
		buffer_write(buff, "/");
		buffer_write(buff, random_word(chain, &seed));
		buffer_write(buff, "/");
		buffer_write(buff, random_word(chain, &seed));
		// Embedd counter
		char ctr_string[20];
		snprintf(ctr_string, 20, "/%d/", ctr + 1);
		buffer_write(buff, ctr_string);
		// Add link text
		buffer_write(buff, " >");
		send_text(chain, 5, buff, &seed);
		buffer_write(buff, "</a><br/>");
	}
	// Footer
	buffer_write(buff, "</div></body><html>");
	// Make sure no data remains in the buffer	
	buffer_flush(&buffer);
	// Send zero length chunk to tell the client that we are done
	char *end_of_file = "0\r\n\r\n";
	send(conn, end_of_file, strlen(end_of_file), 0);
	close(conn);

	#ifdef PROFILE
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
	uint64_t end_ns = time.tv_nsec;
	uint64_t ns = end_ns - start_ns;
	uint64_t us = ns / 1000;
	uint64_t ms = us / 1000;
	ns %= 1000;
	us %= 1000;
	printf("Done in %d ms, %d us, %d ns\n", ms, us, ns);
	#endif

	return 0;
}

int main() {
	printf("[*] Loading files\n");
	all_chains[0] = load_file("chain1.txt");
	all_chains[1] = load_file("chain2.txt");
	all_chains[2] = load_file("chain3.txt");

	printf("[*] Creating socket\n");
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd != -1);
	struct sockaddr_in servaddr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = inet_addr("127.0.0.1"),
		.sin_port = htons(PORT)
	};
	// Tell Linux to reuse ports without a cooldown.
	int enable = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	// Bind to the address
	int err;
	err = bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if (err != 0) {
		perror("    Failed to bind socket");
		printf("[*] Is the port already in use?\n");
		return 1;
	}
	
	err = listen(sockfd, 5);
	assert(err == 0);

	printf("[*] Serving garbage!\n");
	while (1) {
		// Accept connections
		struct sockaddr_storage their_addr; unsigned int sin_size;
		size_t conn = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (conn != -1) {
			// Spin up thread to handle it
			pthread_t thread;
			// Slight pthread abuse to send an fd instead of pointer
			pthread_create(&thread, NULL, thread_start, (void*)conn);
		}
	}
}
