/**
 * @file   editor.h
 * @brief  editor related functions for gummi
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 *
 */

#ifndef GUMMI_EDITOR_H
#define GUMMI_EDITOR_H

#include <glib.h>
#include <gtk/gtk.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceview.h>

#define SEARCH_RESULT_INIT_SIZE 16
#define ec_sourcebuffer GTK_TEXT_BUFFER(ec->sourcebuffer)
#define ec_sourceview GTK_TEXT_VIEW(ec->sourceview)

typedef struct _GuEditor {
    GtkWidget *sourceview;
    GtkSourceBuffer *sourcebuffer;
    GtkTextTag* errortag;
    GtkTextTag* searchtag;
    GtkTextTagTable* editortags;
    gboolean replace_activated;
    gchar* term;
    gboolean backwards;
    gboolean wholeword;
    gboolean matchcase;
    gboolean cur_swap;
} GuEditor;

GuEditor* editor_init(GtkBuilder *builder);
void editor_sourceview_config(GuEditor* ec);
void editor_activate_spellchecking(GuEditor* ec, gboolean status);
void editor_fill_buffer(GuEditor* ec, const gchar* text);
gchar* editor_grab_buffer(GuEditor* ec);
void editor_insert_package(GuEditor* ec, const gchar* package);
void editor_insert_bib(GuEditor* ec, const gchar* package);
void editor_set_selection_textstyle(GuEditor* ec, const gchar* type);
void editor_apply_errortags(GuEditor* ec, gint line);
void editor_jumpto_search_result(GuEditor* ec, gint direction);
void editor_start_search(GuEditor* ec, const gchar* term,
        gboolean backwards, gboolean wholeword, gboolean matchcase, gboolean cs);
void editor_apply_searchtag(GuEditor* ec);
void editor_search_next(GuEditor* ec, gboolean inverse);
void editor_start_replace_next(GuEditor* ec, const gchar* term,
        const gchar* rterm, gboolean backwards, gboolean wholeword,
        gboolean matchcase);
void editor_start_replace_all(GuEditor* ec, const gchar* term,
        const gchar* rterm, gboolean backwards, gboolean wholeword,
        gboolean matchcase);
void editor_get_current_iter(GuEditor* ec, GtkTextIter* current);
void editor_undo_change(GuEditor* ec);
void editor_redo_change(GuEditor* ec);

#endif /* GUMMI_EDITOR_H */
