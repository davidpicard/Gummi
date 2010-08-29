#ifndef GUMMI_PREVIEW_H
#define GUMMI_PREVIEW_H

#include <gtk/gtk.h>
void create_preview(GtkBuilder *);
gboolean on_expose(GtkWidget*, GdkEventExpose*, gpointer);

#endif /* GUMMI_PREVIEW_H */
