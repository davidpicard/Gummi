#ifndef GUMMI_GUI_H
#define GUMMI_GUI_H

#include <gtk/gtk.h>

#include "iofunctions.h"


extern GtkWidget *mainwindow;

void gui_init(GtkBuilder* builder);

void on_menu_new_activate(GtkWidget *widget, void * user);
void on_menu_open_activate(GtkWidget *widget, void * user);
void on_menu_save_activate(GtkWidget *widget, void * user);
void on_menu_saveas_activate(GtkWidget *widget, void * user);


gchar * get_open_filename();
gchar * get_save_filename();
gboolean check_for_save ();

#endif /* GUMMI_GUI_H */
