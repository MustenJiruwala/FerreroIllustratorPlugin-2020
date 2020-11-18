#include "IllustratorSDK.h"
#include "ArticleJob.h"
#include "MenuPlayID.h"
#include "ArtworkUtil.h"
#include "AIDocumentAction.h"
#include "AINativeAction.h"
#include "FerreroServerFacade.h"
#include "TemplateLog.h"
#include "Logger.h"
#include <cstdio>
#ifndef _WINDOWS
    #include "dirent.h"
#endif
#define kIllustratorCCFileFormat "Illustrator CC"

ArticleJob::ArticleJob()
{
	fPanelController = NULL;
	m_nLayerCount = 0;
}

ArticleJob::ArticleJob(string strResponse, CEPTestPanelController* panelController)
{
	do
	{
		m_nLayerCount = 0;
		fPanelController = panelController;
		XercesDOMParser* poParser = new XercesDOMParser();
		poParser->setValidationScheme(XercesDOMParser::Val_Always);
		poParser->setDoNamespaces(true);    // optional
		ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
		poParser->setErrorHandler(errHandler);
		const char *poChars = strResponse.c_str();
		XMLByte *pcResponse = (XMLByte *)(poChars);
		xercesc::MemBufInputSource oSource(pcResponse, strResponse.length(), "Response XML");
		try
		{
			poParser->parse(oSource);
			xercesc::DOMDocument * poDoc = poParser->getDocument();
			if (poDoc == nullptr)
				break;
			DOMElement * poElement = poDoc->getDocumentElement();
			if (poElement == nullptr)
				break;
			const XMLCh *poImageTag = XMLString::transcode("image");
			DOMNodeList * poImageList = poElement->getElementsByTagName(poImageTag);
			for (int nIndex = 0; nIndex < poImageList->getLength(); nIndex++)
				m_arrGraphicItems.insert(m_arrGraphicItems.begin(), *(new GraphicItem (poImageList->item(nIndex))));
			const XMLCh *poPageTag = XMLString::transcode("page");
			DOMNodeList * poPage = poElement->getElementsByTagName(poPageTag);
			if (poPage->getLength() > 0)
			{
				DOMNode *poNode = poPage->item(0);
				DOMNamedNodeMap * poNodeNameMap = poNode->getAttributes();
				if (poNodeNameMap != nullptr)
				{
					std::string::size_type tSize;
					ArtworkUtil oUtil;
					m_nWidth = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "width"));
					m_nHeight = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "height"));
				}
			}
		}
		catch (...)
		{
			break;
		}
		delete poParser;
		delete errHandler;
	} while (false);
}

void ArticleJob::SetTemplateId(string strTemplateId)
{
	m_strTemplateId = strTemplateId;
}

std::string ArticleJob::GetTemplateId()
{
	return m_strTemplateId;
}

void ArticleJob::SetWidth(AIReal nWidth)
{
	m_nWidth = nWidth;
}

void ArticleJob::SetHeight(AIReal nHeight)
{
	m_nHeight = nHeight;
}

AIReal ArticleJob::GetWidth()
{
	return m_nWidth;
}

AIReal ArticleJob::GetHeight()
{
	return m_nHeight;
}

void ArticleJob::CreateNewTemplate()
{
	ArtworkUtil oUtil;
	string strErrorString = "";
	do
	{
		TemplateLog oTemplateLog;
		AIDocumentHandle oDocument = NULL;
		string strPrintPreset = ai::Locale::getApplicationLocaleID() == ai::Locale::kItaly ? "Stampa" : "Print";
		ASErr tRresult = NewDocument(ai::UnicodeString(strPrintPreset), oDocument);
		aisdk::check_ai_error(tRresult);
		SDK_ASSERT(oDocument);
		if (tRresult != 0)
			oTemplateLog.SetErrorString("Creating new document failed with error code :" + to_string(tRresult) + "\n");
		AILayerHandle oDefaultLayer;
		SDK_ASSERT(sAILayer);
		sAILayer->GetFirstLayer(&oDefaultLayer);
		CreateDefaultLayers();
		CreateTemplateLayers(true, strErrorString);
		PlaceDefaultDTFile(oDocument);
		if (oDefaultLayer != NULL)
			tRresult = sAILayer->DeleteLayer(oDefaultLayer);
		string strDocPath = GetTemplateDocumentPath();
		oUtil.SaveDocument(oDocument, strDocPath);
		oUtil.CloseDocument(oDocument);
		oUtil.OpenDocument(oDocument, strDocPath);
	} while (false);
}

