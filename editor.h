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

#include <gtk/gtk.h>
#include <gtksourceview/gtksourcebuffer.h>
#include <gtksourceview/gtksourceview.h>

typedef struct _editor_context {
    GtkWidget *sourceview;
    GtkSourceBuffer *sourcebuffer;
    time_t textchange;
    time_t prevchange;
} editor_context_t;

editor_context_t* editor_init(GtkBuilder *builder);
void editor_sourceview_config(editor_context_t* ec);
void editor_insert_package(editor_context_t* ec, const gchar* package);
void editor_insert_bib(editor_context_t* ec, const gchar* package);
gboolean editor_check_buffer_changed(editor_context_t* ec);
void editor_set_buffer_changed(editor_context_t* ec);
char editor_get_buffer(editor_context_t* ec);

#endif
