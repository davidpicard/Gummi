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
    gint workfd;
    gchar* typesetter;
    gchar* filename;
    gchar* pdffile;
    gchar* workfile;
    const gchar* tmpdir;
    guint update;
} GuMotion;

GuMotion* motion_init(gint dum);
void motion_create_environment(GuMotion* mc, const gchar* filename);
void motion_set_filename(GuMotion* motion, const gchar* name);
void motion_initial_preview(GuMotion* mo, GuEditor* ec, GuPreview* pr);
void motion_update_workfile(GuMotion* mo, GuEditor* ec);
void motion_update_pdffile(GuMotion* motion);
void motion_update_auxfile(GuMotion* motion);
void motion_start_updatepreview(GuMotion* motion);
void motion_stop_updatepreview(GuMotion* motion);
void motion_export_pdffile(GuMotion* motion, const gchar* path);
gboolean motion_updatepreview();


#endif /* GUMMI_MOTION_H */

