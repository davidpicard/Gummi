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
//#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <glib.h>

#include "utils.h"

void updatecheck(void) {
    struct sockaddr_in servaddr;
    struct hostent *hp;
    gint sock_fd;
    gchar data[BUFSIZ];
    const gchar* request = "GET /redmine/projects/gummi/repository/raw/"
        "trunk/dev/latest HTTP/1.1\n";

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

    write(sock_fd, request, strlen(request));
    read(sock_fd, data, BUFSIZ);
    slog(L_G_INFO, "%s\n", data);

    return;

error:
    slog(L_G_ERROR, "Update check failed!\n");
    return;
}
