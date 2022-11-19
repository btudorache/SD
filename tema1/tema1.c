// Copyright 314CA Tudorache Bogdan-Mihai
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./PlaylistFunctions.h"

int main(int argc, char *argv[]) {
	// initializarea listei si declararea variabilelor necesare
	struct DoublyLinkedList* playlist = initialize_list();
	char input_file[15], output_file[15];
	struct Metadata* data;

	// programul nu accepta decat 3 parametri in linia de comanda
	if (argc != 3) {
		fprintf(stderr, "usage: %s input_file output_file\n", argv[0]);
		return -1;
	}
	// copierea inputului necesar pentru deschiderea fisierelor in doua stringuri
	snprintf(input_file, sizeof(input_file), "%s", argv[1]);
	snprintf(output_file, sizeof(output_file), "%s", argv[2]);

	// deschiderea fisierelor
	FILE *fin = fopen(input_file, "r+");
	file_error(fin);
	FILE *fout = fopen(output_file, "w+");
	file_error(fout);

	int commands;
	// citirea din fisier a numarului de comenzi
	fscanf(fin, "%d\n", &commands);

	char command[15], song[MAX_LINE_LENGTH], line[MAX_LINE_LENGTH];
	// pentru fiecare comanda se efectueaza o operatie
	for (int i = 0; i < commands; i++) {
		// citire linie cu linie din fisier
		fgets(line, MAX_LINE_LENGTH, fin);
		if (line[strlen(line)-1] == '\n') {
			line[strlen(line)-1] = '\0';
		}
		// separarea comenzii de numele melodiei in 2 stringuri diferite
		sscanf(line, "%s", command);
		memset(song, 0, MAX_LINE_LENGTH);
		snprintf(song, sizeof(song), "%s", line + strlen(command) + 1);

		// fiecarei comenzi ii este asociata o operatie specifica
		if (strcmp(command, "ADD_FIRST") == 0) {
			/* in cazul operatiilor de add, se construieste structura ce contine datele
			   despre melodie si se transmite functiei aferente comenzii ca argument */
			data = get_data(song);
			add_first(playlist, data, fout);
		} else if (strcmp(command, "ADD_LAST") == 0) {
			data = get_data(song);
			add_last(playlist, data, fout);
		} else if (strcmp(command, "ADD_AFTER") == 0) {
			data = get_data(song);
			add_after(playlist, data, fout);
		} else if (strcmp(command, "MOVE_NEXT") == 0) {
			move_next(playlist, fout);
		} else if (strcmp(command, "MOVE_PREV") == 0) {
			move_prev(playlist, fout);
		} else if (strcmp(command, "SHOW_FIRST") == 0) {
			show_first(playlist, fout);
		} else if (strcmp(command, "SHOW_LAST") == 0) {
			show_last(playlist, fout);
		} else if (strcmp(command, "SHOW_CURR") == 0) {
			show_curr(playlist, fout);
		} else if (strcmp(command, "SHOW_PLAYLIST") == 0) {
			show_playlist(playlist, fout);
		} else if (strcmp(command, "DEL_FIRST") == 0) {
			del_first(playlist, fout);
		} else if (strcmp(command, "DEL_LAST") == 0) {
			del_last(playlist, fout);
		} else if (strcmp(command, "DEL_CURR") == 0) {
			del_curr(playlist, fout);
		} else if (strcmp(command, "DEL_SONG") == 0) {
			/* in cazul operatiei "DEL_SONG", se obtine titlul melodiei si se transmite
			   functiei aferente comenzii ca argument */
			char* song_check = get_title(song);
			del_song(playlist, fout, song_check);
			// eliberarea pointerului la titlul melodiei
			free(song_check);
		}
	}

	// inchiderea fisierelor si eliberarea memoriei
	fclose(fin);
	fclose(fout);
	free_list(&playlist);
	return 0;
}
