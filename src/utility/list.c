#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "list.h"

List* initList() {
	List *a = malloc(sizeof(List));

	a->array = malloc(1 * sizeof(char*));
	a->used = 0;
	a->size = 1;

	return a;
}

void insertElementIntoList(List *a, char *element) {
	// a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
	// Therefore a->used can go up to a->size
	if (a->used == a->size) {
		a->size *= 2;
		a->array = realloc(a->array, a->size * sizeof(char*));
	}
	a->array[a->used++] = element;
}

char* getElement(List *a, int pos ){
	return a->array[pos];
}

bool contains(List *files, char *name) {

	for (int x = 0; x < files->used; ++x) {
		char *temp = getElement(files, x);

		if(strcmp(temp, name) == 0) {
			return true;
		}
	}

	return false;
}

int findFirstList(List *a, char *to, int from) {

	for (int x = from; x < a->used; ++x) {
		char *t = getElement(a, x);

		if(strcmp(t, to) == 0) {
			return x;
		}
	}

	return -1;
}

void printList(List *a) {
	for (int x = 0; x < a->used; ++x) {
		printf("\n%s", getElement(a, x));
	}
}

List* mapList(List *a, char *f(char *element)) {
	List *duplicate = initList();

	for (int x = 0; x < a->used; ++x) {
		insertElementIntoList(duplicate, f(getElement(a, x)));
	}

	return duplicate;
}
