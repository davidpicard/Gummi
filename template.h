/**
 * @file   template.h
 * @brief  
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#ifndef GUMMI_TEMPLATE_H
#define GUMMI_TEMPLATE_H

#include <glib.h>
#include <gtk/gtk.h>

typedef struct _template {
    GtkWindow* templatewindow;
    GtkIconView* iconview;
    GtkButton* template_ok;
} template_t;

template_t* template_init(GtkBuilder* builder);
void template_update_window(GdkEvent* event, void* button);
const gchar* template_get(template_t* templ);

#endif /* GUMMI_TEMPLATE_H */
