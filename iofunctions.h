/**
 * @file   iofunctions.h
 * @brief  
 * @author Alexander van der Mey <alexvandermey@gmail.com>
 *
 * Copyright (C) 2010 - Alexander van der Mey <alexvandermey@gmail.com>
 * All Rights reserved.
 */

#ifndef GUMMI_IOFUNCTIONS_H
#define GUMMI_IOFUNCTIONS_H

#include "editor.h"

typedef struct _iofunctions {
    gint dummy;
} iofunctions_t;

iofunctions_t* iofunctions_init(void);
void iofunctions_error_message (const gchar *message);
void iofunctions_load_file(iofunctions_t*, gchar*);

#endif /* GUMMI_IOFUNCTIONS_H */



