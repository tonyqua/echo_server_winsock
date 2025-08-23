#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    WSADATA wsaData = {0};
    SOCKET clientSock;
    SOCKADDR_IN SockAddr = {0};

    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return -1;
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return -1;
    }

    clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSock == INVALID_SOCKET) {
        printf("Socket failed: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons((WORD)atoi(argv[2]));
    SockAddr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(clientSock, (SOCKADDR*)&SockAddr, sizeof(SockAddr)) == SOCKET_ERROR) {
        printf("Connect failed: %d\n", WSAGetLastError());
        closesocket(clientSock);
        WSACleanup();
        return -1;
    }

    printf("Connected! Enter Message: ");
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    fgets(buff, sizeof(buff) - 1, stdin);
    send(clientSock, buff, strlen(buff), 0);

    int iRet = recv(clientSock, buff, sizeof(buff) - 1, 0);
    if (iRet > 0) {
        buff[iRet] = '\0';
        printf("Echo Message: %s\n", buff);
    } else {
        printf("Recv failed or connection closed.\n");
    }

    closesocket(clientSock);
    WSACleanup();
    return 0;
}