
#include <stdlib.h>
#include <glib.h>

#include "environment.h"
#include "gui.h"

extern gummi_t* gummi;

GtkWidget   *mainwindow;
GtkWidget   *statusbar;
guint        statusid;
gchar       *filename=NULL;

/* Many of the functions in this file are based on the excellent GTK+
 * tutorials written by Micah Carrick that can be found on: 
 * http://www.micahcarrick.com/gtk-glade-tutorial-part-3.html */

void gui_init(GtkBuilder* builder) {
    GtkWidget    *hpaned;
    gint     width, height;
    
    mainwindow = GTK_WIDGET (gtk_builder_get_object (builder, "mainwindow"));
    statusbar = GTK_WIDGET (gtk_builder_get_object (builder, "statusbar"));
    statusid = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "Gummi");
    gtk_window_get_size (GTK_WINDOW (mainwindow), &width, &height);
    
    hpaned= GTK_WIDGET (gtk_builder_get_object(builder, "hpaned" ));
    gtk_paned_set_position (GTK_PANED (hpaned), (width/2)); 
}

void on_menu_new_activate(GtkWidget *widget, void * user) {
    printf("new\n");
}

void on_menu_open_activate(GtkWidget *widget, void * user) {
    gchar       *filename;
    
    if (check_for_save() == TRUE) {
    on_menu_save_activate (NULL, NULL);  
    }
    filename = get_open_filename();
    if (filename != NULL) 
	    iofunctions_load_file(gummi->iofunc, filename); 
}

void on_menu_save_activate(GtkWidget *widget, void * user) {
    gchar           *filename;
      
    filename = get_save_filename();
    if (filename != NULL) 
	    iofunctions_write_file(gummi->iofunc, filename); 
}

void on_menu_saveas_activate(GtkWidget *widget, void * user) {
    editor_jumpto_search_result(gummi->editor, 1);
    printf("saveas\n");
}

gboolean check_for_save () {
    gboolean      ret = FALSE;
    GtkTextBuffer     *buffer;
    
    buffer = gtk_text_view_get_buffer 
          (GTK_TEXT_VIEW (gummi->editor->sourceview));
    
    if (gtk_text_buffer_get_modified (buffer) == TRUE) {
    /* we need to prompt for save */    
    GtkWidget       *dialog;
    
    const gchar *msg  = "Do you want to save the changes you have made?";
    
    dialog = gtk_message_dialog_new (NULL, 
             GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
             GTK_MESSAGE_QUESTION,
             GTK_BUTTONS_YES_NO,
             "%s", msg);
    
    gtk_window_set_title (GTK_WINDOW (dialog), "Save?");
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_NO) {
        ret = FALSE;
    }      
    else ret = TRUE;
        
    gtk_widget_destroy (dialog);      
    }     
    return ret;
}

gchar* get_open_filename() {
    GtkWidget   *chooser;
       
    chooser = gtk_file_chooser_dialog_new ("Open File...",
           GTK_WINDOW (mainwindow),
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

gchar* get_save_filename() {
    GtkWidget       *chooser;
    gchar           *filename=NULL;
        
    chooser = gtk_file_chooser_dialog_new ("Save File...",
                           GTK_WINDOW (mainwindow),
                           GTK_FILE_CHOOSER_ACTION_SAVE,
                           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                           GTK_STOCK_SAVE, GTK_RESPONSE_OK,
                           NULL);
                           
    if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    }
    
    gtk_widget_destroy (chooser);
    return filename;
}

void statusbar_set_message(gchar *message) {
    gtk_statusbar_push (GTK_STATUSBAR(statusbar), statusid, message);
    g_timeout_add_seconds(4,statusbar_del_message, NULL);
}

gboolean statusbar_del_message() {
    gtk_statusbar_pop(GTK_STATUSBAR(statusbar),statusid);
    return FALSE;
}




