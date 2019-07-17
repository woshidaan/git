// UKeyLauncherDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "wtcplugin_i.h"
#include "parsexml.h"
#include "cloudenc.h"

#define MAX_LENGTH 512
#define BUFF_SIZE 512

#pragma comment(lib, "cloudenc")

extern "C" _declspec(dllexport) bool UKey_InitComComponents();
extern "C" _declspec(dllexport) int UKey_KoalLogin(const char *szPin);
extern "C" _declspec(dllexport) const char* UKey_GetUserName();
extern "C" _declspec(dllexport) bool UKey_IsAdmin(const char *szUserName);
extern "C" _declspec(dllexport) int UKey_CTLogin(const char *szUserName);
extern "C" _declspec(dllexport) void UKey_Config();
extern "C" _declspec(dllexport) void UKey_GetKeyStatus();
extern "C" _declspec(dllexport) void Taskbar_Show_Hide();
extern "C" _declspec(dllexport) void sendFailMessage(const char *szUserName);
// 认证控件对象
Iplugin *pAuthPlug;
BSTR bstrRes;
BSTR bstrPin/* = ::SysAllocString(L"")*/;
string strPin;
Json::Value root;
Json::Reader reader;
char vmip[20] = { 0 };


string GetModulePath() //获取应用程序文件夹名
{
	HMODULE module = GetModuleHandle(0);
	char pFileName[MAX_PATH];
	GetModuleFileName(module, pFileName, MAX_PATH);
	string csFullPath(pFileName);
	int nPos = csFullPath.rfind('\\');
	if (nPos == string::npos)
		return string("");
	else
		return csFullPath.substr(0, nPos);
}

void wr_Log(string p)//写日志
{
	FILE *fp;
	string strLogPath = "C:\\Users\\Public\\CloudTerm\\UKeyLauncher.log";
	struct _stat info;
	_stat(strLogPath.c_str(), &info);
	int size = info.st_size;
	//cout << size << endl;
	if (size > 1024 * 1024)
	{
		remove(strLogPath.c_str());
	}

	fopen_s(&fp, strLogPath.c_str(), _T("a+"));
	if (fp == NULL)
	{
		return;
	}
	SYSTEMTIME nowTime;
	GetLocalTime(&nowTime);
	char szTime[128];
	ZeroMemory(szTime, sizeof(szTime));
	sprintf_s(szTime, 128, "%04d-%02d-%02d %02d:%02d:%02d", nowTime.wYear, nowTime.wMonth, nowTime.wDay,
		nowTime.wHour, nowTime.wMinute, nowTime.wSecond);
	fprintf(fp, "%s\n%s\n", szTime, p.c_str());
	fclose(fp);
}

string getCCIP(const string &strNode){
	string strCCIpConfPath(GetModulePath());
	strCCIpConfPath = strCCIpConfPath.substr(0, strCCIpConfPath.rfind('\\'));
	strCCIpConfPath = strCCIpConfPath + TEXT("\\config.xml");
	string strIpTmp;
	///////////////////////////////////////////////////////////////////////////////
	TiXmlDocument dom(strCCIpConfPath.c_str());
	bool bSuccess = dom.LoadFile();
	if (!bSuccess)
	{
		TCHAR buf[BUFF_SIZE];
		sprintf_s(buf, BUFF_SIZE, "load config.xml failed: %s\n", dom.ErrorDesc());
		wr_Log(buf);
	}
	TiXmlElement *root = dom.RootElement();
	TiXmlElement *p = NULL;
	if (GetNodePointerByName(root, strNode, p))
	{
		strIpTmp = p->GetText();
	}
	return strIpTmp;
}

static size_t write_data(void* buffer, size_t size, size_t nmemb, void* stream)
{
	string* str = dynamic_cast<string*>((string *)stream);
	if (NULL == str || NULL == buffer)
	{
		return -1;
	}

	char* pData = (char*)buffer;
	str->append(pData, size * nmemb);
	return size * nmemb;
}

bool HttpByPost(string &strRes, string &url, string &post)
{
	string strHttpsUrl("http://");
	//string vmip = getCCIP("ccip");
	strHttpsUrl +=url;

	wr_Log(strHttpsUrl);
	///////////////////////////////////////////////////////////////////////////////////////////
	CURL *curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, strHttpsUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post.c_str());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); //对返回的数据进行操作的函数地址
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strRes); //这是write_data的第四个参数值

		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8);//连接超时，这个数值如果设置太短可能导致数据请求不到就断开了
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);//接收数据时超时设置，如果10秒内数据未接收完，直接退出

		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
		{
			TCHAR buf[BUFF_SIZE];
			sprintf_s(buf, 128, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			wr_Log(buf);
			return FALSE;
		}

		curl_easy_cleanup(curl);
	}
	curl_global_cleanup();

	return TRUE;
}

