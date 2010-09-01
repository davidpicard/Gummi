/**
 * @file   preview.c
 * @brief  
 * @author Alexander van der Mey <alexvandermey@gmail.com>
 *
 * Copyright (C) 2010 - Alexander van der Mey <alexvandermey@gmail.com>
 * All Rights reserved.
 */

#include "preview.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <cairo.h>
#include <poppler.h> 

PopplerDocument* doc;
PopplerPage* page;
GtkWidget *drawarea;
int pagetotal;

GuPreview* preview_init(GtkBuilder * builder) {
    GuPreview* p = (GuPreview*)g_malloc(sizeof(GuPreview));
    GtkWidget *drawarea;
    
    GdkColor white = {0,0xffff,0xffff,0xffff};
    
    drawarea = GTK_WIDGET(gtk_builder_get_object(builder, "preview_draw"));
    gtk_widget_modify_bg (drawarea, GTK_STATE_NORMAL, &white); 

    g_signal_connect(GTK_OBJECT (drawarea), "expose-event",
            G_CALLBACK(on_expose), NULL); 
     

    return p;
}


void preview_set_pdffile(gchar *pdffile) {
    GError *err = NULL;
    gchar *uri = "file://";
    uri = g_strconcat(uri, pdffile, NULL);

    doc = poppler_document_new_from_file(uri, NULL, &err);
    pagetotal = poppler_document_get_n_pages(doc); 
    page = poppler_document_get_page(doc, 0);
}


gboolean on_expose(GtkWidget* w, GdkEventExpose* e, gpointer data) {
    cairo_t* cr;
    cr = gdk_cairo_create(w->window);
    
    // import python lines for calculating scale here
    cairo_scale(cr, 0.7, 0.7);
    
    
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, 0, 0, 500, 10);
    cairo_fill(cr);
    
    poppler_page_render(page, cr);
    cairo_destroy(cr);
    return FALSE;
} 
