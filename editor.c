#include <gtksourceview/gtksourceview.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtk/gtk.h>

#include "configfile.h"
#include "editor.h"
#include "utils.h"

static GtkWidget *sourceview;
static GtkSourceBuffer *sourcebuffer;

void editor_init(GtkBuilder* builder) {
    GtkWidget *scroll;
    GtkSourceLanguageManager* manager = gtk_source_language_manager_new();
    GtkSourceLanguage* lang = gtk_source_language_manager_get_language
                                                        (manager, "latex");
    sourcebuffer = gtk_source_buffer_new_with_language(lang);
    sourceview = gtk_source_view_new_with_buffer(sourcebuffer);
    
    scroll = GTK_WIDGET (gtk_builder_get_object (builder, "editor_scroll"));
    gtk_container_add (GTK_CONTAINER (scroll), sourceview);
    
    set_sourceview_config();
}

void set_sourceview_config() {
    GtkWrapMode wrapmode;
    slog(L_DEBUG, "loading default text\n");
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(sourcebuffer), config_get_value("welcome"), -1);

    gtk_source_buffer_set_highlight_matching_brackets(sourcebuffer, TRUE);

    const char* font = config_get_value("font");
    slog(L_DEBUG, "setting font to %s\n", font);
    PangoFontDescription* font_desc = pango_font_description_from_string(font);
    gtk_widget_modify_font(sourceview, font_desc);
    pango_font_description_free(font_desc);

    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(sourceview),
            (gboolean)config_get_value("line_numbers"));
    gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(sourceview),
        (gboolean)config_get_value("highlighting"));

    if (config_get_value("textwrapping"))
        wrapmode += 1;
    if (config_get_value("wordwrapping"))
        wrapmode += 2;
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(sourceview), wrapmode);
}

char get_buffer() {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(sourcebuffer), &start, &end);
    char *text = gtk_text_iter_get_text (&start, &end);
    return *text;
}
