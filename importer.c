/**
 * @file   importer.c
 * @brief  
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#include "importer.h"

importer_t* importer_init(GtkBuilder* builder) {
    importer_t* i = (importer_t*)malloc(sizeof(importer_t));

    i->import_tab = gtk_builder_get_object(builder, "import_tab");

    i->image_pane = gtk_builder_get_object(builder, "image_pane");
    i->image_file = gtk_builder_get_object(builder, "image_file");
    i->image_caption = gtk_builder_get_object(builder, "image_caption");
    i->image_label = gtk_builder_get_object(builder, "image_label");
    i->image_scale = gtk_builder_get_object(builder, "image_scale");
    i->image_scaler = gtk_builder_get_object(builder, "image_scaler");

    i->table_pane = gtk_builder_get_object(builder, "table_pane");
    i->table_comboalign = gtk_builder_get_object(builder, "table_comboalign");
    i->table_comboborder = gtk_builder_get_object(builder, "table_comboborder");
    i->table_rows = gtk_builder_get_object(builder, "table_rows");
    i->table_cols = gtk_builder_get_object(builder, "table_cols");

    i->matrix_rows = gtk_builder_get_object(builder, "table_rows");
    i->matrix_cols = gtk_builder_get_object(builder, "table_cols");
    i->matrix_combobracket =gtk_builder_get_object(builder,"table_combobracket");
    gtk_adjustment_set_value(i->table_cols, 3);
    gtk_adjustment_set_value(i->table_rows, 3);
    gtk_adjustment_set_value(i->matrix_cols, 3);
    gtk_adjustment_set_value(i->matrix_rows, 3);
    return i;
}
