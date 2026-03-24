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

void sendInt(int sock, int value) {
    send(sock,(const char*) &value, sizeof(int), 0);
}

void sendString(int sock, const char* str) {
    int len = strlen(str);
    sendInt(sock, len);
    send(sock, str, len, 0);
}

int main (int argc, char* argv[]) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
    int client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(9000);

    int res = connect(client, (struct sockaddr*)&addr, sizeof(addr));
    if(res == -1) {
        printf("Khong the ket noi den server\n");
        return 1;
    }

    char cwd[1024];
    getcwd(cwd, sizeof(cwd));

    sendString(client, cwd);

    DIR *dir = opendir(".");
    if (dir == NULL) {
        printf("Khong the mo thu muc\n");
        close(client);
        return 1;
    }
    struct dirent *entry;
    struct stat st;

    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        stat(entry->d_name, &st);
        if (S_ISREG(st.st_mode)) {
            count++;
        }
    }

    sendInt(client, count);
    rewinddir(dir);

    while((entry = readdir(dir)) != NULL) {
        stat(entry->d_name, &st);
        if (S_ISREG(st.st_mode)) {
            char *name = entry->d_name;

            stat(name, &st);
            sendString(client, name);

            int size = (int)st.st_size;
            sendInt(client, size);
        }
    }

    closedir(dir);
    closesocket(client);
    WSACleanup();
    return 0;
}