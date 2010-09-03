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

#include "utils.h"

PopplerDocument* doc;
PopplerPage* page;
GtkWidget *drawarea;
GtkWidget *page_next;
GtkWidget *page_prev;
GtkWidget *page_label;
GtkWidget *page_input;
GtkWidget *scrollw;

gchar *uri;

int page_total;
int page_current;
double page_scale;
double page_width;
double page_height;
double page_ratio;
gboolean fit_width = TRUE;
gboolean best_fit = FALSE;


GuPreview* preview_init(GtkBuilder * builder) {
    GuPreview* p = (GuPreview*)g_malloc(sizeof(GuPreview));
    GdkColor bg = {0,0xed00,0xec00,0xeb00};
    drawarea = GTK_WIDGET(gtk_builder_get_object(builder, "preview_draw"));
    scrollw = GTK_WIDGET(gtk_builder_get_object(builder, "preview_scroll"));
    page_next = GTK_WIDGET(gtk_builder_get_object(builder, "page_next"));
    page_prev = GTK_WIDGET(gtk_builder_get_object(builder, "page_prev"));
    page_label = GTK_WIDGET(gtk_builder_get_object(builder, "page_label"));
    page_input = GTK_WIDGET(gtk_builder_get_object(builder, "page_input"));
    gtk_widget_modify_bg (drawarea, GTK_STATE_NORMAL, &bg); 

    g_signal_connect(GTK_OBJECT (drawarea), "expose-event",
            G_CALLBACK(on_expose), NULL); 
    return p;
}


void preview_set_pdffile(gchar *pdffile) {
    GError *err = NULL;
    page_current = 0;
    
    uri = g_strconcat("file://", pdffile, NULL);
    doc = poppler_document_new_from_file(uri, NULL, &err);
    page = poppler_document_get_page(doc, page_current);
    poppler_page_get_size(page, &page_width, &page_height);
    page_total = poppler_document_get_n_pages(doc);
    page_ratio = (page_width / page_height);
    page_scale = 1.0;
    preview_set_pagedata();
}

void preview_refresh() {
    GError *err = NULL;
    doc = poppler_document_new_from_file(uri, NULL, &err);
    page_total = poppler_document_get_n_pages(doc);
    preview_set_pagedata();
    page = poppler_document_get_page(doc, page_current);    
    gtk_widget_queue_draw(drawarea);
}


void preview_set_pagedata() {
    if ((page_total - 1) > page_current) {
        gtk_widget_set_sensitive(GTK_WIDGET(page_next), TRUE);
    }
    else if (page_current >= page_total) {
        preview_goto_page(page_total -1);
    }
    char current[8];
    snprintf(current, sizeof current, "%d", (page_current+1));
    char total[8];
    snprintf(total, sizeof total, "of %d", page_total);

    gtk_entry_set_text(GTK_ENTRY(page_input), current);
    gtk_label_set_text(GTK_LABEL(page_label), total);
    
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

void preview_next_page(GtkWidget* widget, void* user) {
    preview_goto_page(page_current + 1);
}
    
void preview_prev_page(GtkWidget* widget, void* user) {
    preview_goto_page(page_current - 1);
}


void preview_goto_page(int page_number) {
    if (page_number < 0 || page_number >= page_total) {
        slog(L_ERROR, "page_number is a negative number!\n");
    }
    page_current = page_number;
    gtk_widget_set_sensitive(page_prev, (page_number > 0));
    gtk_widget_set_sensitive(page_next, (page_number < (page_total -1)));
    preview_refresh();
    // set label info
}


void preview_zoom_change(GtkWidget* widget, void* user) {
    gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    double opts[9] = {0.50, 0.70, 0.85, 1.0, 1.25, 1.5, 2.0, 3.0, 4.0}; 

    if (index < 0) slog(L_ERROR, "preview zoom level is < 0.\n");
    
    fit_width = best_fit = FALSE;
    if (index < 2) {
        if (index == 0) {
            best_fit = TRUE;
        }
        else if (index == 1) {
            fit_width = TRUE;
        }
    }
    else {
        page_scale = opts[index-2];
    }
    
    gtk_widget_queue_draw(drawarea);
}



