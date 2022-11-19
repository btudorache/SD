// Copyright 314CA Tudorache Bogdan-Mihai
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./PlaylistFunctions.h"

// in cazul in care fisierul nu se poate deschide, se iese fortat din program
void file_error(FILE *file) {
    if (file == NULL) {
        printf("Can't open file!\n");
        exit(-1);
    }
}

// initializarea listei dublu-inlantuite
struct DoublyLinkedList* initialize_list() {
	struct DoublyLinkedList* list = malloc(sizeof(struct DoublyLinkedList));
	list->head = NULL;
	list->tail = NULL;
	list->cursor = NULL;
	list->size = 0;
	return list;
}

/* se verifica daca o melodie anume se afla deja in playlist, si in caz
   afirmativ se intoarce un pointer la nodul ce contine melodia */
struct Node* check_list(struct DoublyLinkedList* list, char* song) {
	if (list == NULL || list->size == 0) {
		return NULL;
	}
	struct Node* curr = list->head;
	// parcurgerea fiecarui nod din lista
	for (int i = 0; i < list->size; i++) {
		if (strcmp(curr->container_data->title, song) == 0) {
			break;
		}
		curr = curr->next;
	}
	return curr;
}

// stergerea primei melodii din lista
void del_first(struct DoublyLinkedList* list, FILE* output) {
	if (list == NULL || list->head == NULL) {
		fprintf(output, "Error: delete from empty playlist\n");
		return;
	}
	struct Node* curr = list->head;
	// daca in lista se afla un singur nod, se elibereaza lista
	if (list->size == 1) {
		// se elibereaza nodul si structura de date
		free(curr->container_data);
		free(curr);
		list->head = NULL;
		list->cursor = NULL;
		list->size--;
	} else {
		// cursorul se plaseaza fie in fata, fie devine NULL
		if (list->cursor == curr) {
			if (curr->next != NULL) {
				list->cursor = curr->next;
			} else {
				list->cursor = NULL;
			}
		}
		// nodul din fata actualului head devine noul head
		list->head = curr->next;
		curr->next->prev = NULL;
		// se elibereaza nodul si structura de date
		free(curr->container_data);
		free(curr);
		list->size--;
	}
}

// stergerea ultimei melodii din lista
void del_last(struct DoublyLinkedList* list, FILE* output) {
	if (list == NULL || list->tail == NULL) {
		fprintf(output, "Error: delete from empty playlist\n");
		return;
	}
	struct Node* curr = list->tail;
	// daca in lista se afla un singur nod, se elibereaza lista
	if (list->size == 1) {
		// se elibereaza nodul si structura de date
		free(curr->container_data);
		free(curr);
		list->tail = NULL;
		list->cursor = NULL;
		list->size--;
	} else {
		// cursorul se plaseaza fie in spate, fie devine NULL
		if (list->cursor == curr) {
			if (curr->prev != NULL) {
				list->cursor = curr->prev;
			} else {
				list->cursor = NULL;
			}
		}
		// nodul din spatele actualului tail devine noul tail
		list->tail = list->tail->prev;
		list->tail->next = NULL;
		// se elibereaza nodul si structura de date
		free(curr->container_data);
		free(curr);
		list->size--;
	}
}

// stergerea unei melodii specifice
void del_song(struct DoublyLinkedList* list, FILE* output, char* song) {
	if (list == NULL || list->head == NULL) {
		fprintf(output, "Error: no song found to delete\n");
		return;
	}
	// se cauta nodul in lista cu ajutorul functiei check_list
	struct Node* curr = check_list(list, song);
	// daca nu se gaseste melodia in playlist, se ignora comanda
	if (curr == NULL) {
		fprintf(output, "Error: no song found to delete\n");
	// daca melodia cautata este in head, se sterge primul element
	} else if (curr == list->head) {
		del_first(list, output);
	// daca melodia cautata este in tail, se sterge ultimul element
	} else if (curr == list->tail) {
		del_last(list, output);
	} else {
		// daca cursorul se afla pe melodia cautata, cursorul trece pe urmatoarea
		if (list->cursor == curr) {
			list->cursor = curr->next;
		}
		// se leaga nodurile anterioare si posterioare fata de cel curent
		curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
        // se elibereaza nodul curent si structura de date
        free(curr->container_data);
		free(curr);
        list->size--;
	}
}

/* functia care intoare un pointer la un struct Metadata ce contine
   datele despre o melodie deja completat */
struct Metadata* get_data(char* song) {
	// alocarea unui struct Metadata
	struct Metadata* data = malloc(sizeof(struct Metadata));
	memset(data, 0, sizeof(struct Metadata));

	// deschiderea melodiei pentru a prelua datele binare
	char file[MAX_LINE_LENGTH];
	snprintf(file, sizeof(file), "songs/%s", song);
	FILE *song_file = fopen(file, "rb");
	file_error(song_file);

