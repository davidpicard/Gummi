/**
 * @file   preview.c
 * @brief  
 *
 * Copyright (C) 2010 Gummi-Dev Team <alexvandermey@gmail.com>
 * All Rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
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
#include <math.h>

PopplerDocument* doc;
PopplerPage* page;
GtkWidget *drawarea;
GtkWidget *scrollw;

gchar *uri;
int pagetotal;

double page_scale;
double page_width;
double page_height;
double page_ratio;
gboolean fit_width = FALSE;
gboolean best_fit = TRUE;


GuPreview* preview_init(GtkBuilder * builder) {
    GuPreview* p = (GuPreview*)g_malloc(sizeof(GuPreview));
    GdkColor bg = {0,0xed00,0xec00,0xeb00};
    drawarea = GTK_WIDGET(gtk_builder_get_object(builder, "preview_draw"));
    scrollw = GTK_WIDGET(gtk_builder_get_object(builder, "preview_scroll"));
    gtk_widget_modify_bg (drawarea, GTK_STATE_NORMAL, &bg); 

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
    
    poppler_page_get_size(page, &page_width, &page_height);
    page_ratio = (page_width / page_height);
    page_scale = 1.0;
}

void preview_refresh() {
    GError *err = NULL;
    printf("refresh\n");
    doc = poppler_document_new_from_file(uri, NULL, &err);
    pagetotal = poppler_document_get_n_pages(doc); 
    page = poppler_document_get_page(doc, 0);
    gtk_widget_queue_draw(drawarea);
}


gboolean on_expose(GtkWidget* w, GdkEventExpose* e, gpointer data) {
    GtkAllocation scrollwsize;
    cairo_t* cr;
    cr = gdk_cairo_create(w->window);
    
    gtk_widget_get_allocation(scrollw, &scrollwsize);
    double scrollw_ratio = (scrollwsize.width / scrollwsize.height);
    
    // TODO: STOP WITH ERROR IF PAGE RATIO OR PAGE WIDTH IS NULL!
    
    if (best_fit == TRUE || fit_width == TRUE) {
        if (scrollw_ratio < page_ratio || fit_width == TRUE) {
            page_scale = scrollwsize.width / page_width;
        }
        else {
            page_scale = scrollwsize.height / page_height;
        }
    }
    
    if (best_fit == FALSE && fit_width == FALSE) {
        gtk_widget_set_size_request(drawarea, (page_width * page_scale), (page_height * page_scale));
    }
    else if (fit_width == TRUE) {
        if (fabs(page_ratio - scrollw_ratio) > 0.01) {
            gtk_widget_set_size_request(drawarea, -1, (page_height*page_scale));
        }
    }
    else if (best_fit == TRUE) {
        gtk_widget_set_size_request(drawarea, -1, (page_height*page_scale)-10);
    }
    
    // import python lines for calculating scale here
    cairo_scale(cr, page_scale, page_scale);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, 0, 0, page_width, page_height);
    cairo_fill(cr);
    
    poppler_page_render(page, cr);
    cairo_destroy(cr);
    return FALSE;
} 



