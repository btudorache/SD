// Copyright 314CA Tudorache Bogdan-Mihai
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BUFFER_SIZE 100

struct info {
    void *key;
    int freq;
};

struct Hashtable {
    struct info *buckets;
    int hmax;
    unsigned int (*hash_function)(void*);
    int (*compare_function)(void*, void*);
};

// functie de comparare a string-urilor
int compare_function_strings(void *a, void *b) {
    char *str_a = (char *)a;
    char *str_b = (char *)b;

    return strcmp(str_a, str_b);
}

// functie de hash pentru string
unsigned int hash_function_string(void *a) {
    /*
     * Credits: http://www.cse.yorku.ca/~oz/hash.html
     */
    unsigned char *puchar_a = (unsigned char*) a;
    uint64_t hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

// initializarea nodului ce stocheaza informatia
void init_info(struct info *info) {
    info->key = NULL;
    info->freq = 0;
}

// initializarea hashtable-ului
void init_ht(struct Hashtable *ht, int hmax,
             unsigned int (*hash_function)(void*),
             int (*compare_function)(void*, void*)) {
    ht->buckets = malloc(hmax * sizeof(struct info));
    for (int i = 0; i < hmax; i++) {
        init_info(&ht->buckets[i]);
    }
    ht->hmax = hmax;
    ht->hash_function = hash_function;
    ht->compare_function = compare_function;
}

// introducerea intrarilor in hashtable
void put(struct Hashtable *ht, void *key, int key_size_bytes) {
    if (ht == NULL) {
        return;
    }
    int index = ht->hash_function(key) % ht->hmax;
    if (ht->buckets[index].freq != 0) {
    	// daca intrarea si cheia din bucket sun egale creste frecventa
        if (ht->compare_function(ht->buckets[index].key, key) == 0) {
                ht->buckets[index].freq++;
                return;
        }
        // evitarea coliziunilor prin linear probing
        int ok = 0, start_pos = index;
        while (index <= ht->hmax) {
            index++;
            if (ht->buckets[index].freq == 0) {
                ok = 1;
                break;
            }
        }
        if (ok != 1) {
            index = 0;
            while (index < start_pos) {
                index++;
                if (ht->buckets[index].freq == 0) {
                    ok = 1;
                    break;
                }
            }
        }
        // verificare daca nu mai e loc in hashtable
        if (ok != 1) {
            printf("couldn't add element\n");
            return;
        }
        // adaugarea noului element dupa linear probing
        void *cheie_noua = malloc(key_size_bytes);
        memcpy(cheie_noua, key, key_size_bytes);
        ht->buckets[index].key = cheie_noua;
        ht->buckets[index].freq++;
    } else {
    	// adaugarea noului element daca nu se gaseste nimic la bucketul respectiv
        void *cheie_noua = malloc(key_size_bytes);
        memcpy(cheie_noua, key, key_size_bytes);
        ht->buckets[index].key = cheie_noua;
        ht->buckets[index].freq = 1;
    }
}

// afisarea elementelor din hashtable
void print_hashtable(struct Hashtable *ht) {
    struct info* date;
    for (int i = 0; i < ht->hmax; i++) {
        if (ht->buckets[i].freq != 0) {
            date = &ht->buckets[i];
            printf("%s %d\n", (char*)date->key, date->freq);
        }
    }
}

// eliberarea hashtable-ului
void free_ht(struct Hashtable *ht) {
    if (ht == NULL) {
        return;
    }
    for (int i = 0; i < ht->hmax; i++) {
        free(ht->buckets[i].key);
    }
    free(ht->buckets);
    free(ht);
}

// numararea liniilor dintr-un fisier
int count_lines(FILE* file) {
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	int lines = 0;
	// citire din fisier pana la EOF
	while (fgets(buffer, sizeof(buffer), file)) {
		lines++;
	}
	// plasarea la inceputul fisierului dupa prelucrare
	fseek(file, 0, SEEK_SET);
	return lines;
}

// citirea datelor de la stdin si introducerea lor in hashtable
void add_data(FILE* file, struct Hashtable* ht) {
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, sizeof(buffer));
	// citire pana la EOF
	while (fgets(buffer, sizeof(buffer), file)) {
		buffer[strlen(buffer) - 1] = '\0';
		// introducere in hashtable
		put(ht, buffer, sizeof(buffer));
	}
}

int main() {
	// initializarea hashtable-ului
	int hmax = count_lines(stdin);
	struct Hashtable *ht_hash = malloc(sizeof(struct Hashtable));
	init_ht(ht_hash, hmax, hash_function_string, compare_function_strings);

	add_data(stdin, ht_hash);
	print_hashtable(ht_hash);

	free_ht(ht_hash);
	return 0;
}
