
#ifndef __EXTLogger__
#define __EXTLogger__

#include <iostream>
#include <string.h>
#include <fstream>
using namespace std;
class EXTLogger
{
public:
	EXTLogger();
	~EXTLogger();
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