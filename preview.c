/**
 * @file   preview.c
 * @brief  
 *
 * Copyright (C) 2010 Gummi-Dev Team <alexvandermey@gmail.com>
 * All Rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

gchar *uri;
int pagetotal;

GuPreview* preview_init(GtkBuilder * builder) {
    GuPreview* p = (GuPreview*)g_malloc(sizeof(GuPreview));
    GdkColor white = {0,0xffff,0xffff,0xffff};
    
    drawarea = GTK_WIDGET(gtk_builder_get_object(builder, "preview_draw"));
    gtk_widget_modify_bg (drawarea, GTK_STATE_NORMAL, &white); 

    g_signal_connect(GTK_OBJECT (drawarea), "expose-event",
            G_CALLBACK(on_expose), NULL); 
    return p;
}


void preview_set_pdffile(gchar *pdffile) {
    GError *err = NULL;
    
    uri = g_strconcat("file://", pdffile, NULL);
    doc = poppler_document_new_from_file(uri, NULL, &err);
    pagetotal = poppler_document_get_n_pages(doc); 
    page = poppler_document_get_page(doc, 0);
}

void preview_refresh() {
    GError *err = NULL;
    
    doc = poppler_document_new_from_file(uri, NULL, &err);
    pagetotal = poppler_document_get_n_pages(doc); 
    page = poppler_document_get_page(doc, 0);
    gtk_widget_queue_draw(drawarea);
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
