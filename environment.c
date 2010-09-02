/**
 * @file    environment.c
 * @brief    
 *
 * Copyright (C) 2010 Gummi-Dev Team <alexvandermey@gmail.com>
 * All Rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "environment.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

Gummi* gummi_init(GtkBuilder* bd, GuEditor* ed, GuImporter* im,
        GuIOFunc* iof, GuMotion* mo, GuPreview* prev, GuTemplate* tpl) {
    Gummi* g = (Gummi*)g_malloc(sizeof(Gummi));
    g->workfd = -1;
    g->filename = NULL;   /* current opened file name in workspace */
    g->pdffile = NULL;
    g->workfile = NULL;
    g->tmpdir = g_get_tmp_dir();
    g->builder = bd;
    g->editor = ed;
    g->importer = im;
    g->iofunc = iof;
    g->motion = mo;
    g->preview = prev;
    g->templ = tpl;
    return g;
}

void gummi_create_environment(Gummi* gummi, const gchar* filename) {
    gchar tname[BUFSIZ];
    snprintf(tname, BUFSIZ, "%s/gummi_XXXXXXX", gummi->tmpdir);
    gint tname_len = strlen(tname) + 1;

    if (gummi->workfd != -1) {
        close(gummi->workfd);
    } // close previous work file using its file descriptor

    gummi_set_filename(gummi, filename);
    
    gummi->workfd = g_mkstemp(tname); 
    if (gummi->workfile) g_free(gummi->workfile);
    gummi->workfile = (gchar*)g_malloc(tname_len);
    strcpy(gummi->workfile, tname);

    if (gummi->pdffile) g_free(gummi->pdffile);
    gummi->pdffile = (gchar*)g_malloc(tname_len + 4);    
    strncpy(gummi->pdffile, tname, tname_len +4);
    strncat(gummi->pdffile, ".pdf", tname_len - strlen(gummi->pdffile) +3);
    
    slog(L_INFO, "Environment created for:\n");
    slog(L_INFO, "TEX: %s\n", gummi->filename);
    slog(L_INFO, "TMP: %s\n", gummi->workfile);
    slog(L_INFO, "PDF: %s\n\n", gummi->pdffile); 
}

void gummi_set_filename(Gummi* gummi, const gchar* name) {
    if (gummi->filename)
        g_free(gummi->filename);
    if (name) {
        gummi->filename = (gchar*)g_malloc(strlen(name) + 1);
        strcpy(gummi->filename, name);
    } else {
        gummi->filename = NULL;
    }
}
