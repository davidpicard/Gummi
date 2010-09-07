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
#include "editor.h"
#include "environment.h"
#include "preview.h"
#include "utils.h"

GuMotion* motion_init(GuEditor* ec, GuPreview* pc) {
    L_F_DEBUG;
    GuMotion* m = (GuMotion*)g_malloc(sizeof(GuMotion));
    /* initialize basis */
    m->b_editor = ec;
    m->b_preview = pc;

    /* initialize members */
    const gchar* typesetter = config_get_value("typesetter");
    m->typesetter = (gchar*)g_malloc(strlen(typesetter) + 1);
    strncpy(m->typesetter, typesetter, strlen(typesetter) + 1);
    m->workfd = -1;
    m->filename = NULL;   /* current opened file name in workspace */
    m->pdffile = NULL;
    m->workfile = NULL;
    m->tmpdir = g_get_tmp_dir();
    m->update = 0;
    m->timer = 0;
    return m;
}

void motion_create_environment(GuMotion* mc, const gchar* filename) {
    L_F_DEBUG;
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

void motion_set_filename(GuMotion* mc, const gchar* name) {
    L_F_DEBUG;
    if (mc->filename)
        g_free(mc->filename);
    if (name) {
        mc->filename = (gchar*)g_malloc(strlen(name) + 1);
        strcpy(mc->filename, name);
    } else {
        mc->filename = NULL;
    }
}

void motion_initial_preview(GuMotion* mc) {
    L_F_DEBUG;
    motion_update_workfile(mc);
    motion_update_pdffile(mc);
    preview_set_pdffile(mc->b_preview, mc->pdffile);
    // TODO: run some checks
    // if config says compile_status is true:
    
    // TODO: shouldnt pass ec!!!
    motion_start_updatepreview(mc);
}

void motion_update_workfile(GuMotion* mc) {
    L_F_DEBUG;
    gchar *text;
    FILE *fp;

    // TODO: the following line caused hangups in python, attention!
    gtk_widget_set_sensitive(mc->b_editor->sourceview, FALSE);
    text = editor_grab_buffer(mc->b_editor);
    gtk_widget_set_sensitive(mc->b_editor->sourceview, TRUE);
    
    // TODO: restore text iters to selection before compile
    //gtk_text_buffer_select_range(ec->sourcebuffer, ec->
    
    fp = fopen(mc->workfile, "w");
    
    if(fp == NULL) {
        slog(L_ERROR, "unable to create workfile in tmpdir");
        return;
    }
    fwrite(text, strlen(text), 1, fp);
    g_free(text);
    fclose(fp);
    // TODO: Maybe add editorviewer grab focus line here if necessary
    gtk_widget_grab_focus(mc->b_editor->sourceview);
}

void motion_update_pdffile(GuMotion* motion) {
    L_F_DEBUG;
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


void motion_start_updatepreview(GuMotion* mc) {
    L_F_DEBUG;
    if (0 == strcmp(config_get_value("compile_scheme"), "on_idle")) {
        mc->shandlers[0] = g_signal_connect(mc->b_editor->sourceview,
                                            "key-press-event",
                                            G_CALLBACK(on_key_press_cb),
                                            (void*)mc);
        mc->shandlers[1] = g_signal_connect(mc->b_editor->sourceview,
                                            "key-release-event",
                                            G_CALLBACK(on_key_release_cb),
                                            (void*)mc);
        motion_start_timer(mc);
    } else  {
        mc->update = g_timeout_add_seconds(
                atoi(config_get_value("compile_timer")),
                motion_updatepreview, (void*)mc);
    }
}

void motion_stop_updatepreview(GuMotion* mc) {
    L_F_DEBUG;
    if (0 == strcmp(config_get_value("compile_scheme"), "on_idle")) {
        g_signal_handler_disconnect(mc->b_editor->sourceview, mc->shandlers[0]);
        g_signal_handler_disconnect(mc->b_editor->sourceview, mc->shandlers[1]);
        motion_stop_timer(mc);
    } else if (mc->update > 0) {
        g_source_remove(mc->update);
        mc->update = 0;
    }
}

void motion_update_auxfile(GuMotion* mc) {
    L_F_DEBUG;
    gchar command[BUFSIZ];
    snprintf(command, sizeof command, "%s "
                                      "--draftmode "
                                      "-interaction=nonstopmode "
                                      "--output-directory='%s' '%s'", \
                                      mc->typesetter,
                                      mc->tmpdir, mc->workfile);
    utils_popen_r(command);
}

void motion_export_pdffile(GuMotion* mc, const gchar* path) {
    L_F_DEBUG;
    FILE *in, *out;
    gchar savepath[PATH_MAX];
    gchar buf[BUFSIZ];
    if (0 != strcmp(path + strlen(path) -4, ".pdf"))
        snprintf(savepath, PATH_MAX, "%s.pdf", path);
    else
        strncpy(savepath, path, PATH_MAX);
    /* I use this to copy file instead of g_file_copy or other OS dependent
     * functions */
    if (NULL == (in = fopen(mc->pdffile, "rb")))
        slog(L_G_ERROR, "Failed to export PDF\n");
    if (NULL == (out = fopen(savepath, "wb")))
        slog(L_G_ERROR, "Failed to save %s\n", savepath);

    while (fread(buf, BUFSIZ, 1, in))
        fwrite(buf, BUFSIZ, 1, out);
    fclose(in);
    fclose(out);
}

gboolean motion_updatepreview(void* user) {
    L_F_DEBUG;
    GuMotion* mc = (GuMotion*)user;
    motion_update_workfile(mc);
    motion_update_pdffile(mc);
    preview_refresh(mc->b_preview);
    return 0 != strcmp(config_get_value("compile_scheme"), "on_idle");
}

void motion_start_timer(GuMotion* mc) {
    L_F_DEBUG;
    motion_stop_timer(mc);
    mc->timer = g_timeout_add_seconds(atoi(config_get_value("compile_timer")),
            motion_updatepreview, (void*)mc);
}

void motion_stop_timer(GuMotion* mc) {
    L_F_DEBUG;
    if (mc->timer > 0) {
        g_source_remove(mc->timer);
        mc->timer = 0;
    }
}
    
gboolean on_key_press_cb(GtkWidget* widget, GdkEventKey* event, void* user) {
    L_F_DEBUG;
    motion_stop_timer((GuMotion*)user);
    return FALSE;
}

gboolean on_key_release_cb(GtkWidget* widget, GdkEventKey* event, void* user) {
    L_F_DEBUG;
    motion_start_timer((GuMotion*)user);
    return FALSE;
}
