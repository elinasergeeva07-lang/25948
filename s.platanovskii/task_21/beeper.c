#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t beep_count = 0;

// Обработчик для CTRL-C (SIGINT)
void handle_sigint(int sig) {
    (void)sig;
    beep_count++;
    // Выводим звуковой сигнал (символ \a - ASCII Bell)
    // и сбрасываем буфер, чтобы звук сработал сразу
    printf("\a"); 
    fflush(stdout);
}

// Обработчик для CTRL-\ (SIGQUIT)
void handle_sigquit(int sig) {
    (void)sig;
    printf("\nПрозвучало сигналов: %d\n", beep_count);
    exit(0);
}

int main(void) {
    // Регистрируем обработчики
    // signal() устарел, но в учебных целях работает нормально
    if (signal(SIGINT, handle_sigint) == SIG_ERR) {
        perror("signal SIGINT");
        return 1;
    }
    if (signal(SIGQUIT, handle_sigquit) == SIG_ERR) {
        perror("signal SIGQUIT");
        return 1;
    }

    printf("Программа запущена.\n");
    printf("Нажмите CTRL-C (SIGINT) для писка.\n");
    printf("Нажмите CTRL-\\ (SIGQUIT) для выхода.\n");

    // Бесконечный цикл. Программа висит здесь и ждёт сигналов.
    // pause() усыпляет процесс до получения любого сигнала.
    while (1) {
        pause(); 
    }

    return 0;
}