void ArticleJob::CreateNewArtwork(string strLayerInfo)
{
	do
	{
		ArtworkUtil oUtil;
		TemplateLog oTemplateLog;
		AIDocumentHandle oDocument = NULL;
		string strPrintPreset = ai::Locale::getApplicationLocaleID() == ai::Locale::kItaly ? "Stampa" : "Print";
		ASErr tRresult = NewDocument(ai::UnicodeString(strPrintPreset), oDocument);
		aisdk::check_ai_error(tRresult);
		SDK_ASSERT(oDocument);
		if (tRresult != 0)
			oTemplateLog.SetErrorString("Creating new document failed with error code :" + to_string(tRresult) + "\n");
		AILayerHandle oDefaultLayer;
		SDK_ASSERT(sAILayer);
		sAILayer->GetFirstLayer(&oDefaultLayer);
		CreateDefaultLayers();
		CreateArtworkLayers(strLayerInfo);
		if (oDefaultLayer != NULL)
			tRresult = sAILayer->DeleteLayer(oDefaultLayer);
		string strDocPath = GetArtworkDocumentPath();
		oUtil.SaveDocument(oDocument, strDocPath);
		oUtil.CloseDocument(oDocument);
		oUtil.OpenDocument(oDocument, strDocPath);
	} while (false);
}

void ArticleJob::CreateDocument(bool bIsNewTemplate, TemplateLog &oTemplateLog)
{
	Logger::Info("ArticleJob", "CreateDocument - creating new document", "");
	string strMessage = "";
	ArtworkUtil oUtil;
	oUtil.SendEventMessageToPanel("com.ferrero.log", "creating new document");
	AIDocumentHandle oDocument = NULL;
    string strPrintPreset = ai::Locale::getApplicationLocaleID() == ai::Locale::kItaly ? "Stampa" : "Print";
	ASErr tRresult = NewDocument(ai::UnicodeString(strPrintPreset), oDocument);
	aisdk::check_ai_error(tRresult);
	SDK_ASSERT(oDocument);
	if (tRresult != 0)
		oTemplateLog.SetErrorString("Creating new document failed with error code :"+ to_string(tRresult)+"\n");
    Logger::Info("ArticleJob", "Creating new document successful!", "");
	oUtil.SendEventMessageToPanel("com.ferrero.log", "Creating new document successful!");
	AILayerHandle oDefaultLayer;
	sAILayer->GetFirstLayer(&oDefaultLayer);
	string strUnZipFile = GetTemplateZipPath();
	string strUnzipFilePath = strUnZipFile.substr(0, strUnZipFile.find_last_of("."));
    Logger::Info("ArticleJob", "Creating unzip folder", strUnzipFilePath);
	int nError = oUtil.CreateFolder(strUnzipFilePath);
    Logger::Info("ArticleJob", "Creating unzip folder result is ", to_string(nError));
	if (bIsNewTemplate)
		DownloadGraphicItems();
	string strErrorString = "";
	oUtil.SendEventMessageToPanel("com.ferrero.log", "Unzipping downloaded file start...");
	UnzipFile(strUnZipFile, strUnzipFilePath, strErrorString);
	do
	{
		if (strErrorString.length() > 0)
		{
			oTemplateLog.SetErrorString(strErrorString);
			break;
		}
		oUtil.SendEventMessageToPanel("com.ferrero.log", "Unzipping downloaded file end...");
		int nLayerCount = 0;
		if (bIsNewTemplate)
		{
			nLayerCount = 4;
			CreateDefaultLayers();
			nLayerCount += CreateTemplateLayers(true, strErrorString);
			PlaceDefaultDTFile(oDocument);
		}
		else
			nLayerCount = CreateTemplateLayers(false, strErrorString);
		if (strErrorString.length() > 0)
		{
			oTemplateLog.SetErrorString(strErrorString);
			break;
		}
		SetLayerCount(nLayerCount);
		if (oDefaultLayer != NULL)
			tRresult = sAILayer->DeleteLayer(oDefaultLayer);
		PlaceItems(oDocument, bIsNewTemplate, strErrorString);
		if (strErrorString.length() > 0)
		{
			oTemplateLog.SetErrorString(strErrorString);
			break;
		}
		string strDocPath = GetTemplateDocumentPath();
		strMessage = "Saving document to path : " + strDocPath;
		oUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
		oUtil.SaveDocument(oDocument, strDocPath);
		oUtil.CloseDocument(oDocument);
		oUtil.OpenDocument(oDocument, strDocPath);
		if (bIsNewTemplate)
		{
			string strPDFFile = strDocPath.substr(0, strDocPath.find_last_of("."));
			strPDFFile.append(".pdf");
			oUtil.SaveDocumentAsPDF(oDocument, strPDFFile);
		}
	} while (false);
	oUtil.CloseDocument(oDocument);
	oUtil.SendEventMessageToPanel("com.ferrero.log", "Closing document");
}

