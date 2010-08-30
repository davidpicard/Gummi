/**
 * @file   environment.c
 * @brief  
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "environment.h"

/* reference to global environment instance */
extern gummi_t* gummi;
int workfd = -1;


gummi_t* gummi_init(editor_t* ed, iofunctions_t* iof, motion_t* mo,
        preview_t* prev) {
    gummi_t* g = (gummi_t*)malloc(sizeof(gummi_t));
    
    tmpdir = g_get_tmp_dir();
    g->editor = ed;
    g->iofunc = iof;
    g->motion = mo;
    g->preview = prev;
    return g;
}


void create_environment(gchar *filename) {
    if (workfd != -1) {
        close(workfd);
    } // close previous work file using its file descriptor
    
    // TODO: use const char TMPDIR (see environment.h)
    char tname[1024] = "/tmp/gummi_XXXXXXX"; 
    workfd = mkstemp(tname); 
    gummi->workfile = tname;
    gummi->filename = filename;

    char *tmp;    
    strcpy(tmp, tname);
    strncat(tmp, ".pdf", sizeof (tmp + 4));
    gummi->pdffile = tmp;
    
    printf("Environment created for:\nTEX: %s\nTMP: %s\nPDF: %s\n", \
              gummi->filename,
              gummi->workfile,
              gummi->pdffile); 
}


