#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "environment.h"
#include "motion.h"
#include "utils.h"

motion_t* motion_init(gint dum) {
    motion_t* m = (motion_t*)g_malloc(sizeof(motion_t));
    m->dummy = dum;
    return m;
}

void motion_initial_preview(editor_t* ec) {
    motion_update_workfile(ec);
    motion_update_pdffile(ec);
    preview_set_pdffile("/tmpfile.pdf");
}


void motion_update_workfile(editor_t* ec) {
    GtkTextIter start;
    GtkTextIter end;
    gchar *text;
    FILE *fp;

    // TODO: the following line caused hangups in python, attention!
    gtk_widget_set_sensitive(ec->sourceview, FALSE);
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ec->sourcebuffer),
            &start, &end);
    text = gtk_text_iter_get_text (&start, &end);
    gtk_widget_set_sensitive(ec->sourceview, TRUE);
    
    // TODO: gummi->workfile doesn't work properly here.. 
    fp = fopen("tmpfile", "w");
    
    if(fp == NULL) {
        perror("failed to open sample.txt");
        // TODO: do your debug thingie!
    }
    fwrite(text, strlen(text), 1, fp);
    g_free(text);
    fclose(fp);
    // TODO: Maybe add editorviewer grab focus line here if necessary
    
}

void motion_update_pdffile(editor_t* ec) {
    FILE *fp;
    int status;
    char path[PATH_MAX];
    
    fp = popen("pdflatex tmpfile", "r");
    if (fp == NULL) {
        // handle error
    }
    while (fgets(path, PATH_MAX, fp) != NULL)
        printf("%s", path);
        
    status = pclose(fp);
    if (status == -1) {
         // handle error
    }
}
