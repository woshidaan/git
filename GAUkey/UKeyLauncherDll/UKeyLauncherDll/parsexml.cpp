#include "parsexml.h"

bool GetNodePointerByName(TiXmlElement* pRootEle, const std::string &strNodeName, TiXmlElement* &Node)
{
	// 假如等于根节点名，就退出
	if (strNodeName == pRootEle->Value())
	{
		Node = pRootEle;
		return true;
	}

	TiXmlElement* pEle = pRootEle;
	for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())
	{
		//递归处理子节点
		if (GetNodePointerByName(pEle, strNodeName, Node))
		{
			return true;
		}
	}

	return false;
}