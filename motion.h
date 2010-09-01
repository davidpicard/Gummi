#ifndef GUMMI_MOTION_H
#define GUMMI_MOTION_H

#include <glib.h>

#include "editor.h"

typedef struct _GuMotion {
    gint dummy; // you can add members along the way
} GuMotion;

GuMotion* motion_init(gint dum);
void motion_initial_preview(GuEditor* ec);
void motion_update_workfile(GuEditor* ec);
void motion_update_pdffile(GuEditor* ec);


#endif /* GUMMI_MOTION_H */

