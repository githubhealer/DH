#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
long modexp(int base,int exp,int mod){
    long ans = 1;
    for(int i = 0 ; i < exp ; i++){
        ans = (ans%mod * base%mod)%mod;
    }
    return ans%mod;
}
int main(){
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2),&wsa);
    SOCKET sock = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(8080);
    bind(sock,(struct sockaddr*)&servaddr,sizeof(servaddr));
    listen(sock,5);
    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    while(1){
        SOCKET clientsock = accept(sock,(struct sockaddr*)&clientaddr,&clientlen);
        int flag = 1;
        setsockopt(clientsock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
        printf("Connected to client\n");
        char buf[32];
        int n;
        long A = 0;
        n = recv(clientsock, buf, 31, 0); buf[n] = '\0'; A = atol(buf);
        int g = 0;
        n = recv(clientsock, buf, 31, 0); buf[n] = '\0'; g = atoi(buf);
        int p = 0;
        n = recv(clientsock, buf, 31, 0); buf[n] = '\0'; p = atoi(buf);
        printf("Alice public key: %ld\nGenerator: %d\nMod: %d\n",A,g,p);
        int Xb = 4;
        long B = modexp(g,Xb,p);
        sprintf(buf, "%ld", B);
        send(clientsock, buf, strlen(buf), 0);
        long K = 0;
        K = modexp(A,Xb,p);
        printf("Shared Secret Key: %d",K);
        closesocket(clientsock);
    }
    closesocket(sock);
    WSACleanup();
    return 0;
}