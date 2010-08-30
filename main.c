#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <glib.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configfile.h"
#include "editor.h"
#include "iofunctions.h"
#include "preview.h"
#include "gui.h"
#include "gummi.h"
#include "motion.h"
#include "utils.h"

/* global gui instance */
gui_t* gui;

static int debug = 0;

static GOptionEntry entries[] = {
    { (const gchar*)"debug", (gchar)'d', 0, G_OPTION_ARG_NONE, &debug, 
        (gchar*)"show debug info", NULL}
};

void on_window_destroy (GtkObject *object, gpointer user_data) {
    gtk_main_quit();
}

int main (int argc, char *argv[]) {
    GtkBuilder* builder;
    editor_t* ec;
    iofunctions_t* iofunc;

    GError* error = NULL;
    GOptionContext* context = g_option_context_new("files");
    g_option_context_add_main_entries(context, entries, PACKAGE);
    g_option_context_parse(context, &argc, &argv, &error);

    slog_init(debug);
    config_init("gummi.cfg");
    gtk_init (&argc, &argv);
    
    slog(L_DEBUG, PACKAGE" version: "VERSION"\n");
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "gummi.glade", NULL);


    // setup sourceview editor pane:
    ec = editor_init(builder);
    //editor_start_search(ec, "svn", FALSE, TRUE, FALSE);

    // setup iofunctions
    iofunc = iofunctions_init(ec);

    // setup poppler preview pane:
    create_preview(builder);
    
    // setup gui:
    gui = gui_init(iofunc, builder);
    
    // either load a file or load the default text based on cli arguments:
    // ... 
    
    // setup work files and such:
   
    // start motion:
    //g_timeout_add_seconds(1, update_preview, NULL);

    gtk_builder_connect_signals (builder, NULL);       
    g_object_unref (G_OBJECT (builder));
    
    gtk_widget_show_all (mainwindow);
       
    gtk_main ();
    
    return 0;
}
