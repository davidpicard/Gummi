/**
 * @file   importer.h
 * @brief  
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#ifndef GUMMI_IMPORTER_H
#define GUMMI_IMPORTER_H

#include <glib.h>
#include <gtk/gtk.h>

#include "editor.h"

typedef struct _GuImporter {
    GtkNotebook* import_tabs;

    GtkViewport* image_pane;
    GtkEntry* image_file;
    GtkEntry* image_caption;
    GtkEntry* image_label;
    GtkSpinButton* image_scale;
    GtkAdjustment* scaler;

    GtkViewport* table_pane;
    GtkComboBox* table_comboalign;
    GtkComboBox* table_comboborder;
    GtkAdjustment* table_rows;
    GtkAdjustment* table_cols;

    GtkAdjustment* matrix_rows;
    GtkAdjustment* matrix_cols;
    GtkComboBox* matrix_combobracket;
} GuImporter;

GuImporter* importer_init(GtkBuilder* builder);
void importer_insert_table(GuImporter* ic, GuEditor* ec);
void importer_insert_matrix(GuImporter* ic, GuEditor* ec);
void importer_insert_image(GuImporter* ic, GuEditor* ec);
void importer_imagegui_set_sensitive(GuImporter* ic, const gchar* name,
        gboolean mode);
const gchar* importer_generate_table(GuImporter* ic);
const gchar* importer_generate_matrix(GuImporter* ic);
const gchar* importer_generate_image(GuImporter* ic);

#endif /* GUMMI_IMPORTER_H */
