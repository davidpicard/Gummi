#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <glib.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configfile.h"
#include "environment.h"
#include "gui.h"
#include "iofunctions.h"
#include "utils.h"

/* global environment instance */
gummi_t* gummi = 0;

static int debug = 0;

static GOptionEntry entries[] = {
    { (const gchar*)"debug", (gchar)'d', 0, G_OPTION_ARG_NONE, &debug, 
        (gchar*)"show debug info", NULL},
    { NULL, NULL, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL }
};

void on_window_destroy (GtkObject *object, gpointer user_data) {
    gtk_main_quit();
}

int main (int argc, char *argv[]) {
    GtkBuilder* builder;
    editor_t* editor;
    iofunctions_t* iofunc;
    motion_t* motion;
    preview_t* preview;

    GError* error = NULL;
    GOptionContext* context = g_option_context_new("files");
    g_option_context_add_main_entries(context, entries, PACKAGE);
    g_option_context_parse(context, &argc, &argv, &error);

    slog_init(debug);
    config_init("gummi.cfg");
    gtk_init (&argc, &argv);
    
    slog(L_DEBUG, PACKAGE" version: "VERSION"\n");
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "gui/gummi.glade", NULL);

    /* initialize classes */
    editor = editor_init(builder);
    iofunc = iofunctions_init();
    preview = preview_init(builder);
    motion = motion_init(0); 
    gummi = gummi_init(editor, iofunc, motion, preview);

    gui_init(builder);

    if ( argc != 2 ) {
        iofunctions_load_default_text(gummi->iofunc);
        gummi_create_environment(NULL);
    } else {
        iofunctions_load_file(gummi->iofunc, argv[1]);
        gummi_create_environment(argv[1]);
    }
	
    initial_preview();

    gtk_builder_connect_signals (builder, NULL);       
    g_object_unref (G_OBJECT (builder));
    
    gtk_widget_show_all (mainwindow);
       
    gtk_main ();
    
    return 0;
}