void ArticleJob::DeleteLogAndSwatchFile(string strUnzipFilePath)
{
	string strFoderToZip = strUnzipFilePath;
	strFoderToZip += kFilePathSeparator;
	strFoderToZip += GetTemplateId();
	strFoderToZip += kFilePathSeparator;
	string strLogFile = strFoderToZip +"FERRERO_" +GetTemplateId() + "_exporteps.log";
	string strSwatchFile = strFoderToZip + "FERRERO_" + GetTemplateId() + ".ase";
	try
	{
		ai::UnicodeString strPath = ai::UnicodeString(strLogFile);
		ai::FilePath oLogPath = ai::FilePath(strPath);
		if (oLogPath.Exists(false))
			remove(strLogFile.c_str());
		ai::UnicodeString strswatchPath = ai::UnicodeString(strSwatchFile);
		ai::FilePath oSwatchPath = ai::FilePath(strswatchPath);
		if (oSwatchPath.Exists(false))
			remove(strSwatchFile.c_str());
	}
	catch (ai::Error& tError)
	{
		Logger::Error("ArticleJob", "DeleteLogAndSwatchFile", "Error : " + to_string(tError));
	}
}

void ArticleJob::UploadTemplate(TemplateLog &oTemplateLog)
{
	FerreroServerFacade oFerreroServerFacade;
	ArtworkUtil oUtil;
	string strErrorString = "";
	string strUnZipFile = GetTemplateZipPath();
	string strUnzipFilePath = strUnZipFile.substr(0, strUnZipFile.find_last_of("."));
	string strFoderToZip = strUnzipFilePath;
	strFoderToZip += kFilePathSeparator;
	strFoderToZip.append(GetTemplateId());
	DeleteLogAndSwatchFile(strUnzipFilePath);
	if (oUtil.ZipFolder(strFoderToZip))
	{
		string strZipFilePath = strFoderToZip;
		strZipFilePath.append(".zip");
		oUtil.SendEventMessageToPanel("com.ferrero.log", "Uploading converted document start");
		oFerreroServerFacade.UploadFile(strZipFilePath, GetTemplateId(), strErrorString);
		oUtil.SendEventMessageToPanel("com.ferrero.log", "Uploading converted document end");
	}
	else
	{
		oTemplateLog.SetErrorString("Error zipping folder : " + strFoderToZip);
	}
	if (strErrorString.length() > 0)
		oTemplateLog.SetErrorString(strErrorString);
}

