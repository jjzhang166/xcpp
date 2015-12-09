#include "XAfx.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>

#include "XLog.h"
#include "XSettings.h"
#include "XEString.h"
#define XSETTING_LOG XLOG_PRINT


CXSettings::CXSettings()
{
	XSettingSection emptyMap;
	m_mapVal.insert(XSETTING_MAP::value_type("", emptyMap));
	m_iterRead = m_mapVal.begin();
	m_iterWrite = m_iterRead;
}

CXSettings::~CXSettings()
{
}

BOOL CXSettings::Load(const char* pszSettingFile, UINT nMaxLine)
{
	_ASSERT(pszSettingFile);

	std::fstream file_in(pszSettingFile, std::ios_base::in);

	if (file_in.is_open())
	{
		std::string line;
		while (getline(file_in, line))
		{
			size_t lineSize = line.size();

			if (lineSize > nMaxLine)
			{
				XSETTING_LOG(LOG_WARNING, "<%s>: Read setting files while input string too long(%d), drop it", __FUNCTION__, line.size());
				continue;
			}
			if (lineSize == 0)
			{
				continue;
			}

			char* pszLine = (char*)_alloca(lineSize + 1);
			if (pszLine == NULL)
			{
				XSETTING_LOG(LOG_ERROR, "Read setting files alloca failed", line.size());
				_ASSERT(FALSE);
				return FALSE;
			}

			XStrCpyNA(pszLine, line.c_str(), lineSize+1);

			char* pSzKey = XStrTrimA(pszLine);


			switch (pSzKey[0])
			{
				//comment & empty;
			case EOS:
			case '#':
			case ';':
				continue;

				//section
			case '[':
				{
					char* pszSetionNameEnd = strchr(pSzKey, ']');
					if (pszSetionNameEnd == NULL)
					{
						XSETTING_LOG(LOG_WARNING, "<%s>:Section name need ']'", __FUNCTION__);
						continue;
					}

					std::string sectionName(pSzKey+1, pszSetionNameEnd-1-pSzKey);
					XSETTING_MAP::iterator iter = m_mapVal.find(sectionName);
					if (iter != m_mapVal.end())
					{
						XSETTING_LOG(LOG_WARNING, "<%s>:Section name(%s) repeated, setting may be overide \']\'", __FUNCTION__, sectionName.c_str());
						m_iterRead = iter;
						continue;
					}
					XSettingSection emptyMap;
					std::pair<XSETTING_MAP::iterator, bool> newIter = m_mapVal.insert(XSETTING_MAP::value_type(sectionName, emptyMap));
					m_iterRead = newIter.first;
				}
				break;

				//key-value
			default:
				{
					char* pSep = strchr(pSzKey, '=');
					if (pSep == NULL)
					{
						XSETTING_LOG(LOG_WARNING, "<%s>:Section name(%)->(%s) not type of key=value",
							__FUNCTION__, m_iterRead->first.c_str(), pSzKey);
					}
					else
					{
						char* pszValue = pSep + 1;
						*pSep = EOS;
						char* pszValidKey = XStrTrimA(pSzKey);
						char* pszValidValue = XStrTrimA(pszValue);

						std::string key(pszValidKey);
						std::string value(pszValidValue);
						m_iterRead->second.insert(XSettingSection::value_type(key, value));
					}
				}
				break;
			}
		}
		file_in.close();
		return TRUE;
	}
	return FALSE;
}

void CXSettings::DumpAll()
{
	int count = EnumValue();
	UNUSED(count);
}

BOOL CXSettings::enumFuncPrint(const std::string& secionName, const std::string& key, std::string& value)
{
	printf("[%s]<%s>=%s\n", secionName.c_str(), key.c_str(), value.c_str());
	return TRUE;
}

int CXSettings::EnumValue(ENUM_SETTING_FUNC pFunc)
{
	if (NULL == pFunc)
		return 0;

	//TODO::
	int iCount = 0;
	XSETTING_MAP::iterator mapciter = m_mapVal.begin();
	for (; mapciter != m_mapVal.end(); ++mapciter)
	{
		XSettingSection& sectionName = mapciter->second;
		for (XSettingSection::iterator iter = sectionName.begin();
			iter != sectionName.end(); iter++)
		{
			iCount++;
			if (!(*pFunc)(mapciter->first, iter->first, iter->second))
			{
				break;
			}
		}
	}

	return iCount;
}

