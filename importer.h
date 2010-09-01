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

typedef struct _importer {
    GtkNotebook* import_tab;

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
} importer_t;

importer_t* importer_init(GtkBuilder* builder);
void importer_insert_table(importer_t* ic, editor_t* ec);
void importer_insert_matrix(importer_t* ic, editor_t* ec);
void importer_insert_image(importer_t* ic, editor_t* ec);
void importer_imagegui_set_sensitive(importer_t* ic, const gchar* name,
        gboolean mode);
const gchar* importer_generate_table(importer_t* ic);
const gchar* importer_generate_matrix(importer_t* ic);
const gchar* importer_generate_image(importer_t* ic);

#endif /* GUMMI_IMPORTER_H */
