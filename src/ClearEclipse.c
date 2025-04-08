/*
 * ClearEclipse.c
 *
 *  Created on: 7 apr 2025
 *      Author: fatiraco
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <sys/stat.h>

#if defined(__CYGWIN__) || defined(_WIN32)
#include <dirent.h>
#include <windows.h>
#include <shlwapi.h>
#else
#include <fnmatch.h>
#endif

#include "utility/utils.h"
#include "utility/list.h"

char *getFileLastChangeTime(char *path) {
    struct stat attr;
    stat(path, &attr);

    struct tm *tm = localtime(&attr.st_mtime);

    char *format;
    asprintf(&format, "%s/%s/%s",
        	tm->tm_mday < 10 ? "0%i": "%i",
        	tm->tm_mon + 1 < 10 ? "0%i": "%i",
        	"%i");

    char *buf;
    asprintf(&buf, format, tm->tm_mday, tm->tm_mon + 1,1900 + tm->tm_year);

    return buf;
}

List* listAllFiles(const char *file)
{
	List *allFile = initList();

    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(file))){
    	printf("\nFolder not found: %s", file);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {

    	if (strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0 ) continue;

    	char *path = createPath(file, entry->d_name);

    	if(!isFolder(path)) {
    		insertElementIntoList(allFile, path);
    	}
    }

    closedir(dir);

    return allFile;
}

List* extractDuplicates(List* allFile) {
	List *duplicate = initList();

	// Search duplicate
	for (int x = 0; x < allFile->used; ++x) {
		char *name = getElement(allFile, x);

		// If is not size - 1
		if (x + 1 < allFile->used) {

			if (findFirstList(allFile, name, x + 1) > 0 && findFirstList(duplicate, name, 0) < 0) {
				insertElementIntoList(duplicate, name);
			}
		}
	}

	return duplicate;
}

char *mapped(char *path) {
	char *clone;
	asprintf(&clone, "%s", basename(path));

	char *name;
	asprintf(&name, "%s", strtok(clone, "_"));
	return name;
}

void printHelp() {
	printf("Sintassi: ClearEclipse [-h] [--drayrun] path date");

	printf("\nArgs:");
	printf("\n\t path \t\t Path assoluto della cartella plugins di eclipse");
	printf("\n\t date \t\t Data dei file doppiani da cancellare");

	printf("\nOpzioni:");
	printf("\n\t -h \t\t Mostrare help");
	printf("\n\t -d \t\t Mostrare solo i duplicati");
	printf("\n\t --drayrun \t Mostra solo i file da cancellare");

	exit(0);
}

/**
 * E:\Programmazione\eclipse-mio\plugins 19/12/2024 --drayrun
 */
int main(int argc, char **argv) {
	char *path = NULL;
	char *date = NULL;
	char *value = NULL;
	static int drayRun;
	static int showDuplicate;

	static struct option long_options[] = {
		/* These options set a flag. */
		{ "drayrun", 	no_argument, 			&drayRun, 	1 	},
		{ "d", 			no_argument, 			NULL, 		'd' },
		{ "h",  		no_argument,       		NULL, 		'h' },
		/* The following options don't set a flag. */
		{ "value1", 	required_argument, 		NULL, 		'v' },
		{ 0, 0, 0, 0 }
	};

	while (1) {

		/* getopt_long stores the option index here. */
		int option_index = 0;

		int c = getopt_long(argc, argv, "hdv:", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1) {
			break;
		}

		switch (c) {
			case 0:
				/* (In this example) only options which set */
				/* a flag return zero, so do nothing. */
				break;
			case 'd':
				showDuplicate = 1;
				break;
			case 'h':
				printHelp();
				break;
			case 'v':
				asprintf(&value, "%s", optarg);
				break;

			default:
				fprintf(stderr, "Invalid option %c\n", c);
				return 1;
		}
	}

	for (int x = optind; x < argc; x++) {
		if(x == optind) {
			path = argv[x];
		} else {
			date = argv[x];
		}
	}

	if((NULL == path || strlen(path) == 0) || ((NULL == date || strlen(date) == 0) && showDuplicate == 0)){
		printf("Errore sintassi non valida\n");

		printHelp();
	}

	List *all = listAllFiles(path);
	List *duplicate = extractDuplicates(mapList(all, mapped));
	List *toDelete = initList();

	if(showDuplicate == 1) {
		printList(duplicate);
		exit(0);
	}


	for (int x = 0; x < all->used; ++x) {
		char *path = getElement(all, x);

		char *change = getFileLastChangeTime(path);

		if(strcmp(change, date) == 0 && findFirstList(duplicate, mapped(path), 0) > -1) {
			printf("\nto delete path: %s", path);

			insertElementIntoList(toDelete, path);
		}
	}

	if(drayRun == 0 && toDelete->used > 0) {

		char input;
		printf("\nDo you want to delete files? Y / N\n");
		scanf("%c",&input);

		if (input == 'Y') {

			for (int x = 0; x < toDelete->used; ++x) {

				char *path = getElement(toDelete, x);
				int status = remove(path);

				/*Check if file has been properly deleted*/
				if (status != 0) {
					printf("Error on delete file %s\n", path);
					exit(0);
				}

				print_progress(x, toDelete->used - 1);
			}
		}
	} else {
		printf("Nothing to delete\n");
	}

	exit(0);
}


