#ifndef GUMMI_MOTION_H
#define GUMMI_MOTION_H

#include <glib.h>

#include "editor.h"

typedef struct _motion {
    gint dummy; // you can add members along the way
} motion_t;

motion_t* motion_init(gint dum);
void motion_initial_preview(editor_t* ec);
void motion_update_workfile(editor_t* ec);
void motion_update_pdffile(editor_t* ec);


#endif /* GUMMI_MOTION_H */

