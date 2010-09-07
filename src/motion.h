/**
 * @file    motion.h
 * @brief   
 *
 * Copyright (C) 2010 Gummi-Dev Team <alexvandermey@gmail.com>
 * All Rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef GUMMI_MOTION_H
#define GUMMI_MOTION_H

#include <glib.h>

#include "editor.h"
#include "preview.h"

typedef struct _GuMotion {
    GuEditor* b_editor;
    GuPreview* b_preview;
    gint workfd;
    gchar* typesetter;
    gchar* filename;
    gchar* pdffile;
    gchar* workfile;
    const gchar* tmpdir;
    gint errorline;
    gint last_errorline;
    guint update;
    guint timer;
    guint shandlers[2];
} GuMotion;

GuMotion* motion_init(GuEditor* ec, GuPreview* pc);
void motion_create_environment(GuMotion* mc, const gchar* filename);
void motion_set_filename(GuMotion* mc, const gchar* name);
void motion_initial_preview(GuMotion* mc);
void motion_update_workfile(GuMotion* mc);
void motion_update_pdffile(GuMotion* mc);
void motion_update_auxfile(GuMotion* mc);
void motion_start_updatepreview(GuMotion* mc);
void motion_stop_updatepreview(GuMotion* mc);
void motion_export_pdffile(GuMotion* mc, const gchar* path);
void motion_start_timer(GuMotion* mc);
void motion_stop_timer(GuMotion* mc);
gboolean on_key_press_cb(GtkWidget* widget, GdkEventKey* event, void* user);
gboolean on_key_release_cb(GtkWidget* widget, GdkEventKey* event, void* user);
gboolean motion_updatepreview(void* user);

#endif /* GUMMI_MOTION_H */

