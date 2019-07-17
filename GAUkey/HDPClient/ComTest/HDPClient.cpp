// HDPClient.cpp : 定义控制台应用程序的入口点。
//2.1	云终端认证流程
//1、云终端用户插入登录UKEY、输入PIN码。
//2、云终端验证模块调用tmVerifyCode接口进行用户PIN码验证。
//3、验证通过后，分别调用如下接口：
//a)	tmGetUniqueTC，获取终端设备标记
//b)	tmGetKeyInfo、获取UKEY设备信息
//c)	tmGetData，获取客户端数据C1
//4、HTTPS方式调用2.3.1云终端认证请求接口，向云安全认证网关服务器发送验证请求，请求的参数为“步骤3”中获取的信息，获取服务器端返回的token值T1和random值S1。
//5、调用2.3.4服务器策略请求接口，从云安全认证网关服务器下载认证组件策略。
//6、调用tmSetSysParam接口设置认证组件策略。
//7、使用tmSignData获取签名数据，参数为：
//a)	bstrClientData：C1
//b)	bstrServerData：S1
//c)	bstrPin：PIN码
//然后调用2.3.2云终端身份验证接口，验证云终端身份，验证通过后服务器返回票据数据、SM4加密密钥KEY和IV。
//8、调用tmSetArtifact接口保存票据数据，并建立VPN安全通道。
//9、调用tmGetEncode接口加密PIN码，参数为：
//a)	bstrKey：KEY
//b)	bstrIV：IV
//c)	bstrPIN：PIN码
//然后调用2.3.3PIN码安全传输接口发送PIN码密文数据云安全认证网关服务器。
//10、云终端认证完成。


#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <comdef.h>
#include <string>
#include <algorithm>
#include <sys/stat.h> 
#include <sys/types.h> 
#include <json/json.h>
#include <curl/curl.h>

#include "wtcplugin_i.h"

#define MAX_PATH 260
#define BUFF_SIZE 512
#define MAX_LENGTH 512

#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )//不显示控制台
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
	string strLogPath = "C:\\Users\\Public\\Cloud\\Koal.log";

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
	sprintf_s(szTime,128, "%04d-%02d-%02d %02d:%02d:%02d", nowTime.wYear, nowTime.wMonth, nowTime.wDay,
		nowTime.wHour, nowTime.wMinute, nowTime.wSecond);
	fprintf(fp, "%s\n%s\n", szTime, p.c_str());
	fclose(fp);
}

/////////////////////////////////////////////////////////

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

int HttpsByPost(string &post, string &strRes, string &url)
{
///////////////////////////////////////////////////////////////////////////////////////
	string strHttpsUrl("https://");
	FILE *fp;
	char buf[BUFF_SIZE];
	ZeroMemory(buf, sizeof(buf));
	string strPath = GetModulePath();
	strPath = strPath.substr(0, strPath.rfind('\\')).substr(0, strPath.rfind('\\'));
	strPath = strPath + "\\conf\\koalconfig.ini";
	fopen_s(&fp, strPath.c_str(), _T("r"));
	if (fp == NULL)
	{
		wr_Log("koalconfig.ini读取失败！");
		return -1;
	}

	fread(buf, sizeof(char), BUFF_SIZE, fp);
	fclose(fp);
	strHttpsUrl += buf + url;

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
			printf("%s",buf);
			wr_Log(buf);
			return -1;
		}
		curl_easy_cleanup(curl);
	}

	curl_global_cleanup();
	return 0;
}