BOOL CXSettings::Save(const char* pszSettingFile)
{
	_ASSERT(pszSettingFile);
	std::fstream file_out(pszSettingFile, std::ios_base::out|std::ios::trunc);
	if (file_out.is_open())
	{
		XSETTING_MAP::const_iterator mapciter = m_mapVal.begin();
		const XSettingSection& section = mapciter->second;
		if (mapciter->second.size() > 0)
		{
			//HEADER
			for (XSettingSection::const_iterator iter = section.begin();
				iter != section.end(); iter++)
			{
				file_out << iter->first << '=' << iter->second << std::endl;
			}
		}

		++mapciter;
		for (; mapciter != m_mapVal.end(); ++mapciter)
		{
			const XSettingSection& sectionNew = mapciter->second;
			file_out << '[' << mapciter->first << ']' << std::endl;
			for (XSettingSection::const_iterator iter = sectionNew.begin();
				iter != sectionNew.end(); iter++)
			{
				file_out << iter->first << '=' << iter->second << std::endl;
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CXSettings::SwitchSectionRead(const char* pszSectionName) const
{
	_ASSERT(pszSectionName);
	XSETTING_MAP::iterator iter = m_mapVal.find(pszSectionName);
	if (iter == m_mapVal.end())
	{
		m_iterRead = m_mapVal.begin();
		return FALSE;
	}

	m_iterRead = iter;
	return TRUE;
}

BOOL CXSettings::SwitchSectionWrite(const char* pszSectionName)
{
	_ASSERT(pszSectionName);
	XSETTING_MAP::iterator iter = m_mapVal.find(pszSectionName);
	if (iter == m_mapVal.end())
	{
		m_iterWrite = m_mapVal.begin();
		return FALSE;
	}

	m_iterWrite = iter;
	return TRUE;
}

BOOL CXSettings::AddSection(const char* pszSectionName)
{
	_ASSERT(pszSectionName);
	XSETTING_MAP::iterator iter = m_mapVal.find(pszSectionName);
	if (iter == m_mapVal.end())
	{
		XSettingSection emptySection;
		std::pair<XSETTING_MAP::iterator, bool> iter = m_mapVal.insert(XSETTING_MAP::value_type(pszSectionName, emptySection));
		m_iterWrite = iter.first;
		return TRUE;
	}

	m_iterWrite = iter;
	return FALSE;
}

const CXSettings::XSettingSection* CXSettings::GetSecionPtrR(const char* pszKey) const
{
	//C++中，引用不能改变指向，所以这里使用指针
	const XSettingSection* pSection = &(m_iterRead->second);
	const char* pSectionSep = strchr(pszKey, ':');

	if (pSectionSep != NULL)
	{
		std::string sectionName(pszKey, pSectionSep - pszKey);
		XSETTING_MAP::const_iterator mapciter = m_mapVal.find(sectionName);
		if (mapciter == m_mapVal.end())
		{
			//TODO:
			XSETTING_LOG(LOG_WARNING, "<%s>:query(%s), not found\n", __FUNCTION__, pszKey);
			return NULL;
		}
		pSection = &(mapciter->second);
	}
	return pSection;
}

CXSettings::XSettingSection* CXSettings::GetSecionPtrW(const char* pszKey)
{
	//C++中，引用不能改变指向，所以这里使用指针
	XSettingSection* pSection = &(m_iterWrite->second);
	const char* pSectionSep = strchr(pszKey, ':');

	if (pSectionSep != NULL)
	{
		std::string sectionName(pszKey, pSectionSep - pszKey);
		XSETTING_MAP::iterator mapciter = m_mapVal.find(sectionName);
		if (mapciter == m_mapVal.end())
		{
			//TODO:
			XSettingSection emptySection;
			std::pair<XSETTING_MAP::iterator, bool> iter = m_mapVal.insert(XSETTING_MAP::value_type(sectionName, emptySection));
			pSection = &(iter.first->second);
		}
		else
		{
			pSection = &(mapciter->second);
		}
	}
	return pSection;
}

int CXSettings::GetInt(const char* pKey, int iDefault) const
{
	const XSettingSection* pSection = GetSecionPtrR(pKey);
	if (NULL == pSection)
	{
		return iDefault;
	}

	std::map<std::string, std::string>::const_iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		return iDefault;
	}

	const char* pValue = iter->second.c_str();
	int ret = iDefault;
	SSCANF_S(pValue, "%d", &ret);
	return ret;
}

INT64 CXSettings::GetInt64(const char* pKey, INT64 i64Default) const
{
	_ASSERT(pKey);
	const XSettingSection* pSection = GetSecionPtrR(pKey);
	if (NULL == pSection)
	{
		return i64Default;
	}

	std::map<std::string, std::string>::const_iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		return i64Default;
	}

	const char* pValue = iter->second.c_str();
	INT64 ret = i64Default;
	SSCANF_S(pValue, "%lld", (long long int*)&ret);
	return ret;
}

BOOL CXSettings::GetBoolean(const char* pKey, BOOL bDefault) const
{
	_ASSERT(pKey);
	const XSettingSection* pSection = GetSecionPtrR(pKey);
	if (NULL == pSection)
	{
		return bDefault;
	}

	std::map<std::string, std::string>::const_iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		return bDefault;
	}

	const char* pValue = iter->second.c_str();
	if (_stricmp(pValue, "Yes") == 0 || _stricmp(pValue, "True") == 0 || _stricmp(pValue, "Ok") == 0)
		return TRUE;

	return FALSE;
}

UINT CXSettings::GetUint(const char* pKey, UINT nDefault) const
{
	_ASSERT(pKey);
	const XSettingSection* pSection = GetSecionPtrR(pKey);
	if (NULL == pSection)
	{
		return nDefault;
	}

	std::map<std::string, std::string>::const_iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		return nDefault;
	}

	const char* pValue = iter->second.c_str();
	UINT ret = 0;
	SSCANF_S(pValue, "%u", &ret);
	return ret;
}

