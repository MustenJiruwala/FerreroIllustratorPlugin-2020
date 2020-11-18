#ifndef _TemplateLog_
#define _TemplateLog_
class TemplateLog
{
public:
	/**	Constructor.
	*/
	TemplateLog();
	/**	Destructor.
	*/
	virtual ~TemplateLog() {}
	virtual void SetTemplateID(string strTemplateId);
	virtual string GetTemplateID();
	virtual void SetCategory(string strCategory);
	virtual string GetCategory();
	virtual void SetNumberOfLayers(int nLayers);
	virtual int GetNumberOfLayers();
	virtual void SetNumberOfObjects(int nObjects);
	virtual int GetNumberOfObjects();
	virtual void SetStatus(string strStatus);
	virtual string GetStatus();
	virtual void SetErrorString(string strError);
	virtual string GetError();
	virtual void WriteCSVLog();
	virtual void WriteErrorLog();
private:
	FILE * OpenLogFile(string strLogFile, char arrAccessMode[]);
	bool IsFileExists(string strFileName);
	string BuildCSVMessage(bool bAddHeader);
	string GetLogFilePath(string strLogFileName);
	string GetCurrentDateTime();
private:
	string m_strTemplateID;
	string m_strCategory;
	int m_nLayers;
	int m_nObjects;
	string m_strStatus;
	string m_strError;

};

#endif