// Copyright 314CA Tudorache Bogdan-Mihai
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUMBERS 2000001

/* functia de citire a datelor de la stdin 
   si adugarea lor in vectorul de frecventa */
void add_data(unsigned char *freq) {
	int n;
	char buffer[10];
	// citire de la stdin pana la EOF
	while (fgets(buffer, sizeof(buffer), stdin)) {
		n = atoi(buffer);
		// actualizare vectorului de frecventa
		freq[n]++;
	}
}

// afisarea vectorului de frecventa
void print_numbers(unsigned char *freq) {
	for (int i = 0; i < MAX_NUMBERS; i++) {
		if (freq[i]) {
			printf("%d %d\n", i, freq[i]);
		}
	}
}

int main() {
	// vectorul de frecventa de tip static unsigned char
	static unsigned char freq[MAX_NUMBERS];

	add_data(freq);
	print_numbers(freq);
	return 0;
}
