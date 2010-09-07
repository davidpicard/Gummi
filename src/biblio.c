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
#include <sys/stat.h> 


#include "biblio.h"
#include "utils.h"
#include "environment.h"

extern Gummi* gummi;
extern GuEditor* ec;

GtkWidget *progressbar;


GuBiblio* biblio_init(GtkBuilder * builder) {
    GuBiblio* b = (GuBiblio*)g_malloc(sizeof(GuBiblio));
    progressbar = GTK_WIDGET(gtk_builder_get_object(builder, "bibprogressbar"));
    b->bibbasename = NULL;
    b->bibdirname = NULL;
    return b;
}


gboolean biblio_detect_bibliography(GuEditor* ec) {
    printf("detect\n");
    gchar *content;
    GMatchInfo *match_info;
    GRegex* bib_regex;
    
    content = editor_grab_buffer(ec);
    bib_regex = g_regex_new("\\\\bibliography{([^{}]*})", 0, 0, NULL);
    g_regex_match (bib_regex, content, 0, &match_info);
    while (g_match_info_matches (match_info)) {
        gchar *word = g_match_info_fetch (match_info, 0);
        g_print ("Found: %s\n", word);
        g_free (word);
        g_match_info_next (match_info, NULL);
    }
    return TRUE;
}
/*
 g_regex_match (regex, string, 0, &match_info);
  while (g_match_info_matches (match_info))
    {
      gchar *word = g_match_info_fetch (match_info, 0);
      g_print ("Found: %s\n", word);
      g_free (word);
      g_match_info_next (match_info, NULL);
    }
  g_match_info_free (match_info);
  g_regex_unref (regex);
  */

/*
 		content = self.editorpane.grab_buffer()
		bib = re.compile('\\\\bibliography{([^{}]*})')
		for elem in bib.findall(content):
			candidate = elem[:-1]
			if candidate[-4:] == ".bib":
				if self.check_valid_file(candidate):
					return True
			else:
				if self.check_valid_file(candidate + ".bib"):
					return True
		return False */

gboolean biblio_compile_bibliography(GuMotion* mc) {
    gchar command[BUFSIZ];
    motion_update_workfile(mc);
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

gboolean biblio_setup_bibliography(GuEditor* ec, GuBiblio* b) {
    gchar *bla;
    printf("hoi\n");
    bla = g_strconcat(b->bibdirname, b->bibbasename, NULL);
    
    printf("%s\n", bla);
    
    editor_insert_bib(ec, bla);
    g_free(bla);
    
    return TRUE;
    
}
    
    /*
	def setup_bibliography(self):
		try:
			# cite is not a standard package
			# self.editorpane.insert_package("cite")
			bibtitle = self.bibbasename[:-4]
			shutil.copy2(self.bibdirname + self.bibbasename,
					Environment.tempdir + "/" + self.bibbasename)
			self.editorpane.insert_bib(self.bibdirname + self.bibbasename)
		except:
			print traceback.print_exc()
		return self.bibbasename, "N/A"*/

gboolean biblio_check_valid_file(GuBiblio* b, gchar *filename) {
    struct stat fileattrib; 
    // TODO: it should return 0 on file exist.. :/
    if (stat(filename, &fileattrib) == -1 ) {
        if (g_path_is_absolute(filename)) {
            b->bibbasename = g_path_get_basename(filename);
            b->bibdirname = g_path_get_dirname(filename);
            return TRUE;
        }
        else {
            b->bibbasename = filename;
            b->bibdirname = g_get_current_dir();
            return TRUE;
        }
    }
    else {
        return FALSE;
    }
}

