#include <gtk/gtk.h>

#include "iofunctions.h"


extern GtkWidget *mainwindow;


typedef struct _gui {
    iofunctions_t* iof;
} gui_t;


void create_gui(GtkBuilder *);


gchar * get_open_filename();
