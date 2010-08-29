
#include "gui.h"


void create_gui(GtkBuilder * builder, gint width) {

    GtkWidget *hpaned;

    hpaned= GTK_WIDGET (gtk_builder_get_object(builder, "hpaned" ));
    gtk_paned_set_position (GTK_PANED (hpaned), (width/2)); 
}


void on_menu_new_activate(GtkWidget *widget, void * user) {
	printf("new\n");
}

void on_menu_open_activate(GtkWidget *widget, void * user) {
	printf("open\n");
}

void on_menu_save_activate(GtkWidget *widget, void * user) {
	printf("save\n");
}

void on_menu_saveas_activate(GtkWidget *widget, void * user) {
	printf("saveas\n");
}





