/**
 * @file    gui.h
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


#ifndef GUMMI_GUI_H
#define GUMMI_GUI_H

#include <gtk/gtk.h>

#include "iofunctions.h"

#define g_e_buffer GTK_TEXT_BUFFER(gummi->editor->sourcebuffer)
#define g_e_view GTK_TEXT_VIEW(gummi->editor->sourceview)

typedef struct _GuPrefsGui {
    GtkWidget* prefwindow;
    GtkNotebook* notebook;
    GtkCheckButton* textwrap_button;
    GtkCheckButton* wordwrap_button;
    GtkCheckButton* line_numbers;
    GtkCheckButton* highlighting;
    GtkCheckButton* autosaving;
    GtkCheckButton* compile_status;
    GtkSpinButton* autosave_timer;
    GtkComboBox* combo_languages;
    GtkListStore* list_languages;
    GtkTextView* default_text;
    GtkTextBuffer* default_buffer;
    GtkComboBox* typesetter;
    GtkFontButton* editor_font;
    GtkComboBox* compile_scheme;
    GtkSpinButton* compile_timer;
    GtkImage* changeimg;
    GtkLabel* changelabel;

    GtkVBox* view_box;
    GtkHBox* editor_box;
    GtkHBox* compile_box;
} GuPrefsGui;

typedef struct _GuSearchGui  {
    GtkWidget* searchwindow;
    GtkEntry* searchentry;
    GtkEntry* replaceentry;
    gboolean backwards;
    gboolean matchcase;
    gboolean wholeword;
} GuSearchGui;

typedef struct _GuImportGui {
    GtkHBox* box_image;
    GtkHBox* box_table;
    GtkHBox* box_matrix;
    GtkViewport* image_pane;
    GtkViewport* table_pane;
    GtkViewport* matrix_pane;
} GuImportGui;

typedef struct _GummiGui {
    GuPrefsGui* prefsgui;
    GuSearchGui* searchgui;
    GuImportGui* importgui;

    GtkWidget *mainwindow;
    GtkTextBuffer *errorbuff;
    GtkHBox* rightpane;
    GtkStatusbar *statusbar;
    guint statusid;
} GummiGui;

/* Main GUI */
GummiGui* gui_init(GtkBuilder* builder);
void gui_main(GtkBuilder* builder);
void gui_quit(void);
void on_menu_new_activate(GtkWidget* widget, void* user);
void on_menu_open_activate(GtkWidget* widget, void* user);
void on_menu_save_activate(GtkWidget* widget, void* user);
void on_menu_saveas_activate(GtkWidget* widget, void* user);
void on_menu_find_activate(GtkWidget* widget, void* user);
void on_menu_cut_activate(GtkWidget* widget, void* user);
void on_menu_copy_activate(GtkWidget* widget, void* user);
void on_menu_paste_activate(GtkWidget* widget, void* user);
void on_menu_undo_activate(GtkWidget* widget, void* user);
void on_menu_redo_activate(GtkWidget* widget, void* user);
void on_tool_textstyle_bold_activate(GtkWidget* widget, void* user);
void on_tool_textstyle_italic_activate(GtkWidget* widget, void* user);
void on_tool_textstyle_underline_activate(GtkWidget* widget, void* user);
void on_tool_textstyle_left_activate(GtkWidget* widget, void* user);
void on_tool_textstyle_center_activate(GtkWidget* widget, void* user);
void on_tool_textstyle_right_activate(GtkWidget* widget, void* user);
void on_button_template_ok_clicked(GtkWidget* widget, void* user);
void on_button_template_cancel_clicked(GtkWidget* widget, void* user);
gboolean on_button_searchwindow_close_clicked(GtkWidget* widget, void* user);
void on_button_searchwindow_find_clicked(GtkWidget* widget, void* user);
void on_button_searchwindow_replace_next_clicked(GtkWidget* widget, void* user);
void on_button_searchwindow_replace_all_clicked(GtkWidget* widget, void* user);
void on_import_tabs_switch_page(GtkNotebook* notebook, GtkNotebookPage* page,
        guint page_num, void* user);

void preview_next_page(GtkWidget* widget, void* user);
void preview_prev_page(GtkWidget* widget, void* user);
void preview_zoom_change(GtkWidget* widget, void* user);

gchar* get_open_filename(const gchar* name, const gchar* filter);
gchar* get_save_filename(const gchar* name, const gchar* filter);
gboolean check_for_save(void);

void errorbuffer_set_text(gchar *message);
void statusbar_set_message(gchar* message);
gboolean statusbar_del_message(void* user);

/* Preference GUI */
GuPrefsGui* prefsgui_init(GummiGui* gui);
void prefsgui_main(void);
void prefsgui_set_current_settings(GuPrefsGui* prefs);

/* Search Window */
GuSearchGui* searchgui_init(GtkBuilder* builder);
void on_toggle_matchcase_toggled(GtkWidget* widget, void* user);
void on_toggle_wholeword_toggled(GtkWidget* widget, void* user);
void on_toggle_backwards_toggled(GtkWidget* widget, void* user);
void on_GuSearchGuiext_changed(GtkEditable* editable, void* user);

/* Import GUI */
GuImportGui* importgui_init(GtkBuilder* builder);
void on_button_import_table_apply_clicked(GtkWidget* widget, void* user);
void on_button_import_image_apply_clicked(GtkWidget* widget, void* user);
void on_button_import_matrix_apply_clicked(GtkWidget* widget, void* user);
void on_image_file_activate(void);

#endif /* GUMMI_GUI_H */
