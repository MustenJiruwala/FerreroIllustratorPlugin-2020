#ifndef _ArticleJob_
#define _ArticleJob_

#include "IllustratorSDK.h"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/sax/HandlerBase.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"
using namespace xercesc;
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <assert.h>
#include "zlib/include/zip.h"
#include "zlib/include/unzip.h"
#include "zlib/include/zlib.h"

#ifdef WINDOWS
#include <conio.h>
#include <process.h>
#endif
#include <map>

#include "GraphicItem.h"
#include "TestPanelController.h"
#include "TemplateLog.h"
using namespace std;
#include "AIDocumentList.h"
#include "SDKErrors.h"
extern  "C" AIDocumentListSuite*			sAIDocumentList;
extern	"C" AIDocumentSuite*				sAIDocument;
extern "C" SPBasicSuite*					sSPBasic;
extern "C" AIActionManagerSuite*			sAIActionManager;
extern "C"	AILayerSuite*					sAILayer;

class ArticleJob
{
	/** Value parameter block (VPB) - a container for action parameters.
	@see AIActionManagerSuite
*/
	class VPB
	{
	public:
		/**	Constructor
		*/
		VPB();

		/**	Destructor
		*/
		~VPB();

		/**	Returns the value parameter block structure.
		*/
		inline operator AIActionParamValueRef(void) const { return fActionParamValueRef; }

		/**	Sets the name key of the document to save as - kAISaveDocumentAsNameKey,
			in the value parameter block.
			@param name IN name to save document as.
		*/
		void SetSaveDocumentAsName(const ai::FilePath& name);
		void SetSaveDocumentAsFormat(const char* format);
	private:
		/** The value parameter block.
		*/
		AIActionParamValueRef fActionParamValueRef;
	}; // end class VPB

public:
	/**	Constructor.
	*/
	ArticleJob();
	ArticleJob(std::string strResponse, CEPTestPanelController* panelController);

	/**	Destructor.
	*/
	virtual ~ArticleJob() {}
	virtual void SetTemplateId(string strTemplateId);
	virtual std::string GetTemplateId();
	virtual void CreateDocument(bool bIsNewTemplate, TemplateLog &oTemplateLog);
	virtual void SetWidth(AIReal nWidth);
	virtual void SetHeight(AIReal nHeight);
	virtual AIReal GetWidth();
	virtual AIReal GetHeight();
	virtual std::vector<GraphicItem> GetGraphicItems();
	virtual int GetGraphicItemsCount();
	virtual void SetLayerCount(int nLayer);
	virtual int GetLayersCount();
	virtual void SetEffects(string strEffects);
	virtual string GetEffects();
	void UploadTemplate(TemplateLog &oTemplateLog);
	string GetTemplateDocumentPath();
	void CreateNewTemplate();
	ASErr UnzipFile(string strUnZipFile, string strUnzipFilePath, string &strErrorString);
	string BuildEffects();
	void CreateNewArtwork(string strLayerInfo);
	string GetArtworkDocumentPath();
	void ProcessTemplate();
private:
	std::string m_strTemplateId;
	std::vector<GraphicItem> m_arrGraphicItems;
	std::map<string, string> m_arrGraphicObjects;
	AIReal m_nWidth;
	AIReal m_nHeight;
	int m_nLayerCount;
	string m_strEffects;
	CEPTestPanelController* fPanelController;
private:
	ASErr NewDocument(const ai::UnicodeString& preset, AIDocumentHandle& document);
	void PlaceItems(AIDocumentHandle &oDocument, bool bIsNewTemplate, string &strErrorString);
	bool ProcessUnzip(unzFile &oZipfile, string strUnzipFilePath);
	string ConstructFilePath(string strUnzipFilePath, string strFilename);
	bool ExtractFile(unzFile &oZipfile, string strFilePath);
	string GetTemplateZipPath();
	string GetKey(string strFilePath);
	void CreateDefaultLayers();
	void DownloadGraphicItems();
	void PlaceDefaultDTFile(AIDocumentHandle &oDocument);
	int CreateTemplateLayers(bool IsNewTemplate, string &strErrorString);
	string GetTemplateLayers(string strTemplateID, bool IsNewTemplate, string &strErrorString);
	void DeleteLogAndSwatchFile(string strUnzipFilePath);
	void CreateArtworkLayers(string strLayerInfo);
	std::vector<GraphicItem> GetNewGraphicItems();
	string buildNewGraphicFileNames(std::vector<GraphicItem> arrNewGraphicItems);
};
#endif //_ArticleJob_
