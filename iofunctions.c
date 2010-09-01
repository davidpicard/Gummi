/**
 * @file   iofunctions.c
 * @brief  
 * @author Alexander van der Mey <alexvandermey@gmail.com>
 *
 * Copyright (C) 2010 - Alexander van der Mey <alexvandermey@gmail.com>
 * All Rights reserved.
 */

#include "iofunctions.h"

#include <stdlib.h>

#include "configfile.h"
#include "editor.h"
#include "environment.h"
#include "gui.h"
#include "utils.h"

// maybe create an environment struct with filename, statusbar, etc?

iofunctions_t* iofunctions_init(void) {
    iofunctions_t* iofunc = (iofunctions_t*)g_malloc(sizeof(iofunctions_t));
    return iofunc;
}

void iofunctions_load_default_text(iofunctions_t* iofunc, editor_t* ec) {
    slog(L_DEBUG, "loading default text\n");
    editor_fill_buffer(ec, config_get_value("welcome"));
}

void iofunctions_load_file(iofunctions_t* iofunc, editor_t* ec, gchar *filename)
{
    GError          *err=NULL;
    gchar           *status;
    gchar           *text;
    gboolean        result;

    slog(L_DEBUG, "loading %s from command line argument\n", filename);

    /* add Loading message to status bar and  ensure GUI is current */
    status = g_strdup_printf ("Loading %s...", filename);
    statusbar_set_message(status);
    g_free(status);
    while (gtk_events_pending()) gtk_main_iteration();
    
    /* get the file contents */
    if (FALSE == (result = g_file_get_contents(filename, &text, NULL, &err))) {
        slog(L_G_ERROR, "%s\n", err->message);
        g_error_free(err);
        iofunctions_load_default_text(iofunc, ec);
        return;
    }
    editor_fill_buffer(ec, text);
    g_free(text); 
}

void iofunctions_write_file(iofunctions_t* iofunc, editor_t* ec,
        gchar *filename) {
    GError          *err=NULL;
    gchar           *status;
    gchar           *text;
    gboolean         result;
    GtkTextBuffer   *buffer;
    GtkTextIter      start, end;

    status = g_strdup_printf ("Saving %s...", filename);
    statusbar_set_message(status);    
    g_free (status);
    while (gtk_events_pending()) gtk_main_iteration();
    
    /* disable text view and get contents of buffer */ 
    gtk_widget_set_sensitive (ec->sourceview, FALSE);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ec->sourceview));
    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);
    text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);       
    gtk_text_buffer_set_modified (buffer, FALSE);
    gtk_widget_set_sensitive (ec->sourceview, TRUE);
    
    /* set the contents of the file to the text from the buffer */
    if (filename != NULL)    
        result = g_file_set_contents (filename, text, -1, &err);
        
    if (result == FALSE) {
        slog(L_G_ERROR, "%s\nPlease try again later.", err->message);
        g_error_free(err);
    }    
    g_free(text); 
}
