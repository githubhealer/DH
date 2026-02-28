#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
long modexp(long base, int exp, int mod) {
    long ans = 1;
    for (int i = 0; i < exp; i++)
        ans = (ans % mod * (base % mod)) % mod;
    return ans % mod;
}
int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        return 1;
    }
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        fprintf(stderr, "socket() failed\n");
        WSACleanup();
        return 1;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));
    struct sockaddr_in mitm_addr;
    memset(&mitm_addr, 0, sizeof(mitm_addr));
    mitm_addr.sin_family      = AF_INET;
    mitm_addr.sin_addr.s_addr = INADDR_ANY;
    mitm_addr.sin_port        = htons(9090);
    if (bind(listen_sock, (struct sockaddr *)&mitm_addr, sizeof(mitm_addr)) != 0) {
        fprintf(stderr, "bind() failed on port 9090\n");
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }
    listen(listen_sock, 1);
    printf("Waiting for client...\n");
    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);
    SOCKET client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &client_len);
    if (client_sock == INVALID_SOCKET) {
        fprintf(stderr, "accept() failed\n");
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }
    int flag = 1;
    setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
    SOCKET server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);
    if (connect(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) != 0) {
        fprintf(stderr, "connect() to server on port 8080 failed\n");
        closesocket(client_sock);
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }
    setsockopt(server_sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
    char buf[32];
    int n;
    long A = 0;
    int  g = 0, p = 0;
    n = recv(client_sock, buf, 31, 0); buf[n] = '\0'; A = atol(buf);
    n = recv(client_sock, buf, 31, 0); buf[n] = '\0'; g = atoi(buf);
    n = recv(client_sock, buf, 31, 0); buf[n] = '\0'; p = atoi(buf);
    printf("Intercepted from client: A=%ld  g=%d  p=%d\n", A, g, p);
    int Xm;
    printf("Enter MITM private key Xm (e.g. 5): ");
    scanf("%d", &Xm);
    long M = modexp(g, Xm, p);
    printf("MITM public key M = %ld\n", M);
    sprintf(buf, "%ld", M); send(server_sock, buf, strlen(buf), 0); Sleep(1);
    sprintf(buf, "%d",  g); send(server_sock, buf, strlen(buf), 0); Sleep(1);
    sprintf(buf, "%d",  p); send(server_sock, buf, strlen(buf), 0);
    long B = 0;
    n = recv(server_sock, buf, 31, 0); buf[n] = '\0'; B = atol(buf);
    printf("Intercepted from server: B=%ld\n", B);
    sprintf(buf, "%ld", M); send(client_sock, buf, strlen(buf), 0);
    long K_with_client = modexp(A, Xm, p);
    long K_with_server = modexp(B, Xm, p);
    printf("Key with client (K1) = %ld\n", K_with_client);
    printf("Key with server (K2) = %ld\n", K_with_server);
    closesocket(client_sock);
    closesocket(server_sock);
    closesocket(listen_sock);
    WSACleanup();
    return 0;
}
