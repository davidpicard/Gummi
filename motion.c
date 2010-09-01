/**
 * @file   motion.c
 * @brief  
 *
 * Copyright (C) 2010 Gummi-Dev Team <alexvandermey@gmail.com>
 * All Rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "motion.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "environment.h"
#include "utils.h"

extern Gummi* gummi;

GuMotion* motion_init(gint dum) {
    GuMotion* m = (GuMotion*)g_malloc(sizeof(GuMotion));
    m->dummy = dum;
    return m;
}

void motion_initial_preview(GuEditor* ec) {
    motion_update_workfile(ec);
    motion_update_pdffile(ec);
    // if succes.. 
    preview_set_pdffile(gummi->pdffile);
}


void motion_update_workfile(GuEditor* ec) {
    GtkTextIter start;
    GtkTextIter end;
    gchar *text;
    FILE *fp;

    // TODO: the following line caused hangups in python, attention!
    gtk_widget_set_sensitive(ec->sourceview, FALSE);
    gtk_text_buffer_get_bounds(GTK_TEXT_BUFFER(ec->sourcebuffer),
            &start, &end);
    text = gtk_text_iter_get_text (&start, &end);
    gtk_widget_set_sensitive(ec->sourceview, TRUE);
    
    fp =fopen(gummi->workfile, "w");
    
    if(fp == NULL) {
        perror("failed to open workfile");
        // TODO: do your debug thingie!
    }
    fwrite(text, strlen(text), 1, fp);
    g_free(text);
    fclose(fp);
    // TODO: Maybe add editorviewer grab focus line here if necessary
}

void motion_update_pdffile(GuEditor* ec) {
    FILE *fp;
    int status;
    char path[PATH_MAX];
    char command[PATH_MAX];
    snprintf(command, sizeof command, "pdflatex "
                                    "-interaction=nonstopmode "
                                    "-file-line-error "
                                    "-halt-on-error "
                                    "-output-directory='%s' '%s'", \
                                    gummi->tmpdir, gummi->workfile);
    fp = popen(command, "r");
    if (fp == NULL) {
        // handle error
    }
    while (fgets(path, PATH_MAX, fp) != NULL)
        printf("%s", path);
        
    status = pclose(fp);
    if (status == -1) {
         // handle error
    }
}
