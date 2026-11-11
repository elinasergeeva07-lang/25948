#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int fd[2];
    pid_t pid;

    // 1. Создаем канал
    // fd[0] - для чтения, fd[1] - для записи
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    // 2. Создаем дочерний процесс
    pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* --- РЕБЕНОК --- */
        close(fd[1]); // Закрываем запись, он только читает

        char buf[256];
        ssize_t n;

        // Читаем из канала, пока не закроется
        while ((n = read(fd[0], buf, sizeof(buf) - 1)) > 0) {
            buf[n] = '\0'; // Добавляем конец строки для printf
            
            // Переводим в верхний регистр и печатаем
            for (int i = 0; i < n; i++) {
                putchar(toupper(buf[i]));
            }
        }
        
        printf("\n"); // Переход на новую строку в конце
        close(fd[0]);
        exit(0);
    } else {
        /* --- РОДИТЕЛЬ --- */
        close(fd[0]); // Закрываем чтение, он только пишет

        // Текст, который будем передавать
        const char *text = "Hello World! This is a Pipe Test.\n";
        
        // Пишем в канал
        write(fd[1], text, strlen(text));
        close(fd[1]); // Закрываем запись, чтобы ребенок понял, что данных больше нет

        wait(NULL); // Ждем завершения ребенка
    }

    return 0;
}
