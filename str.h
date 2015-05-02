#ifndef _STR_H_
#define _STR_H_
#define MAXLINE 1024

int str_all_space(const char *str);
void str_trim_crlf(char *str);
void str_split(const char *str, char *name, char *value, char splitter);
void str_toupper(char *str);

#endif