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
#include "gui.h"

/* reference to global environment instance */
extern gummi_t* gummi;

// maybe create an environment struct with filename, statusbar, etc?

iofunctions_t* iofunctions_init(void) {
    iofunctions_t* iofunc = (iofunctions_t*)malloc(sizeof(iofunctions_t));
    return iofunc;
}

void iofunctions_load_file(iofunctions_t* iofunc, gchar *filename) {
    GError          *err=NULL;
    gchar           *status;
    gchar           *text;
    gboolean        result;

    /* add Loading message to status bar and  ensure GUI is current */
    status = g_strdup_printf ("Loading %s...", filename);
    statusbar_set_message(status);
    g_free (status);
    while (gtk_events_pending()) gtk_main_iteration();
    
    /* get the file contents */
    result = g_file_get_contents (filename, &text, NULL, &err);
    if (result == FALSE)
    {
        /* error loading file, show message to user */
       // iofunctions_error_message(err->message);
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
    gummi->filename = filename;
}


void iofunctions_write_file(iofunctions_t* iofunc, gchar *filename) {
    GError          *err=NULL;
    gchar           *status;
    gchar           *text;
    gboolean         result;
    GtkTextBuffer   *buffer;
    GtkTextIter      start, end;
    
    /* add Saving message to status bar and ensure GUI is current */
    if (filename != NULL)
        status = g_strdup_printf ("Saving %s...", filename);
    //else
    //    status = g_strdup_printf ("Saving %s...", editor->filename);
    statusbar_set_message(status);    
    g_free (status);
    while (gtk_events_pending()) gtk_main_iteration();
    
    /* disable text view and get contents of buffer */ 
    gtk_widget_set_sensitive (gummi->editor->sourceview, FALSE);
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (gummi->editor->sourceview));
    gtk_text_buffer_get_start_iter (buffer, &start);
    gtk_text_buffer_get_end_iter (buffer, &end);
    text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);       
    gtk_text_buffer_set_modified (buffer, FALSE);
    gtk_widget_set_sensitive (gummi->editor->sourceview, TRUE);
    
    /* set the contents of the file to the text from the buffer */
    if (filename != NULL)    
        result = g_file_set_contents (filename, text, -1, &err);
   // else
   //     result = g_file_set_contents (editor->filename, text, -1, &err);
        
    if (result == FALSE)
    {
        /* error saving file, show message to user */
        //error_message (err->message);
        g_error_free (err);
    }    
    
    /* don't forget to free that memory! */ 
    g_free (text); 
    
    if (filename != NULL) 
    {
        /* we need to free the memory used by editor->filename and set 
         it to the new filename instead */
        if (filename != NULL) g_free (filename);
        gummi->filename = filename;
    }
    

}

