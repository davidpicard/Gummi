#ifndef GUMMI_GUI_H
#define GUMMI_GUI_H

#include <gtk/gtk.h>

#include "iofunctions.h"

#define g_builder gummi->builder

void gui_init(void);
void gui_main(void);
void on_menu_new_activate(GtkWidget *widget, void * user);
void on_menu_open_activate(GtkWidget *widget, void * user);
void on_menu_save_activate(GtkWidget *widget, void * user);
void on_menu_saveas_activate(GtkWidget *widget, void * user);
void on_menu_find_activate(GtkWidget *widget, void * user);
void on_menu_cut_activate(GtkWidget *widget, void * user);
void on_menu_copy_activate(GtkWidget *widget, void * user);
void on_menu_paste_activate(GtkWidget *widget, void * user);

gchar * get_open_filename();
gchar * get_save_filename();
gboolean check_for_save ();

void statusbar_set_message(gchar *message);
gboolean statusbar_del_message();

#endif /* GUMMI_GUI_H */
