/**
 * @file    gui.c
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


#include "gui.h"

#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#   include <unistd.h>
#endif

#include <glib.h>

#include "biblio.h"
#include "configfile.h"
#include "editor.h"
#include "environment.h"
#include "importer.h"
#include "updatecheck.h"
#include "utils.h"

extern Gummi* gummi;

/* Many of the functions in this file are based on the excellent GTK+
 * tutorials written by Micah Carrick that can be found on: 
 * http://www.micahcarrick.com/gtk-glade-tutorial-part-3.html */

GummiGui* gui_init(GtkBuilder* builder) {
    GtkWidget    *hpaned;
    GtkWidget    *errortext;
    gint          width, height;

    GummiGui* g = (GummiGui*)g_malloc(sizeof(GummiGui));
    
    errortext = GTK_WIDGET(gtk_builder_get_object(builder, "errorfield"));
    g->mainwindow =
        GTK_WIDGET(gtk_builder_get_object(builder, "mainwindow"));
    g->toolbar =
        GTK_HBOX(gtk_builder_get_object(builder, "toolbar"));
    g->statusbar =
        GTK_STATUSBAR(gtk_builder_get_object(builder, "statusbar"));
    g->rightpane =
        GTK_VBOX(gtk_builder_get_object(builder, "rightpanebox"));
    g->previewoff = GTK_TOGGLE_TOOL_BUTTON(
            gtk_builder_get_object(builder, "tool_previewoff"));
    g->errorbuff =
        gtk_text_view_get_buffer(GTK_TEXT_VIEW(errortext));
    g->menu_spelling =
        GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "menu_spelling"));
    g->menu_toolbar =
        GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "menu_toolbar"));
    g->menu_statusbar =
        GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "menu_statusbar"));
    g->menu_rightpane =
        GTK_CHECK_MENU_ITEM(gtk_builder_get_object(builder, "menu_rightpane"));
    g->statusid =
        gtk_statusbar_get_context_id(GTK_STATUSBAR(g->statusbar), "Gummi");

    g->prefsgui = prefsgui_init(g);
    g->searchgui = searchgui_init(builder);
    g->importgui = importgui_init(builder);
    
    PangoFontDescription* font_desc = 
        pango_font_description_from_string("Monospace 8");
    gtk_widget_modify_font(errortext, font_desc);
    pango_font_description_free(font_desc);
    gtk_window_get_size(GTK_WINDOW(g->mainwindow), &width, &height);
    
    hpaned= GTK_WIDGET(gtk_builder_get_object(builder, "hpaned"));
    gtk_paned_set_position(GTK_PANED(hpaned), (width/2)); 

#ifndef USE_GTKSPELL
    gtk_widget_set_sensitive(GTK_WIDGET(g->menu_spelling), FALSE);
#endif
    if (config_get_value("toolbar")) {
        gtk_check_menu_item_set_active(g->menu_toolbar, TRUE);
        gtk_widget_show(GTK_WIDGET(g->toolbar));
    }

    if (config_get_value("statusbar")) {
        gtk_check_menu_item_set_active(g->menu_statusbar, TRUE);
        gtk_widget_show(GTK_WIDGET(g->statusbar));
    }

    if (config_get_value("rightpane")) {
        gtk_check_menu_item_set_active(g->menu_rightpane, TRUE);
        gtk_widget_show(GTK_WIDGET(g->rightpane));
    } else {
        gtk_toggle_tool_button_set_active(g->previewoff, TRUE);
    }

    return g;
}

void gui_main(GtkBuilder* builder) {
    gtk_builder_connect_signals(builder, NULL);       
    gtk_widget_show_all(gummi->gui->mainwindow);
    gtk_main();
}

void gui_quit() {
    if (check_for_save())
      on_menu_save_activate(NULL, NULL);  
    gtk_main_quit();
}

void on_menu_new_activate(GtkWidget *widget, void* user) {
    const char *text;
    
    if (check_for_save ())
        on_menu_save_activate(NULL, NULL);  
    /* clear editor for a new file */
    text = config_get_value("welcome");
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(g_e_buffer), text, -1);
    gtk_text_buffer_set_modified(g_e_buffer, FALSE);
    motion_create_environment(gummi->motion, NULL);
}

