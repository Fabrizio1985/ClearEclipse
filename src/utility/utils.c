 #define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>


#include "utils.h"


char *createPath(char *dir,  char *name) {
	char *path;
	#if defined(__CYGWIN__) || defined(_WIN32)
    	asprintf(&path, "%s\\%s", dir, name);
    #else
    	asprintf(&path, "%s/%s", dir, name);	
    #endif

	return path;
}


bool isFolder(char *path){
		
	struct stat sb;
    return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

void print_progress(size_t count, size_t max) {
    const int bar_width = 50;

    float progress = (float) count / max;
    int bar_length = progress * bar_width;

    printf("\rProgress: [");
    for (int i = 0; i < bar_length; ++i) {
        printf("#");
    }
    for (int i = bar_length; i < bar_width; ++i) {
        printf(" ");
    }
    printf("] %.2f%%", progress * 100);

    fflush(stdout);
}