unsigned char char_to_hex(unsigned char x)
{
	return (unsigned char)(x > 9 ? x + 55 : x + 48);
}

int is_alpha_number_char(unsigned char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
		return 1;
	return 0;
}

//url编码实现 
void urlencode(unsigned char *src, int src_len, unsigned char *dest, int dest_len)
{
	unsigned char ch;
	int len = 0;

	while (len < (dest_len - 4) && *src)
	{
		ch = (unsigned char)*src;
		if (*src == ' ')
		{
			*dest++ = '+';
		}
		else if (is_alpha_number_char(ch) || strchr("-_.!~*'()", ch))
		{
			*dest++ = *src;
		}
		else
		{
			*dest++ = '%';
			*dest++ = char_to_hex((unsigned char)(ch >> 4));
			*dest++ = char_to_hex((unsigned char)(ch % 16));
		}
		++src;
		++len;
	}
	*dest = 0;

	return;
}

//kill进程from名字  
BOOL KillProcessFromName(string strProcessName)
{
	//创建进程快照(TH32CS_SNAPPROCESS表示创建所有进程的快照)  
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//PROCESSENTRY32进程快照的结构体  
	PROCESSENTRY32 pe;

	//实例化后使用Process32First获取第一个快照的进程前必做的初始化操作  
	pe.dwSize = sizeof(PROCESSENTRY32);

	//下面的IF效果同:  
	//if(hProcessSnap == INVALID_HANDLE_VALUE)   无效的句柄  
	if (!Process32First(hSnapShot, &pe))
	{
		return FALSE;
	}

	//将字符串转换为小写  
	char *pcProcessName = _strlwr(const_cast<char*>(strProcessName.c_str()));

	//如果句柄有效  则一直获取下一个句柄循环下去  
	while (Process32Next(hSnapShot, &pe))
	{
		//pe.szExeFile获取当前进程的可执行文件名称  
		string strTmp = pe.szExeFile;


		//将可执行文件名称所有英文字母修改为小写  
		char *pcTmp = _strlwr(const_cast<char*>(strTmp.c_str()));

		//比较当前进程的可执行文件名称和传递进来的文件名称是否相同  
		//相同的话Compare返回0  
		if (!strcmp(pcTmp, pcProcessName))
		{
			//从快照进程中获取该进程的PID(即任务管理器中的PID)  
			DWORD dwProcessID = pe.th32ProcessID;
			HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
			::TerminateProcess(hProcess, 0);
			CloseHandle(hProcess);
			return TRUE;
		}
	}
	return FALSE;
}

bool UKey_InitComComponents()
{
	// 初始化COM环境
	HRESULT hr = ::CoInitialize(NULL);

	if (SUCCEEDED(hr))
	{
		// 创建认证控件对象
		hr = ::CoCreateInstance(CLSID_plugin, NULL, CLSCTX_INPROC_SERVER, __uuidof(Iplugin), (void**)&pAuthPlug);
		if (SUCCEEDED(hr))
		{
			wr_Log("CoCreateInstance OK!");
		}
		else
		{
			wr_Log("CoCreateInstance failed!");
			return FALSE;
		}
	}
	else
	{
		wr_Log("CoInitialize failed!");
		return FALSE;
	}
	return TRUE;
}

