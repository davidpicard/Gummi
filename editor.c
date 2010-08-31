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
#include <gtksourceview/gtksourceiter.h>
#include <gtksourceview/gtksourcelanguagemanager.h>
#include <gtksourceview/gtksourceview.h>

#define USE_GTKSPELL

#ifdef USE_GTKSPELL
#   include <gtkspell/gtkspell.h>
#endif

#include <gtk/gtk.h>

#include "configfile.h"
#include "environment.h"
#include "utils.h"

/* reference to global environment instance */
extern gummi_t* gummi;

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
    ec->replace_activated = FALSE;
    ec->term = NULL;
    ec->cur_swap = FALSE;
    
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
    gtk_text_buffer_insert(ec_sourcebuffer, &start, text, strlen(text));
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
    if (!gtk_text_iter_backward_search(&mstart, pkgstr, 0, &sstart, &send,
                NULL)) {
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

void editor_jumpto_search_result(editor_t* ec, gint direction) {
    if (!ec->term) return;
    if (direction == 1) {
        editor_search_next(ec, FALSE);
    } else {
        editor_search_next(ec, TRUE);
    }
}

void editor_start_search(editor_t* ec, const gchar* term,
        gboolean backwards, gboolean wholeword, gboolean matchcase) {
    /* save options */
    if (ec->term != term) {
        if (ec->term) g_free(ec->term);
        ec->term = (gchar*)malloc(strlen(term) + 1);
        strcpy(ec->term, term);
    }

    ec->backwards = backwards;
    ec->wholeword = wholeword;
    ec->matchcase = matchcase;

    editor_apply_searchtag(ec);
    editor_search_next(ec, FALSE);
}

void editor_apply_searchtag(editor_t* ec) {
    GtkTextIter start, mstart, mend;
    gboolean ret = FALSE;

    gtk_text_buffer_get_start_iter(ec_sourcebuffer, &start);

    if (gtk_text_tag_table_lookup(ec->editortags, "search"))
        gtk_text_tag_table_remove(ec->editortags, ec->searchtag);
    gtk_text_tag_table_add(ec->editortags, ec->searchtag);

    while (TRUE) {
        ret = gtk_source_iter_forward_search(&start, ec->term,
                (ec->matchcase? 0: GTK_SOURCE_SEARCH_CASE_INSENSITIVE),
                &mstart, &mend, NULL);

        if (ret && (!ec->wholeword || (ec->wholeword
                        && gtk_text_iter_starts_word(&mstart)
                        && gtk_text_iter_ends_word(&mend)))) {
            gtk_text_buffer_apply_tag(ec_sourcebuffer, ec->searchtag,
                    &mstart, &mend);
            start =  mend;
        } else break;
    }
}

void editor_search_next(editor_t* ec, gboolean inverse) {
    GtkTextIter current, start, end, mstart, mend;
    GtkTextMark* mark;
    gboolean ret = FALSE, response = FALSE;

    /* place cursor to the next result */
    mark = gtk_text_buffer_get_insert(ec_sourcebuffer);
    gtk_text_buffer_get_iter_at_mark(ec_sourcebuffer, &current, mark);

    if (ec->backwards ^ inverse)
       ret = gtk_source_iter_backward_search(&current, ec->term,
                (ec->matchcase? 0: GTK_SOURCE_SEARCH_CASE_INSENSITIVE),
                &mstart, &mend, NULL);
    else
       ret = gtk_source_iter_forward_search(&current, ec->term,
                (ec->matchcase? 0: GTK_SOURCE_SEARCH_CASE_INSENSITIVE),
                &mstart, &mend, NULL);

    if (ret && (!ec->wholeword || (ec->wholeword
            && gtk_text_iter_starts_word(&mstart)
            && gtk_text_iter_ends_word(&mend))))
        gtk_text_buffer_place_cursor(ec_sourcebuffer,
                (ec->cur_swap ^ ec->backwards ^ inverse)? &mstart: &mend);

    /* check if the top/bottom is reached */
    gtk_text_buffer_get_start_iter(ec_sourcebuffer, &start);
    gtk_text_buffer_get_end_iter(ec_sourcebuffer, &end);

    if (!ret) {
        if (ec->backwards ^ inverse) {
            response = gummi_yes_no_dialog("Top reached, search from bottom?");
            if (response)
                gtk_text_buffer_place_cursor(ec_sourcebuffer, &end);
        } else {
            response = gummi_yes_no_dialog("Bottom reached, search from top?");
            if (response)
                gtk_text_buffer_place_cursor(ec_sourcebuffer, &start);
        }
    }
}

void editor_start_replace_next(editor_t* ec, const gchar* term,
        const gchar* rterm, gboolean backwards, gboolean wholeword,
        gboolean matchcase) {
    GtkTextIter current, mstart, mend;
    GtkTextMark* mark;
    gboolean ret = FALSE;

    if (!ec->replace_activated) {
        ec->cur_swap = TRUE;
        ec->replace_activated = TRUE;
        editor_start_search(ec, term, backwards, wholeword, matchcase);
        return;
    }

    /* place cursor to the next result */
    mark = gtk_text_buffer_get_insert(ec_sourcebuffer);
    gtk_text_buffer_get_iter_at_mark(ec_sourcebuffer, &current, mark);

    if (backwards)
       ret = gtk_source_iter_backward_search(&current, term,
                (matchcase? 0: GTK_SOURCE_SEARCH_CASE_INSENSITIVE),
                &mstart, &mend, NULL);
    else
       ret = gtk_source_iter_forward_search(&current, term,
                (matchcase? 0: GTK_SOURCE_SEARCH_CASE_INSENSITIVE),
                &mstart, &mend, NULL);

    if (ret && (!wholeword || (wholeword
            && gtk_text_iter_starts_word(&mstart)
            && gtk_text_iter_ends_word(&mend)))) {
        gtk_text_buffer_delete(ec_sourcebuffer, &mstart, &mend);
        gtk_text_buffer_insert(ec_sourcebuffer, &mstart, rterm, strlen(rterm));
        editor_search_next(ec, FALSE);
    }
    return;
}

void editor_start_replace_all(editor_t* ec, const gchar* term,
        const gchar* rterm, gboolean backwards, gboolean wholeword,
        gboolean matchcase) {
    GtkTextIter start, mstart, mend;
    gboolean ret = FALSE;

    gtk_text_buffer_get_start_iter(ec_sourcebuffer, &start);

    while (TRUE) {
        ret = gtk_source_iter_forward_search(&start, term,
                (matchcase? 0: GTK_SOURCE_SEARCH_CASE_INSENSITIVE),
                &mstart, &mend, NULL);

        if (ret && (!wholeword || (wholeword
                && gtk_text_iter_starts_word(&mstart)
                && gtk_text_iter_ends_word(&mend)))) {
            gtk_text_buffer_delete(ec_sourcebuffer, &mstart, &mend);
            gtk_text_buffer_insert(ec_sourcebuffer, &mstart, rterm,
                    strlen(rterm));
            start =  mstart;
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
