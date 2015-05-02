#include "str.h"
#include <ctype.h>
#include <string.h>

int str_all_space(const char *str)
{
	while (*str) {
		if (!isspace(*str))
			return 0;
		str++;
	}
	return 1;
}

void str_trim_crlf(char *str)
{
	char *ptr = str + strlen(str) - 1;
	while (*ptr == '\r' || *ptr == '\n')
		*ptr-- = '\0';
}

void str_split(const char *str, char *name, char *value, char splitter)
{
	char *p = strchr(str, splitter);
	if (p == NULL)
		strncpy(name, str, MAXLINE);
	else {
		strncpy(name, str, p - str);
		strncpy(value, p + 1, MAXLINE);
	}
}

void str_toupper(char *str)
{
	while (*str) {
		*str = toupper(*str);
		str++;
	}
}