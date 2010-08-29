#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#include "editor.h"
#include "preview.h"
#include "gui.h"
#include "motion.h"


void on_window_destroy (GtkObject *object, gpointer user_data)
{
    gtk_main_quit();
}

void setup_environment() {
    char tname[1024] = "/tmp/gummi_XXXXXXX"; 
    int fh = mkstemp(tname); 
    printf("Filename %s", tname); 
}

int main (int argc, char *argv[])
{
    GtkBuilder      *builder;
    GtkWidget       *window;
    gint            width, height;
       
    gtk_init (&argc, &argv);
    
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "gummi.glade", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "mainwindow"));
    gtk_window_get_size (GTK_WINDOW (window), &width, &height);

    // setup gummi gui elements:
    create_gui(builder, width);

    // setup sourceview editor pane:
    create_editor(builder);

    // setup poppler preview pane:
    create_preview(builder);
    
    // either load a file or load the default text based on cli arguments:
    // ... 
    
    // setup work files and such:
    setup_environment();



    // start motion:
    //g_timeout_add_seconds(1, update_preview, NULL);



    gtk_builder_connect_signals (builder, NULL);       
    g_object_unref (G_OBJECT (builder));
    
    gtk_widget_show_all (window);
       
    gtk_main ();
    
    return 0;
}
