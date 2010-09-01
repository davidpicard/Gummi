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

#define _(T) gettext(T)

typedef struct _gummi {
    gchar* filename;
    gchar* pdffile;
    gchar* workfile;
    const gchar* tmpdir;
    GtkBuilder* builder;
    editor_t* editor;
    importer_t* importer;
    iofunctions_t* iofunc;
    motion_t* motion;
    preview_t* preview;
} gummi_t;

gummi_t* gummi_init(GtkBuilder* bd, editor_t* ed, importer_t* im,
        iofunctions_t* iof, motion_t* mo, preview_t* prev);
        
void gummi_create_environment(gummi_t* gummi, const gchar* filename);
void gummi_set_filename(gummi_t* gummi, const gchar* name);
    

#endif /* GUMMI_ENVIRONMENT_H */
