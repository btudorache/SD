// Copyright 314CA Tudorache Bogdan-Mihai
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// numarul de biti dintr-un int (pe arhitectura folosita de mine)
#define BITS_IN_INT 32
// numarul de biti care intra in formarea bucket-ului
#define BITS_VERIFIED 11
// 2 la puterea numarului de biti verificati bucket-uri
#define NUM_BUCKETS 2048
// formula factorului de atenuare
#define A_M 0.7213/(1 + 1.079/NUM_BUCKETS)

// structura folosita pentru a estima numarul de intrari unice
struct Hashtable {
    int *buckets;
    int hmax;
    unsigned int (*hash_function)(void*);
};

// functie de hash pentru int
unsigned int hash_function_int(void *a) {
    /*
     * Credits: https://stackoverflow.com/a/12996028/7883884
     */
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

// initializarea structurii de date folosite
void init_ht(struct Hashtable *ht, int hmax,
             unsigned int (*hash_function)(void*)) {
    ht->buckets = calloc(hmax, sizeof(int));
    ht->hmax = hmax;
    ht->hash_function = hash_function;
}

// functia de procesare si adaugare in structura
void put(struct Hashtable *ht, void *key) {
    if (ht == NULL) {
        return;
    }
    // trecerea inputului prin functia de hash
    unsigned int index = ht->hash_function(key);
    // formarea bucket-ului prin shiftare pe biti
    unsigned int bucket = index >> (BITS_IN_INT - BITS_VERIFIED);
    // aflarea secventei de zero prin shiftare pe biti
    unsigned int zeros = 1;
    for (int i = 0; i < BITS_VERIFIED - 1; i++) {
        zeros = zeros << 1;
        zeros++;
    }
    zeros = zeros << (BITS_IN_INT - BITS_VERIFIED);
    zeros = ~zeros;
    zeros = zeros & index;
    unsigned int num_zeros = BITS_IN_INT - BITS_VERIFIED + 1;
    while (zeros != 0) {
        num_zeros--;
        zeros = zeros >> 1;
    }
    // actualizarea bucket-ului
    if (num_zeros > (unsigned int)ht->buckets[bucket]) {
        ht->buckets[bucket] = num_zeros;
    }
}

// aflarea mediei armonice a intrarilor din structura
double get_mean(struct Hashtable *ht) {
    double pow, s = 0;
    for (int i = 0; i < ht->hmax; i++) {
        pow = 1 << ht->buckets[i];
        s += 1/pow;
    }
    s = 1/s;
    return s;
}

// estimarea numarului de elemente folosind formula specifica
int estimate_elements(struct Hashtable *ht) {
    double mean = get_mean(ht);
    double distinct = mean * NUM_BUCKETS * NUM_BUCKETS * A_M;
    return (int)distinct;
}

// eliberarea structurii folosite
void free_ht(struct Hashtable *ht) {
    if (ht == NULL) {
        return;
    }
    free(ht->buckets);
    free(ht);
}

// in caz ca fisierul nu poate fi citit se iese din program
void file_error(FILE * file) {
	if (file == NULL) {
		printf("Couldn't open file");
		exit(-1);
	}
}

// citirea datelor din fisier si adaugarea lor in structura
void add_data(struct Hashtable *ht, FILE* file) {
	unsigned int n;
	char buffer[10];
	// citire pana la EOF
	while (fgets(buffer, sizeof(buffer), file)) {
		// convertire din string in int
		n = atoi(buffer);
		// introducere in structura
		put(ht, &n);
	}
}

int main(int argc, char *argv[]) {
	// initializarea si alocarea structurii de date
	struct Hashtable *ht_hll = malloc(sizeof(struct Hashtable));
	init_ht(ht_hll, NUM_BUCKETS, hash_function_int);
	// daca comanda de rulare nu este corecta se iese din program
	if (argc != 2) {
		fprintf(stderr, "usage: %s input_file\n", argv[0]);
		return -1;
	}
	// formarea numelui fisierului
	char input_file[40];
	snprintf(input_file, sizeof(input_file), "%s", argv[1]);
	// deschiderea fisierului
	FILE *fin = fopen(input_file, "r+");
	file_error(fin);


	add_data(ht_hll, fin);
	printf("%d\n", estimate_elements(ht_hll));

	fclose(fin);
	free_ht(ht_hll);
	return 0;
}
