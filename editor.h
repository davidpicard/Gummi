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

typedef struct _editor_context {
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
} editor_t;

editor_t* editor_init(GtkBuilder *builder);
void editor_sourceview_config(editor_t* ec);
void editor_activate_spellchecking(editor_t* ec, gboolean status);
void editor_fill_buffer(editor_t* ec, const gchar* text);
gchar* editor_grab_buffer(editor_t* ec);
void editor_insert_package(editor_t* ec, const gchar* package);
void editor_insert_bib(editor_t* ec, const gchar* package);
void editor_set_selection_textstyle(editor_t* ec, const gchar* type);
void editor_apply_errortags(editor_t* ec, gint line);
void editor_jumpto_search_result(editor_t* ec, gint direction);
void editor_start_search(editor_t* ec, const gchar* term,
        gboolean backwards, gboolean wholeword, gboolean matchcase, gboolean cs);
void editor_apply_searchtag(editor_t* ec);
void editor_search_next(editor_t* ec, gboolean inverse);
void editor_start_replace_next(editor_t* ec, const gchar* term,
        const gchar* rterm, gboolean backwards, gboolean wholeword,
        gboolean matchcase);
void editor_start_replace_all(editor_t* ec, const gchar* term,
        const gchar* rterm, gboolean backwards, gboolean wholeword,
        gboolean matchcase);
void editor_undo_change(editor_t* ec);
void editor_redo_change(editor_t* ec);

#endif /* GUMMI_EDITOR_H */
