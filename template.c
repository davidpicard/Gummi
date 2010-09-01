/**
 * @file   template.c
 * @brief  
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#include "template.h"

#include <glib.h>
#include <gtk/gtk.h>

const gchar* template_article = 
"\\documentclass{article}\n"
"\\author{[YOUR NAME]\\\\\n"
"\\texttt{[YOUR EMAIL]}\n"
"}\n"
"\\title{[TITLE OF YOUR ARTICLE]}\n"
"\\begin{document}\n"
"\\maketitle\n"
"\\dots\n"
"\n"
"\\end{document}\n";

const gchar* template_book =
"\\documentclass[12pt]{book}\n"
"\n"
"\\begin{document}\n"
"\n"
"\\chapter*{\\Huge \\center [BOOKTITLE] }\n"
"\\thispagestyle{empty}\n"
"\\section*{\\huge \\center [AUTHOR]}\n"
"\\newpage\n"
"\n"
"\\subsection*{\\center \\normalsize Copyright \\copyright [YEAR] [NAME]}\n"
"\\subsection*{\\center \\normalsize All rights reserved.}\n"
"\\subsection*{\\center \\normalsize ISBN \\dots}\n"
"\\subsection*{\\center \\normalsize Publications}\n"
"\n"
"\\tableofcontents\n"
"\n"
"\\mainmatter\n"
"\\chapter{[CHAPTER1-TITLE]}\n"
"\\dots\n"
"\\chapter{[CHAPTER2-TITLE]}\n"
"\\dots\n"
"\\backmatter\n"
"\n"
"\\end{document}\n";

const gchar* template_letter =
"\\documentclass{letter}\n"
"\n"
"\\signature{[YOURNAME]}\n"
"\\address{[YOURADDRESS]}\n"
"\n"
"\\begin{document}\n"
"\\begin{letter}{Company name \\\\ Street\\\\ City\\\\ Country}\n"
"\\opening{[HEADING]}\n"
"\n"
"\\dots\\\\\\dots\\\\\\dots\\\\\\dots\\\\\\dots\n"
"\n"
"\\closing{[CLOSING]}\n"
"\\end{letter}\n"
"\\end{document}\n";

const gchar* template_report =
"\\documentclass[]{report}\n"
"\\begin{document}\n"
"\n"
"\\title{[YOUR TITLE]}\n"
"\\author{[YOUR NAME]}\n"
"\\maketitle\n"
"\n"
"\\chapter{[CHAPTERTITLE]}\n"
"\\section{Introduction}\n"
"\\dots\n"
"\\chapter{[CHAPTERTITLE]}\n"
"\\section{Introduction}\n"
"\\dots\n"
"\\subsection{Subsection}\n"
"\\end{document}\n";

template_t* template_init(GtkBuilder* builder) {
    template_t* t = (template_t*)g_malloc(sizeof(template_t));
    t->templatewindow =
        GTK_WINDOW(gtk_builder_get_object(builder, "templatewindow"));
    t->iconview =
        GTK_ICON_VIEW(gtk_builder_get_object(builder, "templateicons"));
    t->template_ok =
        GTK_BUTTON(gtk_builder_get_object(builder, "template_ok"));
    gtk_icon_view_set_text_column(t->iconview, 0);
    gtk_icon_view_set_pixbuf_column(t->iconview, 1);
    g_signal_connect(t->iconview, "selection-changed",
            G_CALLBACK(template_update_window), t->template_ok);
    return t;
}

void template_update_window(GdkEvent* event, void* button) {
    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
}

const gchar* template_get(template_t* templ) {
    GList* selection = gtk_icon_view_get_selected_items(templ->iconview);
    switch (*(gint*)(selection->data)) {
        case 0:
            return template_article;
            break;
        case 1:
            return template_book;
            break;
        case 2:
            return template_letter;
            break;
        case 3:
            return template_report;
            break;
    }
    return NULL;
}
