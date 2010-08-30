
#include <stdlib.h>

#include "gui.h"
#include "editor.h"
#include "iofunctions.h"

GtkWidget   *mainwindow;
gchar       *filename=NULL;

gui_t* gui_init(iofunctions_t* iofunc) {
    gui_t* gui = (gui_t*)malloc(sizeof(gui_t));
    gui->iof = iofunc;
    return gui;
}

void create_gui(GtkBuilder * builder) {
    GtkWidget    *hpaned;
    gint         width, height;
    
    mainwindow = GTK_WIDGET (gtk_builder_get_object (builder, "mainwindow"));
    gtk_window_get_size (GTK_WINDOW (mainwindow), &width, &height);
    
    hpaned= GTK_WIDGET (gtk_builder_get_object(builder, "hpaned" ));
    gtk_paned_set_position (GTK_PANED (hpaned), (width/2)); 
}


void on_menu_new_activate(GtkWidget *widget, void * user) {
    printf("new\n");
}

void on_menu_open_activate(gui_t* gui, GtkWidget *widget, void * user) {
    printf("open\n");
    filename = get_open_filename();
    if (filename != NULL) load_file (gui->iof,filename); 
}

void on_menu_save_activate(GtkWidget *widget, void * user) {
    printf("save\n");
}

void on_menu_saveas_activate(GtkWidget *widget, void * user) {
    printf("saveas\n");
}

gchar * get_open_filename() {
    GtkWidget   *chooser;
       
    chooser = gtk_file_chooser_dialog_new ("Open File...",
                           GTK_WINDOW (mainwindow->window),
                           GTK_FILE_CHOOSER_ACTION_OPEN,
                           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                           GTK_STOCK_OPEN, GTK_RESPONSE_OK,
                           NULL);
                           
    if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    }
    
    gtk_widget_destroy (chooser);
    return filename;
}