void on_menu_template_activate(GtkWidget *widget, void * user) {
    gtk_widget_show_all(GTK_WIDGET(gummi->templ->templatewindow));
}

void on_menu_exportpdf_activate(GtkWidget *widget, void * user) {
    // insert contents
}

void on_menu_recent_activate(GtkWidget *widget, void * user) {
    // insert contents
}

void on_menu_open_activate(GtkWidget *widget, void* user) {
    gchar       *filename;
    
    if (check_for_save() == TRUE) {
        on_menu_save_activate(NULL, NULL);  
    }
    filename = get_open_filename("Tex files", "txt/*");
    if (filename != NULL) 
        iofunctions_load_file(gummi->editor, filename); 
}

void on_menu_save_activate(GtkWidget *widget, void* user) {
    gchar* filename = NULL;
    if (!gummi->motion->filename)
        filename = get_save_filename("Tex files", "txt/*");
    if (filename) {
        motion_set_filename(gummi->motion, filename);
        iofunctions_write_file(gummi->editor, filename); 
    }
}

void on_menu_saveas_activate(GtkWidget *widget, void* user) {
    gchar* filename = NULL;
    if (!gummi->motion->filename)
        filename = get_save_filename("Tex files", "txt/*");
    if (filename) {
        iofunctions_write_file(gummi->editor, filename); 
        motion_create_environment(gummi->motion, filename);
    }
}

void on_menu_cut_activate(GtkWidget *widget, void* user) {
    GtkClipboard     *clipboard;
    
    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_cut_clipboard(g_e_buffer, clipboard, TRUE);
    gtk_text_buffer_set_modified(g_e_buffer, TRUE);
}

void on_menu_copy_activate(GtkWidget *widget, void* user) {
    GtkClipboard     *clipboard;
    
    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_copy_clipboard(g_e_buffer, clipboard);
}
void on_menu_paste_activate(GtkWidget *widget, void* user) {
    GtkClipboard     *clipboard;
    
    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_text_buffer_paste_clipboard(g_e_buffer, clipboard, NULL, TRUE);
    gtk_text_buffer_set_modified(g_e_buffer, TRUE);
}

void on_menu_undo_activate(GtkWidget *widget, void* user) {
    editor_undo_change(gummi->editor);
}

void on_menu_redo_activate(GtkWidget *widget, void* user) {
    editor_redo_change(gummi->editor);
}

void on_menu_delete_activate(GtkWidget *widget, void * user) {
    gtk_text_buffer_delete_selection(g_e_buffer, FALSE, TRUE);
}

void on_menu_selectall_activate(GtkWidget *widget, void * user) {
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(g_e_buffer, &start, &end);
    gtk_text_buffer_select_range(g_e_buffer, &start, &end);
}

void on_menu_preferences_activate(GtkWidget *widget, void * user) {
    prefsgui_main();
}

void on_menu_statusbar_toggled(GtkWidget *widget, void * user) {
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
        gtk_widget_show(GTK_WIDGET(gummi->gui->statusbar));
        config_set_value("statusbar", "True");
    } else {
        gtk_widget_hide(GTK_WIDGET(gummi->gui->statusbar));
        config_set_value("statusbar", "False");
    }
}

void on_menu_toolbar_toggled(GtkWidget *widget, void * user) {
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
        gtk_widget_show(GTK_WIDGET(gummi->gui->toolbar));
        config_set_value("toolbar", "True");
    } else {
        gtk_widget_hide(GTK_WIDGET(gummi->gui->toolbar));
        config_set_value("toolbar", "False");
    }
}

void on_menu_rightpane_toggled(GtkWidget *widget, void * user) {
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
        gtk_widget_show(GTK_WIDGET(gummi->gui->rightpane));
        config_set_value("rightpane", "True");
    } else {
        gtk_widget_hide(GTK_WIDGET(gummi->gui->rightpane));
        config_set_value("rightpane", "False");
    }
}

void on_menu_fullscreen_toggled(GtkWidget *widget, void * user) {
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget)))
        gtk_window_fullscreen(GTK_WINDOW(gummi->gui->mainwindow));
    else
        gtk_window_unfullscreen(GTK_WINDOW(gummi->gui->mainwindow));
}