int UKey_KoalLogin(const char *szPin)
{
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = { 0 };
	char pcRet[5] = { 0 };
	int returnCode = 0;//用于保存子程进的返回值;

	string strCMD(GetModulePath());
	strCMD += "\\HDPClient.exe " + string(szPin);

	TCHAR buf[BUFF_SIZE] = { 0 };
	sprintf_s(buf, BUFF_SIZE, "创建HDPClient进程:::%s", strCMD.c_str());
	wr_Log(buf);

	HANDLE g_hEvent = CreateEvent(NULL, FALSE, FALSE, _T("Global\\HDPClientReturn"));
	HANDLE hMapping = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, 256, "HDPClientReturnCode");
	LPVOID lpBase = MapViewOfFile(hMapping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);

	BOOL bRet = CreateProcess(NULL, const_cast<char*>(strCMD.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (bRet)
	{
		wr_Log("HDPClient.exe is running...");	
		//WaitForSingleObject(pi.hProcess, -1);//等待子进程结束
		//wr_Log("HDPClient.exe is finished...");	
		//GetExitCodeProcess(pi.hProcess, &returnCode);//获取子进程的返回值
		
		WaitForSingleObject(g_hEvent, INFINITE);
		CloseHandle(g_hEvent);

		strcpy(pcRet, (char*)lpBase);
		returnCode = atoi(pcRet);

		TCHAR buf[BUFF_SIZE];
		sprintf_s(buf, BUFF_SIZE, "HDPClient.exe return code:%s\n", pcRet);
		wr_Log(buf);
	}
	else
	{
		wr_Log("Create HDPClient.exe error!" );
		return -1;
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	return returnCode;
}

const char* UKey_GetUserName()
{
	wr_Log("UKey_GetUserName start!");
	char username[64] = { 0 };
	HANDLE hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "USERNAME");

	if (hMapping)
	{
		LPVOID lpBase = MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		strcpy(username, (char*)lpBase);
		UnmapViewOfFile(lpBase);
		CloseHandle(hMapping);
	}

	TCHAR buf[BUFF_SIZE];
	sprintf_s(buf, 128, "UserName: %s\n", username);
	wr_Log(buf);

	return username;
}

bool UKey_IsAdmin(const char *szUserName){
	string strPost, strRes, strUrl;
	strPost.clear();
	strUrl.clear();
	strRes.clear();

	string vmip = getCCIP("ccip");

	strUrl = vmip + "/cu/index.php/Home/Client/checkIsAdmin";
	strPost = "{\"name\":\"" + string(szUserName) + "\"}";

	if (HttpByPost(strRes, strUrl, strPost))
	{
		wr_Log(strRes);

		if (!reader.parse(strRes.c_str(), root))
		{
			wr_Log("Json parse failed!");
		}
		else
		{
			string status = root["status"].asString();

			if (status == "true")
				return true;
			else
				return false;
		}
	}
	else
	{
		wr_Log("HttpsByPost failed!");
	}
	return false;
}
void sendFailMessage(const char *szUserName)
{
	string strPost, strRes, strUrl;
	strPost.clear();
	strUrl.clear();
	strRes.clear();
	string vmip = getCCIP("ccip");
	strUrl = vmip + "/cu/index.php/Home/Client/ukeyLoginFailAddAudit"; 
	//strPost = "{\"username\":\"" + string(szUserName) + "\"}";
	strPost = "{\"username\":\"" + string(szUserName) + "\",\"status\":\"failed\"}";
	HttpByPost(strRes, strUrl, strPost);
}

int UKey_CTLogin(const char *szUserName){
	wr_Log("UKey_CTLogin start!");

	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = { 0 };

	string strCMD(GetModulePath());
	strCMD = strCMD.substr(0, strCMD.rfind("\\") + 1);
	strCMD += "pub_new_cloudbrowser.exe ";
	strCMD += szUserName;
	wr_Log(strCMD);

	return CreateProcess(NULL, const_cast<char*>(strCMD.c_str()), NULL, NULL, FALSE, 0, NULL, GetModulePath().c_str(), &si, &pi) ? 0 : (int)GetLastError();
}


SOCKET sock;
bool CheckLinkStatus()
{
	char msg[16] = { 0 };
	int err = 0;
	send(sock, "a", 1, 0);
	err = recv(sock, msg, 15,  0);

	if (err <= 0)
		return false;
	return true;

}

bool GetVMKeyState(string ip)
{
	if (ip == "")
	{
		wr_Log("vmip为空");
		return false;
	}
	Sleep(5000);
	if (!CheckLinkStatus())
	{
		wr_Log("未建立连接，准备建立连接！！！");
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (INVALID_SOCKET == sock)
			return false;

		SOCKADDR_IN m_peerAddr;
		m_peerAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
		m_peerAddr.sin_port = htons(60000);
		m_peerAddr.sin_family = AF_INET;
		connect(sock, (LPSOCKADDR)&m_peerAddr, sizeof(SOCKADDR));

	}

	string strSend = "VMKeyState";
	string strRecv;
	int iRet;

	iRet = send(sock, strSend.c_str(), strSend.length(), 0);
	if (iRet <= 0){
		return false;
	}
	int nNetTimeout = 5000;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&nNetTimeout, sizeof(int));
	char szBuf[16] = { 0 };
	iRet = recv(sock, szBuf, 16, 0);

	if (iRet <= 0)
	{
		wr_Log("recv<=0！！！");
		return false;
	}

	strRecv = szBuf;
	if (strRecv == "online")
	{
		wr_Log("online");
		return true;
	}
	wr_Log("offline");
	return false;
}



