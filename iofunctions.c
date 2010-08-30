/**
 * @file   iofunctions.c
 * @brief  
 * @author Alexander van der Mey <alexvandermey@gmail.com>
 *
 * Copyright (C) 2010 - Alexander van der Mey <alexvandermey@gmail.com>
 * All Rights reserved.
 */

#include <stdlib.h>
#include "environment.h"

/* reference to global environment instance */
extern gummi_t* gummi;

// maybe create an environment struct with filename, statusbar, etc?

iofunctions_t* iofunctions_init(void) {
    iofunctions_t* iofunc = (iofunctions_t*)malloc(sizeof(iofunctions_t));
    return iofunc;
}

void iofunctions_error_message (const gchar *message) {
    GtkWidget           *dialog;
    
    /* log to terminal window */
    g_warning ("%s", message);
    
    /* create an error message dialog and display modally to the user */
    dialog = gtk_message_dialog_new (NULL, 
                     GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                     GTK_MESSAGE_ERROR,
                     GTK_BUTTONS_OK,
                     "%s", message);
    
    gtk_window_set_title (GTK_WINDOW (dialog), "Error!");
    gtk_dialog_run (GTK_DIALOG (dialog));      
    gtk_widget_destroy (dialog);     
}

void iofunctions_load_file(iofunctions_t* iofunc, gchar *filename) {
    GError          *err=NULL;
    gchar           *status;
    gchar           *text;
    gboolean        result;

    /* add Loading message to status bar and  ensure GUI is current */
    status = g_strdup_printf ("Loading %s...", filename);
    //gtk_statusbar_push (GTK_STATUSBAR (editor->statusbar),
    //            editor->statusbar_context_id, status);
    g_free (status);
    while (gtk_events_pending()) gtk_main_iteration();
    
    /* get the file contents */
    result = g_file_get_contents (filename, &text, NULL, &err);
    if (result == FALSE)
    {
        /* error loading file, show message to user */
        iofunctions_error_message(err->message);
        g_error_free (err);
        g_free (filename);
    }
    
    /* disable the text view while loading the buffer with the text */    
    gtk_widget_set_sensitive(gummi->editor->sourceview, FALSE);
    //buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (editor->text_view));
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(gummi->editor->sourcebuffer),
        text, -1);
    gtk_text_buffer_set_modified(GTK_TEXT_BUFFER
        (gummi->editor->sourcebuffer), FALSE);
    gtk_widget_set_sensitive(gummi->editor->sourceview, TRUE);
    g_free (text); 
    
    /* now we can set the current filename since loading was a success */
    if (filename != NULL) g_free (filename);
    //editor->filename = filename;
    
    /* clear loading status and restore default  */
    //gtk_statusbar_pop (GTK_STATUSBAR (editor->statusbar),
    //           editor->statusbar_context_id);
    //reset_default_status (editor);
}

