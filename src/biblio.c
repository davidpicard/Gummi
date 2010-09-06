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



gboolean compile_bibliography(GuMotion* mc) {
    gchar command[BUFSIZ];
    //motion_update_workfile();
    motion_update_auxfile(mc);
    snprintf(command, sizeof command, "bibtex '%s'", mc->workfile);
    pdata res = utils_popen_r(command);
    gtk_widget_set_tooltip_text(progressbar, res.data);
    if(strstr(res.data, "Database file #1") == NULL) {
        return FALSE;
    }
    else {
        return TRUE;
    }
}

