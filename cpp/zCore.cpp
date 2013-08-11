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
	
	return Render(p_szTemplate);
}

zString CCore::Render(zString p_szTemplate)
{
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
			//ssReturn << szStream.substr(iPos);
			break;
		}
		iEnd = szStream.find(szEndTag, iStart);
		if(iEnd < 0)
			throw ("Corrupted file");
		//ssReturn << szStream.substr(iPos, iStart);
		ssReturn << Interpret(szStream.substr(iStart + szStartTag.size(), iEnd - (iStart + szEndTag.size() + 1)));
		//std::cout << "Result: " << Interpret(szStream.substr(iStart + szStartTag.size(), iEnd - (iStart + szEndTag.size() + 1))) << std::endl;
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
	//std::cout << "Stream: " << p_szStream << std::endl;

	std::string szKeyPrefix = m_mpSettings["KeyPrefix"];
	std::string szIncludePattern = m_mpSettings["IncludePattern"];
	//Search for includes
	{
		auto vResults = Parse("(include) $", p_szStream);
		if(vResults.size() > 0)
		{
	 		return Interpret(ReadFile(vResults[0].c_str()));
		}
	}

	//Search for value usages
	{
		auto vResults = Parse("^$", p_szStream);
		if(vResults.size() > 0)
		{
			zString szKey = vResults[0].c_str();
			if(m_mpValueMap.find(szKey) != m_mpValueMap.end())
				return m_mpValueMap.at(szKey);
			else
				return "";
		}
	}
	
	return p_szStream;
}
zString CCore::GetSetting(zString p_szName) const
{
	auto constMap = const_cast<StrMap*>(&m_mpSettings);
	return constMap->at(p_szName).c_str();
}

std::vector<std::string> CCore::Parse(const char* p_cszPattern, const std::string& p_crszText) const
{
	std::vector<std::string> vResults;
	bool bBlock = false;
	bool bInput = false;
	bool bNeedPrefix = false;
	std::stringstream ss;
	const std::string szKeyPrefix = GetSetting("KeyPrefix");
	for(const char& crSymbol : p_crszText)
	{
		//std::cout << "Symbol: " << crSymbol << " State: block - " << bBlock << " bInput - " << bInput << std::endl;
		if(!bBlock && *p_cszPattern == '(')
		{
			p_cszPattern++;
			bBlock = true;
		}

		if(bBlock && *p_cszPattern == ')')
		{
			p_cszPattern++;
			bBlock = false;
		}

		if(bBlock)
		{
			if(crSymbol != *(p_cszPattern++))
				break;
		}
		else
		{
			if(*p_cszPattern == ' ') //Just skip empty spaces in pattern at this point
				p_cszPattern++;

			if(*p_cszPattern == '$')
				bInput = true;

			if(*p_cszPattern == '^')
			{
				bNeedPrefix = true;
				p_cszPattern++;
			}
		}
		if(bNeedPrefix)
		{
			if(crSymbol == ' ')
				continue;
			else if(crSymbol == szKeyPrefix[0])
			{
				bNeedPrefix = false;
				continue;
			}
			else
				break;
		}
		if(bInput)
		{
			if(crSymbol == ' ' && ss.str().size() > 0)
			{
				bInput = false;
				p_cszPattern++;
				vResults.push_back(ss.str());
				ss.str("");
			}
			else if(crSymbol == ' ')
			{
				continue;
			}
			else
			{
				ss << crSymbol;
			}
		}

	}
	if(ss.str().size() > 0)
		vResults.push_back(ss.str());
	return vResults;
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