/**
 * @file   editor.h
 * @brief  editor related functions for gummi
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 *
 */

#include <stdlib.h>
#include <string.h>

#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourceview.h>

#ifdef USE_GTKSPELL
#   include <gtkspell/gtkspell.h>
#endif

#include <gtk/gtk.h>

#include "configfile.h"
#include "editor.h"
#include "utils.h"

editor_context_t* editor_init(GtkBuilder* builder) {
    GtkWidget *scroll;
    GtkSourceLanguageManager* manager = gtk_source_language_manager_new();
    GtkSourceLanguage* lang = gtk_source_language_manager_get_language
                                                        (manager, "latex");
    editor_context_t* ec = (editor_context_t*)malloc(sizeof(editor_context_t));
    ec->sourcebuffer = gtk_source_buffer_new_with_language(lang);
    ec->sourceview = gtk_source_view_new_with_buffer(ec->sourcebuffer);
    time(&ec->textchange);
    time(&ec->prevchange);
    
    scroll = GTK_WIDGET (gtk_builder_get_object (builder, "editor_scroll"));
    gtk_container_add (GTK_CONTAINER (scroll), ec->sourceview);

#ifdef USE_GTKSPELL
    if (config_get_value("spelling") &&
            !gtkspell_new_attach(GTK_TEXT_VIEW(ec->sourceview), NULL, NULL))
        slog(L_INFO, "failed to initialize gtkspell\n");
#endif

    editor_sourceview_config(ec);
    return ec;
}

void editor_sourceview_config(editor_context_t* ec) {
    GtkWrapMode wrapmode = 0;
    slog(L_DEBUG, "loading default text\n");
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(ec->sourcebuffer),
            config_get_value("welcome"), -1);

    gtk_source_buffer_set_highlight_matching_brackets(ec->sourcebuffer, TRUE);

    const char* font = config_get_value("font");
    slog(L_DEBUG, "setting font to %s\n", font);
    PangoFontDescription* font_desc = pango_font_description_from_string(font);
    gtk_widget_modify_font(ec->sourceview, font_desc);
    pango_font_description_free(font_desc);

    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(ec->sourceview),
            (gboolean)config_get_value("line_numbers"));
    gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(ec->sourceview),
        (gboolean)config_get_value("highlighting"));

    if (config_get_value("textwrapping"))
        wrapmode += 1;
    if (config_get_value("wordwrapping"))
        wrapmode += 2;
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(ec->sourceview), wrapmode);
}

void editor_insert_package(editor_context_t* ec, const gchar* package) {
    GtkTextIter start, mstart, mend, sstart, send;
    gchar pkgstr[BUFSIZ / 64];
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(ec->sourcebuffer), &start);
    gtk_text_iter_forward_search(&start, (gchar*)"\\begin{document}", 0,
            &mstart, &mend, NULL);
    snprintf(pkgstr, BUFSIZ / 64, "\\usepackage{%s}\n", package);
    if (!gtk_text_iter_backward_search(&mstart, pkgstr, 0,
                &sstart, &send, NULL)) {
        gtk_source_buffer_begin_not_undoable_action(ec->sourcebuffer);
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(ec->sourcebuffer), &mstart,
                pkgstr, strlen(pkgstr));
        gtk_source_buffer_end_not_undoable_action(ec->sourcebuffer);
    }
}

void editor_insert_bib(editor_context_t* ec, const gchar* package) {
    GtkTextIter start, end, mstart, mend, sstart, send;
    gchar pkgstr[BUFSIZ / 64];
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(ec->sourcebuffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(ec->sourcebuffer), &end);
    gtk_text_iter_backward_search(&end, (gchar*)"\\end{document}", 0,
            &mstart, &mend, NULL);
    snprintf(pkgstr, BUFSIZ / 64,
            "\\bibliography{%s}{}\n\\bibliographystyle{plain}\n", package);
    if (!gtk_text_iter_forward_search(&start, "\\bibliography{", 0,
                &sstart, &send, NULL)) {
        gtk_source_buffer_begin_not_undoable_action(ec->sourcebuffer);
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(ec->sourcebuffer), &mstart,
                pkgstr, strlen(pkgstr));
        gtk_source_buffer_end_not_undoable_action(ec->sourcebuffer);
    }
}

void editor_set_buffer_changed(editor_context_t* ec) {
    time(&ec->textchange);
    //if (config_get_value("compile_status") &&
    //    0 == strcmp(config_get_value("compile_scheme"), "on_idle"))
    //    motion_start_timer();
}

gboolean editor_check_buffer_changed(editor_context_t* ec) {
    if (ec->prevchange != ec->textchange) {
        ec->prevchange = ec->textchange;
        return TRUE;
    } else
        return FALSE;
}

void undo_change(editor_context_t* ec) {
    if (gtk_source_buffer_can_undo(ec->sourcebuffer)) {
        gtk_source_buffer_redo(ec->sourcebuffer);
        editor_set_buffer_changed(ec);
    }
}

void redo_change(editor_context_t* ec) {
    if (gtk_source_buffer_can_redo(ec->sourcebuffer)) {
        gtk_source_buffer_redo(ec->sourcebuffer);
        editor_set_buffer_changed(ec);
    }
}

char editor_get_buffer(editor_context_t* ec) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ec->sourcebuffer), &start, &end);
    char *text = gtk_text_iter_get_text (&start, &end);
    return *text;
}