void on_menu_find_activate(GtkWidget *widget, void* user) {
    gtk_entry_set_text(gummi->gui->searchgui->searchentry, "");
    gtk_entry_set_text(gummi->gui->searchgui->replaceentry, "");
    gtk_widget_grab_focus(GTK_WIDGET(gummi->gui->searchgui->searchentry));
    gtk_widget_show_all(GTK_WIDGET(gummi->gui->searchgui->searchwindow));
}

void on_menu_findnext_activate(GtkWidget *widget, void * user) {
    editor_jumpto_search_result(gummi->editor, 1);
}

void on_menu_findprev_activate(GtkWidget *widget, void * user) {
    editor_jumpto_search_result(gummi->editor, -1);
}

void on_menu_bibload_activate(GtkWidget *widget, void * user) {
    // insert contents
}

void on_menu_bibupdate_activate(GtkWidget *widget, void * user) {
    // insert contents
}

void on_menu_docstat_activate(GtkWidget *widget, void * user) {
    // insert contents
}

#ifdef USE_GTKSPELL
void on_menu_spelling_toggled(GtkWidget *widget, void * user) {
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
        editor_activate_spellchecking(gummi->editor, TRUE);
        config_set_value("spelling", "True");
    } else {
        editor_activate_spellchecking(gummi->editor, FALSE);
        config_set_value("spelling", "False");
    }
}
#endif

void on_menu_update_activate(GtkWidget *widget, void * user) {
    gboolean ret = updatecheck();
    if (!ret)
        slog(L_G_ERROR, "Update check failed!\n");
}

