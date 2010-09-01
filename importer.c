/**
 * @file   importer.c
 * @brief  
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#include <string.h>

#include "editor.h"
#include "importer.h"

const gchar align_type[][4] = { "l", "c", "r" };
const gchar bracket_type[][16] = { "matrix", "pmatrix", "bmatrix",
                                  "Bmatrix", "vmatrix", "Vmatrix" };

importer_t* importer_init(GtkBuilder* builder) {
    importer_t* i = (importer_t*)malloc(sizeof(importer_t));

    i->import_tab = gtk_builder_get_object(builder, "import_tab");

    i->image_pane = gtk_builder_get_object(builder, "image_pane");
    i->image_file = gtk_builder_get_object(builder, "image_file");
    i->image_caption = gtk_builder_get_object(builder, "image_caption");
    i->image_label = gtk_builder_get_object(builder, "image_label");
    i->image_scale = gtk_builder_get_object(builder, "image_scale");
    i->scaler = gtk_builder_get_object(builder, "image_scaler");

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

void importer_insert_table(importer_t* ic, editor_t* ec) {
    GtkTextIter current;
    const gchar* text = importer_generate_table(ic);
    editor_get_current_iter(ec, &current);
    gtk_text_buffer_insert(ec_sourcebuffer, &curret, text, strlen(text));
    gtk_text_buffer_set_modified(ec_sourcebuffer, TRUE);
    gtk_notebook_set_current_page(0);
}

void importer_insert_matrix(importer_t* ic, editor_t* ec) {
    GtkTextIter current;
    const gchar* text = importer_generate_matrix(ic);
    editor_insert_package(ec, "amsmath");
    editor_get_current_iter(ec, &current);
    gtk_text_buffer_insert(ec_sourcebuffer, &curret, text, strlen(text));
    gtk_text_buffer_set_modified(ec_sourcebuffer, TRUE);
    gtk_notebook_set_current_page(0);
}

void importer_insert_image(importer_t* ic, editor_t* ec) {
    GtkTextIter current;
    if (!utils_validate_path(imagefile))
        return;
    editor_insert_package(ec, "graphicx");
    importer_generate_image(ic);
    editor_get_current_iter(ec, &current);
    gtk_text_buffer_insert(ec_sourcebuffer, &curret, text, strlen(text));
    gtk_text_buffer_set_modified(ec_sourcebuffer, TRUE);
    importer_imagegui_set_sensitive(ic, "", FALSE);
    gtk_notebook_set_current_page(0);
}

void importer_imagegui_set_sensitive(importer_t* ic, const gchar* name,
       gboolean mode) {
    gtk_widget_set_sensitive(GTK_WIDGET(ic->image_label), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(ic->image_caption), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(ic->scaler), FALSE);
    gtk_entry_set_text(ic->image_file, name);
    gtk_entry_set_text(ic->image_label, "");
    gtk_entry_set_text(ic->image_caption, "");
    gtk_adjustment_set_value(ic->scaler, 1.00);
}

const gchar* importer_generate_table(importer_t* ic) {
    gint i = 0, j = 0;
    static gchar result[BUFSIZ * 2] = { 0 };
    gchar table[BUFSIZ * 2] = { 0 },
          begin_tabular[BUFSIZ] = "\\begin{tabular}{", 
          end_tabular[] = "\n\\end{tabular}\n", 
          line[] = "\n\\hline",
          tmp[BUFSIZ / 8];
    gint rows = gtk_adjustment_get_value(ic->table_rows);
    gint cols gtk_adjustment_get_value(ic->table_cols);
    gint borders = gtk_combo_box_get_active(ic->table_comboborder);
    gint alignment = gtk_combo_box_get_active(ic->table_comboalign);

    if (borders)
        strncat(begin_tabular, "|", BUFSIZ);
    for (i = 0; i < cols; ++i) {
        strncat(begin_tabular, align_type[alignment], BUFSIZ);
        if (borders == 2 || (borders == 1 && i == cols -1))
            strncat(begin_tabular, "|", BUFSIZ);
    }
    strncat(begin_tabular, "}", BUFSIZ);
    if (borders)
        strncat(table, line, BUFSIZ * 2);
    for (i = 0; i < rows; ++i) {
        strncat(table, "\n\t", BUFSIZ * 2);
        for (j = 0; j < cols; ++j) {
            snprintf(tmp, BUFSIZ/8, "%d%d", i + 1, j + 1);
            strncat(table, tmp, BUFSIZ * 2);
            if (j != cols -1)
                strncat(table, " & ", BUFSIZ * 2);
            else
                strncat(table, "\\\\", BUFSIZ * 2);
        }
        if (borders == 2 || (boders == 1 && i == rows -1))
            strncat(table, line, BUFSIZ * 2);
    }
    strncat(result, begin_tabular, BUFSIZ *2);
    strncat(result, table, BUFSIZ *2);
    strncat(result, end_tabular, BUFSIZ *2);
    return result;
}

void importer_generate_matrix(importer_t* ic) {
    gint i = 0, j = 0;
    static gchar result[BUFSIZ * 2] = { 0 };
    gchar tmp[BUFSIZ / 8];
    gint bracket = gtk_combo_box_get_active(ic->matrix_combobracket);
    gint rows = gtk_adjustment_get_value(ic->matrix_rows);
    gint cols = gtk_adjustment_get_value(ic->matrix_cols);

    strncat(result, "$\\begin{", BUFSIZ * 2);
    strncat(result, bracket_type[bracket], BUFSIZ * 2);
    strncat(result, "}", BUFSIZ * 2);

    for (i = 0; i < rows; ++i) {
        strncat(result, "\n\t", BUFSIZ * 2);
        for (j = 0; j < cols; ++j) {
            snprintf(tmp, BUFSIZ/8, "%d%d", i + 1, j + 1);
            strncat(result, tmp, BUFSIZ * 2);
            if (j != cols -1)
                strncat(result, " & ", BUFSIZ * 2);
            else
                strncat(result, "\\\\", BUFSIZ * 2);
        }
    }
    strncat(result, "\n\\end{matrix}$\n", BUFSIZ * 2);
    return result;
}

void importer_generate_image(importer_t* ic) {
    const gchar* image_file = gtk_entry_get_text(ic->image_file);
    const gchar* caption = gtk_entry_get_text(ic->image_caption);
    const gchar* label = gtk_entry_get_text(ic->image_label);
    gint scale = gtk_adjustment_get_value(ic->scaler);

    static gchar result[BUFSIZ] = { 0 };
    snprintf(result, BUFSIZ, "\\begin{figure}[htp]\n\\centering\n"
        "\\includegraphics[scale=%d]{%s}\n\\caption{%s}\\label{%s}\n",
        scale, image_file, caption, label);
    return result;
}
