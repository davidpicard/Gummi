#ifndef GUMMI_GUI_H
#define GUMMI_GUI_H

#include <gtk/gtk.h>

#include "iofunctions.h"


extern GtkWidget *mainwindow;


typedef struct _gui {
    iofunctions_t* iof;
} gui_t;


gui_t* gui_init(iofunctions_t* iofunc, GtkBuilder* builder);

void on_menu_open_activate(GtkWidget *widget, void * gui);
void on_menu_open_activate(GtkWidget *widget, void * user);
void on_menu_save_activate(GtkWidget *widget, void * user);
void on_menu_saveas_activate(GtkWidget *widget, void * user);
gchar * get_open_filename();

#endif /* GUMMI_GUI_H */
