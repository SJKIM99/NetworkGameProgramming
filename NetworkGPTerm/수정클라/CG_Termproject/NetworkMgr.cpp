#include "std.h"
#include "NetworkMgr.h"

//const char* SERVERIP = "127.0.0.1";

NetworkMgr::NetworkMgr()
{
    sock = -1;
}

NetworkMgr::~NetworkMgr()
{
    closesocket(sock);
    WSACleanup();
}

bool NetworkMgr::Init()
{
    std::cout << "ip주소를 입력하세요 : ";
    char ip[20] = {};
    std::cin >> ip;
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        return false;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == sock) {
        //err_quit("socket()");
        std::cout << "socket error" << std::endl;
    }

    struct sockaddr_in serveraddr;
    ZeroMemory(&serveraddr, sizeof serveraddr);
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serveraddr.sin_addr);
    serveraddr.sin_port = htons(SERVERPORT);
    int retval = connect(sock, reinterpret_cast<struct sockaddr*>(&serveraddr), sizeof serveraddr);
    if (SOCKET_ERROR == retval) {
        std::cout << "connect error" << std::endl;
        return false;
    }

    // TCP_NODELAY 옵션 활성화
   /* int flag = 1;
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int));*/
    std::cout << "Init socket Success !!" << std::endl;
    return true;
}

void NetworkMgr::SendPacket(char* packet, int size)
{
    send(sock, reinterpret_cast<char*>(&size), sizeof(int), 0);
    send(sock, packet, size, 0);
}
//
//void NetworkMgr::RecvPacket(HANDLE hThread)
//{ 
//    int len = 0;
//    char buf[BUFSIZE];
//    recv(sock, (char*)&len, sizeof(int), MSG_WAITALL);
//    recv(sock, buf, len, MSG_WAITALL);
//
//    switch (buf[0]) {
//    case SC_LOGIN_OK: {
//        SC_LOGIN_OK_PACKET* packet = reinterpret_cast<SC_LOGIN_OK_PACKET*>(buf);
//        std::cout << "[플레이어입장]  " << packet->name << "님이 입장하였습니다." << std::endl;
//    }
//        break;
//    case SC_READY_OK: {
//        SC_READY_OK_PACKET* packet = reinterpret_cast<SC_READY_OK_PACKET*>(buf);
//        std::cout << "[플레이어레디]  " << packet->name << "님이 레디하였습니다." << std::endl;
//    }
//   
//    }
//}