double CXSettings::GetDouble(const char* pKey, double dDefault) const
{
	_ASSERT(pKey);
	const XSettingSection* pSection = GetSecionPtrR(pKey);
	if (NULL == pSection)
	{
		return dDefault;
	}

	std::map<std::string, std::string>::const_iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		return dDefault;
	}

	const char* pValue = iter->second.c_str();
	double ret = .0;
	if (1 != SSCANF_S(pValue, "%lf", &dDefault))
	{
		return dDefault;
	}
	return ret;
}

float CXSettings::GetFloat(const char* pKey, float fDefault) const
{
	_ASSERT(pKey);
	const XSettingSection* pSection = GetSecionPtrR(pKey);
	if (NULL == pSection)
	{
		return fDefault;
	}

	std::map<std::string, std::string>::const_iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		return fDefault;
	}

	const char* pValue = iter->second.c_str();
	float ret = .0;
	if (1 != SSCANF_S(pValue, "%f", &ret))
	{
		return fDefault;
	}
	return ret;
}

std::string CXSettings::GetString(const char* pKey, const char* pszDefault) const
{
	_ASSERT(pKey);
	const XSettingSection* pSection = GetSecionPtrR(pKey);
	if (NULL == pSection)
	{
		return std::string(pszDefault);
	}

	std::map<std::string, std::string>::const_iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		return std::string(pszDefault);
	}
	return std::string(iter->second);
}

BOOL CXSettings::SetInt(const char* pKey, int iValue)
{
	_ASSERT(pKey);
	XSettingSection* pSection = GetSecionPtrW(pKey);

	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		char szValue[32] = { 0 };
		SPRINTF_S(szValue, sizeof(szValue), "%d", iValue);
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, szValue));
		return TRUE;
	}
	return FALSE;
}

BOOL CXSettings::SetBoolean(const char* pKey, BOOL bValue)
{
	_ASSERT(pKey);
	XSettingSection* pSection = GetSecionPtrW(pKey);

	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		char szValue[32] = { 0 };
		SPRINTF_S(szValue, sizeof(szValue), "%s", bValue?"Yes":"No");
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, szValue));
		return TRUE;
	}
	return FALSE;
}

BOOL CXSettings::SetUint(const char* pKey, UINT nValue)
{
	_ASSERT(pKey);
	XSettingSection* pSection = GetSecionPtrW(pKey);

	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		char szValue[32] = { 0 };
		SPRINTF_S(szValue, sizeof(szValue), "%u", nValue);
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, szValue));
		return TRUE;
	}
	return FALSE;
}

BOOL CXSettings::SetInt64(const char* pKey, INT64 i64Value)
{
	_ASSERT(pKey);
	XSettingSection* pSection = GetSecionPtrW(pKey);

	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		char szValue[32] = { 0 };
		SPRINTF_S(szValue, sizeof(szValue), "%lld", (long long)i64Value);
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, szValue));
		return TRUE;
	}
	return FALSE;
}

