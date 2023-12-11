/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2020.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 19-1 */
#define _XOPEN_SOURCE 500
#include <sys/select.h>
#include <sys/inotify.h>
#include <limits.h>
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

static int inotifyFd;

// store paths for watch descriptors
static char watchDirs[FD_SETSIZE][PATH_MAX];

// Function to display inotify events
static void displayInotifyEvent(struct inotify_event *i) {
    printf("Event: wd =%2d; ", i->wd);
    if (i->cookie > 0)
        printf("cookie =%4d; ", i->cookie);

    printf("mask = ");
    if (i->mask & IN_ACCESS)        printf("IN_ACCESS ");
    if (i->mask & IN_ATTRIB)        printf("IN_ATTRIB ");
    if (i->mask & IN_CLOSE_NOWRITE) printf("IN_CLOSE_NOWRITE ");
    if (i->mask & IN_CLOSE_WRITE)   printf("IN_CLOSE_WRITE ");
    if (i->mask & IN_CREATE)        printf("IN_CREATE ");
    if (i->mask & IN_DELETE)        printf("IN_DELETE ");
    if (i->mask & IN_DELETE_SELF)   printf("IN_DELETE_SELF ");
    if (i->mask & IN_IGNORED)       printf("IN_IGNORED ");
    if (i->mask & IN_ISDIR)         printf("IN_ISDIR ");
    if (i->mask & IN_MODIFY)        printf("IN_MODIFY ");
    if (i->mask & IN_MOVE_SELF)     printf("IN_MOVE_SELF ");
    if (i->mask & IN_MOVED_FROM)    printf("IN_MOVED_FROM ");
    if (i->mask & IN_MOVED_TO)      printf("IN_MOVED_TO ");
    if (i->mask & IN_OPEN)          printf("IN_OPEN ");
    if (i->mask & IN_Q_OVERFLOW)    printf("IN_Q_OVERFLOW ");
    if (i->mask & IN_UNMOUNT)       printf("IN_UNMOUNT ");
    printf("\n");

    // Handle directory creation
    if ((i->mask & IN_CREATE) && (i->mask & IN_ISDIR)) {
        char path[2 * PATH_MAX + 2];
        if (strlen(watchDirs[i->wd]) + strlen(i->name) + 2 <= sizeof(path)) {
            snprintf(path, sizeof(path), "%s/%s", watchDirs[i->wd], i->name);
            int wd = inotify_add_watch(inotifyFd, path, IN_ALL_EVENTS);
            if (wd == -1) {
                perror("inotify_add_watch failed");
                printf("Failed to add watch for path: %s\n", path);
            } else {
                printf("New directory %s added to watch using wd %d\n", path, wd);
                strncpy(watchDirs[wd], path, PATH_MAX);
            }
        } else {
            fprintf(stderr, "Path length is too long\n");
        }
    }

    // Handle directory deletion
    if ((i->mask & IN_DELETE_SELF) || (i->mask & IN_IGNORED)) {
        printf("Directory %s deleted from watch using wd %d\n", watchDirs[i->wd], i->wd);
        memset(watchDirs[i->wd], 0, PATH_MAX);
    }

    if (i->len > 0)
        printf("        name = %s\n", i->name);
}

// Function to add watch to directories
static int register_watch(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_D) {
        int wd = inotify_add_watch(inotifyFd, fpath, IN_ALL_EVENTS);
        if (wd == -1) {
            perror("inotify_add_watch failed");
            printf("Failed to add watch for path: %s\n", fpath);
        } else {
            printf("Watching %s using wd %d\n", fpath, wd);
            strncpy(watchDirs[wd], fpath, PATH_MAX);
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    inotifyFd = inotify_init();
    if (inotifyFd == -1) {
        perror("inotify_init failed");
        exit(EXIT_FAILURE);
    }
    printf("inotify instance FD: %d\n", inotifyFd);

    memset(watchDirs, 0, sizeof(watchDirs));

    if (nftw(argv[1], register_watch, 20, FTW_PHYS) == -1) {
        perror("nftw failed");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0) {
            fprintf(stderr, "read() from inotify fd returned 0!\n");
            exit(EXIT_FAILURE);
        }

        if (numRead == -1) {
            perror("read failed");
            exit(EXIT_FAILURE);
        }

        printf("Read %ld bytes from inotify fd\n", (long) numRead);

        for (p = buf; p < buf + numRead; ) {
            event = (struct inotify_event *) p;
            displayInotifyEvent(event);

            p += sizeof(struct inotify_event) + event->len;
        }
    }

    exit(EXIT_SUCCESS);
}
