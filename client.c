#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
int gcd(int a,int b){
    if(b==0){
        return a;
    }
    return gcd(b,a%b);
}
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
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    if(connect(sock,(struct sockaddr*)&addr,sizeof(addr))==0){
        printf("Connected to server\n");
    }
    int flag = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag));
    int g;
    printf("Enter g: ");
    scanf("%d",&g);
    int p;
    printf("Enter p: ");
    scanf("%d",&p);
    if(gcd(g,p)!=1){
        printf("Wrong primtive root");
        return 1;
    }
    int Xa = 3;
    long A = modexp(g,Xa,p);
    char buf[32];
    sprintf(buf, "%ld", A); send(sock, buf, strlen(buf), 0); Sleep(1);
    sprintf(buf, "%d",  g); send(sock, buf, strlen(buf), 0); Sleep(1);
    sprintf(buf, "%d",  p); send(sock, buf, strlen(buf), 0);
    printf("SENT TO SERVER\n");
    long B = 0;
    char rbuf[32];
    int n = recv(sock, rbuf, 31, 0); rbuf[n] = '\0';
    B = atol(rbuf);
    long K = 0;
    K = modexp(B,Xa,p);
    printf("Shared Secret Key: %d",K);
    return 0;
}