void ArticleJob::DownloadGraphicItems()
{
	string strDownloadGraphicParam = "function=downloadgraphicitem&graphicid=";
	for (int nIndex = 0; nIndex < m_arrGraphicItems.size(); nIndex++)
	{
		if(nIndex > 0)
			strDownloadGraphicParam.append(";");
		strDownloadGraphicParam.append(m_arrGraphicItems[nIndex].GetGraphic_id());
		strDownloadGraphicParam.append(",");
		strDownloadGraphicParam.append(m_arrGraphicItems[nIndex].GetId());
	}
	strDownloadGraphicParam.append("&templateid=");
	strDownloadGraphicParam.append(GetTemplateId ());
	Logger::Info("ArticleJob", "DownloadGraphicItems strDownloadGraphicParam : ", strDownloadGraphicParam);
	FerreroServerFacade oFerreroServerFacade;
	string strErrorString = "";
	oFerreroServerFacade.download(kFerreroWFESS2URL, "/SapRequestServlet", strDownloadGraphicParam, GetTemplateId(), strErrorString);
}

string ArticleJob::GetTemplateLayers(string strTemplateID, bool IsNewTemplate, string &strErrorString)
{
	string strGetLayerNamesParam = "function=getTemplateLayers&projectid=";
	if (IsNewTemplate)
		strGetLayerNamesParam = "function=getLayerNames&projectid=";
	strGetLayerNamesParam.append(GetTemplateId());
	Logger::Info("ArticleJob", "GetTemplateLayers strGetLayerNamesParam : ", strGetLayerNamesParam);
	FerreroServerFacade oFerreroServerFacade;
	return oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strGetLayerNamesParam, strErrorString);
}

/*
*/

void ArticleJob::CreateDefaultLayers()
{
	ArtworkUtil oUtil;
	oUtil.CreateLayer("check");
	oUtil.CreateLayer("notes");
	oUtil.CreateLayer("technical-drawing");
	oUtil.CreateLayer("shared");
}

void ArticleJob::CreateArtworkLayers(string strLayerInfo)
{
	ArtworkUtil oUtil;
	std::vector<string> arrLayers = oUtil.Split(strLayerInfo, ",");
	for (int nIndex = 0; nIndex < arrLayers.size(); nIndex++)
	{
		oUtil.CreateLayer(arrLayers[nIndex]);
	}
}

int ArticleJob::CreateTemplateLayers(bool IsNewTemplate, string &strErrorString)
{
	ArtworkUtil oUtil;
	string strLayerNames = GetTemplateLayers(GetTemplateId(), IsNewTemplate, strErrorString);
//	string strLayerNames = oUtil.GetFileContent("C:\\Users\\admin\\Documents\\ferrero\\test\\xml\\layer.xml");
	Logger::Info("ArticleJob", "CreateTemplateLayers strLayerNames : ", strLayerNames);
	string strMessage = "Creating template layers :" + strLayerNames;

	oUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
	char * poArtwork;
	poArtwork = strtok((char*)strLayerNames.c_str(), ",");
	int nLayer = 0;
	while (poArtwork != NULL)
	{
		nLayer++;
		string strLayer = "";
		strLayer.append(poArtwork);
		strMessage = "Creating template layer :" + strLayer + " start";
		oUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
		oUtil.CreateLayer(strLayer);
		strMessage = "Creating template layer :" + strLayer + " end";
		oUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
		poArtwork = strtok(NULL, ",");
	}
	return nLayer;
}

string  ArticleJob::GetTemplateZipPath()
{
	ArtworkUtil oUtil;
	string strZipFile = oUtil.GetUserFolder();
	strZipFile.append(GetTemplateId());
	strZipFile.append(".zip");
	return strZipFile;
}

