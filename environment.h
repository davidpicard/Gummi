/**
 * @file   environment.h
 * @brief  
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
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
