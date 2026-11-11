#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

void print_permissions(mode_t mode) {
    if (S_ISDIR(mode))      putchar('d');
    else if (S_ISREG(mode)) putchar('-');
    else                    putchar('?');

    putchar((mode & S_IRUSR) ? 'r' : '-');
    putchar((mode & S_IWUSR) ? 'w' : '-');
    putchar((mode & S_IXUSR) ? 'x' : '-');

    putchar((mode & S_IRGRP) ? 'r' : '-');
    putchar((mode & S_IWGRP) ? 'w' : '-');
    putchar((mode & S_IXGRP) ? 'x' : '-');

    putchar((mode & S_IROTH) ? 'r' : '-');
    putchar((mode & S_IWOTH) ? 'w' : '-');
    putchar((mode & S_IXOTH) ? 'x' : '-');
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file1> [file2 ...]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        struct stat st;

        if (lstat(argv[i], &st) == -1) {
            perror(argv[i]);
            continue;
        }

        print_permissions(st.st_mode);

        printf(" %3lu", (unsigned long)st.st_nlink);

        struct passwd *pw = getpwuid(st.st_uid);
        printf(" %-8s", pw ? pw->pw_name : "unknown");

        struct group *gr = getgrgid(st.st_gid);
        printf(" %-8s", gr ? gr->gr_name : "unknown");

        if (S_ISREG(st.st_mode)) {
            printf(" %8lld", (long long)st.st_size);
        } else {
            printf(" %8s", "");
        }

        char timebuf[64];
        struct tm *tm_info = localtime(&st.st_mtime);
        strftime(timebuf, sizeof(timebuf), "%b %e %H:%M", tm_info);
        printf(" %s", timebuf);

        char *base = strrchr(argv[i], '/');
        if (base) base++; else base = argv[i];
        printf(" %s\n", base);
    }

    return 0;
}
