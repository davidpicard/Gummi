/**
 * @file   utils.c
 * @brief  utility for gummi
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

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
                    "directory\n", BUFSIZ);
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
            gtk_window_set_title(GTK_WINDOW(dialog), "InfoFatal Error!");

        gtk_dialog_run(GTK_DIALOG(dialog));      
        gtk_widget_destroy(dialog);
    }

    if (!L_IS_TYPE(level, L_INFO) &&
        !L_IS_TYPE(level, L_DEBUG) && 
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
