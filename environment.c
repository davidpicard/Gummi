/**
 * @file   environment.c
 * @brief  
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#include <stdlib.h>
#include "environment.h"

gummi_t* gummi_init(editor_t* ed, iofunctions_t* iof, motion_t* mo,
        preview_t* prev) {
    gummi_t* g = (gummi_t*)malloc(sizeof(gummi_t));
    g->editor = ed;
    g->iofunc = iof;
    g->motion = mo;
    g->preview = prev;
    return g;
}