string ArticleJob::GetTemplateDocumentPath()
{
	ArtworkUtil oUtil;
	string strDocumentFile = oUtil.GetUserFolder();
	strDocumentFile.append(GetTemplateId());
	strDocumentFile.append(1, kFilePathSeparator);
	strDocumentFile.append(GetTemplateId());
    strDocumentFile.append(1, kFilePathSeparator);
	strDocumentFile.append("FERRERO_");
	strDocumentFile.append(GetTemplateId());
	strDocumentFile.append(".ai");
	return strDocumentFile;
}

string ArticleJob::GetArtworkDocumentPath()
{
	ArtworkUtil oUtil;
	string strDocumentFile = oUtil.GetArtworkDownloadFolder();
	strDocumentFile.append(GetTemplateId());
	strDocumentFile.append(1, kFilePathSeparator);
	strDocumentFile.append("FERRERO_");
	strDocumentFile.append(GetTemplateId());
	strDocumentFile.append(".ai");
	return strDocumentFile;
}

void ArticleJob::PlaceItems(AIDocumentHandle &oDocument, bool bIsNewTemplate, string &strErrorString)
{
	string strMessage = "Placing graphic items, total items to place :" + m_arrGraphicItems.size();
	ArtworkUtil oUtil;
	oUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
	for (std::vector<GraphicItem>::iterator oIterator = m_arrGraphicItems.begin(); oIterator != m_arrGraphicItems.end(); ++oIterator)
	{
		oIterator->SetFilePath(m_arrGraphicObjects[oIterator->GetObjectKey()]);
		if (bIsNewTemplate)
		{
			AIReal nWidth = oIterator->GetWidth();
			oIterator->SetXpos(nWidth > 0 ? -nWidth : 0.0);
			oIterator->SetYpos(0.0);
			oIterator->SetLayerName("shared");
		}
		oIterator->PlaceItem(oDocument, m_strTemplateId, strErrorString);
	}
	oUtil.SendEventMessageToPanel("com.ferrero.log", "Placing graphic items completed");
}

ASErr ArticleJob::NewDocument(const ai::UnicodeString& strPreset, AIDocumentHandle& oDocument)
{
    Logger::Info("ArticleJob", "NewDocument", "");
	ASErr tResult = kNoErr;
	try 
	{
		SDK_ASSERT(sAIDocumentList);
		AINewDocumentPreset tSettings;
        Logger::Info("ArticleJob", "NewDocument", "Getting preset settings");
		sAIDocumentList->GetPresetSettings(const_cast<ai::UnicodeString&>(strPreset), &tSettings);
        Logger::Info("ArticleJob", "NewDocument", "Got preset settings :" );
		tSettings.docHeight = GetHeight()*(PPI / MMPI);
		tSettings.docWidth = GetWidth()*(PPI / MMPI);
		tSettings.docUnits = kMillimetersUnits;
		string strTitle = "FERRERO_";
		strTitle.append(GetTemplateId ());
		tSettings.docTitle = ai::UnicodeString(strTitle);
        ai::UnicodeString oPreset(strPreset);
        Logger::Info("ArticleJob", "NewDocument", "Opening a new document : Start" );
		tResult = sAIDocumentList->New(oPreset, &tSettings, kDialogOff, &oDocument);
		aisdk::check_ai_error(tResult);
		SDK_ASSERT(oDocument);
        Logger::Info("ArticleJob", "NewDocument", "Opening a new document : Success" );
	}
	catch (ai::Error& tError) 
	{
		tResult = tError;
        Logger::Error("ArticleJob", "NewDocument", "Error : " + to_string(tResult));
	}
	return tResult;
}

