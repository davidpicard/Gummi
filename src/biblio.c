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


GuBiblio* biblio_init(GtkBuilder * builder) {
    GuBiblio* b = (GuBiblio*)g_malloc(sizeof(GuBiblio));
    b->progressbar =
        GTK_PROGRESS_BAR(gtk_builder_get_object(builder, "bibprogressbar"));
    b->progressmon =
        GTK_ADJUSTMENT(gtk_builder_get_object(builder, "bibprogressmon"));
    b->list_biblios = 
        GTK_LIST_STORE(gtk_builder_get_object(builder, "list_biblios"));
    b->filenm_label = 
        GTK_LABEL(gtk_builder_get_object(builder, "bibfilenm"));
    b->refnr_label = 
        GTK_LABEL(gtk_builder_get_object(builder, "bibrefnr"));
    b->progressval = 0.0;
    b->filename = NULL;
    b->basename = NULL;
    b->dirname = NULL;
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

gboolean biblio_compile_bibliography(GuBiblio* bc, GuMotion* mc) {
    gchar command[BUFSIZ];
    motion_update_workfile(mc);
    motion_update_auxfile(mc);
    snprintf(command, sizeof command, "bibtex '%s'", mc->workfile);
    pdata res = utils_popen_r(command);
    gtk_widget_set_tooltip_text(GTK_WIDGET(bc->progressbar), res.data);
    if(strstr(res.data, "Database file #1") == NULL)
        return FALSE;
    else
        return TRUE;
}

gboolean biblio_setup_bibliography(GuBiblio* b, GuEditor* ec) {
    gchar *bibpath;
    gchar *dst;
    
    dst = g_strconcat(g_get_tmp_dir(), G_DIR_SEPARATOR_S, b->basename, NULL);
    
    utils_copy_file(b->filename, dst);
    bibpath = g_strconcat(b->dirname, G_DIR_SEPARATOR_S, b->basename, NULL);
    editor_insert_bib(ec, bibpath);

    g_free(dst);
    g_free(bibpath);
    return TRUE;
}

gboolean biblio_check_valid_file(GuBiblio* b, gchar *filename) {
    if (b->filename) g_free(b->filename);
    if (b->basename) g_free(b->basename);
    if (b->dirname) g_free(b->dirname);

    if (utils_path_exists(filename) == TRUE) {
        b->filename = g_strdup(filename);
        if (g_path_is_absolute(filename)) {
            b->basename = g_strdup(g_path_get_basename(filename));
            b->dirname = g_strdup(g_path_get_dirname(filename));
            return TRUE;
        }
        else {
            b->basename = g_strdup(filename);
            b->dirname = g_strdup(g_get_current_dir());
            return TRUE;
        }
    }
    else {
        return FALSE;
    }
}

int biblio_parse_entries(GuBiblio* bc, gchar *bib_content) {
    int entry_total = 0;
    
    GRegex* regex_entry;
    GRegex* subregex_ident;
    GRegex* subregex_title;
    GRegex* subregex_author;
    GRegex* subregex_year;

    GMatchInfo *match_entry; 
    /*   
    GMatchInfo *match_ident;
    GMatchInfo *match_title;
    GMatchInfo *match_author;
    GMatchInfo *match_year;
    */
    
    regex_entry = g_regex_new(
        "(@article|@book|@booklet|@conference|@inbook|@incollection|"
        "@inproceedings|@manual|@mastersthesis|@misc|@phdthesis|"
        "@proceedings|@techreport|@unpublished)([^@]*)", 
        (G_REGEX_CASELESS | G_REGEX_DOTALL), 0, NULL);
      
    subregex_ident = g_regex_new("@.+{([^,]+,)", 0, 0, NULL);
    subregex_title = g_regex_new("[^book]title[\\s]*=[\\s]*(.*)", 0, 0, NULL);
    subregex_author = g_regex_new("author[\\s]*=[\\s]*(.*)", 0, 0, NULL);
    subregex_year = g_regex_new("year[\\s]*=[\\s]*[{|\"]?([1|2][0-9]{3})", 0,
            0, NULL);
    
    
    g_regex_match(regex_entry, bib_content, 0, &match_entry);
    
    while (g_match_info_matches (match_entry)) {
        
        gchar *entry = g_match_info_fetch (match_entry, 0);

        gchar **ident_res = g_regex_split(subregex_ident, entry, 0);
        gchar **title_res = g_regex_split(subregex_title, entry, 0);
        gchar **author_res = g_regex_split(subregex_author, entry, 0);
        gchar **year_res = g_regex_split(subregex_year, entry, 0);
        
        printf("%s\n%s\n%s\n%s\n\n", ident_res[1], title_res[1], author_res[1], year_res[1]);
        //TODO: split the entry up in its pieces and feed it to the list_store
        
        g_strfreev(ident_res); g_strfreev(title_res);
        g_strfreev(author_res); g_strfreev(year_res);
        
        
        entry_total += 1;
        g_free (entry);
        g_match_info_next (match_entry, NULL);
    }
    
    return entry_total;
}
    
    /*
		entries = re.compile('(@article|@book|@booklet|@conference|@inbook|' \
			'@incollection|@inproceedings|@manual|@mastersthesis|@misc|' \
			'@phdthesis|@proceedings|@techreport|@unpublished)([^@]*)' \
			, re.DOTALL | re.IGNORECASE)
		for elem in entries.findall(bibstr):
			entry = elem[1]
			ident_exp = re.compile('{([^,]*)')		
			author_exp = re.compile('author\s*=\s*(.*)')
			title_exp = re.compile('[^book]title\s*=\s*(.*)')
			year_exp = re.compile('year\s*=\s*{?"?([1|2][0-9][0-9][0-9])}?"?')

			ident_res = ident_exp.findall(entry)[0]
			try: author_res = author_exp.findall(entry)[0]
			except: author_res = "????"
			try: title_res = title_exp.findall(entry)[0]
			except: title_res = "????"
			try: year_res = year_exp.findall(entry)[0]
			except: year_res = "????"

			author_fmt = re.sub("[{|}|\"|\,]", "", author_res)	
			title_fmt = re.sub("[{|}|\"|\,|\$]", "", title_res)
			year_fmt = year_res

			biblist.append([ident_res, title_fmt, author_fmt, year_fmt])
			refnr = refnr + 1
			
		return refnr
*/