BOOL CXSettings::SetString(const char* pKey, const char* pValue)
{
	_ASSERT(pKey);
	_ASSERT(pValue);

	XSettingSection* pSection = GetSecionPtrW(pKey);

	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, pValue));
		return TRUE;
	}
	return FALSE;
}

BOOL CXSettings::SetDouble(const char* pKey, double dValue)
{
	_ASSERT(pKey);
	XSettingSection* pSection = GetSecionPtrW(pKey);

	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		char szValue[32] = { 0 };
		_SPRINTFEX(szValue, sizeof(szValue), "%lf", (long long)dValue);
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, szValue));
		return TRUE;
	}
	return FALSE;
}

BOOL CXSettings::SetFloat(const char* pKey, float fValue)
{
	_ASSERT(pKey);
	XSettingSection* pSection = GetSecionPtrW(pKey);

	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		char szValue[32] = { 0 };
		_SPRINTFEX(szValue, sizeof(szValue), "%f", fValue);
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, szValue));
		return TRUE;
	}
	return FALSE;
}

BOOL CXSettings::SetIntForce(const char* pKey, int iValue)
{
	_ASSERT(pKey);
	XSettingSection* pSection = GetSecionPtrW(pKey);

	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	char szValue[32] = { 0 };
	SPRINTF_S(szValue, sizeof(szValue), "%d", iValue);
	if (iter == pSection->end())
	{
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, szValue));
		return TRUE;
	}
	else
	{
		iter->second = szValue;
	}
	return FALSE;
}

BOOL CXSettings::SetBooleanForce(const char* pKey, BOOL bValue)
{
	XSettingSection* pSection = GetSecionPtrW(pKey);

	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	char szValue[32] = { 0 };
	SPRINTF_S(szValue, sizeof(szValue), "%s", bValue ? "Yes" : "No");

	if (iter == pSection->end())
	{
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, szValue));
		return TRUE;
	}
	else
	{
		iter->second = szValue;
	}
	return FALSE;
}

BOOL CXSettings::SetUintForce(const char* pKey, UINT nValue)
{
	XSettingSection* pSection = GetSecionPtrW(pKey);

	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	char szValue[32] = { 0 };
	SPRINTF_S(szValue, sizeof(szValue), "%u", nValue);
	if (iter == pSection->end())
	{
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, szValue));
		return TRUE;
	}
	else
	{
		iter->second = szValue;
	}
	return FALSE;
}

BOOL CXSettings::SetInt64Force(const char* pKey, INT64 i64Value)
{
	_ASSERT(pKey);
	XSettingSection* pSection = GetSecionPtrW(pKey);
	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	char szValue[32] = { 0 };
	SPRINTF_S(szValue, sizeof(szValue), "%lld", (long long int)i64Value);
	if (iter == pSection->end())
	{
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, szValue));
		return TRUE;
	}
	else
	{
		iter->second = szValue;
	}
	return FALSE;
}

BOOL CXSettings::SetStringForce(const char* pKey, const char* pValue)
{
	_ASSERT(pKey);
	_ASSERT(pValue);
	XSettingSection* pSection = GetSecionPtrW(pKey);

	std::map<std::string, std::string>::iterator iter = pSection->find(pKey);
	if (iter == pSection->end())
	{
		pSection->insert(std::map<std::string, std::string>::value_type(pKey, pValue));
		return TRUE;
	}
	else
	{
		iter->second = pValue;
	}
	return FALSE;
}

///CXCfgFile.cpp
#include "XSettings.h"
#define LINE_BUFFER 512

///public
CXCfgFile::CXCfgFile()
{
}

CXCfgFile::~CXCfgFile()
{
}

bool CXCfgFile::Read(const std::string& file_path)
{
	bool ret = false;

	if (file_path.empty())
	{
		return ret;
	}

	std::fstream file_in(file_path.c_str(), std::ios_base::in);
	if (file_in.is_open())
	{
		std::string str, left, right;
		unsigned int len, pos;
		char szLine[LINE_BUFFER] = { 0 };
		while ( file_in.getline(szLine, sizeof(szLine)) )
		{
			len = strlen(szLine);
			m_data.push_back(szLine);

			str.assign(szLine, len);
			trim_left_right(str);
			if (str[0] != '#')
			{
				pos = str.find('=');
				left = str.substr(0, pos);
				right = str.substr(pos + 1);
				trim_left_right(left);
				trim_left_right(right);
				m_map.insert(std::pair<std::string, std::string>(left, right));
			}

			memset(szLine, 0, sizeof(szLine));
		}
		file_in.close();
		ret = true;
	}

	return ret;
}

