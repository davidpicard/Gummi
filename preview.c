/**
 * @file   preview.c
 * @brief  
 * @author Alexander van der Mey <alexvandermey@gmail.com>
 *
 * Copyright (C) 2010 - Alexander van der Mey <alexvandermey@gmail.com>
 * All Rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <poppler.h> 

#include "preview.h"

PopplerDocument* doc;
PopplerPage* page;

preview_t* preview_init(GtkBuilder * builder) {
    preview_t* p = (preview_t*)malloc(sizeof(preview_t));
    GtkWidget *drawarea;
    GError *err = NULL;
    GdkColor white = {0,0xffff,0xffff,0xffff};
    
    drawarea = GTK_WIDGET(gtk_builder_get_object(builder, "preview_draw"));
    gtk_widget_modify_bg (drawarea, GTK_STATE_NORMAL, &white); 

        doc = poppler_document_new_from_file("file:///home/alexander/Desktop/test.pdf", NULL, &err);
    page = poppler_document_get_page(doc, 0); 
    g_signal_connect(GTK_OBJECT (drawarea), "expose-event",
            G_CALLBACK(on_expose), NULL);
    return p;
}



gboolean on_expose(GtkWidget* w, GdkEventExpose* e, gpointer data) {
    cairo_t* cr;
    cr = gdk_cairo_create(w->window);
    poppler_page_render(page, cr);
    cairo_destroy(cr);
    return FALSE;
} 


