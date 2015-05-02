#ifndef _ERR_H_
#define _ERR_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#define ERROR_EXIT(msg) \
	do {\
			printf("File: %s, Line: %d, %s error!\n", __FILE__, __LINE__, msg);\
			perror("Error: ");\
			exit(-1);\
		} while (0)

#endif