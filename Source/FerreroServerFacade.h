class FerreroServerFacade
{
public:
	/**	Constructor.
	*/
	FerreroServerFacade();

	/**	Destructor.
	*/
	virtual ~FerreroServerFacade() {}
	virtual std::string CallServlet(string strBaseUrl, string strServletPath, string strParams, string &strErrorString);
	virtual void download(string strBaseURL, string strServletPath, string strParams, string strProjectId, string &strErrorString, bool bIsForArtworkCreation = false);
	virtual int UploadFile(string strFilePath, string strTemplateID, string &strErrorString, bool bIsForArtwork = false);
	virtual std::string URLEncode(string strParams);
	virtual std::string CallServletUsingPostRequest(string strBaseURL, string strServletPath, string strParams, string &strErrorString);
private:
	string GetProxyString();
	string BuildUserPass(std::vector<string>arrComponent);
};
