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
#include "environment.h"

/* reference to global environment instance */
extern gummi_t* gummi;
int workfd;

gummi_t* gummi_init(editor_t* ed, iofunctions_t* iof, motion_t* mo,
        preview_t* prev) {
    gummi_t* g = (gummi_t*)malloc(sizeof(gummi_t));
    g->editor = ed;
    g->iofunc = iof;
    g->motion = mo;
    g->preview = prev;
    return g;
}

void create_environment(gchar *filename) {
	
    char tname[1024] = "/tmp/gummi_XXXXXXX"; 
    int fh = mkstemp(tname); 
    gummi->workfile = tname;
    gummi->filename = filename;
    workfd = fh;
    
    char *tmp;
    
    strcpy(tmp, tname);
    
    size_t tmpsize = strlen(tmp);
    strncat(tmp, ".pdf", tmpsize);
    gummi->pdffile = tmp;
    
    printf("Environment created for:\nTEX: %s\nTMP: %s\nPDF: %s\n", \
              gummi->filename,
              gummi->workfile,
              gummi->pdffile); 
    
    
}


