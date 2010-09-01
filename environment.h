/**
 * @file    environment.h
 * @brief   
 *
 * Copyright (C) 2010 Gummi-Dev Team <alexvandermey@gmail.com>
 * All Rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef GUMMI_ENVIRONMENT_H
#define GUMMI_ENVIRONMENT_H

#ifdef HAVE_CONFIG_H
#   include "config.h"
#else
#   define PACKAGE "gummi"
#   define VERSION "svn"
#endif

#include <libintl.h>

#include "editor.h"
#include "importer.h"
#include "iofunctions.h"
#include "motion.h"
#include "preview.h"
#include "template.h"

#define _(T) gettext(T)

typedef struct _Gummi {
    gint workfd;
    gchar* filename;
    gchar* pdffile;
    gchar* workfile;
    const gchar* tmpdir;
    GtkBuilder* builder;
    GuEditor* editor;
    GuImporter* importer;
    GuIOFunc* iofunc;
    GuMotion* motion;
    GuPreview* preview;
    GuTemplate* templ;
} Gummi;

Gummi* gummi_init(GtkBuilder* bd, GuEditor* ed, GuImporter* im,
        GuIOFunc* iof, GuMotion* mo, GuPreview* prev, GuTemplate* tpl);
        
void gummi_create_environment(Gummi* gummi, const gchar* filename);
void gummi_set_filename(Gummi* gummi, const gchar* name);
    

#endif /* GUMMI_ENVIRONMENT_H */
