/**
 * @file    utils.c
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


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#   include <unistd.h>
#   include <sys/types.h>
#   include <sys/wait.h>
#endif

#include <glib.h>
#include <gtk/gtk.h>

#include "environment.h"
#include "utils.h"

static gint slog_debug = 0;

void slog_init(gint debug) {
    slog_debug = debug;
}

void slog(gint level, const gchar *fmt, ...) {
    gchar message[BUFSIZ];
    va_list vap;

    if (L_IS_TYPE(level, L_DEBUG) && !slog_debug) return;

    if (L_IS_TYPE(level, L_DEBUG))
        fprintf(stderr, "[Debug] ");
    else if (L_IS_TYPE(level, L_FATAL) || L_IS_TYPE(level, L_G_FATAL))
        fprintf(stderr, "[Fatal] ");
    else if (L_IS_TYPE(level, L_ERROR) || L_IS_TYPE(level, L_G_ERROR))
        fprintf(stderr, "[Error] ");
    else
        fprintf(stderr, "[Info] ");
    
    va_start(vap, fmt);
    vsnprintf(message, BUFSIZ, fmt, vap);
    va_end(vap);
    fprintf(stderr, "%s", message);

    if (L_IS_GUI(level)) {
        GtkWidget* dialog;
        // TODO: display the tmp file name
        if (L_IS_TYPE(level, L_G_FATAL))
            strncat(message, "\nGummi has encountered a serious error and "
                    "require restart, your can find your file in the /tmp "
                    "directory\n", BUFSIZ - strlen(message) -1);
        dialog = gtk_message_dialog_new (NULL, 
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                L_IS_TYPE(level,L_G_INFO)? GTK_MESSAGE_INFO: GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "%s", message);

        if (L_IS_TYPE(level, L_G_ERROR))
            gtk_window_set_title(GTK_WINDOW(dialog), "Error!");
        else if (L_IS_TYPE(level, L_G_FATAL))
            gtk_window_set_title(GTK_WINDOW(dialog), "Fatal Error!");
        else if (L_IS_TYPE(level, L_G_INFO))
            gtk_window_set_title(GTK_WINDOW(dialog), "Info");

        gtk_dialog_run(GTK_DIALOG(dialog));      
        gtk_widget_destroy(dialog);
    }

    if (!L_IS_TYPE(level, L_INFO) &&
        !L_IS_TYPE(level, L_DEBUG) && 
        !L_IS_TYPE(level, L_ERROR) && 
        !L_IS_TYPE(level, L_G_INFO) &&
        !L_IS_TYPE(level, L_G_ERROR))
        exit(1);
}

gboolean utils_yes_no_dialog(const gchar* message) {
    GtkWidget* dialog;
    gint ret = 0;
    dialog = gtk_message_dialog_new (NULL, 
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            "%s", message);

    ret = gtk_dialog_run(GTK_DIALOG(dialog));      
    gtk_widget_destroy(dialog);
    if (ret == GTK_RESPONSE_YES)
        return TRUE;
    else
        return FALSE;
}

gboolean utils_validate_path(const gchar* path) {
    gboolean result = FALSE;
    GFile* file = g_file_new_for_path(path);
    result = g_file_query_exists(file, NULL);
    g_object_unref(file);
    return result;
}

pdata utils_popen_r(const gchar* cmd) {
    FILE* fp = popen(cmd, "r");
    static gchar buf[BUFSIZ];
    gint status = 0;

    if (!fp)
        slog(L_FATAL, "popen error");
    fread(buf, BUFSIZ, 1, fp);
    status = WEXITSTATUS(pclose(fp));
    return (pdata){status, buf};
}