	char buffer[31];
	memset(buffer, 0, 31);
	// plasarea in fisier cu 94 de octeti fata de sfarsit
	fseek(song_file, START_OF_DATA, SEEK_END);
	// citirea numelui melodiei
	fread(&buffer, sizeof(char), 30, song_file);
	// scrierea numelui melodiei in structura
	snprintf(data->title, sizeof(data->title), "%s", buffer);
	// citirea numelui artistului
	fread(&buffer, sizeof(char), 30, song_file);
	// scrierea numelui artistului in structura
	snprintf(data->artist, sizeof(data->artist), "%s", buffer);
	// citirea numelui albumului
	fread(&buffer, sizeof(char), 30, song_file);
	// scrierea numelui albumului in structura
	snprintf(data->album, sizeof(data->album), "%s", buffer);
	// citirea anului aparitiei
	fread(&buffer, sizeof(char), 4, song_file);
	// scrierea numelui aparitiei in structura
	snprintf(data->year, sizeof(data->year), "%s", buffer);

	// inchiderea fisierului folosit
	fclose(song_file);
	return data;
}

// functie care cauta in fisier numele melodiei
char* get_title(char* song) {
	char file[MAX_LINE_LENGTH];
	// deschiderea melodiei pentru a prelua numele
	snprintf(file, sizeof(file), "songs/%s", song);
	FILE *song_file = fopen(file, "rb");
	file_error(song_file);

	char* song_title = malloc(31 * sizeof(char));
	// plasarea in fisier cu 94 de octeti fata de sfarsit
	fseek(song_file, START_OF_DATA, SEEK_END);
	// citirea numelui melodiei
	fread(song_title, sizeof(char), 30, song_file);
	return song_title;
}

// adaugarea melodiei in prima pozitie a listei
void add_first(struct DoublyLinkedList* list, struct Metadata* data, FILE* f) {
	if (list == NULL) {
		free(data);
		return;
	}

	// daca melodia se gaseste deja in playlist, se sterge nodul aferent
	struct Node* curr = check_list(list, data->title);
	if (curr != NULL) {
		del_song(list, f, data->title);
	}

	// alocarea unui nod pentru a-l adauga in lista
	struct Node* new = malloc(sizeof(struct Node));
	// adaugarea datelor despre melodie in nod
	new->container_data = data;

	new->prev = NULL;
	// adaugarea in lista in cazul in care lista e goala
	if (list->size == 0) {
		list->head = new;
		list->head->prev = NULL;
		list->tail = new;
		list->tail->next = NULL;
		list->cursor = new;
		new->next = NULL;
	} else {
		// adaugarea in mod normal
		new->next = list->head;
		list->head->prev = new;
		list->head = new;
	}
	list->size++;
}

// adaugarea melodiei la finalul listei
void add_last(struct DoublyLinkedList* list, struct Metadata* data, FILE* f) {
	if (list == NULL) {
		free(data);
		return;
	}

	// daca melodia se gaseste deja in playlist, se sterge nodul aferent
	struct Node* curr = check_list(list, data->title);
	if (curr != NULL) {
		del_song(list, f, data->title);
	}

	// alocarea unui nod pentru a-l adauga in lista
	struct Node* new = malloc(sizeof(struct Node));
	// adaugarea datelor despre melodie in nod
	new->container_data = data;

	new->next = NULL;
	// adaugarea in lista in cazul in care lista e goala
	if (list->size == 0) {
		list->head = new;
		list->head->prev = NULL;
		list->tail = new;
		list->tail->next = NULL;
		list->cursor = new;
		new->prev = NULL;
	} else {
		// adaugarea in mod normal
		new->prev = list->tail;
		list->tail->next = new;
		list->tail = new;
	}
	list->size++;
}

// adaugarea melodiei dupa cursor
void add_after(struct DoublyLinkedList* list, struct Metadata* data, FILE* f) {
	if (list == NULL || list->cursor == NULL) {
		// eliberarea pointerului struct Metadata alocat in main daca nu e folosit
		free(data);
		return;
	}
	/* daca melodia adaugata dupa cursor este aceeasi ca cea de pe cursor 
	   se ignora comanda */
	if (strcmp(list->cursor->container_data->title, data->title) == 0) {
		// eliberarea pointerului struct Metadata alocat in main daca nu e folosit
		free(data);
		return;
	}

	// daca melodia se gaseste deja in playlist, se sterge nodul aferent
	struct Node* curr = check_list(list, data->title);
	if (curr != NULL) {
		del_song(list, f, data->title);
	}

	// daca cursorul se afla pe ultimul element, melodia se adauga la final
	if (list->cursor == list->tail) {
		add_last(list, data, f);
	} else {
		// adaugarea in mod normal
		struct Node* new = malloc(sizeof(struct Node));
		new->container_data = data;

		new->next = list->cursor->next;
		new->prev = list->cursor;
		list->cursor->next->prev = new;
		list->cursor->next = new;
		list->size++;
	}
}