void on_menu_about_activate(GtkWidget *widget, void * user) {
    const gchar* authors[] = { "Alexander van der Mey\n"
        "<alexvandermey@gmail.com>",
        "Wei-Ning Huang\n"
        "<aitjcize@gmail.com>\n",
        "Contributors:",
        "Thomas van der Burgt",
        "Cameron Grout", NULL };
    const gchar* artists[] = {"Template icon set from:\n"
        "http://www.fatcow.com/free-icons/",
        "Windows version Icon set from Elemetary Project:\n"
        "http://www.elementary-project.com/", NULL};

    GtkAboutDialog* dialog = GTK_ABOUT_DIALOG(gtk_about_dialog_new());
    gtk_window_set_transient_for(GTK_WINDOW(dialog),
                                 GTK_WINDOW(gummi->gui->mainwindow));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
    gtk_about_dialog_set_authors(dialog, authors);
    gtk_about_dialog_set_program_name(dialog, PACKAGE_NAME);
    gtk_about_dialog_set_version(dialog, PACKAGE_VERSION);
    gtk_about_dialog_set_website(dialog, PACKAGE_URL);
    gtk_about_dialog_set_copyright(dialog, PACKAGE_COPYRIGHT);
    gtk_about_dialog_set_license(dialog, PACKAGE_LICENSE);
    //gtk_about_dialog_set_logo_icon_name(dialog, GUMMI_ICON)
    gtk_about_dialog_set_comments(dialog, PACKAGE_COMMENTS);
    gtk_about_dialog_set_artists(dialog, artists);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void on_tool_previewoff_toggled(GtkWidget *widget, void * user) {
    gboolean value =
        gtk_toggle_tool_button_get_active(GTK_TOGGLE_TOOL_BUTTON(widget));
    config_set_value("compile_status", (!value)?"Ture":"False");
    if (value)
        motion_stop_updatepreview(gummi->motion);
    else
        motion_start_updatepreview(gummi->motion);
}

void on_tool_textstyle_bold_activate(GtkWidget* widget, void* user) {
    editor_set_selection_textstyle(gummi->editor, "tool_bold");
}

void on_tool_textstyle_italic_activate(GtkWidget* widget, void* user) {
    editor_set_selection_textstyle(gummi->editor, "tool_italic");
}

void on_tool_textstyle_underline_activate(GtkWidget* widget, void* user) {
    editor_set_selection_textstyle(gummi->editor, "tool_unline");
}

void on_tool_textstyle_left_activate(GtkWidget* widget, void* user) {
    editor_set_selection_textstyle(gummi->editor, "tool_left");
}

void on_tool_textstyle_center_activate(GtkWidget* widget, void* user) {
    editor_set_selection_textstyle(gummi->editor, "tool_center");
}

void on_tool_textstyle_right_activate(GtkWidget* widget, void* user) {
    editor_set_selection_textstyle(gummi->editor, "tool_right");
}

void on_button_template_ok_clicked(GtkWidget* widget, void* user) {
    const gchar* text = template_get(gummi->templ);
    if (text) {
        editor_fill_buffer(gummi->editor, text);
        motion_create_environment(gummi->motion, NULL);
        gtk_widget_hide(GTK_WIDGET(gummi->templ->templatewindow));
    }
}

void on_button_template_cancel_clicked(GtkWidget* widget, void* user) {
    gtk_widget_hide(GTK_WIDGET(gummi->templ->templatewindow));
}

gboolean on_button_searchwindow_close_clicked(GtkWidget* widget, void* user) {
    gtk_widget_hide(GTK_WIDGET(gummi->gui->searchgui->searchwindow));
    return TRUE;
}

void on_button_searchwindow_find_clicked(GtkWidget* widget, void* user) {
    editor_start_search(gummi->editor,
        gtk_entry_get_text(gummi->gui->searchgui->searchentry),
        gummi->gui->searchgui->backwards,
        gummi->gui->searchgui->wholeword,
        gummi->gui->searchgui->matchcase,
        0
    );
}

void on_button_searchwindow_replace_next_clicked(GtkWidget* widget, void* user)
{
    editor_start_replace_next(gummi->editor,
        gtk_entry_get_text(gummi->gui->searchgui->searchentry),
        gtk_entry_get_text(gummi->gui->searchgui->replaceentry),
        gummi->gui->searchgui->backwards,
        gummi->gui->searchgui->wholeword,
        gummi->gui->searchgui->matchcase
    );
}

void on_button_searchwindow_replace_all_clicked(GtkWidget* widget, void* user) {
    editor_start_replace_all(gummi->editor,
        gtk_entry_get_text(gummi->gui->searchgui->searchentry),
        gtk_entry_get_text(gummi->gui->searchgui->replaceentry),
        gummi->gui->searchgui->backwards,
        gummi->gui->searchgui->wholeword,
        gummi->gui->searchgui->matchcase
    );
}

GuImportGui* importgui_init(GtkBuilder* builder) {
    GuImportGui* i = (GuImportGui*)g_malloc(sizeof(GuImportGui));
    i->box_image =
        GTK_HBOX(gtk_builder_get_object(builder, "box_image"));
    i->box_table =
        GTK_HBOX(gtk_builder_get_object(builder, "box_table"));
    i->box_matrix =
        GTK_HBOX(gtk_builder_get_object(builder, "box_matrix"));
    i->image_pane =
        GTK_VIEWPORT(gtk_builder_get_object(builder, "image_pane"));
    i->table_pane =
        GTK_VIEWPORT(gtk_builder_get_object(builder, "table_pane"));
    i->matrix_pane =
        GTK_VIEWPORT(gtk_builder_get_object(builder, "matrix_pane"));
    return i;
}

void on_button_import_table_apply_clicked(GtkWidget* widget, void* user) {
    importer_insert_table(gummi->importer, gummi->editor);
}

void on_button_import_image_apply_clicked(GtkWidget* widget, void* user) {
    importer_insert_image(gummi->importer, gummi->editor);
}

void on_button_import_matrix_apply_clicked(GtkWidget* widget, void* user) {
    importer_insert_matrix(gummi->importer, gummi->editor);
}

void on_image_file_activate(void) {
    const gchar* filename = get_open_filename("Image files", "image/*");
    importer_imagegui_set_sensitive(gummi->importer, filename, TRUE);
}

void on_import_tabs_switch_page(GtkNotebook* notebook, GtkNotebookPage* page,
        guint page_num, void* user) {
    GList* list = NULL;
    list = gtk_container_get_children(
            GTK_CONTAINER(gummi->gui->importgui->box_image));
    while (list) {
        gtk_container_remove(GTK_CONTAINER(gummi->gui->importgui->box_image),
                GTK_WIDGET(list->data));
        list = list->next;
    }
    list = gtk_container_get_children(
            GTK_CONTAINER(gummi->gui->importgui->box_table));
    while (list) {
        gtk_container_remove(GTK_CONTAINER(gummi->gui->importgui->box_table),
                GTK_WIDGET(list->data));
        list = list->next;
    }
    list = gtk_container_get_children(
            GTK_CONTAINER(gummi->gui->importgui->box_matrix));
    while (list) {
        gtk_container_remove(GTK_CONTAINER(gummi->gui->importgui->box_matrix),
                GTK_WIDGET(list->data));
        list = list->next;
    }

    switch (page_num) {
        case 1:
            gtk_container_add(GTK_CONTAINER(gummi->gui->importgui->box_image),
                    GTK_WIDGET(gummi->gui->importgui->image_pane));
            break;
        case 2:
            gtk_container_add(GTK_CONTAINER(gummi->gui->importgui->box_table),
                    GTK_WIDGET(gummi->gui->importgui->table_pane));
            break;
        case 3:
            gtk_container_add(GTK_CONTAINER(gummi->gui->importgui->box_matrix),
                    GTK_WIDGET(gummi->gui->importgui->matrix_pane));
            break;
    }
}

void on_bibcolumn_clicked(GtkWidget* widget, void* user) {
    gint id = gtk_tree_view_column_get_sort_column_id
        (GTK_TREE_VIEW_COLUMN(widget));
    gtk_tree_view_column_set_sort_column_id
        (GTK_TREE_VIEW_COLUMN(widget), id);
}

void on_bibcompile_clicked(GtkWidget* widget, void* user) {
    g_timeout_add_seconds(10, on_bibprogressbar_update, NULL);
    if (compile_bibliography(gummi->motion)) {
        
    }

}

void on_bibrefresh_clicked(GtkWidget* widget, void* user) {
}

void on_bibreference_clicked(GtkWidget* widget, void* user) {
}

gboolean on_bibprogressbar_update() {
    return FALSE;
}

/*
	def on_bibprogressbar_update(self):
		self.bibprogressmon.set_value(self.bibprogressval)
		self.bibprogressval = self.bibprogressval + 1
		if self.bibprogressval > 60:
			return False
		else:
			return True */

void preview_next_page(GtkWidget* widget, void* user) {
    preview_goto_page(gummi->preview, gummi->preview->page_current + 1);
}
    
void preview_prev_page(GtkWidget* widget, void* user) {
    preview_goto_page(gummi->preview, gummi->preview->page_current - 1);
}

void preview_zoom_change(GtkWidget* widget, void* user) {
    gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    double opts[9] = {0.50, 0.70, 0.85, 1.0, 1.25, 1.5, 2.0, 3.0, 4.0}; 

    if (index < 0) slog(L_ERROR, "preview zoom level is < 0.\n");
    
    gummi->preview->fit_width = gummi->preview->best_fit = FALSE;
    if (index < 2) {
        if (index == 0) {
            gummi->preview->best_fit = TRUE;
        }
        else if (index == 1) {
            gummi->preview->fit_width = TRUE;
        }
    }
    else {
        gummi->preview->page_scale = opts[index-2];
    }
    
    gtk_widget_queue_draw(gummi->preview->drawarea);
}

gboolean check_for_save() {
    gboolean      ret = FALSE;
    
    if (gtk_text_buffer_get_modified (g_e_buffer)) {
        /* we need to prompt for save */    
        GtkWidget       *dialog;

        const gchar *msg  = "Do you want to save the changes you have made?";

        dialog = gtk_message_dialog_new (NULL, 
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                GTK_MESSAGE_QUESTION,
                GTK_BUTTONS_YES_NO,
                "%s", msg);

        gtk_window_set_title (GTK_WINDOW (dialog), "Save?");
        if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_NO) {
            ret = FALSE;
        }      
        else ret = TRUE;

        gtk_widget_destroy (dialog);      
    }
    return ret;
}

