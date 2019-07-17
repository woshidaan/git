#include "parsexml.h"

bool GetNodePointerByName(TiXmlElement* pRootEle, const std::string &strNodeName, TiXmlElement* &Node)
{
	// ������ڸ��ڵ��������˳�
	if (strNodeName == pRootEle->Value())
	{
		Node = pRootEle;
		return true;
	}

	TiXmlElement* pEle = pRootEle;
	for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())
	{
		//�ݹ鴦���ӽڵ�
		if (GetNodePointerByName(pEle, strNodeName, Node))
		{
			return true;
		}
	}

	return false;
}