#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
int main() {
    long A, B_original;
    int  g, p;
    printf("=== REPLAY ATTACK ===\n");
    printf("A (Alice public key): "); scanf("%ld", &A);
    printf("g (generator)       : "); scanf("%d",  &g);
    printf("p (modulus)         : "); scanf("%d",  &p);
    printf("B (server response) : "); scanf("%ld", &B_original);
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port        = htons(8080);
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        printf("Connection failed. Is the server running?\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    int flag = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
    char buf[32];
    sprintf(buf, "%ld", A); send(sock, buf, strlen(buf), 0); Sleep(1);
    sprintf(buf, "%d",  g); send(sock, buf, strlen(buf), 0); Sleep(1);
    sprintf(buf, "%d",  p); send(sock, buf, strlen(buf), 0);
    char rbuf[32];
    int n = recv(sock, rbuf, 31, 0); rbuf[n] = '\0';
    long B_replay = atol(rbuf);
    printf("B_original=%ld  B_replay=%ld\n", B_original, B_replay);
    if (B_replay == B_original)
        printf("B matches: server accepted the replayed session. Attack succeeded.\n");
    else
        printf("B differs: server may have changed private key between sessions.\n");
    closesocket(sock);
    WSACleanup();
    return 0;
}
