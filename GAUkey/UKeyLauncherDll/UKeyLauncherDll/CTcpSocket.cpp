#include "stdafx.h"

using namespace QianLong;

TcpSocket::TcpSocket(){
	
}

TcpSocket::~TcpSocket(){
	if(INVALID_SOCKET != m_conSock)
		closesocket(m_conSock);
	if(INVALID_SOCKET != m_lisSock)
		closesocket(m_lisSock);
}

errno_t TcpSocket::Listen(const string &strLocalIP , const USHORT usPort , const USHORT usListenNums , const bool bReuseAddr ){
	if(INVALID_SOCKET == (m_lisSock = socket(AF_INET,SOCK_STREAM,0)))
		return WSAGetLastError();
	m_localAddr.sin_addr.S_un.S_addr = ("ANY" == strLocalIP ? htonl(INADDR_ANY) : inet_addr(strLocalIP.c_str()));
	m_localAddr.sin_port = htons(usPort);
	m_localAddr.sin_family = AF_INET;

	//if(bReuseAddr)
	//	;//设置地址复用

	if (bind(m_lisSock,(LPSOCKADDR)&m_localAddr,sizeof(SOCKADDR)))
		return WSAGetLastError();

	if(listen(m_lisSock,usListenNums))
		return WSAGetLastError();

	return 0;
}

errno_t TcpSocket::Accept(const bool bPeerAddr ){
	int iLen = sizeof(SOCKADDR);
	m_conSock = bPeerAddr ? accept(m_lisSock,(LPSOCKADDR)&m_peerAddr,&iLen) : accept(m_lisSock,NULL,NULL);
	return INVALID_SOCKET == m_conSock ? WSAGetLastError() : 0;
}

errno_t TcpSocket::SYNCConnect(const string strPeerIP , const USHORT usPort ){
	m_conSock = socket(AF_INET,SOCK_STREAM,0);
	if(INVALID_SOCKET == m_conSock)
		return WSAGetLastError();
	
	m_peerAddr.sin_addr.S_un.S_addr = inet_addr(strPeerIP.c_str());
	m_peerAddr.sin_port = htons(usPort);
	m_peerAddr.sin_family = AF_INET;

	return connect(m_conSock,(LPSOCKADDR)&m_peerAddr,sizeof(SOCKADDR)) ? 0 : WSAGetLastError();
}


errno_t TcpSocket::ASYNCConnect(const string strPeerIP , const USHORT usPort , const USHORT usWaitTime ){
	SOCKET sock = socket(AF_INET,SOCK_STREAM,0);
	if(INVALID_SOCKET == sock)
		return WSAGetLastError();

	m_peerAddr.sin_addr.S_un.S_addr = inet_addr(strPeerIP.c_str());
	m_peerAddr.sin_port = htons(usPort);
	m_peerAddr.sin_family = AF_INET;

	u_long ulBlock = 0,ulUnBlock = 1;

	//设置套接字为非阻塞模式
	if(ioctlsocket(sock,FIONBIO,&ulUnBlock))
		return -1;

	if(connect(sock,(LPSOCKADDR)&m_peerAddr,sizeof(SOCKADDR))){
		FD_SET set;
		FD_ZERO(&set);
		FD_SET(sock,&set);

		TIMEVAL stTime;
		stTime.tv_sec = usWaitTime;
		stTime.tv_usec = 0;

		if(SOCKET_ERROR != select(0,NULL,&set,NULL,&stTime)){
			if(FD_ISSET(sock,&set)){
				//将套接字改回阻塞模式
				if(ioctlsocket(sock,FIONBIO,&ulBlock))
					return -1;
				m_conSock = sock;
				return 0;
			}
			return -1;
		}
		return -1;
	}
	//将套接字改回阻塞模式
	if(ioctlsocket(sock,FIONBIO,&ulBlock))
		return -1;
	m_conSock = sock;
	return 0;
}

errno_t TcpSocket::SendCommand(const string &strCom){
	if(128 < strCom.length())
		return -2;
	int iRet = send(m_conSock,strCom.c_str(),strCom.length(),0);
	if(SOCKET_ERROR == iRet){
		return WSAGetLastError();
	}else if(0 == iRet){
		return -1;
	}
	return 0;
}

