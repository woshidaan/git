// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件: 
#include <windows.h>
#include "tlhelp32.h" 

// TODO:  在此处引用程序需要的其他头文件
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