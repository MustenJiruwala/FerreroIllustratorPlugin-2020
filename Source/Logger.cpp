#include "IllustratorSDK.h"
#include "Logger.h"
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
Logger goTruEditLogger;

FILE *Logger::m_poLogFileStream = NULL;

Logger::Logger()
{
}

void Logger::Initialize()
{
	do
	{
		string strLogFile = GetLogFilePath ();
		char acMode[] = "ab+";
		if (Logger::m_poLogFileStream == NULL)
		{
			Logger::m_poLogFileStream = OpenLogFile(strLogFile, acMode);
			if (Logger::m_poLogFileStream == NULL)
				break;
			fseek(Logger::m_poLogFileStream, 0, SEEK_END);
		}

		long lFileSize = ftell(Logger::m_poLogFileStream);
		if (lFileSize >= kLogFileSize)
		{
			RollLogFile();
			Logger::m_poLogFileStream = OpenLogFile(strLogFile, acMode);
		}
	} while (false);
}

string Logger::GetLogFilePath()
{
	ArtworkUtil oUtil;
	string strLogFile = oUtil.GetUserFolder();
	strLogFile += kFilePathSeparator;
	strLogFile.append(kLogFileName);
	return strLogFile;
}
FILE * Logger::OpenLogFile(string strLogFile, char arrAccessMode[])
{
	FILE *poLogFile = NULL;
#ifdef _WINDOWS
	fopen(&poLogFile, strLogFile.c_str(), arrAccessMode);
#else
    poLogFile = fopen(strLogFile.c_str(), arrAccessMode);
#endif
	return poLogFile;
}

Logger::~Logger()
{
	if (Logger::m_poLogFileStream != NULL)
	{
		fflush(Logger::m_poLogFileStream);
		fclose(Logger::m_poLogFileStream);
	}
}

bool Logger::Info(string strClassName, string strFunctionName, string strMessage)
{
	bool bSuccess = false;
	do
	{
#ifndef NOLOGS
		Initialize();
		if (Logger::m_poLogFileStream != NULL)
		{
			string strLogMessage("");
			strLogMessage.append(BuildMessage(kINFOValue, strClassName, strFunctionName, strMessage));
			size_t nSize = fwrite(strLogMessage.c_str(), sizeof(char), strLogMessage.length(), Logger::m_poLogFileStream);
			fflush(Logger::m_poLogFileStream);
			bSuccess = true;
		}
#endif
	} while (false);

	return bSuccess;
}

bool Logger::Log(string strClassName, string strFunctionName, string strMessage)
{
	bool bSuccess = false;
#ifndef NOLOGS
	Initialize();
	if (Logger::m_poLogFileStream != NULL)
	{
		string strLogMessage("");
		strLogMessage.append(BuildMessage(kINFOValue, strClassName, strFunctionName, strMessage));
		size_t nSize = fwrite(strLogMessage.c_str(), sizeof(char), strLogMessage.length(), Logger::m_poLogFileStream);
		fflush(Logger::m_poLogFileStream);
		bSuccess = true;
	}
#endif
	return bSuccess;
}

bool Logger::Debug(string strClassName, string strFunctionName, string strMessage)
{
	bool bSuccess = false;
	do
	{
#ifndef NOLOGS
#ifdef DEBUGLOGS
		Initialize();
		if (Logger::m_poLogFileStream != NULL)
		{
			string strLogMessage("");
			strLogMessage.append(BuildMessage(kDEBUGValue, strClassName, strFunctionName, strMessage));
			fwrite(strLogMessage.c_str(), sizeof(char), strLogMessage.length(), Logger::m_poLogFileStream);
			fflush(Logger::m_poLogFileStream);
			bSuccess = true;
		}
#endif
#endif
	} while (false);
	return bSuccess;
}

bool Logger::Error(string strClassName, string strFunctionName, string strMessage)
{
	bool bSuccess = false;
#ifndef NOLOGS
	Initialize();
	if (Logger::m_poLogFileStream != NULL)
	{
		string strLogMessage("");
		strLogMessage.append(BuildMessage(kERRORValue, strClassName, strFunctionName, strMessage));
		fwrite(strLogMessage.c_str(), sizeof(char), strLogMessage.length(), Logger::m_poLogFileStream);
		fflush(Logger::m_poLogFileStream);
		bSuccess = true;
	}
#endif
	return bSuccess;
}

bool Logger::Warn(string strClassName, string strFunctionName, string strMessage)
{
	bool bSuccess = false;
#ifndef NOLOGS
	Initialize();
	if (Logger::m_poLogFileStream != NULL)
	{
		string strLogMessage("");
		strLogMessage.append(BuildMessage(kWARNValue, strClassName, strFunctionName, strMessage));
		fwrite(strLogMessage.c_str(), sizeof(char), strLogMessage.length(), Logger::m_poLogFileStream);
		fflush(Logger::m_poLogFileStream);
		bSuccess = true;
	}
#endif
	return bSuccess;
}

string Logger::BuildMessage(string strLogType, string strClassName, string strFunctionName, string strMessage)
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

string Logger::GetCurrentDateTime()
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

void Logger::RollLogFile()
{
	fclose(Logger::m_poLogFileStream);
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

bool Logger::IsFileExists(string strFileName)
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

void Logger::DeleteRollFile(string strRollFileName)
{
	remove(strRollFileName.c_str());
}

void Logger::RenameRollFileName(string strCurrentFileName, int nIndex, string strBaseFileName)
{
	string strNewFileName = strBaseFileName;
	strNewFileName.append(".");
	strNewFileName += nIndex;
	rename(strCurrentFileName.c_str(), strNewFileName.c_str());
}
