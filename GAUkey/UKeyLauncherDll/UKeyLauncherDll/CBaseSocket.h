/****************************************************************
*	[功能描述] BaseSocket类
*	作为TCP套接字、UDP套接字、RAW套接字的基类
*	包含基本的网络函数、参数变量、结构体等
*	虚基类，不可实例化对象
*	命名空间QianLong
*
*	作者:QianLong 
*	时间:2014-10-04
*	版本:V1.0
*
*****************************************************************
*				QL All Rights Reserved
****************************************************************/

#pragma once

#include <WinSock2.h>
#include <fstream>
#include <string>
#include <vector>

#pragma comment(lib,"ws2_32")

using std::fstream;
using std::string;
using std::vector;

namespace QianLong{
	class BaseSocket{
	public:
		BaseSocket();
		virtual ~BaseSocket();

		/************************************************************************/
		/* 监听封装函数,可选是否地址复用                                        */
		/************************************************************************/
		virtual errno_t Listen(const string &strLocalIP = "ANY"/*监听的IP,默认为INADDRANY*/,
							   const USHORT usPort = 8888/*监听端口,默认8888*/,
							   const USHORT usListenNums = 5/*监听数,默认为5*/,
							   const bool bReuseAddr = false/*地址复用,默认false*/) = 0;

		/************************************************************************/
		/* 监听端的接受连接封装函数,可选是否记录对端地址信息                    */
		/************************************************************************/
		virtual errno_t Accept(const bool bPeerAddr = true/*是否记录对端地址信息,默认true*/) = 0;

		/************************************************************************/
		/* 阻塞式连接函数的封装函数                                             */
		/************************************************************************/
		virtual errno_t SYNCConnect(const string strPeerIP = "127.0.0.1"/*要连接的对端IP,默认127.0.0.1*/,
									const USHORT usPort = 8888/*要连接的对端端口,默认8888*/) = 0;

		/************************************************************************/
		/* 非阻塞式连接函数的封装函数                                           */
		/************************************************************************/
		virtual errno_t ASYNCConnect(const string strPeerIP = "127.0.0.1"/*要连接的对端IP,默认127.0.0.1*/,
									 const USHORT usPort = 8888/*要连接的对端端口,默认8888*/,
									 const USHORT usWaitTime = 5/*超时时间,默认5秒*/) = 0;

		/************************************************************************/
		/* 发送结构体,模板函数                                                  */
		/************************************************************************/
		template <typename T>
		errno_t SendStruct(T &elem){
			int iRet = send(m_conSock,(char*)&elem,sizeof(T),0);
			if(SOCKET_ERROR == iRet){
				return WSAGetLastError();
			}else if(0 == iRet){
				return -1;
			}
			return 0;
		}

		/************************************************************************/
		/* 接收结构体,模板函数                                                  */
		/************************************************************************/
		template <typename T>
		errno_t RecvStruct(T &elem){
			int iRet = recv(m_conSock,(char*)&elem,sizeof(T),0);
			if(SOCKET_ERROR == iRet){
				return WSAGetLastError();
			}else if(0 == iRet){
				return -1;
			}
			return 0;
		}

		/************************************************************************/
		/* 发送命令,命令长度不得超过128字节                                     */
		/************************************************************************/
		virtual errno_t SendCommand(const string &strCom) = 0;

		/************************************************************************/
		/* 接收命令,命令长度不得超过128字节                                     */
		/************************************************************************/
		virtual errno_t RecvCommand(string &strCom) = 0;

		/************************************************************************/
		/* 发送长命令                                                           */
		/************************************************************************/
		virtual errno_t SendLongCommand(const string &strCom) = 0;

		/************************************************************************/
		/* 发送文件,可选文件是否以二进制打开                                    */
		/************************************************************************/
		virtual errno_t SendFile(const string &strFilePath/*文件路径*/,
								 const bool bBinary = true/*是否以二进制打开,默认为true*/) = 0;

		/************************************************************************/
		/* 接收文件,可选文件是否以二进制打开                                    */
		/************************************************************************/
		virtual errno_t RecvFile(const string &strFilePath/*文件存放路径*/,
								 const bool bBinary = true/*是否以二进制打开,默认为true*/) = 0;

		/************************************************************************/
		/* 获取本机IP,结果以vector<string>的格式返回                            */
		/************************************************************************/
		virtual errno_t GetLocalIP(vector<string> &strLocalIP) = 0;

		/************************************************************************/
		/* 获取连接成功的本地IP                                                 */
		/************************************************************************/
		virtual errno_t GetConnectedLocalAddr(string &strLocalIP,USHORT &usPort) = 0;

		/************************************************************************/
		/* 获取已连接的套接字,内联函数                                          */
		/************************************************************************/
		SOCKET GetConnectedSocket() const{
			return m_conSock;
		}

		/************************************************************************/
		/* 使用已知的套接字设置连接套接字,内联函数                              */
		/************************************************************************/
		void SetConnectedSocket(const SOCKET conSock){
			m_conSock = conSock;
		}

		/************************************************************************/
		/* 获取本机主机名,内联函数                                              */
		/************************************************************************/
		errno_t GetHostName(string &strHostName){
			char szHost[256] = {0};
			if(gethostname(szHost,sizeof(szHost)))
				return WSAGetLastError();
			strHostName = szHost;
			return 0;
		}
	protected:
		WSADATA m_wsaData;
		SOCKET m_lisSock,m_conSock;
		SOCKADDR_IN m_localAddr,m_peerAddr;
		static const USHORT m_usComLength = 129;
		static const UINT m_uiDataLength = 8193;
	};
}