bool CXCfgFile::Save(const std::string& file_path)
{
	bool ret = false;

	std::fstream file_out(file_path.c_str(), std::ios_base::out);
	if (file_out.is_open())
	{
		//std::vector<std::string>::iterator iter;
		//for (iter = m_data.begin(); iter != m_data.end(); ++iter)
		//{
		//	std::string str = (*iter);
		//	file_out << str << std::endl;
		//}
		std::map<std::string, std::string>::iterator iter = m_map.begin();
		for (; iter!=m_map.end(); ++iter)
		{
			file_out << iter->first << "=" << iter->second << std::endl;
		}
		file_out.close();
		ret = true;
	}

	return ret;
}

std::string CXCfgFile::get_string(const std::string& key, const char* pDefaultValue)
{
	std::string str = "";
	std::map<std::string, std::string>::iterator iter = m_map.find(key);
	if (iter != m_map.end())
	{
		str = iter->second;
	}
	else
	{
		str = std::string(pDefaultValue);
	}
	return str;
}

unsigned int CXCfgFile::get_int(const std::string& key, UINT nDefaultValue)
{
	unsigned int value = 0;

	std::string str = get_string(key);
	if (!str.empty())
	{
		value = atoi(str.c_str());
		return value;
	}
	return nDefaultValue;
}

BOOL CXCfgFile::get_boolean(const std::string& key, BOOL bDefault)
{
	BOOL bRet = bDefault;

	std::string str = get_string(key);
	if (!str.empty())
	{
		const char* pStr = str.c_str();
		if (pStr)
		{
			if (_stricmp(pStr, "yes") == 0 || _stricmp(pStr, "true") == 0)
			{
				bRet = TRUE;
			}
			else
			{
				bRet = FALSE;
			}
		}
	}
	return bRet;
}

bool CXCfgFile::set_string(const std::string& key, const std::string& value)
{
	bool ret = false;

	//	if (!m_map.empty())
	{
		m_map[key] = value;
		set_vector_string(key, value);
		ret = true;
	}

	return ret;
}

bool CXCfgFile::set_int(const std::string& key, const unsigned int value)
{
	bool ret = false;

	//if (!m_map.empty())
	{
		char szValue[32] = { 0 };
#ifdef OS_WIN
		sprintf_s(szValue, "%u", value);
#else
		sprintf(szValue, "%u", value);
#endif
		m_map[key] = szValue;
		set_vector_string(key, szValue);
		ret = true;
	}

	return ret;
}

bool CXCfgFile::set_boolean(const std::string& key, const BOOL bValue)
{
	bool ret = false;

	//if (!m_map.empty())
	{
		char szValue[32] = { 0 };
#ifdef OS_WIN
		sprintf_s(szValue, "%s", bValue?"Yes":"No");
#else
		sprintf(szValue, "%s", bValue?"Yes":"No");
#endif
		m_map[key] = szValue;
		set_vector_string(key, szValue);
		ret = true;
	}

	return ret;
}

///private
void CXCfgFile::trim_left_right(std::string &str)
{
	if (!str.empty())
	{
		int i;
		int len = str.size();
		for (i = 0; i<len; i++)
		{
			if (str[i] != ' ')
			{
				break;
			}
		}
		str.erase(0, i);
		len = str.size();
		for (i = len - 1; i>0; i--)
		{
			if (str[i] != ' ')
			{
				break;
			}
		}
		str.erase(i + 1, len);
	}
}

void CXCfgFile::set_vector_string(const std::string& key, const std::string& value)
{
	//	if (!m_data.empty() && !key.empty())
	{
		std::vector<std::string>::iterator iter;
		std::string str;
		for (iter = m_data.begin(); iter != m_data.end(); ++iter)
		{
			str = *iter;
			trim_left_right(str);
			int pos1 = str.find('=');

			int pos2 = (*iter).find('=');
			int len = (*iter).size();

			std::string substr = str.substr(0, pos1);
			trim_left_right(substr);
			std::string value2 = " " + value;
			if (str[0] != '#' && substr == key)
			{
				(*iter).replace(pos2 + 1, len - pos2 - 1, value2);
			}
		}
	}
}