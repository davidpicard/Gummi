#include "gtksourceview/gtksourceview.h"
#include <gtk/gtk.h>
#include "editor.h"

GtkWidget       *sourceview;
GtkTextBuffer       *sourcebuffer;

void create_editor(GtkBuilder * builder) {
    GtkWidget       *scroll;
    
    scroll = GTK_WIDGET (gtk_builder_get_object (builder, "editor_scroll"));
    sourceview = gtk_source_view_new();
    sourcebuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (sourceview));
    gtk_container_add (GTK_CONTAINER (scroll), sourceview);
    
    set_sourceview_config();
    load_default_text();
}

void set_sourceview_config() {
    gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW (sourceview),
                     TRUE);
}


void load_default_text() {
    printf("loading default text\n");
    gtk_text_buffer_set_text (sourcebuffer, "Hello, this is some text", -1);
}



char get_buffer() {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds (sourcebuffer, &start, &end);
    char *text = gtk_text_iter_get_text (&start, &end);
    return *text;
}








