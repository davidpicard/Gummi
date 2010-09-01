#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include <glib.h>
#include <gtk/gtk.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "configfile.h"
#include "environment.h"
#include "gui.h"
#include "importer.h"
#include "iofunctions.h"
#include "template.h"
#include "utils.h"

static int debug = 0;
Gummi* gummi = 0;

static GOptionEntry entries[] = {
    { (const gchar*)"debug", (gchar)'d', 0, G_OPTION_ARG_NONE, &debug, 
        (gchar*)"show debug info", NULL},
    { NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL }
};

void on_window_destroy (GtkObject *object, gpointer user_data) {
    gtk_main_quit();
}

int main (int argc, char *argv[]) {
    GtkBuilder* builder;
    GuEditor* editor;
    GuImporter* importer;
    GuIOFunc* iofunc;
    GuMotion* motion;
    GuPreview* preview;
    GuTemplate* templ;

    /* set up i18n */
    bindtextdomain(PACKAGE, "/usr/share/locale");
    setlocale(LC_ALL, "");
    textdomain(PACKAGE);

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
    importer = importer_init(builder);
    iofunc = iofunctions_init();
    motion = motion_init(0); 
    preview = preview_init(builder);
    templ = template_init(builder);

    gummi =
        gummi_init(builder, editor, importer, iofunc, motion, preview, templ);

    gui_init();

    if ( argc != 2 ) {
        iofunctions_load_default_text(gummi->iofunc, editor);
        gummi_create_environment(gummi, NULL);
    } else {
        iofunctions_load_file(gummi->iofunc, editor, argv[1]);
        gummi_create_environment(gummi, argv[1]);
    }
	
    motion_initial_preview(editor);
    gui_main();
    return 0;
}
