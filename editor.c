/**
 * @file   editor.c
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
#define GTK_SOURCE_SEARCH_CASE_INSENSITIVE 0

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
    ec->errortag = gtk_text_tag_new("error");
    ec->searchtag = gtk_text_tag_new("search");
    ec->editortags = gtk_text_buffer_get_tag_table(ec_sourcebuffer);
    ec->sresult = search_result_init();
    
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
    gtk_text_buffer_set_text(ec_sourcebuffer,
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
    gtk_text_view_set_wrap_mode(ec_sourceview, wrapmode);
    g_object_set(G_OBJECT(ec->errortag), "background", "red",
            "foreground", "white", NULL);
    g_object_set(G_OBJECT(ec->searchtag), "background", "yellow", NULL);
}

void editor_activate_spellchecking(editor_t* ec, gboolean status) {
#ifdef USE_GTKSPELL
    const gchar* lang = config_get_value("spell_language");
    GError* err = NULL;
    if (status) {
        GtkSpell* spell = gtkspell_new_attach(ec_sourceview, NULL, &err);
        if (!spell)
            slog(L_INFO, "gtkspell: %s\n", err->message);
        if (!gtkspell_set_language(spell, lang, &err))
            slog(L_INFO, "gtkspell: %s\n", err->message);
    } else {
        GtkSpell* spell = gtkspell_get_from_text_view(
                ec_sourceview);
        gtkspell_detach(spell);
    }
#endif
}

void editor_fill_buffer(editor_t* ec, const gchar* text) {
    GtkTextIter start;
    gtk_text_buffer_begin_user_action(ec_sourcebuffer);
    gtk_text_buffer_set_text(ec_sourcebuffer, (gchar*)"", 0);
    gtk_source_buffer_begin_not_undoable_action(ec->sourcebuffer);
    gtk_text_buffer_get_start_iter(ec_sourcebuffer, &start);
    gtk_widget_set_sensitive(GTK_WIDGET(ec->sourceview), FALSE);
    gtk_text_buffer_insert(ec_sourcebuffer, &start,
            text, strlen(text));
    gtk_text_buffer_set_modified(ec_sourcebuffer, FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(ec->sourceview), TRUE);
    gtk_source_buffer_end_not_undoable_action(ec->sourcebuffer);
    gtk_text_buffer_end_user_action(ec_sourcebuffer);
}

gchar* editor_grab_buffer(editor_t* ec) {
    GtkTextIter start, end;
    gtk_widget_set_sensitive(GTK_WIDGET(ec->sourceview), FALSE);
    gtk_text_buffer_get_bounds(ec_sourcebuffer, &start, &end);
    gtk_widget_set_sensitive(GTK_WIDGET(ec->sourceview), TRUE);
    gchar* pstr = gtk_text_iter_get_text (&start, &end);
    return pstr;
}

void editor_insert_package(editor_t* ec, const gchar* package) {
    GtkTextIter start, mstart, mend, sstart, send;
    gchar pkgstr[BUFSIZ / 64];
    gtk_text_buffer_get_start_iter(ec_sourcebuffer, &start);
    gtk_text_iter_forward_search(&start, (gchar*)"\\begin{document}", 0,
            &mstart, &mend, NULL);
    snprintf(pkgstr, BUFSIZ / 64, "\\usepackage{%s}\n", package);
    if (!gtk_text_iter_backward_search(&mstart, pkgstr, 0,
                &sstart, &send, NULL)) {
        gtk_source_buffer_begin_not_undoable_action(ec->sourcebuffer);
        gtk_text_buffer_insert(ec_sourcebuffer, &mstart,
                pkgstr, strlen(pkgstr));
        gtk_source_buffer_end_not_undoable_action(ec->sourcebuffer);
    }
}

void editor_insert_bib(editor_t* ec, const gchar* package) {
    GtkTextIter start, end, mstart, mend, sstart, send;
    gchar pkgstr[BUFSIZ / 64];
    gtk_text_buffer_get_start_iter(ec_sourcebuffer, &start);
    gtk_text_buffer_get_end_iter(ec_sourcebuffer, &end);
    gtk_text_iter_backward_search(&end, (gchar*)"\\end{document}", 0,
            &mstart, &mend, NULL);
    snprintf(pkgstr, BUFSIZ / 64,
            "\\bibliography{%s}{}\n\\bibliographystyle{plain}\n", package);
    if (!gtk_text_iter_forward_search(&start, "\\bibliography{", 0,
                &sstart, &send, NULL)) {
        gtk_source_buffer_begin_not_undoable_action(ec->sourcebuffer);
        gtk_text_buffer_insert(ec_sourcebuffer, &mstart,
                pkgstr, strlen(pkgstr));
        gtk_source_buffer_end_not_undoable_action(ec->sourcebuffer);
    }
}

void editor_set_selection_textstyle(editor_t* ec) {
    //formatting();
    gtk_text_buffer_set_modified(ec_sourcebuffer, TRUE);
}

void editor_apply_errortags(editor_t* ec, gint line) {
    GtkTextIter start, end;
    /* remove the tag from the table if it is in threre */
    if (gtk_text_tag_table_lookup(ec->editortags, "error"))
        gtk_text_tag_table_remove(ec->editortags, ec->errortag);

    if (line) {
        gtk_text_tag_table_add(ec->editortags, ec->errortag);
        gtk_text_buffer_get_iter_at_line(ec_sourcebuffer, &start, line -1);
        gtk_text_buffer_get_iter_at_line(ec_sourcebuffer, &end, line);
        gtk_text_buffer_apply_tag(ec_sourcebuffer, ec->errortag, &start, &end);
    }
}