errno_t TcpSocket::RecvCommand(string &strCom){
	char szBuf[m_usComLength] = {0};
	int iRet = recv(m_conSock,szBuf,m_usComLength - 1,0);
	if(SOCKET_ERROR == iRet){
		return WSAGetLastError();
	}else if(0 == iRet){
		return -1;
	}
	strCom = szBuf;
	return 0;
}

errno_t TcpSocket::SendLongCommand(const string &strCom){
	int iRet = send(m_conSock,strCom.c_str(),strCom.length(),0);
	if(SOCKET_ERROR == iRet){
		return WSAGetLastError();
	}else if(0 == iRet){
		return -1;
	}
	return 0;
}

errno_t TcpSocket::SendFile(const string &strFilePath, const bool bBinary ){
	fstream file;
	file.open(strFilePath.c_str(),(bBinary ? (std::ios::in | std:: ios::binary) : (std::ios::in)));
	if(!file.is_open())
		return GetLastError();

	file.seekg(0,std::ios::end);
	fstream::pos_type fileSize = (long)file.tellg();
	file.seekg(0,std::ios::beg);

	//发送文件长度
	int iRet = SendStruct(fileSize);
	if(iRet){
		file.close();
		return iRet;
	}

	//循环发送文件
	char szBuf[m_uiDataLength] = {0};
	while(!file.eof()){
		file.read(szBuf,m_uiDataLength - 1);
		iRet = send(m_conSock,szBuf,(int)file.gcount(),0);
		if(SOCKET_ERROR == iRet){
			return WSAGetLastError();
		}else if(0 == iRet){
			return -1;
		}
		ZeroMemory(szBuf,m_uiDataLength);
	}
	if(!file.good())
		file.clear();

	file.close();

	return 0;
}

errno_t TcpSocket::RecvFile(const string &strFilePath, const bool bBinary ){
	fstream file;
	file.open(strFilePath.c_str(),(bBinary ? (std::ios::out | std::ios::binary) : (std::ios::out)));
	if(!file.is_open())
		return GetLastError();

	//接收文件长度
	fstream::pos_type fileSize = 0,count = 0;
	int iRet = RecvStruct(fileSize);
	if(iRet){
		file.close();
		return iRet;
	}

	//循环接收文件
	char szBuf[m_uiDataLength] = {0};
	while(count != fileSize){
		iRet = recv(m_conSock,szBuf,m_uiDataLength - 1,0);
		if(SOCKET_ERROR == iRet){
			return WSAGetLastError();
		}else if(0 == iRet){
			return -1;
		}
		file.write(szBuf,iRet);
		ZeroMemory(szBuf,m_uiDataLength);
		count += iRet;
	}

	file.close();

	return 0;
}

errno_t TcpSocket::GetLocalIP(vector<string> &strLocalIP){
	string strHostName;
	int iRet = GetHostName(strHostName);
	if(iRet)
		return WSAGetLastError();

	hostent *pHost = gethostbyname(strHostName.c_str());
	if(!pHost)
		return WSAGetLastError();
	SOCKADDR_IN tempAddr = {0};
	for(int i = 0;pHost->h_addr_list[i];++i){
		memcpy_s(&tempAddr.sin_addr,sizeof(tempAddr.sin_addr),pHost->h_addr_list[i],pHost->h_length);
		strLocalIP.push_back(inet_ntoa(tempAddr.sin_addr));
	}

	return 0;
}

errno_t TcpSocket::GetConnectedLocalAddr(string &strLocalIP,USHORT &usPort){
	SOCKADDR_IN tempAddr = {0};
	int iLen = sizeof(SOCKADDR);
	if(getsockname(m_conSock,(LPSOCKADDR)&tempAddr,&iLen))
		return WSAGetLastError();
	strLocalIP = inet_ntoa(tempAddr.sin_addr);
	usPort = ntohs(tempAddr.sin_port);
	return 0;
}