/**
 * @file   updatecheck.c
 * @brief  
 * @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 *
 * Copyright (C) 2010 -  Wei-Ning Huang (AZ) <aitjcize@gmail.com>
 * All Rights reserved.
 */

#include "updatecheck.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <glib.h>

#include "environment.h"
#include "utils.h"

void updatecheck(void) {
    GtkDialog* dialog = 0;
    struct sockaddr_in servaddr;
    struct hostent *hp;
    gint sock_fd = 0, i = 0;
    gchar data[BUFSIZ];
    const gchar* avail_version;
    const gchar* request = "GET /redmine/projects/gummi/repository/raw/"
        "trunk/dev/latest HTTP/1.1\r\n"
        "User-Agent: Gummi\r\n"
        "Host: dev.midnightcoding.org\r\n"
        "\r\n";

    if (-1 == (sock_fd = socket(AF_INET, SOCK_STREAM, 0))) {
        slog(L_ERROR, "socket() error\n");
        goto error;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    if (NULL == (hp = gethostbyname("dev.midnightcoding.org"))) {
        slog(L_ERROR, "gethostbyname() error\n");
        goto error;
    }

    memcpy((gchar*)&servaddr.sin_addr.s_addr, (gchar*)hp->h_addr, hp->h_length);
    servaddr.sin_port = htons(80);
    servaddr.sin_family = AF_INET;

    if (0 != connect(sock_fd, (struct sockaddr*)&servaddr, sizeof(servaddr))) {
        slog(L_G_ERROR, "connect() error");
        goto error;
    }

    read(sock_fd, data, BUFSIZ);
    /* get version string */
    for (i = strlen(data) -2; i >= 0 && data[i] != '\n'; --i);
    avail_version = data + i + 1;
    
    slog(L_INFO, "Currently installed: "VERSION"\n");
    slog(L_INFO, "Currently available: %s", avail_version);

    dialog = gtk_message_dialog_new (NULL, 
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Currently installed:\n"VERSION"\n\nCurrently available:\n%s",
            avail_version);
    gtk_window_set_title(GTK_WINDOW(dialog), "Update Check");
    gtk_dialog_run(GTK_DIALOG(dialog));      
    gtk_widget_destroy(dialog);
    return;

error:
    slog(L_G_ERROR, "Update check failed!\n");
    return;
}