void editor_apply_searchtags(editor_t* ec) {
    gint i = 0;
    if (gtk_text_tag_table_lookup(ec->editortags, "search"))
        gtk_text_tag_table_remove(ec->editortags, ec->searchtag);
    gtk_text_tag_table_add(ec->editortags, ec->searchtag);
    for (i = 0; i < ec->sresult->len; ++i) {
        gtk_text_buffer_apply_tag(ec_sourcebuffer,
            ec->searchtag, ec->sresult->a_start[i],
            ec->sresult->a_end[i]);
    }
}

void editor_jumpto_search_result(editor_t* ec, gint direction) {
    gint pos = ec->sresult->pos + direction;
    if (pos < 0)
        pos = ec->sresult->size -1;
    else if (pos >= ec->sresult->size)
        pos = 0;
    gtk_text_buffer_place_cursor(ec_sourcebuffer, ec->sresult->a_start[pos]);
}

void editor_start_search(editor_t* ec, const gchar* term, gboolean backwards,
        gboolean wholeword, gboolean matchcase) {
    if (backwards)
        editor_search_buffer_backward(ec, term, wholeword, matchcase);
    else
        editor_search_buffer_forward(ec, term, wholeword, matchcase);
    editor_apply_searchtags(ec);

    if (backwards)
        gtk_text_buffer_place_cursor(ec_sourcebuffer, ec->sresult->a_start[0]);
    else
        gtk_text_buffer_place_cursor(ec_sourcebuffer, ec->sresult->a_end[0]);
}

void editor_search_buffer_forward(editor_t* ec, const gchar* term,
        gboolean wholeword, gboolean matchcase) {
    GtkTextIter current, mstart, mend;
    free(ec->sresult);
    ec->sresult = search_result_init();
    GtkTextMark* mark = gtk_text_buffer_get_insert(ec_sourcebuffer);
    gtk_text_buffer_get_iter_at_mark(ec_sourcebuffer, &current, mark);

    while (TRUE) {
        if (gtk_text_iter_forward_search(&current, term,
                    (matchcase?GTK_SOURCE_SEARCH_CASE_INSENSITIVE:0),
                    &mstart, &mend, NULL)) {
            if (!wholeword || (wholeword && gtk_text_iter_starts_word(&mstart)
                        && gtk_text_iter_ends_word(&mend)))
                search_result_append(ec->sresult, &mstart, &mend);
        } else break;
    }
}

void editor_search_buffer_backward(editor_t* ec, const gchar* term,
        gboolean wholeword, gboolean matchcase) {
    GtkTextIter current, mstart, mend;
    free(ec->sresult);
    ec->sresult = search_result_init();
    GtkTextMark* mark = gtk_text_buffer_get_insert(ec_sourcebuffer);
    gtk_text_buffer_get_iter_at_mark(ec_sourcebuffer, &current, mark);

    while (TRUE) {
        if (gtk_text_iter_backward_search(&current, term,
                    (matchcase?GTK_SOURCE_SEARCH_CASE_INSENSITIVE:0),
                    &mstart, &mend, NULL)) {
            if (!wholeword || (wholeword && gtk_text_iter_starts_word(&mstart)
                        && gtk_text_iter_ends_word(&mend)))
                search_result_append(ec->sresult, &mstart, &mend);
        } else break;
    }
}

void undo_change(editor_t* ec) {
    if (gtk_source_buffer_can_undo(ec->sourcebuffer)) {
        gtk_source_buffer_redo(ec->sourcebuffer);
        gtk_text_buffer_set_modified(ec_sourcebuffer, TRUE);
    }
}

void redo_change(editor_t* ec) {
    if (gtk_source_buffer_can_redo(ec->sourcebuffer)) {
        gtk_source_buffer_redo(ec->sourcebuffer);
        gtk_text_buffer_set_modified(ec_sourcebuffer, TRUE);
    }
}

result_t* search_result_init(void) {
    result_t* res = (result_t*)malloc(sizeof(result_t));
    res->len = SEARCH_RESULT_INIT_SIZE;
    res->size = 0;
    res->pos = 0;
    res->a_start = (GtkTextIter**)malloc(sizeof(GtkTextIter*));
    res->a_end = (GtkTextIter**)malloc(sizeof(GtkTextIter*));
    return res;
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