ASErr ArticleJob::UnzipFile(string strUnZipFile, string strUnzipFilePath, string &strErrorString)
{
	ASErr tErrorCode = 1;
	Logger::Info("ArticleJob", "UnzipFile", "strUnZipFile :" + strUnZipFile);
	Logger::Info("ArticleJob", "UnzipFile", "strUnzipFilePath :" + strUnzipFilePath);
	do
	{
		unzFile oZipfile = unzOpen(strUnZipFile.c_str());
		if (oZipfile == NULL)
		{
			Logger::Info("ArticleJob", "UnzipFile", "Null pointer");
			strErrorString += "Error unzipping file : "+ strUnZipFile  +" Null pointer";
			break;
		}
		// Get info about the zip file
		unz_global_info global_info;
		if (unzGetGlobalInfo(oZipfile, &global_info) != UNZ_OK)
		{
            Logger::Info("ArticleJob", "UnzipFile", "UNZip NOT OK");
			strErrorString += "Error unzipping file : " + strUnZipFile + " UNZip NOT OK";
			unzClose(oZipfile);
			break;
		}
		// Buffer to hold data read from the zip file.
		for (int nIndex = 0; nIndex < global_info.number_entry; ++nIndex)
		{
			if (!ProcessUnzip(oZipfile, strUnzipFilePath))
				break;
			unzCloseCurrentFile(oZipfile);
			// Go the the next entry listed in the zip file.
			if ((nIndex + 1) < global_info.number_entry)
			{
				if (unzGoToNextFile(oZipfile) != UNZ_OK)
				{
                    Logger::Info("ArticleJob", "UnzipFile", "unzGoToNextFile NOT OK");
					strErrorString += "Error unzipping file : " + strUnZipFile + " unzGoToNextFile NOT OK";
					unzClose(oZipfile);
					break;
				}
			}
		}
		unzClose(oZipfile);
		tErrorCode = 0;
	} while (false);
	return tErrorCode;
}

//------------------------------------------------------------------------------------------
//	UnzipFile
//	Utility method to unzip a folder.
//	oZipFile [IN] - IDFile consisting of the file to unzip.
//	strUnzipFilePath [IN] - String consisting of the location to unzip the zip file contents.
//	returns ErrorCode, kSuccess if file was unzipped successfully, otherwise kFailure.
//------------------------------------------------------------------------------------------
bool ArticleJob::ProcessUnzip(unzFile &oZipfile, string strUnzipFilePath)
{
	Logger::Info("ArticleJob", "ProcessUnzip", "strUnzipFilePath :" + strUnzipFilePath);
	bool bSuccess = true;
	do
	{
		// Get info about current file.
		ArtworkUtil oUtil;
		unz_file_info file_info;
		char strFilename[MAX_FILENAME];
		if (unzGetCurrentFileInfo(oZipfile, &file_info, strFilename, MAX_FILENAME, NULL, 0, NULL, 0) != UNZ_OK)
		{
            Logger::Info("ArticleJob", "ProcessUnzip", "unzGetCurrentFileInfo NOT OK");
			unzClose(oZipfile);
			bSuccess = false;
			break;
		}
		// Check if this entry is a directory or file.
		const size_t nFilename_length = strlen(strFilename);
		string strFilePath = ConstructFilePath(strUnzipFilePath, strFilename);
		if (strFilename[nFilename_length - 1] == kFilePathSeparator || strFilename[nFilename_length - 1] == '/')
		{
            int nError = oUtil.CreateFolder(strFilePath);
            Logger::Info("ArticleJob", "ProcessUnzip", "CreateFolder : "+strFilePath+(nError == 0 ? " - OK" : "- Failed!"));
		}
		else
			bSuccess = ExtractFile(oZipfile, strFilePath);
	} while (false);
    Logger::Info("ArticleJob", "ProcessUnzip", "bSuccess :"+to_string(bSuccess));
	return bSuccess;
}

