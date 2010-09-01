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

typedef struct _GuIOFunc {
    gint dummy;
} GuIOFunc;

GuIOFunc* iofunctions_init(void);
void iofunctions_error_message (const gchar *message);
void iofunctions_load_default_text(GuIOFunc* iofunc, GuEditor* ec);
void iofunctions_load_file(GuIOFunc*, GuEditor* ec, gchar*);
void iofunctions_write_file(GuIOFunc* iofunc, GuEditor* ec,
        gchar *filename);

#endif /* GUMMI_IOFUNCTIONS_H */