void _tmain(int argc, _TCHAR* argv[])
{
	// 认证控件对象
	Iplugin *pAuthPlug = NULL;

	// 初始化COM环境
	HRESULT hr=::CoInitialize(NULL);
	int retCode;

	if (SUCCEEDED(hr))
	{
		// 创建认证控件对象
		hr = ::CoCreateInstance(CLSID_plugin, NULL, CLSCTX_INPROC_SERVER, __uuidof(Iplugin), (void**)&pAuthPlug);

		if (SUCCEEDED(hr))
		{
			wr_Log("CoCreateInstance OK!");
			BSTR xxxx;
			pAuthPlug->tmGetKeyStatus(L"0", L"0", &xxxx);

			// PIN认证（此处需输入UKEY的实际PIN码）
			BSTR bstrPin = ::SysAllocString(_bstr_t(argv[1]));
			BSTR bstrRes;

			
			pAuthPlug->tmVerifyCode(bstrPin, &bstrRes);//成功返回TM_SUCCESS，PIN码错误，返回TM_FAILED,UKEY离线，返回TM_FAILED_KEY_NOFIND
													   //TM_SUCCESS,TM_FAILED,TM_FAILED_KEY_NOFIND均为宽字符串			
			if (!wcscmp(bstrRes,L"TM_SUCCESS"))
			{
				wr_Log("tmVerifyCode:TM_SUCCESS");

				BSTR bstrUniqueTCRes, bstrKeyInfoRes, bstrC1;
				string strUniqueTC, strKeyInfo, strC1, strT1;
				string strPost, strRes, strUrl;

				pAuthPlug->tmGetUniqueTC(&bstrUniqueTCRes);//获取云终端设备唯一标记,此接口当前返回的值为固定值，只区分Windows和Linux终端类型
				pAuthPlug->tmGetKeyInfo(&bstrKeyInfoRes);//获取UKEY设备的信息,成功返回设备的信息，失败返回TM_FAILED														
				pAuthPlug->tmGetData(&bstrC1);//生成随机数,返回生成的32字节随机数

				size_t i;
				char buff[MAX_LENGTH];
				wcstombs_s(&i,buff, bstrUniqueTCRes, MAX_LENGTH);
				strUniqueTC=buff;
				wcstombs_s(&i, buff, bstrKeyInfoRes, MAX_LENGTH);
				strKeyInfo = buff;
				wcstombs_s(&i, buff, bstrC1, MAX_LENGTH);
				strC1 = buff;
		
				::SysFreeString(bstrUniqueTCRes);
				::SysFreeString(bstrKeyInfoRes);
				//::SysFreeString(bstrC1);

				Json::Value root;
				Json::Reader reader;

				//云终端认证请求
				BSTR bstrS1 = ::SysAllocString(L"");
				strPost.clear();
				strUrl.clear();
				strRes.clear();
				strUrl = "/sign/request?data=" + strC1 + "&tc=" + strUniqueTC + "&key=" + strKeyInfo;
				
				if (HttpsByPost(strPost,strRes,strUrl) == 0)
				{
					wr_Log(strRes);
					
					if (!reader.parse(strRes.c_str(), root))
					{
						wr_Log("Json parse failed!");
					}
					else
					{						
						strT1 = root["token"].asString();//{"result":"false", "code":"错误信息代码"}
						string strS1 = root["random"].asString();
						bstrS1 = _bstr_t(strS1.c_str());
					}
				}
				else
				{
					wr_Log("HttpsByPost failed!");
				}

				//服务器策略请求
				//UKEY策略
				BSTR bstrUkey = ::SysAllocString(L"");
				strPost.clear();
				strUrl.clear();
				strRes.clear();
				strUrl = "/sysconfig/request?data=usbkey&extdata=" + strUniqueTC;
				if (HttpsByPost(strPost, strRes, strUrl) == 0)
				{
					//cout << strRes << endl;
					wr_Log(strRes);

					if (!reader.parse(strRes.c_str(), root))
					{
						wr_Log("Json parse failed!");
					}
					else
					{
						string strUkey = root["udev"].asString();
						bstrUkey = _bstr_t(strUkey.c_str());
						pAuthPlug->tmSetSysParam(L"usbkey", bstrUkey, &bstrRes);
					}
				}
				else
				{
					wr_Log("HttpsByPost failed!");
				}

				// VPN策略
				BSTR bstrVpn = ::SysAllocString(L""), bstrVpnStatus = ::SysAllocString(L"");
				strPost.clear();
				strRes.clear();
				strUrl = "/sysconfig/request?data=vpn&extdata=";
				if (HttpsByPost(strPost, strRes, strUrl) == 0)
				{
					wr_Log(strRes);

					if (!reader.parse(strRes.c_str(), root))
					{
						wr_Log("Json parse failed!");
					}
					else
					{
						string strVpn = root["ip"].asString();//{"port":"xxxxx","status":"1","type":"xxxxx","ip":"xxxxx"}
						string strVpnStatus = root["status"].asString();
						bstrVpn = _bstr_t(strVpn.c_str());
						pAuthPlug->tmSetSysParam(L"vpn", bstrVpn, &bstrRes);
						bstrVpnStatus = _bstr_t(strVpnStatus.c_str());
						pAuthPlug->tmSetSysParam(L"vpnstatus", bstrVpnStatus, &bstrRes);
						
					}
				}
				else
				{
					wr_Log("HttpsByPost failed!");
				}

				// 用户类型策略
				BSTR bstrUserType = ::SysAllocString(L"");
				strPost.clear();
				strRes.clear();
				strUrl = "/sysconfig/request?data=usertype&extdata=";
				if (HttpsByPost(strPost, strRes, strUrl) == 0)
				{
					wr_Log(strRes);

					if (!reader.parse(strRes.c_str(), root))
					{
						wr_Log("Json parse failed!");
					}
					else
					{
						string strUserType = root["LDAP_SEARCH_FIELDS"].asString();
						strUserType = strUserType.substr(1,2);
						bstrUserType = _bstr_t(strUserType.c_str());
						pAuthPlug->tmSetSysParam(L"usertype", bstrUserType, &bstrRes);
						
					}
				}
				else
				{
					wr_Log("HttpsByPost failed!");
				}	
				
				pAuthPlug->tmSignData(bstrC1, bstrS1, bstrPin, &bstrRes);
				string strSignData = (_bstr_t)bstrRes;

				wr_Log(strSignData);
				/*BSTR bstrUser = ::SysAllocString(L"");
				pAuthPlug->tmGetUser(&bstrUser);


				char pcUser[MAX_LENGTH] = { 0 };
				wcstombs_s(&i, pcUser, bstrUser, MAX_LENGTH);

				TCHAR buf[BUFF_SIZE] = { 0 };
				sprintf_s(buf, BUFF_SIZE, "UserName:::%s", pcUser);
				wr_Log(buf);*/

				////将用户名写入内存映射
				//int nRetCode = 0;			
				//HANDLE hMapping = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, 4096, "ShareMemory");
				//LPVOID lpBase = MapViewOfFile(hMapping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
				//strcpy((char*)lpBase, pcUser);

				//云终端身份验证
				BSTR bstrTicket = ::SysAllocString(L""), bstrIv = ::SysAllocString(L""), bstrKey = ::SysAllocString(L"");
				string strTicket;
				strPost.clear();
				strUrl.clear();
				strRes.clear();
				strUrl = "/sign/verify";

				string strP = strSignData + "&token=" + strT1;
	
				char szURLPost[4096] = { 0 };
				urlencode((unsigned char*)strP.c_str(), strlen(strP.c_str()), (unsigned char*)szURLPost, sizeof(szURLPost));
				strPost = "data=";
				strPost += szURLPost;

				wr_Log(strPost);
				//cout << strPost <<endl;
				if (HttpsByPost(strPost, strRes, strUrl) == 0)
				{
					wr_Log(strRes);

					if (!reader.parse(strRes.c_str(), root))
					{
						wr_Log("Json parse failed!");
					}
					else
					{
						strTicket = root["ticket"].asString();
						string strIv = root["iv"].asString();
						string strKey = root["key"].asString();
						bstrTicket = _bstr_t(strTicket.c_str());
						bstrIv = _bstr_t(strIv.c_str());
						bstrKey = _bstr_t(strKey.c_str());
					}
				}
				else
				{
					wr_Log("HttpsByPost failed!");
				}

				pAuthPlug->tmSetArtifact(bstrTicket, L"1", &bstrRes);//保留票据数据，启动VPN服务和云安全认证网关建立VPN通道
				pAuthPlug->tmGetUser(&bstrRes);
				string username = (_bstr_t)bstrRes;

				HANDLE hMapping = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, 256, "USERNAME");
				if (hMapping)
				{
					LPVOID lpBase = MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
					strcpy((char*)lpBase, username.c_str());
					/*UnmapViewOfFile(lpBase);
					CloseHandle(hMapping);*/
				}

				pAuthPlug->tmGetEncode(bstrKey, bstrIv, bstrPin, &bstrRes);//使用SM4算法加密PIN码数据
				string strEncodePin = (_bstr_t)bstrRes;

				//PIN码安全传输;
				strPost.clear();
				strUrl.clear();
				strRes.clear();
				strUrl = "/sign/userencodedata?ticket=" + strTicket + "&pin=" + strEncodePin;
				if (HttpsByPost(strPost, strRes, strUrl) == 0)
				{
					wr_Log(strRes);

					if (!reader.parse(strRes.c_str(), root))
					{
						wr_Log("Json parse failed!");
					}
					else
					{
						string strResult = root["result"].asString();
						
					}
				}
				else
				{
					wr_Log("HttpsByPost failed!");
				}
				retCode = HDP_SUCCESS;
			}
			else if (!wcscmp(bstrRes, L"TM_FAILED"))
			{
				wr_Log("tmVerifyCode:TM_FAILED");
				retCode = HDP_PIN_ERROR;
			}
			else if (!wcscmp(bstrRes, L"TM_FAILED_KEY_NOFIND"))
			{
				wr_Log("tmVerifyCode:TM_FAILED_KEY_NOFIND");
				retCode = HDP_UKEY_OFFLINE;
			}
			else
			{
				wr_Log("tmVerifyCode:UNDEFINE");
				retCode = HDP_UNDEFINE;
			}

			::SysFreeString(bstrRes);	

		}
		else
		{
			wr_Log("CoCreateInstance failed!");
		}
	
	}
	else
	{
		wr_Log("CoInitialize failed!");
	}
	

	char pcRet[5];
	_itoa(retCode, pcRet, 10);

	TCHAR buf[BUFF_SIZE] = { 0 };
	sprintf_s(buf, BUFF_SIZE, "HDPClient中:::%s", pcRet);
	wr_Log(buf);	
	//////////////////////
	HANDLE hMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, "HDPClientReturnCode");
	if (hMapping)
	{
		LPVOID lpBase = MapViewOfFile(hMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		char szBuffer[20] = { 0 };
		strcpy((char*)lpBase, pcRet);
		UnmapViewOfFile(lpBase);
		CloseHandle(hMapping);
	}

	HANDLE g_hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, _T("Global\\HDPClientReturn"));
	SetEvent(g_hEvent);
	SignalObjectAndWait(g_hEvent, NULL, INFINITE, FALSE);
	//if (!g_hEvent)
	//{
	//	cout << "error!" << endl;
	//	return 0;
	//}

	if (retCode > 0)
		return;
	Sleep(INFINITE);

	return ;
}