gchar* get_open_filename(const gchar* name, const gchar* filter) {
    GtkWidget   *chooser;
    gchar       *filename = NULL;
       
    chooser = gtk_file_chooser_dialog_new ("Open File...",
           GTK_WINDOW (gummi->gui->mainwindow),
           GTK_FILE_CHOOSER_ACTION_OPEN,
           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
           GTK_STOCK_OPEN, GTK_RESPONSE_OK,
           NULL);
           
    if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK) {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    }
    
    gtk_widget_destroy (chooser);
    return filename;
}

gchar* get_save_filename(const gchar* name, const gchar* filter) {
    GtkWidget       *chooser;
    gchar           *filename=NULL;
        
    chooser = gtk_file_chooser_dialog_new ("Save File...",
                           GTK_WINDOW (gummi->gui->mainwindow),
                           GTK_FILE_CHOOSER_ACTION_SAVE,
                           GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                           GTK_STOCK_SAVE, GTK_RESPONSE_OK,
                           NULL);
                           
    if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
    {
        filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
    }
    
    gtk_widget_destroy (chooser);
    return filename;
}

void errorbuffer_set_text(gchar *message) {
    gtk_text_buffer_set_text(gummi->gui->errorbuff, message, -1);
}

void statusbar_set_message(gchar *message) {
    gtk_statusbar_push (GTK_STATUSBAR(gummi->gui->statusbar),
            gummi->gui->statusid, message);
    g_timeout_add_seconds(4, statusbar_del_message, NULL);
}

