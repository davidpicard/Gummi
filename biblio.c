/**
 * @file   biblio.c
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

#include <gtk/gtk.h>
#include <string.h> 

#include "biblio.h"
#include "utils.h"
#include "environment.h"

extern Gummi* gummi;
extern GuEditor* ec;

GtkWidget *progressbar;


GuBiblio* biblio_init(GtkBuilder * builder) {
    GuBiblio* b = (GuBiblio*)g_malloc(sizeof(GuBiblio));
    progressbar = GTK_WIDGET(gtk_builder_get_object(builder, "bibprogressbar"));
    return b;
}


void on_bibcolumn_clicked(GtkWidget* widget, void* user) {
    gint id = gtk_tree_view_column_get_sort_column_id
        (GTK_TREE_VIEW_COLUMN(widget));
    gtk_tree_view_column_set_sort_column_id
        (GTK_TREE_VIEW_COLUMN(widget), id);
}

void on_bibcompile_clicked(GtkWidget* widget, void* user) {
    g_timeout_add_seconds(10, on_bibprogressbar_update, NULL);
    if (compile_bibliography() == TRUE) {
        
    }
    
    

}

/*
		self.bibprogressval = 0
		glib.timeout_add(10, self.on_bibprogressbar_update)
		if self.biblio.compile_bibliography(self.bibprogressbar):
			self.iofunc.set_status(_("Compiling bibliography file.."))
			self.bibprogressbar.set_text(_("bibliography compiled without errors"))
		else:
			self.iofunc.set_status(_("Error compiling bibliography file or none detected.."))
			self.bibprogressbar.set_text(_("error compiling bibliography file"))
*/




void on_bibrefresh_clicked(GtkWidget* widget, void* user) {
}

void on_bibreference_clicked(GtkWidget* widget, void* user) {
}

gboolean compile_bibliography() {
    gchar command[BUFSIZ];
    //motion_update_workfile();
    motion_update_auxfile();
    snprintf(command, sizeof command, "bibtex '%s'", gummi->workfile);
    pdata res = utils_popen_r(command);
    gtk_widget_set_tooltip_text(progressbar, res.data);
    if(strstr(res.data, "Database file #1") == NULL) {
        return FALSE;
    }
    else {
        return TRUE;
    }
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
