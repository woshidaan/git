#include "stdafx.h"

QianLong::BaseSocket::BaseSocket():m_lisSock(INVALID_SOCKET),m_conSock(INVALID_SOCKET){
	WSAStartup(MAKEWORD(2,2),&m_wsaData);
	ZeroMemory(&m_localAddr,sizeof(SOCKADDR_IN));
	ZeroMemory(&m_peerAddr,sizeof(SOCKADDR_IN));
}

QianLong::BaseSocket::~BaseSocket(){
	WSACleanup();
}