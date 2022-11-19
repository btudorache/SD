// Copyright 314CA Tudorache Bogdan-Mihai
#ifndef PLAYLISTFUNCTIONS_H_
#define PLAYLISTFUNCTIONS_H_

#define MAX_LINE_LENGTH 100  // lungimea maxima a unei linii
#define MAX_PLAYLIST_LENGTH 2048  // lungimea maxima a intregului playlist
#define START_OF_DATA -94  // pozitia la care ma plasez in fisier fata de final

// structura in care memorez datele despre melodie
struct Metadata {
	char title[31];
	char artist[31];
	char album[31];
	char year[5];
};

// structura Node care contine si un pointer catre struct Metadata
struct Node {
	struct Node* next;
	struct Node* prev;
	struct Metadata* container_data;
};

// structura ce defineste lista dublu-inlantuita
struct DoublyLinkedList {
	struct Node* head;
	struct Node* tail;
	struct Node* cursor;
	int size;
};

// declaratiile functiilor folosite in cadrul temei
void file_error(FILE *file);

struct DoublyLinkedList* initialize_list();

struct Node* check_list(struct DoublyLinkedList* list, char* song);

void del_first(struct DoublyLinkedList* list, FILE* output);

void del_last(struct DoublyLinkedList* list, FILE* output);

void del_song(struct DoublyLinkedList* list, FILE* output, char* song);

struct Metadata* get_data(char* song);

char* get_title(char* song);

void add_first(struct DoublyLinkedList* list, struct Metadata* data, FILE* f);

void add_last(struct DoublyLinkedList* list, struct Metadata* data, FILE* f);

void add_after(struct DoublyLinkedList* list, struct Metadata* data, FILE* f);

void move_next(struct DoublyLinkedList* list, FILE *output);

void move_prev(struct DoublyLinkedList* list, FILE *output);

void print_data(struct Metadata* data, FILE* output);

void show_first(struct DoublyLinkedList* list, FILE* output);

void show_last(struct DoublyLinkedList* list, FILE* output);

void show_curr(struct DoublyLinkedList* list, FILE* output);

void show_playlist(struct DoublyLinkedList* list, FILE* output);

void del_curr(struct DoublyLinkedList* list, FILE* output);

void free_list(struct DoublyLinkedList **pp_list);

#endif  // PLAYLISTFUNCTIONS_H_