void UKey_GetKeyStatus()
{
	BSTR bstrR = ::SysAllocString(L"");
	bool bCTUkey = false, bVMUkey = false; //CT、VM中的UKey状态
	QianLong::BaseSocket *pBS = new QianLong::TcpSocket();
	//pBS->ASYNCConnect("172.20.156.232", 60000, 2);
	while (1)
	{
		pAuthPlug->tmGetKeyStatus(L"0", L"0", &bstrR);
		if (!wcscmp(bstrR, L"TM_SUCCESS"))
		{
			wr_Log("Ukey在CT！！！");
			bCTUkey = true;
			Sleep(2000);
		}
		else
		{
			bCTUkey = false;

			HANDLE hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, TEXT("VMIP"));
			if (hMapping)
			{
				LPVOID lpBase = MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
				strcpy(vmip, (char*)lpBase);
				UnmapViewOfFile(lpBase);
				CloseHandle(hMapping);
			}
			wr_Log("spice虚拟机IP::"+ string(vmip));

			bVMUkey = GetVMKeyState(vmip);

			if (!bVMUkey)
				bVMUkey = GetVMKeyState(vmip);

			TCHAR buf[BUFF_SIZE];
			sprintf_s(buf, BUFF_SIZE, "bVMUkey state:%d", bVMUkey);
			wr_Log(buf);

			if (!bVMUkey)
			{

				wr_Log("Ukey已拔出");
				//Sleep(1000);
				pAuthPlug->tmGetKeyStatus(L"0", L"0", &bstrR);
				if (wcscmp(bstrR, L"TM_SUCCESS"))
				{
					bCTUkey = false;
					::SendMessage(::FindWindow(NULL, "中云操作系统"), 0x0402, 0, 0);
					::SendMessage(::FindWindow(NULL, "pubBrowser"), WM_CLOSE, 0, 0);
					//::SendMessage(::FindWindow(NULL, "中云操作系统 - Cloud Browser"), WM_CLOSE, 0, 0);
					
					KillProcessFromName("remote-viewer.exe");
					KillProcessFromName("HDPClient.exe");
					/////////////////////////////////////////////////////////////////////////////////////////
					//string strPost, strRes, strUrl;
					//strPost.clear();
					//strUrl.clear();
					//strRes.clear();

					//string ccip = getCCIP("ccip");
					//strUrl = ccip + "/cu/index.php/Home/Client/sendMessageToAgent";
					//strPost = "{\"vmusername\":\"cac\",\"ip\":\"" + string(vmip) + "\",\"type\":\"logoff\"}";//{"vmusername":"cac","ip":"172.20.156.101","type":"logoff"}

					//if (HttpByPost(strRes, strUrl, strPost))
					//{
					//	wr_Log(strRes);
					//}
					//else
					//{
					//	wr_Log("HttpsByPost failed!");
					//}
					/////////////////////////////////////////////////////////////////////////////////
					Sleep(2000);
				}
				else
				{
					wr_Log("Ukey在VM中");
					bCTUkey = true;
				}
				
			}		
		}

		while (!bCTUkey && !bVMUkey)
		{
			wr_Log("Ukey未插入");
			pAuthPlug->tmGetKeyStatus(L"0", L"0", &bstrR);
			if (!wcscmp(bstrR, L"TM_SUCCESS"))
			{
				bCTUkey = true;
			}
			Sleep(2000);
		}		

	}
}
	
void Taskbar_Show_Hide()
{
	STARTUPINFO si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = { 0 };

	string strCMD(GetModulePath());
	strCMD += "\\TaskBarHider.exe -h";

	CreateProcess(NULL, const_cast<char*>(strCMD.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}

//void GetVMIP()
//{
//	////////////////读取spice虚拟机IP///////////////////////////////////////////////////////////////////////
//	//HANDLE g_hEvent = CreateEvent(NULL, FALSE, FALSE, _T("Global\\OPENSPICE"));
//	//WaitForSingleObject(g_hEvent, INFINITE);
//	//CloseHandle(g_hEvent);
//
//	HANDLE hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "VMIP");
//	if (hMapping)
//	{
//		LPVOID lpBase = MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
//		strcpy(vmip, (char*)lpBase);
//		UnmapViewOfFile(lpBase);
//		CloseHandle(hMapping);
//	}
//	wr_Log("spice虚拟机IP::" + string(vmip));
//}