gboolean statusbar_del_message(void* user) {
    gtk_statusbar_pop(GTK_STATUSBAR(gummi->gui->statusbar),
            gummi->gui->statusid);
    return FALSE;
}

GuPrefsGui* prefsgui_init(GummiGui* gui) {
    GuPrefsGui* p = (GuPrefsGui*)g_malloc(sizeof(GuPrefsGui));
    GtkBuilder* builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "gui/prefs.glade", NULL);
    gtk_builder_set_translation_domain(builder, PACKAGE);

    p->prefwindow =
        GTK_WIDGET(gtk_builder_get_object(builder, "prefwindow"));
    p->notebook =
        GTK_NOTEBOOK(gtk_builder_get_object(builder, "notebook1"));
    p->textwrap_button =
        GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "textwrapping"));
    p->wordwrap_button =
        GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "wordwrapping"));
    p->line_numbers =
        GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "line_numbers")); 
    p->highlighting =
        GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "highlighting")); 
    p->autosaving =
        GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "autosaving")); 
    p->compile_status =
        GTK_CHECK_BUTTON(gtk_builder_get_object(builder, "compile_status")); 
    p->autosave_timer =
        GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "autosave_timer"));
    p->combo_languages =
        GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_languages"));
    p->list_languages =
        GTK_LIST_STORE(gtk_builder_get_object(builder, "list_languages"));
    p->default_text =
        GTK_TEXT_VIEW(gtk_builder_get_object(builder, "default_text"));
    p->default_buffer = 
        gtk_text_view_get_buffer(p->default_text);
    p->typesetter =
        GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_typesetter"));
    p->editor_font =
        GTK_FONT_BUTTON(gtk_builder_get_object(builder, "editor_font"));
    p->compile_scheme =
        GTK_COMBO_BOX(gtk_builder_get_object(builder, "combo_compilescheme"));
    p->compile_timer =
        GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "compile_timer"));

    p->view_box =
        GTK_VBOX(gtk_builder_get_object(builder, "view_box"));
    p->editor_box =
        GTK_HBOX(gtk_builder_get_object(builder, "editor_box"));
    p->compile_box =
        GTK_HBOX(gtk_builder_get_object(builder, "compile_box"));

    p->changeimg =
        GTK_IMAGE(gtk_builder_get_object(builder, "changeimg"));
    p->changelabel =
        GTK_LABEL(gtk_builder_get_object(builder, "changelabel"));

    gtk_window_set_transient_for(GTK_WINDOW(p->prefwindow), 
                                 GTK_WINDOW(gui->mainwindow));

#ifndef USE_GTKSPELL
    /* remove gtkspell related GUIs if not used */
    GtkHBox* hbox11 = GTK_HBOX(gtk_builder_get_object(builder, "hbox11"));
    GtkHBox* hbox10 = GTK_HBOX(gtk_builder_get_object(builder, "hbox10"));
    GtkLabel* label9 = GTK_LABEL(gtk_builder_get_object(builder, "label9"));
    gtk_container_remove(GTK_CONTAINER(hbox11), GTK_WIDGET(label9));
    gtk_container_remove(GTK_CONTAINER(hbox10), GTK_WIDGET(p->combo_languages));
#endif

    prefsgui_set_current_settings(p);
    gtk_builder_connect_signals(builder, NULL);

    return p;
}

