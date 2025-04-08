#include <stdbool.h>

typedef struct {
	char **array;
	size_t used;
	size_t size;
} List;


List* initList();
void insertElementIntoList(List *a, char *element);
char* getElement(List *a, int pos );
bool contains(List *a, char *name);
int findFirstList(List *a, char *element, int from);
void printList(List *a);
List* mapList(List *a, char *f(char *element));
