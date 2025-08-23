#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <string.h>

/*
 * План роботи ехо-сервера:
 * 1) socket    - Створюємо сокет: "телефонну лінію" для спілкування в мережі (TCP, IPv4).
 * 2) bind      - Прив'язуємо сокет до IP-адреси та порту, щоб сервер знав, де "слухати".
 * 3) listen    - Налаштовуємо сокет на очікування вхідних підключень від клієнтів.
 * 4) accept    - Приймаємо вхідне підключення від клієнта, створюємо новий сокет для спілкування.
 * 5) recv      - Читаємо дані, які надіслав клієнт, у буфер.
 * 6) send      - Відправляємо отримані дані назад клієнту (ехо).
 * 7) goto 5    - Повторюємо кроки 5 і 6, поки клієнт не відключиться.
 */

int main(int argc, char **argv) {
    // Ініціалізація мережної підсистеми Windows
    WSADATA wsaData = {0};
    // Створюємо сокет - це як телефонна лінія, через яку наш сервер буде спілкуватися з клієнтами.
    SOCKET sock;
    // Створюємо змінну wPort типу WORD (16-бітове число) для зберігання номера порту, який вкаже користувач.
    WORD wPort;
    // AF_INET - IPv4, SOCK_STREAM - тип з'єднання

    // Зберігаємо інформацію про IP-адресу та порт Сервера в змінну SvrAddr
    SOCKADDR_IN SvrAddr = {0};
    // Зберігаємо інформацію про IP-адресу та порт Клієнта в змінну CliAddr
    SOCKADDR_IN CliAddr = {0};

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }

    wPort = (WORD)atoi(argv[1]);
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {  // Додано перевірку
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return -1;
    }

    // 1) Створюємо сокет
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        printf("Failed to create socket: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    // Налаштування адреси сервера
    SvrAddr.sin_family = AF_INET; // Вказуємо, що використовуємо IPv4.
    SvrAddr.sin_port = htons(wPort); // задаємо порт із wPort. Функція htons перетворює число на мережевий формат (порядок байтів).
    SvrAddr.sin_addr.s_addr = INADDR_ANY; // сервер слухатиме всі доступні IP-адреси на комп'ютері (наприклад, 0.0.0.0).

    // 2) Прив'язуємо сокет до адреси та порту
    if (bind(sock, (SOCKADDR *)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR) {
        printf("Failed to bind socket: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    if (listen(sock, 3) == SOCKET_ERROR) {  // Додано перевірку
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    printf("Server listening on port %d...\n", wPort);

    while (1) {
        int iSize = sizeof(CliAddr);
        SOCKET hClient = accept(sock, (SOCKADDR *)&CliAddr, &iSize);
        if (hClient == INVALID_SOCKET) {
            printf("Accept failed: %d\n", WSAGetLastError());
            continue;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(CliAddr.sin_addr), ntohs(CliAddr.sin_port));

        char buff[1024];
        while (1) {  // Цикл для кількох повідомлень від клієнта (goto 5)
            memset(buff, 0, sizeof(buff));  // Обнуляємо буфер
            int iRet = recv(hClient, buff, sizeof(buff) - 1, 0);  // -1 для '\0'
            if (iRet > 0) {
                buff[iRet] = '\0';  // Завершуємо рядок
                printf("Recv [%s:%d]: %s\n", inet_ntoa(CliAddr.sin_addr), ntohs(CliAddr.sin_port), buff);

                if (strcmp(buff, "exit") == 0) {  // Перевірка на exit
                    break;
                }

                send(hClient, buff, iRet, 0);  // Ехо назад
            } else if (iRet == 0) {
                printf("Client disconnected.\n");
                break;
            } else {
                printf("Recv failed: %d\n", WSAGetLastError());
                break;
            }
        }

        closesocket(hClient);  // Закриваємо клієнтський сокет
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}