//------------------------------------------------------------------------------------------
//	ConstructFilePath
//	Utility method to construct the file path based on platform.
//	strUnzipFilePath [IN] - String consisting of the file path.
//	strFilename [IN] - String consisting of the file name.
//	returns PMString consisting of platform specific file path.
//------------------------------------------------------------------------------------------
string ArticleJob::ConstructFilePath(string strUnzipFilePath, string strFilename)
{
	Logger::Info("ArticleJob", "ConstructFilePath", "strUnzipFilePath [IN] :" + strUnzipFilePath);
	Logger::Info("ArticleJob", "ConstructFilePath", "strFilename [IN] :" + strFilename);
	ArtworkUtil oUtil;
	string strFilePath = strUnzipFilePath;
	std::vector<string> arrPath = oUtil.Split(strFilename, "/\\");
	char cLastChar = strFilename[strFilename.length() - 1];
	for (int nIndex = 0; nIndex < arrPath.size(); nIndex++)
	{
		strFilePath.append("/");
		strFilePath.append(arrPath[nIndex]);
		if (cLastChar == '/' || cLastChar == '\\')
			oUtil.CreateFolder(strFilePath);
	}
//	strFilePath = Utils <ITEUtils>()->TextReplaceAll(strFilePath, "\\", "/");
	Logger::Info("ArticleJob", "ConstructFilePath", "strFilePath [OUT] :" + strFilePath);
	return strFilePath;
}


//------------------------------------------------------------------------------------------
//	ExtractFile
//	This method writes the contents from source file to destination file.
//	oZipfile [IN] - unzFile consisting the source file that needs to be written.
//	strFilePath [IN] - PMString consisting of the destination file path.
//	returns Boolean, kTrue if file was extracted successfully, false otherwise.
//------------------------------------------------------------------------------------------
bool ArticleJob::ExtractFile(unzFile &oZipfile, string strFilePath)
{
	Logger::Info("ArticleJob", "ConstructFilePath", "strFilePath [IN] :" + strFilePath);
	bool bSuccess = true;
	char oRead_buffer[READ_SIZE];
	do
	{
		if (unzOpenCurrentFile(oZipfile) != UNZ_OK)
		{
			unzClose(oZipfile);
			bSuccess = false;
			break;
		}
		// Open a file to write out the data.
		ArtworkUtil oUtil;
		FILE *poOutput = oUtil.OpenFile(strFilePath);
		if (poOutput == NULL)
		{
			unzCloseCurrentFile(oZipfile);
			unzClose(oZipfile);
			bSuccess = false;
			break;
		}
		int nError = UNZ_OK;
		do
		{
			nError = unzReadCurrentFile(oZipfile, oRead_buffer, READ_SIZE);
			if (nError < 0)
			{
				unzCloseCurrentFile(oZipfile);
				unzClose(oZipfile);
				bSuccess = false;
				break;
			}
			// Write data to file.
			if (nError > 0)
			{
				fwrite(oRead_buffer, nError, 1, poOutput); // You should check return of fwrite...
			}
		} while (nError > 0);
		if (bSuccess)
		{
			m_arrGraphicObjects[GetKey(strFilePath)] = strFilePath;
			fclose(poOutput);
		}
	} while (false);
	Logger::Info("ArticleJob", "ConstructFilePath", "bSuccess [OUT] :" + to_string(bSuccess));
	return bSuccess;
}

string ArticleJob::GetKey(string strFilePath)
{
	string strKey = strFilePath;
	int nPos = strFilePath.find_last_of("/\\");
	if (nPos > 0)
	{
		strKey = strKey.substr(nPos + 1);
		strKey = strKey.substr(0, strKey.find_last_of("."));
	}
	return strKey;
}

ArticleJob::VPB::VPB() : fActionParamValueRef(NULL)
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(sAIActionManager);
		result = sAIActionManager->AINewActionParamValue(&fActionParamValueRef);
		SDK_ASSERT(!result);
		SDK_ASSERT(fActionParamValueRef);
	}
	catch (ai::Error) {
	}
}

/*
*/
ArticleJob::VPB::~VPB()
{
	ASErr result = kNoErr;
	try {
		SDK_ASSERT(sAIActionManager);
		result = sAIActionManager->AIDeleteActionParamValue(fActionParamValueRef);
		SDK_ASSERT(!result);
		fActionParamValueRef = NULL;
	}
	catch (ai::Error) {
	}
}

