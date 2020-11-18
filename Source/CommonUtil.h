#ifndef _CommonUtil_
#define _CommonUtil_

#include "IllustratorSDK.h"
#include "xercesc/dom/DOM.hpp"
#include "MenuPlayID.h"
#include "TestPanelController.h"
#include "ArtworkUtil.h"
#include "ArticleJob.h"
#include "zlib/include/zip.h"
#include "zlib/include/unzip.h"
#include "zlib/include/zlib.h"
using namespace xercesc;
class CommonUtil
{
public:
	/**	Constructor.
	*/
	CommonUtil();

	/**	Destructor.
	*/
	virtual ~CommonUtil() {}
	virtual string GetCategoryTemplates(string strCategory, CEPTestPanelController* panelController, string &strErrorString);
	virtual std::vector<string>  StringToArray(string strContent, string strSeparator);
	virtual void ConvertTemplate(string strTemplateID, CEPTestPanelController* panelController);
	virtual void AddSwatches(string strTemplateString, CEPTestPanelController* panelController);
	string addTemplateToConvertedTable(string strTemplateID, string strCategoryID, string &strErrorString, TemplateLog oTemplateLog);
	void DeleteTemplateFiles(string strTemplateID);
	string CleanupConversion(string strTemplateID);
	string addErrorToConvertedKOTable(string strTemplateID, string strCategoryID, string strErrorString);
#ifdef _WINDOWS
	bool CanConvertTemplate(string strTemplateID, string &strErrorString);
#else
	Boolean CanConvertTemplate(string strTemplateID, string &strErrorString);
#endif
	string GetMissingFonts(string strTemplateID, string &strErrorString);
	void InspectSwatchLibrary(string strTemplateID, ArticleJob oArticleJob);
	void AddSwatchToDocument(AISwatchListRef &oSwatchlist, ArticleJob oArticleJob);
	void ProcessUploadTemplate(ArticleJob oArticleJob, TemplateLog oTemplateLog);
	void RemoveDocumentSwatch(AISwatchListRef &oDocumentSwatchListRef);
	string addToConvertedTemplatesToCheckTable(TemplateLog oTemplateLog, string strEffects);
private:
	void processTemplateStatus(TemplateLog oTemplateLog, ArticleJob oArticleJob);
#ifdef _WINDOWS
	bool IsSwatchLibraryFound(string strTemplateID);
	bool IsSwatchExists(AISwatchListRef &oDocumentSwatchListRef, ai::UnicodeString strSwatchName);
#else
	Boolean IsSwatchLibraryFound(string strTemplateID);
	Boolean IsSwatchExists(AISwatchListRef &oDocumentSwatchListRef, ai::UnicodeString strSwatchName);
#endif
};
#endif // _CommonUtil_
