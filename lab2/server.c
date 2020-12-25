#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include "info.h"

int sock;

void signal_handler(int signum)
{
    printf("closing socket because of ctrl+C...\n");
    close(sock);
    unlink(SOCKET_NAME);
}


void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

 void itoa(int n, char s[], int radix)
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* записываем знак */
         n = -n;          /* делаем n положительным числом */
     i = 0;
     do {       /* генерируем цифры в обратном порядке */
        if (radix == 16)
        {
            int res = n % radix;
            if (res == 10)
                s[i++] = 'A';
            else if (res == 11)
                s[i++] = 'B';
            else if (res == 12)
                s[i++] = 'C';
            else if (res == 13)
                s[i++] = 'D';
            else if (res == 14)
                s[i++] = 'E';
            else if (res == 15)
                s[i++] = 'F';
            else
                s[i++] = res + '0';
        }
        else
        {
            s[i++] = n % radix + '0';
        }
            /* берем следующую цифру */
     } while ((n /= radix) > 0);     /* удаляем */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

int main(void)
{
    struct sockaddr sock_adr;
    char msg[MSG_LEN];

    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        perror("socket error\n");
        return sock;
    }

    sock_adr.sa_family = AF_UNIX;
    strcpy(sock_adr.sa_data, SOCKET_NAME);

    if (bind(sock, &sock_adr, sizeof(sock_adr)) < 0)
    {
        printf("closing socket ...\n");
        close(sock);
        unlink(SOCKET_NAME);
        perror("binding error\n");
        return -1;
    }

    printf("server waits for client's number...\n");

    signal(SIGINT, signal_handler);

    for (; ;)
    {
        int rec = recv(sock, msg, sizeof(msg), 0);
        if (rec < 0)
        {
            close(sock);
            unlink(SOCKET_NAME);
            perror("message recieving error\n");
            return rec;
        }

        msg[rec] = 0;
        printf("Clien't sent this number: %s + %d\n", msg, rec);

        char* endptr;
        int number = strtol(msg, &endptr, 10);
        printf("number in 10: %d\n", number);

        printf("mes is %s\n", msg);
        itoa(number, endptr, 2);
        printf("number in 2: %s\n", endptr);

        itoa(number, endptr, 7);
        printf("number in 7: %s\n", endptr);

        itoa(number, endptr, 8);
        printf("number in 8: %s\n", endptr);

        itoa(number, endptr, 16);
        printf("number in 16: %s\n", endptr);

        printf("=================================\n");
        
    }

    printf("closing socket ...\n");
    close(sock);
    unlink(SOCKET_NAME);
    return 0;
}