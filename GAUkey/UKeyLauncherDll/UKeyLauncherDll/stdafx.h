// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�: 
#include <windows.h>
#include "tlhelp32.h" 

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <string>
#include <tchar.h>
#include <comdef.h>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <json/json.h>
#include <curl/curl.h>
#include <tinyxml/tinystr.h>
#include <tinyxml/tinyxml.h>
#include "CBaseSocket.h"
#include "CTcpSocket.h"

using namespace std;