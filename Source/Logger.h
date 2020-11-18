
#ifndef __Logger__
#define __Logger__

#include <iostream>
#include <string.h>
#include <fstream>
using namespace std;
class Logger
{
public:
	Logger();
	~Logger();
	static bool		Info(string strClassName, string strFunctionName, string strMessage);
	static bool		Debug(string strClassName, string strFunctionName, string strMessage);
	static bool		Error(string strClassName, string strFunctionName, string strMessage);
	static bool		Warn(string strClassName, string strFunctionName, string strMessage);
	static bool		Log(string strClassName, string strFunctionName, string strMessage);
	static FILE		*m_poLogFileStream;
private:
	static void			Initialize();
	static FILE*		OpenLogFile(string strLogFile, char arrAccessMode[]);
	static string		BuildMessage(string strLogType, string strClassName, string strFunctionName, string strMessage);
	static string		GetCurrentDateTime();
	static string		GetLogFilePath();
	static void			RollLogFile();
	static bool			IsFileExists(string strFileName);
	static void			DeleteRollFile(string strRollFileName);
	static void			RenameRollFileName(string strCurrentFileName, int nIndex, string strBaseFileName);
};

#endif