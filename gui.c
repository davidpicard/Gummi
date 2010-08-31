
#include <stdlib.h>

#include <glib.h>

#include "configfile.h"
#include "environment.h"
#include "gui.h"

extern gummi_t* gummi;

GtkWidget   *mainwindow;
GtkWidget   *statusbar;
guint        statusid;

/* Many of the functions in this file are based on the excellent GTK+
 * tutorials written by Micah Carrick that can be found on: 
 * http://www.micahcarrick.com/gtk-glade-tutorial-part-3.html */

void gui_init() {
    GtkWidget    *hpaned;
    gint          width, height;
    
    mainwindow = GTK_WIDGET(gtk_builder_get_object (g_builder, "mainwindow"));
    statusbar = GTK_WIDGET(gtk_builder_get_object (g_builder, "statusbar"));
    statusid = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "Gummi");
    gtk_window_get_size (GTK_WINDOW (mainwindow), &width, &height);
    
    hpaned= GTK_WIDGET (gtk_builder_get_object(g_builder, "hpaned" ));
    gtk_paned_set_position (GTK_PANED (hpaned), (width/2)); 
}

void gui_main() {
    gtk_builder_connect_signals (g_builder, NULL);       
    gtk_widget_show_all (mainwindow);
    gtk_main ();
}

void on_menu_new_activate(GtkWidget *widget, void * user) {
    const char *text;
    
    if (check_for_save () == TRUE) {
          on_menu_save_activate(NULL, NULL);  
    }
    /* clear editor for a new file */
    text = config_get_value("welcome");
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER(g_e_buffer), text, -1);
    gtk_text_buffer_set_modified (g_e_buffer, FALSE);
    gummi_create_environment(gummi, NULL);
}

void on_menu_open_activate(GtkWidget *widget, void * user) {
    gchar       *filename;
    
    if (check_for_save() == TRUE) {
    on_menu_save_activate (NULL, NULL);  
    }
    filename = get_open_filename();
    if (filename != NULL) 
        iofunctions_load_file(gummi->iofunc, gummi->editor, filename); 
}

void on_menu_save_activate(GtkWidget *widget, void * user) {
    gchar* filename = NULL;
    if (!gummi->filename)
        filename = get_save_filename();
    if (filename) {
        gummi_set_filename(gummi, filename);
        iofunctions_write_file(gummi->iofunc, gummi->editor, filename); 
    }
}

void on_menu_saveas_activate(GtkWidget *widget, void * user) {
    gchar* filename = NULL;
    if (!gummi->filename)
        filename = get_save_filename();
    if (filename) {
        iofunctions_write_file(gummi->iofunc, gummi->editor, filename); 
        gummi_create_environment(gummi, filename);
    }
}

void on_menu_find_activate(GtkWidget *widget, void * user) {
    GtkWidget *searchwin;
    searchwin = GTK_WIDGET(gtk_builder_get_object (g_builder, "searchwindow"));
    gtk_widget_show(searchwin);
    gtk_widget_grab_focus(searchwin);
}

void on_menu_cut_activate(GtkWidget *widget, void * user) {
    GtkClipboard     *clipboard;
    
    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_cut_clipboard(g_e_buffer, clipboard, TRUE);
    gtk_text_buffer_set_modified(g_e_buffer, TRUE);
}

void on_menu_copy_activate(GtkWidget *widget, void * user) {
    GtkClipboard     *clipboard;
    
    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_copy_clipboard(g_e_buffer, clipboard);
}
void on_menu_paste_activate(GtkWidget *widget, void * user) {
    GtkClipboard     *clipboard;
    
    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_paste_clipboard(g_e_buffer, clipboard, NULL, TRUE);
    gtk_text_buffer_set_modified(g_e_buffer, TRUE);
}

gboolean check_for_save() {
    gboolean      ret = FALSE;
    
    if (gtk_text_buffer_get_modified (g_e_buffer) == TRUE) {
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
    gchar       *filename;
       
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
