#pragma once

#define _CRT_SECURE_NO_WARNINGS // 구형 C 함수 사용 시 경고 끄기
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 구형 소켓 API 사용 시 경고 끄기

#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더

#include <tchar.h> // _T(), ...
#include <stdio.h> // printf(), ...
#include <stdlib.h> // exit(), ...
#include <string.h> // strncpy(), ...

#pragma comment(lib, "ws2_32") // ws2_32.lib 링크
#include "protocol.h"
//#include "C:\Users\user\Desktop\넷겜플텀프\NGP_TermProject\NetworkGPTerm\NetGamePrgm/error.h"

//#define SERVERPORT 9000
//#define BUFSIZE 512
//#define NAMESIZE 20
//

class NetworkMgr
{
	SOCKET sock;

public:
	NetworkMgr();
	~NetworkMgr();

	SOCKET GetSocket() { return sock; }
	bool Init();

public:
	void SendPacket(char* packet, int size);
};

