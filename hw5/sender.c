#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int receiver_pid;
int number_to_send;
int bit_count = 0;
int bit_acknowledged = 1;

void send_bit(int bit) {
    if (bit == 0) {
        kill(receiver_pid, SIGUSR1);
    } else {
        kill(receiver_pid, SIGUSR2);
    }

    bit_acknowledged = 0;
    while (!bit_acknowledged) {
        usleep(10000); // Ждем подтверждения от приемника
    }
}

void send_number() {
    for (int i = 31; i >= 0; i--) {
        int bit = (number_to_send >> i) & 1;
        send_bit(bit);
        usleep(50000); // Добавляем небольшую задержку для стабильности передачи
    }

    kill(receiver_pid, SIGUSR1); // Сигнал о завершении передачи
}

void ack_handler(int sig) {
    if (sig == SIGUSR1) {
        bit_acknowledged = 1;
    }
}

int main() {
    printf("My PID: %d\n", getpid());

    signal(SIGUSR1, ack_handler);
    signal(SIGUSR2, ack_handler);

    printf("Enter the receiver's PID: ");
    scanf("%d", &receiver_pid);

    printf("Enter a decimal number to send: ");
    scanf("%d", &number_to_send);

    send_number();

    return 0;
}