// kAISaveDocumentAsAction parameters
/*
*/
void ArticleJob::VPB::SetSaveDocumentAsName(const ai::FilePath& name)
{
	SDK_ASSERT(sAIActionManager);
	ASErr result = sAIActionManager->AIActionSetStringUS(this->fActionParamValueRef, kAISaveDocumentAsNameKey, name.GetFullPath());
	aisdk::check_ai_error(result);
}

/*
*/
void ArticleJob::VPB::SetSaveDocumentAsFormat(const char* format)
{
	SDK_ASSERT(sAIActionManager);
	ASErr result = sAIActionManager->AIActionSetString(this->fActionParamValueRef, kAISaveDocumentAsFormatKey, format);
	aisdk::check_ai_error(result);
}

std::vector<GraphicItem> ArticleJob::GetGraphicItems()
{
	return m_arrGraphicItems;
}

void ArticleJob::PlaceDefaultDTFile(AIDocumentHandle &oDocument)
{
	GraphicItem oGraphicItem;
	string strDTFileName = kFerreroDTPrefix;
	strDTFileName.append(GetTemplateId());
	strDTFileName.append(".pdf");
	oGraphicItem.SetFileName(strDTFileName);
	oGraphicItem.SetLayerName("technical-drawing");
	oGraphicItem.SetXpos(0);
	oGraphicItem.SetYpos(0);
	string strerror = "";
	oGraphicItem.PlaceItem(oDocument, GetTemplateId(), strerror);
}

int ArticleJob::GetGraphicItemsCount()
{
	return m_arrGraphicItems.size();
}

int ArticleJob::GetLayersCount()
{
	return m_nLayerCount;
}

void ArticleJob::SetLayerCount(int nLayer)
{
	m_nLayerCount = nLayer;
}

void ArticleJob::SetEffects(string strEffects)
{
	m_strEffects = strEffects;
}

string ArticleJob::GetEffects()
{
	return m_strEffects;
}

string ArticleJob::BuildEffects()
{
	string strEffects = "";
	for (int nIndex = 0; nIndex < m_arrGraphicItems.size(); nIndex++)
		strEffects += m_arrGraphicItems[nIndex].GetEffects();
	return strEffects;
}

void ArticleJob::ProcessTemplate()
{
	string strErrorString = "";
	ArtworkUtil oArtworkUtil;
	do
	{
		int nLayers = CreateTemplateLayers(true, strErrorString);
		if (!strErrorString.empty())
		{
			Logger::Error("ArticleJob", "ProcessTemplate", strErrorString);
			oArtworkUtil.showAlert(strErrorString);
			break;
		}
		std::vector<GraphicItem> arrNewGraphicItems = GetNewGraphicItems ();
		if (arrNewGraphicItems.size() == 0)
			break;
		string strMessage = GetTemplateId()+"=" + buildNewGraphicFileNames(arrNewGraphicItems);
		oArtworkUtil.SendEventMessageToExtension("com.ferrero.downloadgraphic", strMessage);
	} while (false);
}

string ArticleJob::buildNewGraphicFileNames(std::vector<GraphicItem> arrNewGraphicItems)
{
	string strNewGraphicNames = "";
	if (arrNewGraphicItems.size() > 0)
	{
		for (std::vector<GraphicItem>::iterator oIterator = arrNewGraphicItems.begin(); oIterator != arrNewGraphicItems.end(); ++oIterator)
		{
			string strMessage = oIterator->Stringify();
			strNewGraphicNames += strNewGraphicNames.empty() ? ("[" + strMessage) : ("," + strMessage);
		}
		strNewGraphicNames += "]";
	}
	return strNewGraphicNames;
}

std::vector<GraphicItem> ArticleJob::GetNewGraphicItems()
{
	std::vector<GraphicItem> arrNewGraphicItems;
	for (int nIndex = 0; nIndex < m_arrGraphicItems.size(); nIndex++)
	{
		if (m_arrGraphicItems.at(nIndex).IsNewImage())
			arrNewGraphicItems.push_back(m_arrGraphicItems.at(nIndex));
	}
	return arrNewGraphicItems;
}