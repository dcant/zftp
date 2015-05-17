#include "parseconf.h"
#include "tunables.h"
#include "str.h"
#include "err.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static struct parseconf_bool_setting {
	const char *name;
	int *value;
} parse_bool_array[] = 
{
	{"anonymous_enable", &tunable_anonymous_enable},
	{"local_enable", &tunable_local_enable},
	{"pasv_enable", &tunable_pasv_enable},
	{"port_enable", &tunable_port_enable},
	{"anonymous_upload_enable", &tunable_anonymous_upload_enable},
	{"ascii_mode_enable", &tunable_ascii_mode_enable},
	{"bin_mode_enable", &tunable_bin_mode_enable},
	{NULL, NULL}
};

static struct parseconf_int_setting {
	const char *name;
	int *value;
} parse_int_array[] =
{
	{"max_clients", &tunable_max_clients},
	{"max_conn_per_ip", &tunable_max_conn_per_ip},
	{"ctrl_conn_idle_time", &tunable_ctrl_conn_idle_time},
	{"data_conn_idle_time", &tunable_data_conn_idle_time},
	{"accept_timeout", &tunable_accept_timeout},
	{"connect_timeout", &tunable_connect_timeout},
	{"listen_port", &tunable_listen_port},
	{NULL, NULL}
};

static struct parseconf_str_setting {
	const char *name;
	const char **pvalue;
} parse_str_array[] =
{
	{"listen_addr", &tunable_listen_addr},
	{"dir_root", &tunable_dir_root},
	{NULL, NULL}
};

static void _load_conf_setting(const char *line)
{
	while (isspace(*line))
		line++;
	char name[MAXLINE] = {0};
	char value[MAXLINE] = {0};
	str_split(line, name, value, '=');
	if (strlen(value) == 0)
		ERROR_EXIT("conf split");

	// bool conf
	{
		struct parseconf_bool_setting *pbsetting = parse_bool_array;
		while (pbsetting->name != NULL) {
			if (strcmp(name, pbsetting->name) == 0) {
				if (strcmp("T", value) == 0 || strcmp("t", value) == 0)
					*(pbsetting->value) = 1;
				else if (strcmp("F", value) == 0 || strcmp("f", value) == 0)
					*(pbsetting->value) = 0;
				else
					ERROR_EXIT("conf value");
				return;
			}
			pbsetting++;
		}
	}

	// int conf
	{
		struct parseconf_int_setting *pisetting = parse_int_array;
		while (pisetting->name != NULL) {
			if (strcmp(name, pisetting->name) == 0) {
				*(pisetting->value) = atoi(value);
				return;
			}
			pisetting++;
		}
	}

	// str conf
	{
		struct parseconf_str_setting *pssetting = parse_str_array;
		while (pssetting->name != NULL) {
			if (strcmp(name, pssetting->name) == 0) {
				if (*(pssetting->pvalue))
					free((void *)*(pssetting->pvalue));
				*(pssetting->pvalue) = strdup(value);
				return;
			}
			pssetting++;
		}
	}
}

static void _load_conf_file(const char *file)
{
	FILE *fp = fopen(file, "r");
	if (fp == NULL)
		ERROR_EXIT("fopen");
	char bufline[MAXLINE + 1] = {0};
	while (fgets(bufline, MAXLINE, fp) != NULL) {
		if (strlen(bufline) == 0 || bufline[0] == '#' 
			|| str_all_space(bufline))
			continue;
		str_trim_crlf(bufline);
		_load_conf_setting(bufline);
		memset(bufline, 0, sizeof(bufline));
	}
	fclose(fp);
}

void init_conf(const char *file)
{
	_load_conf_file(file);
}