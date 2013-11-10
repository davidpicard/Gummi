/**
 * @file   external.h
 * @brief  existence and compability checks for external tools
 *
 * Copyright (C) 2009-2012 Gummi-Dev Team <alexvandermey@gmail.com>
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

#ifndef __GUMMI_EXTERNAL_H__
#define __GUMMI_EXTERNAL_H__

#include <glib.h>

typedef struct {
  gboolean exists;
  gchar* version;
} External;

typedef enum _ExternalProg {
  EX_TEXLIVE = 0,
  EX_LATEX,
  EX_PDFLATEX,
  EX_XELATEX,
  EX_RUBBER,
  EX_LATEXMK,
  EX_TEXCOUNT
} ExternalProg;


gboolean external_exists(const gchar* program);
gboolean external_hasflag(const gchar* program, const gchar* flag);

gchar* external_version(const gchar* program);
gdouble external_version2(ExternalProg program);

#endif /* __GUMMI_EXTERNAL_H__ */
