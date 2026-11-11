#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

#define MAX_LEN 40
#define ERASE 127
#define KILL 21
#define CTRL_W 23
#define CTRL_D 4
#define CTRL_G 7

struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void redraw_line(const char *buf, int pos, int len) {
    printf("\r");
    for (int i = 0; i < 80; i++) putchar(' ');
    printf("\r");

    for (int i = 0; i < len; i++) {
        if (i > 0 && i % MAX_LEN == 0) {
            printf("\r\n");
        }
        putchar(buf[i]);
    }

    int cur_row = pos / MAX_LEN;
    int cur_col = pos % MAX_LEN;
    
    printf("\r");
    for (int i = 0; i < cur_row; i++) {
        printf("\n");
    }
    printf("\033[%dC", cur_col);
    
    fflush(stdout);
}

int main(void) {
    enable_raw_mode();

    char buf[1024] = {0};
    int pos = 0;
    int len = 0;
    char c;

    printf("Введите текст (Ctrl+D - выход):\r\n");

    while (1) {
        if (read(STDIN_FILENO, &c, 1) != 1) break;

        if (c == CTRL_D && len == 0) {
            break;
        } else if (c == '\n' || c == '\r') {
            printf("\r\n");
            len = 0;
            pos = 0;
            buf[0] = '\0';
            continue;
        } else if (c == ERASE || c == 8) {
            if (pos > 0) {
                memmove(&buf[pos - 1], &buf[pos], len - pos + 1);
                pos--;
                len--;
                redraw_line(buf, pos, len);
            }
        } else if (c == KILL) {
            len = 0;
            pos = 0;
            buf[0] = '\0';
            redraw_line(buf, pos, len);
        } else if (c == CTRL_W) {
            while (pos > 0 && isspace(buf[pos - 1])) {
                memmove(&buf[pos - 1], &buf[pos], len - pos + 1);
                pos--; len--;
            }
            while (pos > 0 && !isspace(buf[pos - 1])) {
                memmove(&buf[pos - 1], &buf[pos], len - pos + 1);
                pos--; len--;
            }
            redraw_line(buf, pos, len);
        } else if (c < 32) {
            putchar(CTRL_G);
        } else {
            if (len < sizeof(buf) - 1) {
                memmove(&buf[pos + 1], &buf[pos], len - pos);
                buf[pos] = c;
                pos++;
                len++;
                buf[len] = '\0';
                redraw_line(buf, pos, len);
            } else {
                putchar(CTRL_G);
            }
        }
    }

    printf("\r\nВыход.\n");
    return 0;
}
