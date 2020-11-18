#include "IllustratorSDK.h"
#include "CommonUtil.h"
#include "Logger.h"
#include "AIDocumentList.h"
#include "AINativeAction.h"
#include "AIDocumentAction.h"
#include "AIObjectAction.h"
#include "AIPDFFormatAction.h"
#include "AITransformArt.h"
#include "FerreroServerFacade.h"
#include "HtmlUIController.h"
#include "TemplateLog.h"
#include "ASUserInteraction.h"
#include "AISwatchLibraries.h"

#include "SDKErrors.h"
#ifdef _WINDOWS
#include <direct.h>
#else
#include <sys/stat.h>
#endif
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <cstdio>
#ifndef _WINDOWS
#include "dirent.h"
#endif

extern "C"	AILayerSuite*					sAILayer;
extern "C"	AIFoldersSuite*					sAIFolder;
extern  "C" AIDocumentListSuite*			sAIDocumentList;
extern	"C" AIDocumentSuite*				sAIDocument;
extern "C" SPBasicSuite*					sSPBasic;
extern "C" AIActionManagerSuite*            sAIActionManager;
extern "C" AIDictionarySuite*				sAIDictionary;
extern "C" AIPlacedSuite*					sAIPlaced;
extern "C" AIArtSuite*						sAIArt;
extern "C" AIRealMathSuite*					sAIRealMath;
extern "C" AITransformArtSuite*				sAITransformArt;
extern "C" AIUserSuite*						sAIUserSuite;
extern "C" ASUserInteractionSuite*			sAIUserInteractionSuite;
extern "C" AISwatchListSuite*				sAISwatchListSuite;
extern "C" AISwatchLibrariesSuite*			sAISwatchLibrariesSuite;
extern "C" AIPreferenceSuite*				sAIPreferenceSuite;

static const char* ILST_APP = "ILST";

CommonUtil::CommonUtil()
{

}

string CommonUtil::GetCategoryTemplates(string strCategory, CEPTestPanelController* panelController, string &strErrorString)
{
	string strTemplates = "";
	FerreroServerFacade oFerreroServerFacade;
	string strGetCategoriesParam = "function=getCategoryTemplates&category="+ strCategory;
	strTemplates = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strGetCategoriesParam, strErrorString);
	return strTemplates;
}

std::vector<string>  CommonUtil::StringToArray(string strContent, string strSeparator)
{
	std::vector<string> arrItems;
	char * poItem;
	poItem = strtok((char*)strContent.c_str(), strSeparator.c_str());
	while (poItem != NULL)
	{
		string strString = "";
		strString.append(poItem);
		arrItems.push_back(strString);
		poItem = strtok(NULL, strSeparator.c_str());
	}
	return arrItems;
}

