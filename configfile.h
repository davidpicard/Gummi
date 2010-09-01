/**
 * @file   configfile.h
 * @brief  handle configuration file
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#ifndef GUMMI_CONFIGFILE
#define GUMMI_CONFIGFILE

#include <glib.h>

#define CONFIG_MAX 64
#define BUF_MAX 256

typedef struct _finfo {
    gchar** pbuf;
    int len;
} finfo;

/**
 * @brief initialize config file
 * @param filename filename of the configuration file
 */
void config_init(const gchar* filename);

/**
 * @brief reset settings to default
 */
void config_set_default(void);

/**
 * @brief get value of a setting
 * @param term the name of the setting
 * @return a pointer that points to the static gchar* of the setting value. If
 * the value type is boolean, config_get_value will return NULL for False
 * and non-NULL for True
 */
const gchar* config_get_value(const gchar* term);

/**
 * @brief set value of a setting
 * @param term the name of the setting
 * @param value the value of the setting
 */
void config_set_value(const gchar* term, const gchar* value);

/* [Internal] */
finfo config_load(void); 
void config_save(gchar** pbuf, int len);
int config_find_index_of(gchar** pbuf, const gchar* term);

#endif /* GUMMI_CONFIGFILE */
