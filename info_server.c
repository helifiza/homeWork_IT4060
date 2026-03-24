#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <dirent.h>
#include <sys/stat.h>

#pragma comment(lib, "ws2_32.lib")

int recvInt(int sock) {
    int value;
    recv(sock, (char*)&value, sizeof(value), MSG_WAITALL);
    return value;
}

void recvString(int sock, char *buff){
    int len = recvInt(sock);
    recv(sock, buff, len, MSG_WAITALL);
    buff[len] = '\0';
}

int main (int argc, char* argv[]) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
    int server = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9000);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    listen(server, 1);

    int client = accept(server, NULL, NULL);

    char path[1024];
    recvString(client, path);

    int n = recvInt(client);
    printf("Path đã nhận: %s\n", path);
    for(int i = 0; i < n; i++){
        char name[256];
        recvString(client, name);
        int size = recvInt(client);
        printf("%s - %d bytes\n", name, size);
    }

    WSACleanup();
    closesocket(client);
    closesocket(server);
    return 0;
}