void CommonUtil::ConvertTemplate(string strTemplateString, CEPTestPanelController* panelController)
{
	do
	{
		ArtworkUtil oUtil;
		try
		{
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& oException)
		{
			oUtil.SendEventMessageToPanel("com.ferrero.converttemplate.result", "");
			break;
		}
		sAIUserInteractionSuite->SetInteractionAllowed(kASInteractWithNone);
		FerreroServerFacade oFerreroServerFacade;
		TemplateLog oTemplateLog;
		ArtworkUtil oArtworkUtil;
		string strTemplateID = "";
		string strCategoryID = "";
		std::vector<string> arrCategory = oArtworkUtil.Split(strTemplateString, ",");
		int nSize = arrCategory.size();
		if(nSize > 0)
			oTemplateLog.SetCategory(arrCategory[0]);
		if(nSize > 1)
			strTemplateID = arrCategory[1];
		if (nSize > 2)
			strCategoryID = arrCategory[2];
		if (strTemplateID.length() < 2)
		{
			oUtil.SendEventMessageToPanel("com.ferrero.converttemplate.result", "");
			break;
		}
		string strErrorString = "";
		Logger::Info("MenuPlayPlugin", "ConvertTemplate template id : ", strTemplateID);
		oTemplateLog.SetTemplateID(strTemplateID);

		if (!CanConvertTemplate(strTemplateID, strErrorString))
		{
			Logger::Info("MenuPlayPlugin", "ConvertTemplate template id : ", strTemplateID);
			Logger::Info("MenuPlayPlugin", "Template already being converting by other process : ", strTemplateID);
			oUtil.SendEventMessageToPanel("com.ferrero.log", "Template already being converting by other process : "+ strTemplateID);
			oUtil.SendEventMessageToPanel("com.ferrero.converttemplate.result", "");
			break;
		}
		if (strErrorString.length() > 0)
		{
			Logger::Info("MenuPlayPlugin", "Error checking for can convert template call : ", strErrorString);
			oUtil.SendEventMessageToPanel("com.ferrero.log", "Error checking for can convert template call : " + strErrorString);
			oUtil.SendEventMessageToPanel("com.ferrero.converttemplate.result", "");
			break;
		}
		string strMissingFonts = GetMissingFonts(strTemplateID, strErrorString);
		if (strErrorString.length() > 0)
			oTemplateLog.SetErrorString(strErrorString);
		else if (strMissingFonts.length() > 0)
		{
			int nError = strMissingFonts.find("KO");
			string strerror = (nError < 0 ? "The conversion process stopped due to missing font: " : "Error checking for missing fonts: ");
			oTemplateLog.SetErrorString( strerror + strMissingFonts);
			Logger::Info("MenuPlayPlugin", "ConvertTemplate template id : ", strTemplateID);
			Logger::Info("MenuPlayPlugin", strerror, strMissingFonts);
		}
		strErrorString = oTemplateLog.GetError();
		if (strErrorString.length() > 0)
		{
			ArticleJob oEmptyArticleJob;
			processTemplateStatus(oTemplateLog, oEmptyArticleJob);
			break;
		}

		string strDownloadTemplateParam = "function=getConvertTemplateZip&projectid=";
		strDownloadTemplateParam.append(strTemplateID);
		string oResponse = oUtil.GetTemplateXML(strTemplateID, strErrorString);
//		string oResponse = oArtworkUtil.GetFileContent("C:\\Users\\admin\\Documents\\ferrero\\test\\xml\\template.xml");
		if (strErrorString.length() > 0)
			oTemplateLog.SetErrorString(strErrorString);
		Logger::Info("MenuPlayPlugin", "ConvertArtwork - XML Response : ", oResponse);
		ArticleJob oArticleJob(oResponse, panelController);
		AIReal nWidth = oArticleJob.GetWidth(); // oUtil.GetComponent(strTemplateInfo, 2).size() > 0 ? std::stod(oUtil.GetComponent(strTemplateInfo, 2)) : 215.9;
		Logger::Info("MenuPlayPlugin", "ConvertTemplate Page width : ", to_string(nWidth));
		AIReal nHeight = oArticleJob.GetHeight(); // oUtil.GetComponent(strTemplateInfo, 3).size() > 0 ? std::stod(oUtil.GetComponent(strTemplateInfo, 3)) : 279.9;
		Logger::Info("MenuPlayPlugin", "ConvertTemplate Page height : ", to_string(nHeight));
		oArticleJob.SetTemplateId(strTemplateID);
		oArticleJob.SetWidth(nWidth);
		oArticleJob.SetHeight(nHeight);
		oTemplateLog.SetNumberOfObjects(oArticleJob.GetGraphicItemsCount());
		string strMessage = "Downloading template : " + strTemplateID + " Start";
		oUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
		oFerreroServerFacade.download(kFerreroWFESS2URL, "/SapRequestServlet", strDownloadTemplateParam, strTemplateID, strErrorString);
		if (strErrorString.length() > 0)
		{
			oTemplateLog.SetErrorString(strErrorString);
			strMessage = "Downloading template Error : " + strErrorString;
			oUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);

		}
		if (strErrorString.length() == 0)
		{
			strMessage = "Downloading template : " + strTemplateID + " End";
			oUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
			try
			{
				oArticleJob.CreateDocument(false, oTemplateLog);
			}
			catch (...)
			{
				strErrorString = "Error creating document\n";
				oTemplateLog.SetErrorString(strErrorString);
			}
		}
		strErrorString = oTemplateLog.GetError();
		if (strErrorString.length() == 0)
		{
			oArticleJob.SetEffects(oArticleJob.BuildEffects());
			if (IsSwatchLibraryFound(strTemplateID))
			{
				oTemplateLog.SetNumberOfLayers(oArticleJob.GetLayersCount());
				string strDocPath = oArticleJob.GetTemplateDocumentPath();
				AIDocumentHandle oDocument = NULL;
				oArtworkUtil.OpenDocument(oDocument, strDocPath);
				strMessage = oArtworkUtil.GetSwatchFilePath(strTemplateID).GetFullPath().as_Platform() +"~"+strTemplateID + "~" + to_string(oTemplateLog.GetNumberOfLayers()) + "~" + to_string(oTemplateLog.GetNumberOfObjects())+"~"+ oArticleJob.GetEffects();
				oArtworkUtil.SendEventMessageToPanel("com.ferrero.addswatches", strMessage);
			}
			else
				ProcessUploadTemplate(oArticleJob, oTemplateLog);
		}
		else
			processTemplateStatus(oTemplateLog, oArticleJob);
	} while (false);
	try
	{
		XMLPlatformUtils::Terminate();
	}
	catch (const XMLException& oException)
	{
	}
}

