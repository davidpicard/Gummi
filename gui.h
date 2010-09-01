#ifndef GUMMI_GUI_H
#define GUMMI_GUI_H

#include <gtk/gtk.h>

#include "iofunctions.h"

#define g_e_buffer GTK_TEXT_BUFFER(gummi->editor->sourcebuffer)
#define g_builder gummi->builder

typedef struct _searchgui  {
    GtkWidget* searchwindow;
    GtkEntry* searchentry;
    GtkEntry* replaceentry;
    gboolean backwards;
    gboolean matchcase;
    gboolean wholeword;
} searchgui_t;

typedef struct _importgui {
    GtkHBox* box_image;
    GtkHBox* box_table;
    GtkHBox* box_matrix;
    GtkViewport* image_pane;
    GtkViewport* table_pane;
    GtkViewport* matrix_pane;
} importgui_t;

/* Main GUI */
void gui_init(void);
void gui_main(void);
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

gchar* get_open_filename(const gchar* name, const gchar* filter);
gchar* get_save_filename(const gchar* name, const gchar* filter);
gboolean check_for_save(void);

void statusbar_set_message(gchar* message);
gboolean statusbar_del_message(void* user);

/* Search Window */
searchgui_t* searchgui_init(void);
void on_toggle_matchcase_toggled(GtkWidget* widget, void* user);
void on_toggle_wholeword_toggled(GtkWidget* widget, void* user);
void on_toggle_backwards_toggled(GtkWidget* widget, void* user);
void on_searchgui_text_changed(GtkEditable* editable, void* user);

/* Import GUI */
importgui_t* importgui_init(void);
void on_button_import_table_apply_clicked(GtkWidget* widget, void* user);
void on_button_import_image_apply_clicked(GtkWidget* widget, void* user);
void on_button_import_matrix_apply_clicked(GtkWidget* widget, void* user);
void on_image_file_activate(void);

#endif /* GUMMI_GUI_H */
