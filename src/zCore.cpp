#include "zCore.h"
#include "Util.h"
#include <iostream>
#include <fstream>
#include <sstream> 
using namespace zRender;

CCore theApp;

CCore::CCore()
{
	m_mpSettings["StartTag"] = "<<";
	m_mpSettings["EndTag"] = ">>";
	m_mpSettings["KeyPrefix"] = "$";
}

CCore::~CCore()
{

}

StrMap& CCore::GetSettings()
{
	return m_mpSettings;
}

zString CCore::Render(zString p_szTemplate, Param p_Parameters)
{
	//Handle parameters
	Param* pParam = &p_Parameters;
	while(pParam != nullptr)
	{
		m_mpValueMap[pParam->m_szKey] = pParam->m_szVal;
		pParam = pParam->m_pNext;
	}

	std::string szStream = ReadFile(p_szTemplate);
	std::string szStartTag = m_mpSettings["StartTag"];
	std::string szEndTag = m_mpSettings["EndTag"];

	std::stringstream ssReturn;
	int iPos = 0;
	while(true)
	{
		int iStart = 0;
		int iEnd = 0;
		iStart = szStream.find(szStartTag, iPos);
		if(iStart < 0)
		{
			//Add left-overs
			ssReturn << szStream.substr(iPos);
			break;
		}
		iEnd = szStream.find(szEndTag, iStart);
		if(iEnd < 0)
			throw ("Corrupted file");
		ssReturn << szStream.substr(iPos, iStart);
		ssReturn << Interpret(szStream.substr(iStart + szStartTag.size(), iEnd - (iStart + szEndTag.size() + 1)));
		iPos = iEnd;
	}
	
	return ssReturn.str().c_str();
}

std::string CCore::ReadFile(zString p_szName) const
{
	std::ifstream in(p_szName, std::ios::in | std::ios::binary);
	if (in)
	{
		std::string szContent;
		in.seekg(0, std::ios::end);
		szContent.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&szContent[0], szContent.size());
		in.close();
		return szContent;
	}
	return "";
}

std::string CCore::Interpret(std::string p_szStream)
{
	//Trim
	p_szStream = Util::Trim(p_szStream);

	std::string szKeyPrefix = m_mpSettings["KeyPrefix"];
	int iPos = p_szStream.find(szKeyPrefix);
	if(iPos >= 0)
	{
		zString szKey = p_szStream.substr(iPos + szKeyPrefix.size()).c_str();
		if(m_mpValueMap.find(szKey) != m_mpValueMap.end())
			return m_mpValueMap.at(szKey);
		else
			return "";

	}
	return p_szStream;
}
//Methods forwarding
EXPORT void SetSetting(zString p_szKey, zString p_szValue)
{
	auto& rSettings = theApp.GetSettings();
	rSettings[p_szKey] = p_szValue;
}

EXPORT zString GetSetting(zString p_szKey)
{
	return theApp.GetSettings()[p_szKey].c_str();
}

EXPORT zString Render(zString p_szTemplate, zRender::Param p_Parameters)
{
	return theApp.Render(p_szTemplate, p_Parameters);
}