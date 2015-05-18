#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <CoreServices/CoreServices.h>

static char path_buf[1024 * 1024], *path_end;
static char real_path_buf[1024 * 1024];
static char alias_buf[1024 * 1024];

static char progress_buf[10240];
static int last_progress_len = 0;

static int results = 0;

static const int max_progress_len = 79;

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

    if (strlen(progress_buf) > max_progress_len) {
        strcpy(progress_buf + max_progress_len-3, "...");
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
    FSRef fsref;
    AliasHandle itemAlias;
    HFSUniStr255 targetName;
    HFSUniStr255 volumeName;
    CFStringRef pathString;
    FSAliasInfoBitmap returnedInInfo;
    FSAliasInfo info;
    bool found_realpath = false, found_alias = false;

    if (*path_buf && path_end[-1] != '/')
        *path_end++ = '/';
    path_end = stpcpy(path_end, dir_name);

    progress("Searching: %s", path_buf);

    realpath(path_buf, real_path_buf);
    if (0 != strcmp(path_buf, real_path_buf)) {
        output("Found (realpath): '%s' != '%s'\n", path_buf, real_path_buf);
        found_realpath = true;
    }

    FSPathMakeRefWithOptions(path_buf, kFSPathMakeRefDoNotFollowLeafSymlink, &fsref, NULL);
    FSNewAlias(NULL, &fsref, &itemAlias);
    FSCopyAliasInfo(itemAlias, &targetName, &volumeName, &pathString, &returnedInInfo, &info);
    CFStringGetCString(pathString, alias_buf, sizeof(alias_buf), kCFStringEncodingUTF8);
    if (0 != strcmp(path_buf, alias_buf)) {
        found_alias = true;
    }

    if (found_realpath || found_alias) {
        ++results;
        output("Result %ld:\n"
               "  readdir:         %s\n"
               " %crealpath:        %s\n"
               " %cFSCopyAliasInfo: %s\n", (long)results, path_buf, (found_realpath ? '!' : ' '), real_path_buf, (found_alias ? '!' : ' '), alias_buf);
        goto skip_children;
    }

    dirp = opendir(path_buf);
    if (dirp) {
        while ((dirent = readdir(dirp)) != NULL) {
            if (0 == strcmp(dirent->d_name, ".") || 0 == strcmp(dirent->d_name, ".."))
                continue;
            // printf("%s\n", dirent->d_name);
            if (dirent->d_type == DT_DIR && depth < 100) {
                walk(dirent->d_name, depth + 1);
            }
        }
        closedir(dirp);
    }

skip_children:
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