void CommonUtil::AddSwatches(string strTemplateString, CEPTestPanelController* panelController)
{
	ArtworkUtil oArtworkUtil;
	string strTemplateID = "";
	string strLayerCount = "";
	string strObjectCount = "";
	string strEffects = "";
	std::vector<string> arrTemplateInfo = oArtworkUtil.Split(strTemplateString, "~");
	if(arrTemplateInfo.size() > 1)
		strTemplateID = arrTemplateInfo[1];
	if (arrTemplateInfo.size() > 2)
		strLayerCount = arrTemplateInfo[2];
	if (arrTemplateInfo.size() > 3)
		strObjectCount = arrTemplateInfo[3];
	if (arrTemplateInfo.size() > 4)
		strEffects = arrTemplateInfo[4];
	ArticleJob oArticleJob;
	TemplateLog oTemplateLog;
	oArticleJob.SetTemplateId(strTemplateID);
	oArticleJob.SetLayerCount(std::stoi(strLayerCount));
	oArticleJob.SetEffects(strEffects);
	oTemplateLog.SetTemplateID(strTemplateID);
	oTemplateLog.SetNumberOfLayers(std::stoi(strLayerCount));
	oTemplateLog.SetNumberOfObjects(std::stoi(strObjectCount));
	InspectSwatchLibrary(strTemplateID, oArticleJob);
	ProcessUploadTemplate(oArticleJob, oTemplateLog);
}
#ifdef _WINDOWS
	bool CommonUtil::IsSwatchLibraryFound(string strTemplateID)
#else
	Boolean CommonUtil::IsSwatchLibraryFound(string strTemplateID)
#endif
{
#ifdef _WINDOWS
	bool bIsFound = false;
#else
	Boolean bIsFound = false;
#endif
	ArtworkUtil oArtworkUtil;
	ai::FilePath oSwatchFilePath = oArtworkUtil.GetSwatchFilePath(strTemplateID);
	string strpath = oSwatchFilePath.GetFullPath().as_Platform();
	bIsFound = oSwatchFilePath.Exists(false);
	return bIsFound;
}

void CommonUtil::ProcessUploadTemplate(ArticleJob oArticleJob, TemplateLog oTemplateLog)
{
 	oArticleJob.UploadTemplate(oTemplateLog);
	oTemplateLog.SetNumberOfLayers(oArticleJob.GetLayersCount());
	string strErrorString = oTemplateLog.GetError();
	string strEffects = oArticleJob.GetEffects();
	if (strErrorString.length() > 0)
		oTemplateLog.SetErrorString(strErrorString);
	else if (strEffects.length() == 0)
    {
        string strError = oTemplateLog.GetError();
		addTemplateToConvertedTable(oTemplateLog.GetTemplateID(), oTemplateLog.GetCategory(), strError, oTemplateLog);
    }
	processTemplateStatus(oTemplateLog, oArticleJob);
}

void CommonUtil::processTemplateStatus(TemplateLog oTemplateLog, ArticleJob oArticleJob)
{
	ArtworkUtil oUtil;
	string strErrorString = oTemplateLog.GetError();
	string strTemplateID = oTemplateLog.GetTemplateID();
	string strMessage = "";
	string strEffects = oArticleJob.GetEffects();
	oTemplateLog.SetStatus(strErrorString.length() > 0 ? "KO" : strEffects.length() > 0 ? "OK-Need review" : "OK");
	if (strErrorString.length() > 0)
		strMessage = "Error converting template " + strTemplateID + "  Error message : " + strErrorString;
	strMessage += " \n Convertion status for template " + strTemplateID + ": " + oTemplateLog.GetStatus();
	oUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
	oTemplateLog.WriteCSVLog();
	oTemplateLog.WriteErrorLog();
	DeleteTemplateFiles(oTemplateLog.GetTemplateID());
	strMessage = CleanupConversion(oTemplateLog.GetTemplateID());
	oUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
	if (strErrorString.length() > 0)
		addErrorToConvertedKOTable(oTemplateLog.GetTemplateID(), oTemplateLog.GetCategory(), strErrorString);
	if (strEffects.length() > 0)
		addToConvertedTemplatesToCheckTable(oTemplateLog, strEffects);
	oUtil.SendEventMessageToPanel("com.ferrero.converttemplate.result", "");
}

