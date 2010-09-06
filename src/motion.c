/**
 * @file   motion.c
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

#include "motion.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <glib.h>

#include "configfile.h"
#include "environment.h"
#include "utils.h"

extern Gummi* gummi;

GuMotion* motion_init(gint dum) {
    GuMotion* m = (GuMotion*)g_malloc(sizeof(GuMotion));
    const gchar* typesetter = config_get_value("typesetter");
    m->typesetter = (gchar*)g_malloc(strlen(typesetter) + 1);
    strncpy(m->typesetter, typesetter, strlen(typesetter) + 1);
    m->workfd = -1;
    m->filename = NULL;   /* current opened file name in workspace */
    m->pdffile = NULL;
    m->workfile = NULL;
    m->tmpdir = g_get_tmp_dir();
    m->update = 0;
    return m;
}

void motion_create_environment(GuMotion* mc, const gchar* filename) {
    gchar tname[BUFSIZ];
    snprintf(tname, BUFSIZ, "%s/gummi_XXXXXXX", mc->tmpdir);
    gint tname_len = strlen(tname) + 1;

    if (mc->workfd != -1) {
        close(mc->workfd);
    } // close previous work file using its file descriptor

    motion_set_filename(mc, filename);
    
    mc->workfd = g_mkstemp(tname); 
    if (mc->workfile) g_free(mc->workfile);
    mc->workfile = (gchar*)g_malloc(tname_len);
    strcpy(mc->workfile, tname);

    if (mc->pdffile) g_free(mc->pdffile);
    mc->pdffile = (gchar*)g_malloc(tname_len + 4);    
    strncpy(mc->pdffile, tname, tname_len +4);
    strncat(mc->pdffile, ".pdf", tname_len - strlen(mc->pdffile) +3);
    
    slog(L_INFO, "Environment created for:\n");
    slog(L_INFO, "TEX: %s\n", mc->filename);
    slog(L_INFO, "TMP: %s\n", mc->workfile);
    slog(L_INFO, "PDF: %s\n\n", mc->pdffile); 
}

void motion_set_filename(GuMotion* motion, const gchar* name) {
    if (motion->filename)
        g_free(motion->filename);
    if (name) {
        motion->filename = (gchar*)g_malloc(strlen(name) + 1);
        strcpy(motion->filename, name);
    } else {
        motion->filename = NULL;
    }
}

void motion_initial_preview(GuMotion* mo, GuEditor* ec, GuPreview* pr) {
    motion_update_workfile(mo, ec);
    motion_update_pdffile(mo);
    preview_set_pdffile(pr, mo->pdffile);
    // TODO: run some checks
    // if config says compile_status is true:
    
    // TODO: shouldnt pass ec!!!
    motion_start_updatepreview(mo);
}

void motion_update_workfile(GuMotion* mc, GuEditor* ec) {
    GtkTextIter start;
    GtkTextIter end;
    gchar *text;
    FILE *fp;

    // TODO: the following line caused hangups in python, attention!
    gtk_widget_set_sensitive(ec->sourceview, FALSE);
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ec->sourcebuffer), &start, &end);
    text = gtk_text_iter_get_text (&start, &end);
    gtk_widget_set_sensitive(ec->sourceview, TRUE);
    
    // TODO: restore text iters to selection before compile
    //gtk_text_buffer_select_range(ec->sourcebuffer, ec->
    
    fp = fopen(mc->workfile, "w");
    
    if(fp == NULL) {
        slog(L_DEBUG, "unable to create workfile in tmpdir");
    }
    fwrite(text, strlen(text), 1, fp);
    g_free(text);
    fclose(fp);
    // TODO: Maybe add editorviewer grab focus line here if necessary
    gtk_widget_grab_focus(ec->sourceview);
}

void motion_update_pdffile(GuMotion* motion) {
    gchar command[BUFSIZ];
    snprintf(command, sizeof command, "%s "
                                      "-interaction=nonstopmode "
                                      "-file-line-error "
                                      "-halt-on-error "
                                      "-output-directory='%s' '%s'", \
                                      motion->typesetter,
                                      motion->tmpdir, motion->workfile);

    pdata cresult = utils_popen_r(command);
    errorbuffer_set_text(cresult.data);
}


void motion_start_updatepreview(GuMotion* motion) {
    gboolean compile_activated;
    guint compile_timer;
    compile_activated = (gboolean)config_get_value("compile_status");
    compile_timer = atoi(config_get_value("compile_timer"));
    if (compile_activated)
        motion->update = g_timeout_add_seconds(compile_timer,
                motion_updatepreview, NULL);
}

void motion_stop_updatepreview(GuMotion* motion) {
    g_source_remove(motion->update);
}

void motion_update_auxfile(GuMotion* motion) {
    gchar command[BUFSIZ];
    snprintf(command, sizeof command, "%s "
                                      "--draftmode "
                                      "-interaction=nonstopmode "
                                      "--output-directory='%s' '%s'", \
                                      motion->typesetter,
                                      motion->tmpdir, motion->workfile);
    utils_popen_r(command);
}


gboolean motion_updatepreview(void) {
    if (gtk_text_buffer_get_modified(g_e_buffer)) {
        motion_update_workfile(gummi->motion, gummi->editor);
        motion_update_pdffile(gummi->motion);
        preview_refresh(gummi->preview);
    }
    return TRUE;
}
