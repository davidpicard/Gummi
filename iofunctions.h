#ifndef GUMMI_IOFUNCTIONS_H
#define GUMMI_IOFUNCTIONS_H

#include "editor.h"

typedef struct _iofunctions {
    editor_t* editor;
} iofunctions_t;


void load_file(iofunctions_t*, gchar*);

#endif /* GUMMI_IOFUNCTIONS_H */