void prefsgui_main(void) {
    gtk_widget_show_all(GTK_WIDGET(gummi->gui->prefsgui->prefwindow));
}

void prefsgui_set_current_settings(GuPrefsGui* prefs) {
    /* set font */
    const gchar* font = config_get_value("font");
    slog(L_DEBUG, "setting font to %s\n", font);
    PangoFontDescription* font_desc = pango_font_description_from_string(font);
    gtk_widget_modify_font(GTK_WIDGET(prefs->default_text), font_desc);
    pango_font_description_free(font_desc);

    /* set all checkboxs */
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefs->textwrap_button),
            (gboolean)config_get_value("textwrapping"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefs->wordwrap_button),
            (gboolean)config_get_value("wordwrapping"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefs->line_numbers),
            (gboolean)config_get_value("line_numbers"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefs->highlighting),
            (gboolean)config_get_value("highlighting"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefs->autosaving),
            (gboolean)config_get_value("autosaving"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefs->compile_status),
            (gboolean)config_get_value("compile_status"));

    gtk_spin_button_set_value(prefs->autosave_timer,
            atoi(config_get_value("autosave_timer"))/60);
    gtk_spin_button_set_value(prefs->compile_timer,
            atoi(config_get_value("compile_timer")));
    gtk_font_button_set_font_name(prefs->editor_font,
            config_get_value("font"));
    gtk_text_buffer_set_text(prefs->default_buffer,
            config_get_value("welcome"), strlen(config_get_value("welcome")));

    if (0 == strcmp(config_get_value("typesetter"), "xelatex"))
        gtk_combo_box_set_active(prefs->typesetter, 1);

    if (0 == strcmp(config_get_value("compile_scheme"), "real_time"))
        gtk_combo_box_set_active(prefs->compile_scheme, 1);

#ifdef USE_GTKSPELL
    /* list available languages */
    gchar* ptr = 0;

    pdata pret = utils_popen_r("enchant-lsmod -list-dicts");

    ptr = strtok(pret.data, " \n");
    while (ptr) {
        GtkTreeIter iter;
        if (ptr[0] != '(') {
            gtk_list_store_append(prefs->list_languages, &iter);
            gtk_list_store_set(prefs->list_languages, &iter, 0, ptr, -1);
        }
        ptr = strtok(NULL, " \n");
    }
    gtk_combo_box_set_active(prefs->combo_languages, 0);
#endif
}

void toggle_linenumbers(GtkWidget* widget, void* user) {
    gint newval = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    config_set_value("line_numbers", newval? "True": "False");
    gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(
                gummi->editor->sourceview), newval);
}

void toggle_highlighting(GtkWidget* widget, void* user) {
    gint newval = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    config_set_value("highlighting", newval? "True": "False");
    gtk_source_view_set_highlight_current_line(GTK_SOURCE_VIEW(
                gummi->editor->sourceview), newval);
}

void toggle_textwrapping(GtkWidget* widget, void* user) {
    gint newval = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    config_set_value("textwrapping", newval? "True": "False");
    if (newval) {
        gtk_text_view_set_wrap_mode(g_e_view, GTK_WRAP_CHAR);
        gtk_widget_set_sensitive(
                GTK_WIDGET(gummi->gui->prefsgui->wordwrap_button), TRUE);
    } else {
        gtk_text_view_set_wrap_mode(g_e_view, GTK_WRAP_NONE);
        config_set_value("wordwrapping", "False");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(
                    gummi->gui->prefsgui->wordwrap_button), FALSE);
        gtk_widget_set_sensitive(
                GTK_WIDGET(gummi->gui->prefsgui->wordwrap_button), FALSE);
    }
}

void toggle_wordwrapping(GtkWidget* widget, void* user) {
    gint newval = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    config_set_value("wordwrapping", newval? "True": "False");
    if (newval)
        gtk_text_view_set_wrap_mode(g_e_view, GTK_WRAP_WORD);
    else
        gtk_text_view_set_wrap_mode(g_e_view, GTK_WRAP_CHAR);
}

