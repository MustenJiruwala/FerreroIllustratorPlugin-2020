#include "IllustratorSDK.h"
#include "EXTLogger.h"
#include "ArtworkUtil.h"
#include <chrono>
using namespace std::chrono;
#include <stdio.h>
#define kLogFileSize						5 * 1024 * 1024
#define kClassName		typeid(*this).name ()
#define kINFOValue		"INFO"
#define kDEBUGValue		"DEBUG"
#define kERRORValue		"ERROR"
#define kWARNValue		"WARN"
#define kTrueValue		"True"
#define kLogValue		"log"
#define kMaxRollCount						10
EXTLogger goTruEditEXTLogger;

FILE *EXTLogger::m_poLogFileStream = NULL;

EXTLogger::EXTLogger()
{
}

void EXTLogger::Initialize()
{
	do
	{
		string strLogFile = GetLogFilePath ();
		char acMode[] = "ab+";
		if (EXTLogger::m_poLogFileStream == NULL)
		{
			EXTLogger::m_poLogFileStream = OpenLogFile(strLogFile, acMode);
			if (EXTLogger::m_poLogFileStream == NULL)
				break;
			fseek(EXTLogger::m_poLogFileStream, 0, SEEK_END);
		}

		long lFileSize = ftell(EXTLogger::m_poLogFileStream);
		if (lFileSize >= kLogFileSize)
		{
			RollLogFile();
			EXTLogger::m_poLogFileStream = OpenLogFile(strLogFile, acMode);
		}
	} while (false);
}

string EXTLogger::GetLogFilePath()
{
	ArtworkUtil oUtil;
	string strLogFile = oUtil.GetUserFolder();
	strLogFile += kFilePathSeparator;
	strLogFile.append(kExtLogFileName);
	return strLogFile;
}
FILE * EXTLogger::OpenLogFile(string strLogFile, char arrAccessMode[])
{
	FILE *poLogFile = NULL;
#ifdef _WINDOWS
	fopen(&poLogFile, strLogFile.c_str(), arrAccessMode);
#else
    poLogFile = fopen(strLogFile.c_str(), arrAccessMode);
#endif
	return poLogFile;
}

EXTLogger::~EXTLogger()
{
	if (EXTLogger::m_poLogFileStream != NULL)
	{
		fflush(EXTLogger::m_poLogFileStream);
		fclose(EXTLogger::m_poLogFileStream);
	}
}

bool EXTLogger::Log(string strClassName, string strFunctionName, string strMessage)
{
	bool bSuccess = false;
#ifndef NOLOGS
	Initialize();
	if (EXTLogger::m_poLogFileStream != NULL)
	{
		string strLogMessage("");
		strLogMessage.append(BuildMessage(kINFOValue, strClassName, strFunctionName, strMessage));
		size_t nSize = fwrite(strLogMessage.c_str(), sizeof(char), strLogMessage.length(), EXTLogger::m_poLogFileStream);
		fflush(EXTLogger::m_poLogFileStream);
		bSuccess = true;
	}
#endif
	return bSuccess;
}

string EXTLogger::BuildMessage(string strLogType, string strClassName, string strFunctionName, string strMessage)
{
	string strLogMessage = "";
	strLogMessage.append(GetCurrentDateTime());
	strLogMessage.append("\t");
	strLogMessage.append(strLogType);
	strLogMessage.append("\t");
#ifdef MACINTOSH
	strLogMessage.append(strClassName);
	strLogMessage.append(" - ");
#endif
	strLogMessage.append(strFunctionName);
	if (!strMessage.size() == 0)
		strLogMessage.append(" : ");
	strLogMessage.append(strMessage);
	strLogMessage.append("\r\n");
	return strLogMessage;
}

string EXTLogger::GetCurrentDateTime()
{
	string strDateTime = "";
	milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	char pcBuffer[80];
	time_t oNow = ms.count()/1000;
	strftime(pcBuffer, sizeof pcBuffer, "%Y-%m-%d %H:%M:%S", gmtime(&oNow));
	int nMilliSec = ms.count() % 1000;
	char pcDateTime[90] = "";
	sprintf(pcDateTime, "%s.%03dZ", pcBuffer, nMilliSec);
	strDateTime = string(pcDateTime);

	return strDateTime;
}

void EXTLogger::RollLogFile()
{
	fclose(EXTLogger::m_poLogFileStream);
	string strBaseFileName = GetLogFilePath ();
	for (int nIndex = kMaxRollCount; nIndex >= 0; nIndex--)
	{
		string strRollFileName = strBaseFileName;
		if (nIndex != 0)
		{
			strRollFileName.append(".");
			strRollFileName += nIndex;
		}
		bool bFileExists = IsFileExists(strRollFileName);
		if (nIndex == kMaxRollCount && bFileExists)
			DeleteRollFile(strRollFileName);
		else if (bFileExists)
			RenameRollFileName(strRollFileName, nIndex + 1, kLogFileName);
	}
}

bool EXTLogger::IsFileExists(string strFileName)
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

void EXTLogger::DeleteRollFile(string strRollFileName)
{
	remove(strRollFileName.c_str());
}

void EXTLogger::RenameRollFileName(string strCurrentFileName, int nIndex, string strBaseFileName)
{
	string strNewFileName = strBaseFileName;
	strNewFileName.append(".");
	strNewFileName += nIndex;
	rename(strCurrentFileName.c_str(), strNewFileName.c_str());
}