string CommonUtil::addTemplateToConvertedTable(string strTemplateID,string strCategoryID, string &strErrorString, TemplateLog oTemplateLog)
{
	FerreroServerFacade oFerreroServerFacade;
	string strAddTemplateParam = "function=addToConvertTemplates&templateid=" + strTemplateID+"&presidenzaid=&layercount="+ to_string(oTemplateLog.GetNumberOfLayers())+"&objectcount="+ to_string(oTemplateLog.GetNumberOfObjects());
	string strResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strAddTemplateParam, strErrorString);
	return strResponse;
}

string CommonUtil::addToConvertedTemplatesToCheckTable(TemplateLog oTemplateLog, string strEffects)
{
	FerreroServerFacade oFerreroServerFacade;
	string strErrorString = "";
	string strAddTemplateParam = "function=addToConvertedTemplatesToCheckTable&templateid=" + oTemplateLog.GetTemplateID() + "&layercount=" + to_string(oTemplateLog.GetNumberOfLayers()) + "&objectcount=" + to_string(oTemplateLog.GetNumberOfObjects()) + "&effects=" + oFerreroServerFacade.URLEncode(strEffects);
	string strResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strAddTemplateParam, strErrorString);
	if (strErrorString.length() > 0)
		Logger::Error("CommonUtil", "Error addToConvertedTemplatesToCheckTable : ", strErrorString);
	return strResponse;
}

string CommonUtil::addErrorToConvertedKOTable(string strTemplateID, string strCategoryID, string strErrorString)
{
	ArtworkUtil oUtil;
	Logger::Info("addErrorToConvertedKOTable", "template id : ", strTemplateID);
	FerreroServerFacade oFerreroServerFacade;
	string strAddTemplateParam = "function=addToConvertTemplatesko&templateid=" + strTemplateID + "&errorlog=" + oFerreroServerFacade.URLEncode(strErrorString);
	string strResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strAddTemplateParam, strErrorString);
	Logger::Info("addErrorToConvertedKOTable", "strErrorString : ", strErrorString);
	Logger::Info("addErrorToConvertedKOTable", "strResponse : ", strResponse);
	return strResponse;
}

string CommonUtil::CleanupConversion(string strTemplateID)
{
	FerreroServerFacade oFerreroServerFacade;
	string strAddTemplateParam = "function=cleanupConversion&projectid=" + strTemplateID;
	string strErrorString = "";
	string strResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strAddTemplateParam, strErrorString);
	return strResponse;
}

void CommonUtil::DeleteTemplateFiles(string strTemplateID)
{
	ArtworkUtil oUtil;
	string strTemplateFolder = oUtil.GetUserFolder();
	strTemplateFolder.append(strTemplateID);
	oUtil.DeleteFolder(strTemplateFolder);
	string strTemplateZip = strTemplateFolder;
	strTemplateZip.append(".zip");
	remove(strTemplateZip.c_str());
}
#ifdef _WINDOWS
	bool CommonUtil::CanConvertTemplate(string strTemplateID, string &strErrorString)
#else
	Boolean CommonUtil::CanConvertTemplate(string strTemplateID, string &strErrorString)
#endif
{
	FerreroServerFacade oFerreroServerFacade;
	string strAddTemplateParam = "function=canConvertTemplate&templateid=" + strTemplateID;
 	string strResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strAddTemplateParam, strErrorString);
#ifdef _WINDOWS
	bool bCanConvert = (strResponse == "OK");
#else
	Boolean bCanConvert = (strResponse == "OK");
#endif
	return bCanConvert;
}

