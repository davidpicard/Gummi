/**
 * @file   configfile.c
 * @brief  handle configuration file
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "environment.h"
#include "configfile.h"
#include "utils.h"

static const char* config_filename = 0;

const char config_str[] =
"[Editor]\n"
"line_numbers = True\n"
"highlighting = True\n"
"textwrapping = True\n"
"wordwrapping = True\n"
"spelling = False\n"
"toolbar = True\n"
"statusbar = True\n"
"rightpane = True\n"
"spell_language = None\n"
"font = Monospace 10\n"
"\n"
"[File]\n"
"autosaving = False\n"
"autosave_timer = 600\n"
"\n"
"[Compile]\n"
"compile_status = True\n"
"typesetter = pdflatex\n"
"compile_scheme = on_idle\n"
"compile_timer = 1\n"
"idle_threshold = 2000\n"
"\n"
"[Misc]\n"
"recent1 = __NULL__\n"
"recent2 = __NULL__\n"
"recent3 = __NULL__\n"
"welcome = \\documentclass{article}\n"
"	\\begin{document}\n"
"	\\begin{center}\n"
"	\\Huge{Welcome to Gummi} \\\\\\\n"
"	\\\\\n"
"	\\LARGE{You are using the "VERSION" version.\\\\\n"
"	I welcome your suggestions at\\\\\n"
"	http://gummi.midnightcoding.org}\\\\\n"
"	\\end{center}\n"
"	\\end{document}\n\n";

void config_init(const char* filename) {
    config_filename = filename;
}

const char* config_get_value(const char* term) {
    FILE* fh = 0;
    char buf[BUF_MAX];
    static char ret[BUF_MAX];
    char* pstr;

    if (!(fh = fopen(config_filename, "r"))) {
        slog(L_INFO, "can't find configuration file, reseting to default\n");
        config_set_default();
        return config_get_value(term);
    }

    while (!feof(fh)) {
        fgets(buf, BUF_MAX, fh);
        buf[strlen(buf) -1] = 0;
        if (NULL == (pstr = strtok(buf, "[=] ")))
            continue;

        if (0 != strcmp(pstr, term))
            continue;
        if (NULL == (pstr = strtok(NULL, "=")))
            continue;
        else {
            strncpy(ret, pstr + 1, BUF_MAX);
            while (!feof(fh)) {
                fgets(buf, BUF_MAX, fh);
                buf[strlen(buf) -1] = 0;
                if (buf[0] == '\t') {
                    strncat(ret, "\n", BUF_MAX);
                    strncat(ret, buf + 1, BUF_MAX);
                } else break;
            }
            break;
        }
    }
    fclose(fh);
    if (0 == strcmp(ret, "False"))
        return NULL;
    return ret;
}

void config_set_value(const char* term, const char* value) {
    int i = 0, count = 0;
    int max = strlen(value) > BUF_MAX -1? BUF_MAX -1: strlen(value);
    finfo fin = config_load();
    int index = 0;
    char buf[BUF_MAX];

    if (-1 == (index = config_find_index_of(fin.pbuf, term)))
        slog(L_FATAL, "invalid configuration\n");

    fin.pbuf[index][strlen(term) + 3] = 0;
    for (i = 0; i < max; ++i) {
        if (count == BUF_MAX -1) break;
        buf[count++] = value[i];
        if (value[i] == '\n')
            buf[count++] = '\t';
    }
    buf[count] = 0;

    strncat(fin.pbuf[index], buf, BUF_MAX -2);
    strncat(fin.pbuf[index], "\n", BUF_MAX);

    for (i = index + 1; i < fin.len; ++i) {
        if (fin.pbuf[i][0] == '\t')
            fin.pbuf[i][0] = 0;
        else break;
    }

    config_save(fin.pbuf, fin.len);

    for (i = 0; i < CONFIG_MAX; ++i)
        g_free(fin.pbuf[i]);
    g_free(fin.pbuf);
}

void config_set_default(void) {
    FILE* fh = 0;
    if (!(fh = fopen(config_filename, "w")))
        slog(L_FATAL, "can't open config for writing... abort\n");

    fwrite(config_str, strlen(config_str), 1, fh);
    fclose(fh);
}

finfo config_load(void) {
    int i = 0, count = 0;
    FILE* fh = 0;

    char** pbuf = (char**) g_malloc(CONFIG_MAX * sizeof(char*));
    for (i = 0; i < CONFIG_MAX; ++i)
        pbuf[i] = (char*) g_malloc(BUF_MAX * sizeof(char));

    if (!(fh = fopen(config_filename, "r"))) {
        slog(L_INFO, "can't find configuration file, reseting to default\n");
        config_set_default();
        return config_load();
    }

    while (!feof(fh)) {
        if (count == BUF_MAX -1)
            slog(L_FATAL, "maximum buffer size reached\n");
        fgets(pbuf[count++], BUF_MAX, fh);
        pbuf[count -1][strlen(pbuf[count -1]) -1] = 0;
    }
    fclose(fh);
    return (finfo){ pbuf, count };
}

void config_save(char** pbuf, int len) {
    FILE* fh = 0;
    int i = 0;
    if (!(fh = fopen(config_filename, "w")))
        slog(L_FATAL, "can't open config for writing... abort\n");

    for (i = 0; i < len; ++i) {
        fputs(pbuf[i], fh);
        fputs("\n", fh);
    }
    fclose(fh);
}

int config_find_index_of(char** pbuf, const char* term) {
    int i = 0;
    for (i = 0; i < CONFIG_MAX; ++i) {
        if (0 == strncmp(pbuf[i], term, strlen(term)))
            return i;
    }
    return -1;
}
