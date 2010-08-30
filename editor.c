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

#define USE_GTKSPELL

#ifdef USE_GTKSPELL
#   include <gtkspell/gtkspell.h>
#endif

#include <gtk/gtk.h>

#include "configfile.h"
#include "editor.h"
#include "utils.h"

editor_t* editor_init(GtkBuilder* builder) {
    GtkWidget *scroll;
    GtkSourceLanguageManager* manager = gtk_source_language_manager_new();
    GtkSourceLanguage* lang = gtk_source_language_manager_get_language
                                                        (manager, "latex");
    editor_t* ec = (editor_t*)malloc(sizeof(editor_t));
    ec->sourcebuffer = gtk_source_buffer_new_with_language(lang);
    ec->sourceview = gtk_source_view_new_with_buffer(ec->sourcebuffer);
    ec->errortag = gtk_text_tag_new(NULL);
    ec->searchtag = gtk_text_tag_new(NULL);
    ec->editortags = gtk_text_buffer_get_tag_table(GTK_TEXT_BUFFER(
                ec->sourcebuffer));
    time(&ec->textchange);
    time(&ec->prevchange);
    
    scroll = GTK_WIDGET (gtk_builder_get_object (builder, "editor_scroll"));
    gtk_container_add (GTK_CONTAINER (scroll), ec->sourceview);

#ifdef USE_GTKSPELL
    if (config_get_value("spelling"))
        editor_activate_spellchecking(ec, TRUE);
#endif

    editor_sourceview_config(ec);
    return ec;
}

void editor_sourceview_config(editor_t* ec) {
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
    g_object_set(G_OBJECT(ec->errortag), "background", "red",
            "foreground", "white", NULL);
    g_object_set(G_OBJECT(ec->searchtag), "background", "yellow", NULL);
}

void editor_activate_spellchecking(editor_t* ec, gboolean status) {
#ifdef USE_GTKSPELL
    const gchar* lang = config_get_value("spell_language");
    GError* err = NULL;
    if (status) {
        GtkSpell* spell = gtkspell_new_attach(GTK_TEXT_VIEW(ec->sourceview),
                NULL, &err);
        if (!spell)
            slog(L_INFO, "gtkspell: %s\n", err->message);
        if (!gtkspell_set_language(spell, lang, &err))
            slog(L_INFO, "gtkspell: %s\n", err->message);
    } else {
        GtkSpell* spell = gtkspell_get_from_text_view(
                GTK_TEXT_VIEW(ec->sourceview));
        gtkspell_detach(spell);
    }
#endif
}

void editor_fill_buffer(editor_t* ec, const gchar* text) {
    GtkTextIter start;
    gtk_text_buffer_begin_user_action(GTK_TEXT_BUFFER(ec->sourcebuffer));
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(ec->sourcebuffer), (gchar*)"", 0);
    gtk_source_buffer_begin_not_undoable_action(ec->sourcebuffer);
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(ec->sourcebuffer), &start);
    gtk_widget_set_sensitive(GTK_WIDGET(ec->sourceview), FALSE);
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(ec->sourcebuffer), &start,
            text, strlen(text));
    gtk_text_buffer_set_modified(GTK_TEXT_BUFFER(ec->sourcebuffer), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(ec->sourceview), TRUE);
    gtk_source_buffer_end_not_undoable_action(ec->sourcebuffer);
    gtk_text_buffer_end_user_action(GTK_TEXT_BUFFER(ec->sourcebuffer));
}

gchar* editor_grab_buffer(editor_t* ec) {
    GtkTextIter start, end;
    gtk_widget_set_sensitive(GTK_WIDGET(ec->sourceview), FALSE);
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ec->sourcebuffer), &start, &end);
    gtk_widget_set_sensitive(GTK_WIDGET(ec->sourceview), TRUE);
    gchar* pstr = gtk_text_iter_get_text (&start, &end);
    return pstr;
}

void editor_insert_package(editor_t* ec, const gchar* package) {
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

void editor_insert_bib(editor_t* ec, const gchar* package) {
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

void editor_set_selection_textstyle(editor_t* ec) {
    //formatting();
    editor_set_buffer_changed(ec);
}

void editor_apply_errortags(editor_t* ec, gint line) {
    GtkTextIter start, end;
    /* remove the tag from the table if it is in threre */
    gtk_text_tag_table_remove(ec->editortags, ec->errortag);
    if (line) {
        gtk_text_tag_table_add(ec->editortags, ec->errortag);
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(ec->sourcebuffer),
                &start, line -1);
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(ec->sourcebuffer),
                &end, line);
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(ec->sourcebuffer),
                ec->errortag, &start, &end);
    }
}

void editor_apply_searchtags(editor_t* ec, result_t result) {
    gint i = 0;
    gtk_text_tag_table_remove(ec->editortags, ec->searchtag);
    ec->search_result = (result_t){ 0, 0, 0, 0, 0 };
    gtk_text_tag_table_add(ec->editortags, ec->searchtag);
    for (i = 0; i < result.len; ++i) {
        search_result_append(&ec->search_result, result.a_start[i],
                result.a_end[i]);
        gtk_text_buffer_apply_tag(GTK_TEXT_BUFFER(ec->sourcebuffer),
                ec->searchtag, result.a_start[i], result.a_end[i]);
    }
}

void editor_jumpto_search_result(editor_t* ec, gint direction) {
    gint pos = ec->search_result.pos + direction;
    if (pos < 0)
        pos = ec->search_result.size -1;
    else if (pos >= ec->search_result.size)
        pos = 0;
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(ec->sourcebuffer),
            ec->search_result.a_start[pos]);
}

void editor_start_search(editor_t* ec, const gchar* term, gboolean backwards,
        gboolean wholeword, gboolean matchcase) {
    ec->search_result = (result_t){ 0, 0, 0, 0, 0 };
}

void editor_set_buffer_changed(editor_t* ec) {
    time(&ec->textchange);
    //if (config_get_value("compile_status") &&
    //    0 == strcmp(config_get_value("compile_scheme"), "on_idle"))
    //    motion_start_timer();
}

gboolean editor_check_buffer_changed(editor_t* ec) {
    if (ec->prevchange != ec->textchange) {
        ec->prevchange = ec->textchange;
        return TRUE;
    } else
        return FALSE;
}

void undo_change(editor_t* ec) {
    if (gtk_source_buffer_can_undo(ec->sourcebuffer)) {
        gtk_source_buffer_redo(ec->sourcebuffer);
        editor_set_buffer_changed(ec);
    }
}

void redo_change(editor_t* ec) {
    if (gtk_source_buffer_can_redo(ec->sourcebuffer)) {
        gtk_source_buffer_redo(ec->sourcebuffer);
        editor_set_buffer_changed(ec);
    }
}

void search_result_append(result_t* sc, GtkTextIter* start, GtkTextIter* end) {
    if (sc->size == sc->len) {
        GtkTextIter** nstart = (GtkTextIter**)malloc(sc->size * 2 *
                sizeof(GtkTextIter*));
        GtkTextIter** nend = (GtkTextIter**)malloc(sc->size * 2 *
                sizeof(GtkTextIter*));
        memcpy(nstart, sc->a_start, sc->size * sizeof(GtkTextIter*));
        memcpy(nend, sc->a_end, sc->size * sizeof(GtkTextIter*));
        sc->len *= 2;
        free(sc->a_start);
        free(sc->a_end);
        sc->a_start = nstart;
        sc->a_end = nend;
    }
    sc->a_start[sc->size] = start;
    sc->a_end[sc->size] = end;
    ++sc->size;
}
