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

#include "environment.h"
#include "utils.h"



GuPreview* preview_init(GtkBuilder * builder) {
    GuPreview* p = (GuPreview*)g_malloc(sizeof(GuPreview));
    GdkColor bg = {0,0xed00,0xec00,0xeb00};
    p->drawarea = GTK_WIDGET(gtk_builder_get_object(builder, "preview_draw"));
    p->scrollw = GTK_WIDGET(gtk_builder_get_object(builder, "preview_scroll"));
    p->page_next = GTK_WIDGET(gtk_builder_get_object(builder, "page_next"));
    p->page_prev = GTK_WIDGET(gtk_builder_get_object(builder, "page_prev"));
    p->page_label = GTK_WIDGET(gtk_builder_get_object(builder, "page_label"));
    p->page_input = GTK_WIDGET(gtk_builder_get_object(builder, "page_input"));
    p->fit_width = TRUE;
    p->best_fit = FALSE;
    gtk_widget_modify_bg(p->drawarea, GTK_STATE_NORMAL, &bg); 

    g_signal_connect(GTK_OBJECT(p->drawarea), "expose-event",
            G_CALLBACK(on_expose), p); 
    return p;
}

void preview_set_pdffile(GuPreview* prev, const gchar *pdffile) {
    GError *err = NULL;
    prev->page_current = 0;
    
    prev->uri = g_strconcat("file://", pdffile, NULL);
    prev->doc = poppler_document_new_from_file(prev->uri, NULL, &err);
    prev->page = poppler_document_get_page(prev->doc, prev->page_current);
    poppler_page_get_size(prev->page, &prev->page_width, &prev->page_height);
    prev->page_total = poppler_document_get_n_pages(prev->doc);
    prev->page_ratio = (prev->page_width / prev->page_height);
    prev->page_scale = 1.0;
    preview_set_pagedata(prev);
}

void preview_refresh(GuPreview* prev) {
    GError *err = NULL;
    prev->doc = poppler_document_new_from_file(prev->uri, NULL, &err);
    prev->page_total = poppler_document_get_n_pages(prev->doc);
    preview_set_pagedata(prev);
    prev->page = poppler_document_get_page(prev->doc, prev->page_current);    
    gtk_widget_queue_draw(prev->drawarea);
}


void preview_set_pagedata(GuPreview* prev) {
    if ((prev->page_total - 1) > prev->page_current) {
        gtk_widget_set_sensitive(GTK_WIDGET(prev->page_next), TRUE);
    }
    else if (prev->page_current >= prev->page_total) {
        preview_goto_page(prev, prev->page_total -1);
    }
    char current[8];
    snprintf(current, sizeof current, "%d", (prev->page_current+1));
    char total[8];
    snprintf(total, sizeof total, "of %d", prev->page_total);

    gtk_entry_set_text(GTK_ENTRY(prev->page_input), current);
    gtk_label_set_text(GTK_LABEL(prev->page_label), total);
}

void preview_goto_page(GuPreview* prev, int page_number) {
    if (page_number < 0 || page_number >= prev->page_total)
        slog(L_ERROR, "page_number is a negative number!\n");

    prev->page_current = page_number;
    gtk_widget_set_sensitive(prev->page_prev, (page_number > 0));
    gtk_widget_set_sensitive(prev->page_next,
            (page_number < (prev->page_total -1)));
    preview_refresh(prev);
    // set label info
}

gboolean on_expose(GtkWidget* w, GdkEventExpose* e, GuPreview* prev) {
    GtkAllocation scrollwsize;
    cairo_t* cr;
    cr = gdk_cairo_create(w->window);
    
    gtk_widget_get_allocation(prev->scrollw, &scrollwsize);
    double scrollw_ratio = (scrollwsize.width / scrollwsize.height);
    
    // TODO: STOP WITH ERROR IF PAGE RATIO OR PAGE WIDTH IS NULL!
    
    if (prev->best_fit || prev->fit_width) {
        if (scrollw_ratio < prev->page_ratio || prev->fit_width) {
            prev->page_scale = scrollwsize.width / prev->page_width;
        }
        else {
            prev->page_scale = scrollwsize.height / prev->page_height;
        }
    }
    
    if (!prev->best_fit && !prev->fit_width) {
        gtk_widget_set_size_request(prev->drawarea, (prev->page_width *
                    prev->page_scale), (prev->page_height * prev->page_scale));
    }
    else if (prev->fit_width) {
        if (fabs(prev->page_ratio - scrollw_ratio) > 0.01) {
            gtk_widget_set_size_request(prev->drawarea, -1,
                    (prev->page_height*prev->page_scale));
        }
    }
    else if (prev->best_fit) {
        gtk_widget_set_size_request(prev->drawarea, -1,
                (prev->page_height*prev->page_scale)-10);
    }
    
    // import python lines for calculating scale here
    cairo_scale(cr, prev->page_scale, prev->page_scale);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_rectangle(cr, 0, 0, prev->page_width, prev->page_height);
    cairo_fill(cr);
    
    poppler_page_render(prev->page, cr);
    cairo_destroy(cr);
    return FALSE;
} 
