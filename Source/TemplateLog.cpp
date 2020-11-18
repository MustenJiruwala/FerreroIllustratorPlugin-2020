#include "IllustratorSDK.h"
#include "TemplateLog.h"
#include "ArtworkUtil.h"
#include <chrono>
using namespace std::chrono;
#ifdef _WINDOWS
#define kFilePathSeparator '\\'
#else
#define kFilePathSeparator '/'
#endif
#define kCSVLogFileName "templateConversion.csv"
TemplateLog::TemplateLog()
{
	m_strTemplateID = "";
	m_strCategory = "";
	m_nLayers = 0;
	m_nObjects = 0;
	m_strStatus = "";
	m_strError = "";
}

void TemplateLog::SetTemplateID(string strTemplateId)
{
	m_strTemplateID = strTemplateId;
}

string TemplateLog::GetTemplateID()
{
	return m_strTemplateID;
}

void TemplateLog::SetCategory(string strCategory)
{
	m_strCategory = strCategory;
}

string TemplateLog::GetCategory()
{
	return m_strCategory;
}
void TemplateLog::SetNumberOfLayers(int nLayers)
{
	m_nLayers = nLayers;
}

int TemplateLog::GetNumberOfLayers()
{
	return m_nLayers;
}

void TemplateLog::SetNumberOfObjects(int nObjects)
{
	m_nObjects = nObjects;
}

int TemplateLog::GetNumberOfObjects()
{
	return m_nObjects;
}
void TemplateLog::SetStatus(string strStatus)
{
	m_strStatus = strStatus;
}

string TemplateLog::GetStatus()
{
	return m_strStatus;
}

void TemplateLog::SetErrorString(string strError)
{
	m_strError += strError + "\n";
}

string TemplateLog::GetError()
{
	return m_strError;
}

void TemplateLog::WriteCSVLog()
{
	do
	{
		string strCSVLogFile = GetLogFilePath(kCSVLogFileName);
		bool bAddHeader = !IsFileExists(strCSVLogFile);
		FILE * poLogFileStream = NULL;
		char acMode[] = "ab+";
		poLogFileStream = OpenLogFile(strCSVLogFile, acMode);
		if (poLogFileStream == NULL)
				break;
		fseek(poLogFileStream, 0, SEEK_END);
		string strLogMessage = BuildCSVMessage (bAddHeader);
		size_t nSize = fwrite(strLogMessage.c_str(), sizeof(char), strLogMessage.length(), poLogFileStream);
		fflush(poLogFileStream);
		fclose(poLogFileStream);
	} while (false);
}

void TemplateLog::WriteErrorLog()
{
	do
	{
		string strLogMessage = GetError();
		if (strLogMessage.length() == 0)
			break;
		string strFileName = GetTemplateID() + ".error";
		string strTemplateLogFile = GetLogFilePath(strFileName);
		FILE * poLogFileStream = NULL;
		char acMode[] = "ab+";
		poLogFileStream = OpenLogFile(strTemplateLogFile, acMode);
		if (poLogFileStream == NULL)
			break;
		size_t nSize = fwrite(strLogMessage.c_str(), sizeof(char), strLogMessage.length(), poLogFileStream);
		fflush(poLogFileStream);
		fclose(poLogFileStream);
	} while (false);
}

string TemplateLog::GetLogFilePath(string strLogFileName)
{
	ArtworkUtil oUtil;
	string strLogFile = oUtil.GetUserFolder();
	strLogFile += kFilePathSeparator;
	strLogFile.append(strLogFileName);
	return strLogFile;
}

FILE * TemplateLog::OpenLogFile(string strLogFile, char arrAccessMode[])
{
	FILE *poLogFile = NULL;
#ifdef _WINDOWS
	fopen(&poLogFile, strLogFile.c_str(), arrAccessMode);
#else
	poLogFile = fopen(strLogFile.c_str(), arrAccessMode);
#endif
	return poLogFile;
}

bool TemplateLog::IsFileExists(string strFileName)
{
	bool bExists = false;
	FILE *ptFile = NULL;
#ifdef _WINDOWS
	fopen(&ptFile, strFileName.c_str(), "r");
#else
	ptFile = fopen(strFileName.c_str(), "r");
#endif
	if (ptFile != NULL)
	{
		fclose(ptFile);
		bExists = true;
	}
	return bExists;
}
#define kDateTimeHeader "Date"
#define kCategoryHeader "Category"
#define kTemplateIDHeader "Template ID"
#define kNumberOfLayersHeader "Number of Layers"
#define kNumberOfObjectsHeader "Number of Objects"
#define kStatusHeader "Status"
string TemplateLog::BuildCSVMessage(bool bAddHeader)
{
	string strMessage = "";
	if (bAddHeader)
	{
		strMessage.append(kDateTimeHeader);
		strMessage.append(",");
		strMessage.append(kCategoryHeader);
		strMessage.append(",");
		strMessage.append(kTemplateIDHeader);
		strMessage.append(",");
		strMessage.append(kNumberOfLayersHeader);
		strMessage.append(",");
		strMessage.append(kNumberOfObjectsHeader);
		strMessage.append(",");
		strMessage.append(kStatusHeader);
		strMessage.append("\n");
	}
	strMessage.append(GetCurrentDateTime());
	strMessage.append(",");
	strMessage.append(GetCategory());
	strMessage.append(",");
	strMessage.append(GetTemplateID());
	strMessage.append(",");
	strMessage.append(to_string(GetNumberOfLayers()));
	strMessage.append(",");
	strMessage.append(to_string(GetNumberOfObjects()));
	strMessage.append(",");
	strMessage.append(GetStatus());
	strMessage.append("\n");
	return strMessage;
}

string TemplateLog::GetCurrentDateTime()
{
	string strDateTime = "";
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	char pcBuffer[80];
	time_t oNow = ms.count() / 1000;
	strftime(pcBuffer, sizeof pcBuffer, "%Y-%m-%d %H:%M:%S", gmtime(&oNow));
	int nMilliSec = ms.count() % 1000;
	char pcDateTime[90] = "";
	sprintf(pcDateTime, "%s.%03dZ", pcBuffer, nMilliSec);
	strDateTime = string(pcDateTime);
	return strDateTime;
}
