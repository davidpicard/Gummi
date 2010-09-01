/**
 * @file   environment.c
 * @brief  
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#include "environment.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

int workfd = -1;

gummi_t* gummi_init(GtkBuilder* bd, editor_t* ed, importer_t* im,
        iofunctions_t* iof, motion_t* mo, preview_t* prev) {
    gummi_t* g = (gummi_t*)g_malloc(sizeof(gummi_t));
    g->builder = bd;
    g->editor = ed;
    g->importer = im;
    g->iofunc = iof;
    g->motion = mo;
    g->preview = prev;
    g->filename = NULL;   /* current opened file name in workspace */
    g->pdffile = NULL;
    g->workfile = NULL;
    g->tmpdir = g_get_tmp_dir();
    return g;
}

void gummi_create_environment(gummi_t* gummi, const gchar* filename) {
    gchar tname[BUFSIZ];
    snprintf(tname, BUFSIZ, "%s/gummi_XXXXXXX", gummi->tmpdir);
    gint tname_len = strlen(tname) + 1;

    if (workfd != -1) {
        close(workfd);
    } // close previous work file using its file descriptor

    gummi_set_filename(gummi, filename);
    
    workfd = g_mkstemp(tname); 
    if (gummi->workfile) g_free(gummi->workfile);
    gummi->workfile = (gchar*)g_malloc(tname_len);
    strcpy(gummi->workfile, tname);

    if (gummi->pdffile) g_free(gummi->pdffile);
    gummi->pdffile = (gchar*)g_malloc(tname_len + 4);    
    strncpy(gummi->pdffile, tname, tname_len +4);
    strncat(gummi->pdffile, ".pdf", tname_len +4);
    
    slog(L_INFO, "Environment created for:\n");
    slog(L_INFO, "TEX: %s\n", gummi->filename);
    slog(L_INFO, "TMP: %s\n", gummi->workfile);
    slog(L_INFO, "PDF: %s\n\n", gummi->pdffile); 
}

void gummi_set_filename(gummi_t* gummi, const gchar* name) {
    if (gummi->filename)
        g_free(gummi->filename);
    if (name) {
        gummi->filename = (gchar*)g_malloc(strlen(name) + 1);
        strcpy(gummi->filename, name);
    } else {
        gummi->filename = NULL;
    }
}
