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
#include "utils.h"

/* global environment instance */
gummi_t* gummi = 0;

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
    gtk_builder_add_from_file (builder, "gummi.glade", NULL);


    // setup sourceview editor pane:
    editor = editor_init(builder);
    //editor_start_search(editor, "svn", FALSE, TRUE, FALSE);

    // setup iofunctions
    iofunc = iofunctions_init();

    // setup preview pane:
    preview = preview_init(builder);

    // setup motion
    motion = motion_init(0);

    // setup global environment
    gummi = gummi_init(editor, iofunc, motion, preview);

    // setup gui
    gui_init(builder);
    
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
