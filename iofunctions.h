#ifndef GUMMI_IOFUNCTIONS_H
#define GUMMI_IOFUNCTIONS_H

#include "editor.h"

typedef struct _iofunctions {
    editor_t* editor;
} iofunctions_t;

iofunctions_t* iofunctions_init(editor_t* ec);
void iofunctions_error_message (const gchar *message);
void iofunctions_load_file(iofunctions_t*, gchar*);

#endif /* GUMMI_IOFUNCTIONS_H */



