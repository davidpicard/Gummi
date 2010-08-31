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

#include "editor.h"
#include "iofunctions.h"
#include "motion.h"
#include "preview.h"

const gchar *tmpdir;

typedef struct _gummi {
    gchar* filename;
    gchar* pdffile;
    gchar* workfile;
    editor_t* editor;
    iofunctions_t* iofunc;
    motion_t* motion;
    preview_t* preview;
} gummi_t;

gummi_t* gummi_init(editor_t* ed, iofunctions_t* iof, motion_t* mo,
        preview_t* prev);
        
void gummi_create_environment();
    

#endif /* GUMMI_ENVIRONMENT_H */
