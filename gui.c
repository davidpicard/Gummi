
#include "gui.h"


void create_gui(GtkBuilder * builder, gint width) {

    GtkWidget *hpaned;

    hpaned= GTK_WIDGET (gtk_builder_get_object(builder, "hpaned" ));
    gtk_paned_set_position (GTK_PANED (hpaned), (width/2)); 
}



