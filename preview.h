/**
 * @file   preview.h
 * @brief  
 * @author Alexander van der Mey <alexvandermey@gmail.com>
 *
 * Copyright (C) 2010 - Alexander van der Mey <alexvandermey@gmail.com>
 * All Rights reserved.
 */

#ifndef GUMMI_PREVIEW_H
#define GUMMI_PREVIEW_H
#include <gtk/gtk.h>

typedef struct _GuPreview {
    gint dummy;
} GuPreview;

GuPreview* preview_init(GtkBuilder * builder);
void preview_set_pdffile(gchar *pdffile);
gboolean on_expose(GtkWidget*, GdkEventExpose*, gpointer);

#endif /* GUMMI_PREVIEW_H */
