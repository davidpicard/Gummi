#ifndef GUMMI_EDITOR_H
#define GUMMI_EDITOR_H

#include <gtk/gtk.h>

void editor_init(GtkBuilder *);
char get_buffer();
void set_sourceview_config();

#endif