// mutarea cursorului pe pozitia urmatoare
void move_next(struct DoublyLinkedList* list, FILE *output) {
	if (list == NULL || list->cursor == NULL) {
		fprintf(output, "Error: no track playing\n");
		return;
	}
	// mutarea se face daca in fata cursorului exista alta melodie
	if (list->cursor->next != NULL) {
		list->cursor = list->cursor->next;
	}
}

// mutarea cursorului pe pozitia anterioara
void move_prev(struct DoublyLinkedList* list, FILE *output) {
	if (list == NULL || list->cursor == NULL) {
		fprintf(output, "Error: no track playing\n");
		return;
	}
	// mutarea se face daca in spatele cursurului exista alta melodie
	if (list->cursor->prev != NULL) {
		list->cursor = list->cursor->prev;
	}
}

// afisarea elementelor dintr-un struct Metadata intr-un fisier
void print_data(struct Metadata* data, FILE* output) {
	char line[MAX_LINE_LENGTH];
	memset(line, 0, MAX_LINE_LENGTH);

	snprintf(line, sizeof(line), "Title: %s", data->title);
	fprintf(output, "%s\n", line);

	snprintf(line, sizeof(line), "Artist: %s", data->artist);
	fprintf(output, "%s\n", line);

	snprintf(line, sizeof(line), "Album: %s", data->album);
	fprintf(output, "%s\n", line);

	snprintf(line, sizeof(line), "Year: %s", data->year);
	fprintf(output, "%s\n", line);
}

// afisarea primului element din lista
void show_first(struct DoublyLinkedList* list, FILE* output) {
	if (list->head == NULL || list == NULL || list->size == 0) {
		char line[MAX_LINE_LENGTH];
		snprintf(line, sizeof(line), "Error: show empty playlist\n");
		fprintf(output, "%s", line);
	} else {
		print_data(list->head->container_data, output);
	}
}

// afisarea ultimului element din lista
void show_last(struct DoublyLinkedList* list, FILE* output) {
	if (list->tail == NULL || list == NULL || list->size == 0) {
		char line[MAX_LINE_LENGTH];
		snprintf(line, sizeof(line), "Error: show empty playlist\n");
		fprintf(output, "%s", line);
	} else {
		print_data(list->tail->container_data, output);
	}
}

// afisarea elementului pe care se afla cursorul
void show_curr(struct DoublyLinkedList* list, FILE* output) {
	if (list->cursor == NULL || list == NULL || list->size == 0) {
		char line[MAX_LINE_LENGTH];
		snprintf(line, sizeof(line), "Error: show empty playlist\n");
		fprintf(output, "%s", line);
	} else {
		print_data(list->cursor->container_data, output);
	}
}

// afisarea intregului playlist
void show_playlist(struct DoublyLinkedList* list, FILE* output) {
	if (list == NULL) {
		return;
	}
	if (list->head == NULL || list->size == 0) {
		fprintf(output, "[]\n");
		return;
	}
	char playlist[MAX_PLAYLIST_LENGTH];
	snprintf(playlist, sizeof(playlist), "[");

	struct Node* curr = list->head;
	char *title;
	// se parcurge fiecare element al listei si se introduce titlul intr-un string
	while (curr->next != NULL) {
		title = curr->container_data->title;
		snprintf(playlist + strlen(playlist), sizeof(playlist), "%s; ", title);
		curr = curr->next;
	}
	title = curr->container_data->title;
	snprintf(playlist + strlen(playlist), sizeof(playlist), "%s", title);
	// afisarea playlistului intr-un fisier
	fprintf(output, "%s]\n", playlist);
}

// stergerea melodiei pe care se afla cursorul
void del_curr(struct DoublyLinkedList* list, FILE* output) {
	if (list == NULL || list->cursor == NULL) {
		fprintf(output, "Error: no track playing\n");
		return;
	}
	// daca cursorul se afla pe head, se sterge primul element
	if (list->cursor == list->head) {
		del_first(list, output);
	// daca cursorul se afla pe tail, se sterge ultimul element
	} else if (list->cursor == list->tail) {
		del_last(list, output);
	} else {
		// stergere in mod normal
		struct Node* curr = list->cursor;
		list->cursor = curr->next;

		curr->prev->next = curr->next;
        curr->next->prev = curr->prev;
        // se elibereaza structura de date si nodul curent
        free(curr->container_data);
		free(curr);
        list->size--;
	}
}

// eliberarea listei dublu-inlantuite
void free_list(struct DoublyLinkedList **pp_list) {
	struct DoublyLinkedList* list = *pp_list;
    struct Node* curr = list->head;
    struct Node* prev;
    if (list->size != 0) {
    	// daca lista nu e goala, se elibereaza fiecare nod in parte
    	while(curr != NULL) {
    		prev = curr;
    		curr = curr->next;
    		free(prev->container_data);
    		free(prev);
    	}
    }
    // eliberarea structurii LinkedList
    free(list);
}
