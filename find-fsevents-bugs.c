#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>

static char path_buf[1024 * 1024], *path_end;
static char real_path_buf[1024 * 1024];

static char progress_buf[10240];
static int last_progress_len = 0;

static int results = 0;

void erase() {
    int i;
    printf("\r");
    for (i = 0; i < last_progress_len; i++) putchar(' ');
    printf("\r");
}

void show_progress() {
    printf("%s", progress_buf);
    last_progress_len = strlen(progress_buf);
    fflush(stdout);
}

void progress(const char *fmt, ...) {
    erase();

    va_list va;
    va_start(va, fmt);
    vsprintf(progress_buf, fmt, va);
    va_end(va);

    if (strlen(progress_buf) > 120) {
        strcpy(progress_buf + 120-3, "...");
    }

    show_progress();
}

void output(const char *fmt, ...) {
    erase();

    va_list va;
    va_start(va, fmt);
    vprintf(fmt, va);
    va_end(va);

    show_progress();
}

void walk(const char *dir_name, int depth) {
    int i;
    struct dirent *dirent;
    DIR *dirp;
    char *path_end_saved = path_end;

    if (*path_buf && path_end[-1] != '/')
        *path_end++ = '/';
    path_end = stpcpy(path_end, dir_name);

    progress("Searching: %s", path_buf);

    realpath(path_buf, real_path_buf);
    if (0 != strcmp(path_buf, real_path_buf)) {
        output("Found: '%s' != '%s'\n", path_buf, real_path_buf);
        ++results;
    }

    dirp = opendir(path_buf);
    while ((dirent = readdir(dirp)) != NULL) {
        if (0 == strcmp(dirent->d_name, ".") || 0 == strcmp(dirent->d_name, ".."))
            continue;
        // printf("%s\n", dirent->d_name);
        if (dirent->d_type == DT_DIR && depth < 100) {
            walk(dirent->d_name, depth + 1);
        }
    }
    closedir(dirp);

    path_end = path_end_saved;
    *path_end = 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: find-fsevents-bugs /path/to/root/folder\n");
        exit(10);
    }

    const char *path = argv[1];

    *path_buf = 0;
    path_end = path_buf;
    walk(path, 0);
    progress("Done, %d result(s).\n", results);

    return 0;
}