string CommonUtil::GetMissingFonts(string strTemplateID, string &strErrorString)
{
	FerreroServerFacade oFerreroServerFacade;
	string strAddTemplateParam = "function=getMissingFonts&projectid=" + strTemplateID;
	string strResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strAddTemplateParam, strErrorString);
	return strResponse;
}

void CommonUtil::InspectSwatchLibrary(string strTemplateID, ArticleJob oArticleJob)
{
	ai::int32 nCount = 0;
	sAISwatchLibrariesSuite->CountLibraries(&nCount);
	AISwatchListRef oSwatchlist;
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		ai::UnicodeString name;
		sAISwatchLibrariesSuite->GetNthLibraryName(nIndex, name);
		string strLibraryName = name.as_Platform();
		int nFound = strLibraryName.find(strTemplateID);
		if (nFound >= 0)
		{
			sAISwatchLibrariesSuite->GetNthLibrary(nIndex, &oSwatchlist);
			AddSwatchToDocument(oSwatchlist, oArticleJob);
		}
	}
}

void CommonUtil::AddSwatchToDocument(AISwatchListRef &oSwatchlist, ArticleJob oArticleJob)
{
	AIDocumentHandle oDocument = NULL;
	ASErr result = sAIDocumentList->GetNthDocument(&oDocument, 0);
	aisdk::check_ai_error(result);
	AISwatchListRef oDocumentSwatchListRef;
	result = sAISwatchListSuite->GetSwatchList(oDocument, &oDocumentSwatchListRef);
	ai::int32 nSwatchCount = sAISwatchListSuite->CountSwatches(oSwatchlist);
	if (nSwatchCount > 0)
		RemoveDocumentSwatch(oDocumentSwatchListRef);
	for (int nIndex = 0; nIndex < nSwatchCount; nIndex++)
	{
		AISwatchRef oSwatchRef = sAISwatchListSuite->GetNthSwatch(oSwatchlist, nIndex);
		ai::UnicodeString strSwatchName;
		result = sAISwatchListSuite->GetSwatchName(oSwatchRef, strSwatchName);
		if (IsSwatchExists(oDocumentSwatchListRef, strSwatchName))
			continue;
		AIColor aicolor;
		result = sAISwatchListSuite->GetAIColor(oSwatchRef, &aicolor);
		AISwatchRef oNewSwatch = sAISwatchListSuite->InsertNthSwatch(oDocumentSwatchListRef, -1);
		result = sAISwatchListSuite->SetSwatchName(oNewSwatch, strSwatchName);
		result = sAISwatchListSuite->SetAIColor(oNewSwatch, &aicolor);
	}
	string strDocPath = oArticleJob.GetTemplateDocumentPath();
	string strMessage = "Saving document to path : " + strDocPath;
	ArtworkUtil oUtil;
	oUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
	oUtil.SaveDocument(oDocument, strDocPath);
	oUtil.CloseDocument(oDocument);
}

void CommonUtil::RemoveDocumentSwatch(AISwatchListRef &oDocumentSwatchListRef)
{
	ai::int32 nSwatchCount = sAISwatchListSuite->CountSwatches(oDocumentSwatchListRef);
	for (int nIndex = nSwatchCount - 1; nIndex >=0; nIndex--)
		sAISwatchListSuite->RemoveNthSwatch(oDocumentSwatchListRef, nIndex);
}

#ifdef _WINDOWS
	bool CommonUtil::IsSwatchExists(AISwatchListRef &oDocumentSwatchListRef, ai::UnicodeString strSwatchName)
#else
	Boolean CommonUtil::IsSwatchExists(AISwatchListRef &oDocumentSwatchListRef, ai::UnicodeString strSwatchName)
#endif
{
#ifdef _WINDOWS
	bool bSwatchExists = false;
#else
	Boolean bSwatchExists = false;
#endif
	ai::int32 nSwatchCount = sAISwatchListSuite->CountSwatches(oDocumentSwatchListRef);
	for (int nIndex = 0; !bSwatchExists && nIndex < nSwatchCount; nIndex++)
	{
		AISwatchRef oSwatchRef = sAISwatchListSuite->GetNthSwatch(oDocumentSwatchListRef, nIndex);
		ai::UnicodeString strDocSwatchName;
		ASErr result = sAISwatchListSuite->GetSwatchName(oSwatchRef, strDocSwatchName);
		bSwatchExists = (strDocSwatchName.compare(strSwatchName) == 0);
	}
	return bSwatchExists;
}
