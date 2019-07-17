// HDPClient.cpp : �������̨Ӧ�ó������ڵ㡣
//2.1	���ն���֤����
//1�����ն��û������¼UKEY������PIN�롣
//2�����ն���֤ģ�����tmVerifyCode�ӿڽ����û�PIN����֤��
//3����֤ͨ���󣬷ֱ�������½ӿڣ�
//a)	tmGetUniqueTC����ȡ�ն��豸���
//b)	tmGetKeyInfo����ȡUKEY�豸��Ϣ
//c)	tmGetData����ȡ�ͻ�������C1
//4��HTTPS��ʽ����2.3.1���ն���֤����ӿڣ����ư�ȫ��֤���ط�����������֤��������Ĳ���Ϊ������3���л�ȡ����Ϣ����ȡ�������˷��ص�tokenֵT1��randomֵS1��
//5������2.3.4��������������ӿڣ����ư�ȫ��֤���ط�����������֤������ԡ�
//6������tmSetSysParam�ӿ�������֤������ԡ�
//7��ʹ��tmSignData��ȡǩ�����ݣ�����Ϊ��
//a)	bstrClientData��C1
//b)	bstrServerData��S1
//c)	bstrPin��PIN��
//Ȼ�����2.3.2���ն������֤�ӿڣ���֤���ն���ݣ���֤ͨ�������������Ʊ�����ݡ�SM4������ԿKEY��IV��
//8������tmSetArtifact�ӿڱ���Ʊ�����ݣ�������VPN��ȫͨ����
//9������tmGetEncode�ӿڼ���PIN�룬����Ϊ��
//a)	bstrKey��KEY
//b)	bstrIV��IV
//c)	bstrPIN��PIN��
//Ȼ�����2.3.3PIN�밲ȫ����ӿڷ���PIN�����������ư�ȫ��֤���ط�������
//10�����ն���֤��ɡ�


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

#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )//����ʾ����̨
string GetModulePath() //��ȡӦ�ó����ļ�����
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

void wr_Log(string p)//д��־
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

//url����ʵ�� 
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
		wr_Log("koalconfig.ini��ȡʧ�ܣ�");
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
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); //�Է��ص����ݽ��в����ĺ�����ַ
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strRes); //����write_data�ĵ��ĸ�����ֵ
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		
		
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8);//���ӳ�ʱ�������ֵ�������̫�̿��ܵ����������󲻵��ͶϿ���
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);//��������ʱ��ʱ���ã����10��������δ�����ֱ꣬���˳�

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
	// ��֤�ؼ�����
	Iplugin *pAuthPlug = NULL;

	// ��ʼ��COM����
	HRESULT hr=::CoInitialize(NULL);
	int retCode;

	if (SUCCEEDED(hr))
	{
		// ������֤�ؼ�����
		hr = ::CoCreateInstance(CLSID_plugin, NULL, CLSCTX_INPROC_SERVER, __uuidof(Iplugin), (void**)&pAuthPlug);

		if (SUCCEEDED(hr))
		{
			wr_Log("CoCreateInstance OK!");
			BSTR xxxx;
			pAuthPlug->tmGetKeyStatus(L"0", L"0", &xxxx);

			// PIN��֤���˴�������UKEY��ʵ��PIN�룩
			BSTR bstrPin = ::SysAllocString(_bstr_t(argv[1]));
			BSTR bstrRes;

			
			pAuthPlug->tmVerifyCode(bstrPin, &bstrRes);//�ɹ�����TM_SUCCESS��PIN����󣬷���TM_FAILED,UKEY���ߣ�����TM_FAILED_KEY_NOFIND
													   //TM_SUCCESS,TM_FAILED,TM_FAILED_KEY_NOFIND��Ϊ���ַ���			
			if (!wcscmp(bstrRes,L"TM_SUCCESS"))
			{
				wr_Log("tmVerifyCode:TM_SUCCESS");

				BSTR bstrUniqueTCRes, bstrKeyInfoRes, bstrC1;
				string strUniqueTC, strKeyInfo, strC1, strT1;
				string strPost, strRes, strUrl;

				pAuthPlug->tmGetUniqueTC(&bstrUniqueTCRes);//��ȡ���ն��豸Ψһ���,�˽ӿڵ�ǰ���ص�ֵΪ�̶�ֵ��ֻ����Windows��Linux�ն�����
				pAuthPlug->tmGetKeyInfo(&bstrKeyInfoRes);//��ȡUKEY�豸����Ϣ,�ɹ������豸����Ϣ��ʧ�ܷ���TM_FAILED														
				pAuthPlug->tmGetData(&bstrC1);//���������,�������ɵ�32�ֽ������

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

				//���ն���֤����
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
						strT1 = root["token"].asString();//{"result":"false", "code":"������Ϣ����"}
						string strS1 = root["random"].asString();
						bstrS1 = _bstr_t(strS1.c_str());
					}
				}
				else
				{
					wr_Log("HttpsByPost failed!");
				}

				//��������������
				//UKEY����
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

				// VPN����
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

				// �û����Ͳ���
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

				////���û���д���ڴ�ӳ��
				//int nRetCode = 0;			
				//HANDLE hMapping = CreateFileMapping(NULL, NULL, PAGE_READWRITE, 0, 4096, "ShareMemory");
				//LPVOID lpBase = MapViewOfFile(hMapping, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
				//strcpy((char*)lpBase, pcUser);

				//���ն������֤
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

				pAuthPlug->tmSetArtifact(bstrTicket, L"1", &bstrRes);//����Ʊ�����ݣ�����VPN������ư�ȫ��֤���ؽ���VPNͨ��
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

				pAuthPlug->tmGetEncode(bstrKey, bstrIv, bstrPin, &bstrRes);//ʹ��SM4�㷨����PIN������
				string strEncodePin = (_bstr_t)bstrRes;

				//PIN�밲ȫ����;
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
	sprintf_s(buf, BUFF_SIZE, "HDPClient��:::%s", pcRet);
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