void on_prefs_close_clicked(GtkWidget* widget, void* user) {
    GtkTextIter start, end;
    if (2 == gtk_notebook_get_current_page(gummi->gui->prefsgui->notebook)) {
        gtk_text_buffer_get_start_iter(gummi->gui->prefsgui->default_buffer,
                &start);
        gtk_text_buffer_get_end_iter(gummi->gui->prefsgui->default_buffer,
                &end);
        config_set_value("welcome", gtk_text_buffer_get_text(
                gummi->gui->prefsgui->default_buffer, &start, &end, FALSE));
    }
    gtk_widget_hide(GTK_WIDGET(gummi->gui->prefsgui->prefwindow));
}

void on_prefs_reset_clicked(GtkWidget* widget, void* user) {
    config_set_default();
    prefsgui_set_current_settings(gummi->gui->prefsgui);
}

void on_autosave_value_changed(GtkWidget* widget, void* user) {
    gint newval = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
    gchar val_str[16];

    snprintf(val_str, 16, "%d", newval);
    config_set_value("autosave_timer", val_str);
    //iofunction_reset_autosave();
}

void on_compile_value_changed(GtkWidget* widget, void* user) {
    gint newval = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
    gchar val_str[16];

    snprintf(val_str, 16, "%d", newval);
    config_set_value("compile_timer", val_str);
    if (config_get_value("compile_status")) {
       motion_stop_updatepreview(gummi->motion);
       motion_start_updatepreview(gummi->motion);
    }
}

void on_editor_font_set(GtkWidget* widget, void* user) {
    const gchar* font = gtk_font_button_get_font_name(GTK_FONT_BUTTON(widget));
    slog(L_DEBUG, "setting font to %s\n", font);
    PangoFontDescription* font_desc = pango_font_description_from_string(font);
    gtk_widget_modify_font(GTK_WIDGET(gummi->editor->sourceview), font_desc);
    pango_font_description_free(font_desc);
}

void on_combo_typesetter_changed(GtkWidget* widget, void* user) {
    gint selected = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    const gchar typesetter[][16] = { "pdflatex", "xelatex" };
    config_set_value("typesetter", typesetter[selected]);
    gtk_widget_show(GTK_WIDGET(gummi->gui->prefsgui->changeimg));
    gtk_widget_show(GTK_WIDGET(gummi->gui->prefsgui->changelabel));
}

#ifdef USE_GTKSPELL
void on_combo_language_changed(GtkWidget* widget, void* user) {
    gchar* selected = gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget));
    config_set_value("spell_language", selected);
    editor_activate_spellchecking(gummi->editor, FALSE);
    editor_activate_spellchecking(gummi->editor, TRUE);
}
#endif

void on_combo_compilescheme_changed(GtkWidget* widget, void* user) {
    gint selected = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    const gchar scheme[][16] = { "on_idle", "real_time" };
    if (config_get_value("compile_status")) {
        motion_stop_updatepreview(gummi->motion);
        config_set_value("compile_scheme", scheme[selected]);
        motion_start_updatepreview(gummi->motion);
    } else {
        config_set_value("compile_scheme", scheme[selected]);
    }
}

GuSearchGui* searchgui_init(GtkBuilder* builder) {
    GuSearchGui* s;
    s = (GuSearchGui*)g_malloc(sizeof(GuSearchGui));
    s->searchwindow =
        GTK_WIDGET(gtk_builder_get_object(builder, "searchwindow"));
    s->searchentry =
        GTK_ENTRY(gtk_builder_get_object(builder, "searchentry"));
    s->replaceentry =
        GTK_ENTRY(gtk_builder_get_object(builder, "replaceentry"));
    s->matchcase = TRUE;
    g_signal_connect(s->searchentry, "changed",
            G_CALLBACK(on_searchgui_text_changed), NULL);
    s->backwards = FALSE;
    s->wholeword = FALSE;
    return s;
}

void on_toggle_matchcase_toggled(GtkWidget *widget, void* user) {
    gummi->gui->searchgui->matchcase =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    gummi->editor->replace_activated = FALSE;
}

void on_toggle_wholeword_toggled(GtkWidget *widget, void* user) {
    gummi->gui->searchgui->wholeword =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    gummi->editor->replace_activated = FALSE;
}

void on_toggle_backwards_toggled(GtkWidget *widget, void* user) {
    gummi->gui->searchgui->backwards =
        gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    gummi->editor->replace_activated = FALSE;
}

void on_searchgui_text_changed(GtkEditable *editable, void* user) {
    gummi->editor->replace_activated = FALSE;
}

