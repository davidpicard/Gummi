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

typedef struct _search_result {
    gint len;
    gint size;
    gint pos;
    GtkTextIter** a_start;
    GtkTextIter** a_end;
} result_t;

typedef struct _editor_context {
    GtkWidget *sourceview;
    GtkSourceBuffer *sourcebuffer;
    GtkTextTag* errortag;
    GtkTextTag* searchtag;
    GtkTextTagTable* editortags;
    result_t* sresult;
} editor_t;

editor_t* editor_init(GtkBuilder *builder);
void editor_sourceview_config(editor_t* ec);
void editor_activate_spellchecking(editor_t* ec, gboolean status);
void editor_fill_buffer(editor_t* ec, const gchar* text);
gchar* editor_grab_buffer(editor_t* ec);
void editor_insert_package(editor_t* ec, const gchar* package);
void editor_insert_bib(editor_t* ec, const gchar* package);
void editor_set_selection_textstyle(editor_t* ec);
void editor_apply_errortags(editor_t* ec, gint line);
void editor_apply_searchtags(editor_t* ec);
void editor_jumpto_search_result(editor_t* ec, gint direction);
void editor_start_search(editor_t* ec, const gchar* term, gboolean backwards,
        gboolean wholeword, gboolean matchcase);
void editor_search_buffer_forward(editor_t* ec, const gchar* term,
        gboolean wholeword, gboolean matchcase);
void editor_search_buffer_backward(editor_t* ec, const gchar* term,
        gboolean wholeword, gboolean matchcase);

result_t* search_result_init(void);
void search_result_append(result_t* sc, GtkTextIter* start, GtkTextIter* end);

#endif
