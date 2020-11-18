//========================================================================================
//  
//  $File: //ai_stream/rel_21_0/devtech/sdk/public/samplecode/MenuPlay/Source/MenuPlayPlugin.cpp $
//
//  $Revision: #1 $
//
//  Copyright 1987 Adobe Systems Incorporated. All rights reserved.
//  
//  NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance 
//  with the terms of the Adobe license agreement accompanying it.  If you have received
//  this file from a source other than Adobe, then your use, modification, or 
//  distribution of it requires the prior written permission of Adobe.
//  
//========================================================================================

#include "IllustratorSDK.h"
#include "MenuPlayPlugin.h"
#include "MenuPlayID.h"
#include "FerreroServerFacade.h"
#include "ArticleJob.h"
#include "ArtworkUtil.h"
#include "Logger.h"
#include "IAIStringFormatUtils.h"
#include "AIStringFormatUtils.h"
#include "AIPerspectiveTransform.h"
#include "AITransformArt.h"
#include "AIArt.h"
#include "AIGroup.h"
#include "RuleData.h"
#include "AIMenuCommandNotifiers.h"
#include "AIUUID.h"
#include "IAIAutoBuffer.h"
#include "TemplateData.h"
#include "Path.h"
#include "AICSXS.h"
#include "CommonUtil.h"
#include "ASUserInteraction.h"
#include "AISwatchLibraries.h"

#ifdef WINDOWS
#include <windows.h>
#endif
#include <iostream>
using namespace std;
using namespace ai;

extern "C"
{
	AIDocumentListSuite*			sAIDocumentList = NULL;
	AIDocumentSuite*				sAIDocument = NULL;
	AIArtSuite*						sAIArt = NULL;
	AIPlacedSuite*					sAIPlaced = NULL;
	AILayerSuite*					sAILayer = NULL;
	AIFoldersSuite*					sAIFolder = NULL;
	AIActionManagerSuite*			sAIActionManager = NULL;
    AIStringFormatUtilsSuite*       sAIStringFormatUtils = NULL;
	AIRealMathSuite*				sAIRealMath = NULL;
	AIPerspectiveTransformSuite*	sAIPerspectiveTransform = NULL;
	AITransformArtSuite*			sAITransformArt = NULL;
    AIMaskSuite*                    sAIMask = NULL;
    AIPathSuite*                    sAIPath = NULL;
    AIGroupSuite*                   sAIGroup = NULL;
	AIMatchingArtSuite*				sAIMatchingArt = NULL;
	AIDictionarySuite*				sAIDictionary = NULL;
	AIUserSuite*					sAIUserSuite = NULL;
	AIUUIDSuite*					sAIUUID = NULL;
	AIBlendStyleSuite*				sAIBlendStyle = NULL;
	AIPathStyleSuite*				sAIPathStyle = NULL;
	ASUserInteractionSuite*			sAIUserInteractionSuite = NULL;
	AIMdMemorySuite*				sAIMdMemory = NULL;
	AICSXSExtensionSuite*			sAICSXSExtensionSuite = NULL;
	AISwatchListSuite*				sAISwatchListSuite = NULL;
	AISwatchLibrariesSuite*			sAISwatchLibrariesSuite = NULL;
	AIPreferenceSuite*				sAIPreferenceSuite = NULL;
};

MenuPlayPlugin* gPlugin = NULL;
/*
*/
Plugin* AllocatePlugin(SPPluginRef pluginRef)
{
	return new MenuPlayPlugin(pluginRef);
}

/*
*/
void FixupReload(Plugin* plugin)
{
	MenuPlayPlugin::FixupVTable((MenuPlayPlugin*)plugin);
}

ImportSuite	gPostStartupSuites[] = {

	kAIDocumentListSuite, kAIDocumentListSuiteVersion, &sAIDocumentList,
	kAIArtSuite, kAIArtSuiteVersion, &sAIArt,
	kAIDocumentSuite, kAIDocumentVersion, &sAIDocument,
	kAIActionManagerSuite, kAIActionManagerSuiteVersion, &sAIActionManager,
	kAIPlacedSuite, kAIPlacedSuiteVersion, &sAIPlaced,
	kAILayerSuite,kAILayerSuiteVersion, &sAILayer,
	kAIFilePathSuite, kAIFilePathSuiteVersion, &sAIFilePath,
	kAIFoldersSuite, kAIFoldersSuiteVersion, &sAIFolder,
	kAIStringFormatUtilsSuite, kAIStringFormatUtilsSuiteVersion, &sAIStringFormatUtils,
	kAIRealMathSuite, kAIRealMathSuiteVersion, &sAIRealMath,
	kAIPerspectiveTransformSuite, kAIPerspectiveTransformSuiteVersion, &sAIPerspectiveTransform,
	kAITransformArtSuite, kAITransformArtSuiteVersion, &sAITransformArt,
	kAIMaskSuite, kAIMaskSuiteVersion, &sAIMask,
    kAIPathSuite, kAIPathSuiteVersion, &sAIPath,
    kAIGroupSuite, kAIGroupSuiteVersion, &sAIGroup,
	kAIMatchingArtSuite, kAIMatchingArtSuiteVersion, &sAIMatchingArt,
	kAIDictionarySuite, kAIDictionarySuiteVersion, &sAIDictionary,
	kAIUserSuite, kAIUserSuiteVersion, &sAIUserSuite,
	kAIUUIDSuite, kAIUUIDVersion, &sAIUUID,
	kAIBlendStyleSuite, kAIBlendStyleSuiteVersion, &sAIBlendStyle,
	kAIPathStyleSuite, kAIPathStyleSuiteVersion, &sAIPathStyle,
	kASUserInteractionSuite, kASUserInteractionSuiteVersion, &sAIUserInteractionSuite,
	kAIMdMemorySuite, kAIMdMemorySuiteVersion, &sAIMdMemory,
	kAICSXSExtensionSuite, kAICSXSExtensionSuiteVersion, &sAICSXSExtensionSuite,
	kAISwatchListSuite, kAISwatchListSuiteVersion, &sAISwatchListSuite,
	kAISwatchLibrariesSuite, kAISwatchLibrariesSuiteVersion, &sAISwatchLibrariesSuite,
	kAIPreferenceSuite, kAIPreferenceSuiteVersion, &sAIPreferenceSuite,
	nullptr, 0, nullptr
};
/*
*/
MenuPlayPlugin::MenuPlayPlugin(SPPluginRef pluginRef)
	: Plugin(pluginRef),
	fManualUpdateAIMenu(NULL),
	fMultiDemoAIMenu(NULL),
	fConvertTemplateAIMenu(NULL),
	fAboutPluginMenu(NULL),
	fNotifySelectionChanged (NULL),
	fNotifyArtObjectChanged (NULL),
	fCheckMeChecked(false),
	fCheckMeEnabled(false),
	fPanelController(NULL),
	fPlugPlugSetupCompleteNotifier(NULL)
{
	strncpy(fPluginName, kMenuPlayPluginName, kMaxStringLength);
}

/*
*/
ASErr MenuPlayPlugin::Message(char* caller, char* selector, void *message)
{
	ASErr error = kNoErr;

	try {
		error = Plugin::Message(caller, selector, message);
	}
	catch (ai::Error& ex) {
		error = ex;
	}
	catch (...) {
		error = kCantHappenErr;
	}
	if (error) {
		if (error == kUnhandledMsgErr) {
			// Defined by Plugin.hpp and used in Plugin::Message - ignore.
			error = kNoErr;
		}
		else {
			Plugin::ReportError(error, caller, selector, message);
		}
	}
	return error;
}


/*
*/
ASErr MenuPlayPlugin::StartupPlugin(SPInterfaceMessage* message)
{
	ASErr error = kNoErr;
	do
	{
		error = Plugin::StartupPlugin(message);
		if (error)
			break;
		error = this->AddMenus(message);
		if (error)
			break;
		// Create panel controller
		if (fPanelController == NULL)
		{
			fPanelController = new CEPTestPanelController();
			error = Plugin::LockPlugin(true);

			if (error)
				return error;
		}
		// Add notifier for adding CSXS event listeners
		error = sAINotifier->AddNotifier(fPluginRef, kMenuPlayPluginName,
			kAICSXSPlugPlugSetupCompleteNotifier, &fPlugPlugSetupCompleteNotifier);


		// Add notifier.
		error = this->AddNotifier(message);
		aisdk::check_ai_error(error);
	} while (false);
	return error;
}

/*
*/
ASErr MenuPlayPlugin::AddMenus(SPInterfaceMessage* message)
{
	ASErr error = kNoErr;
	// First we create a normal plug-in menu to use a hierarchical root		
	AIPlatformAddMenuItemDataUS throwAwayMenuData;
	throwAwayMenuData.groupName = kWindowUtilsMenuGroup;
	throwAwayMenuData.itemText = ai::UnicodeString(kMenuPlaySDKString);
	AIMenuItemHandle throwAwayAIMenu;
	error = sAIMenu->AddMenuItem(message->d.self, kSDKMenuGroup, &throwAwayMenuData, 0, &throwAwayAIMenu);
	if (error)
		goto error;
	// Now we declare the menu item a menu group
	AIMenuGroup	throwAwayMenuGroup;
	sAIMenu->AddMenuGroupAsSubMenu(kSDKMenuGroup, 0, throwAwayAIMenu, &throwAwayMenuGroup);
	throwAwayMenuData.groupName = kSDKMenuGroup;
	throwAwayMenuData.itemText = ai::UnicodeString(kMenuPlayConvertTemplateString);
	// We keep the AIMenuItemHandle to figure out which menu was selected later
	error = sAIMenu->AddMenuItem(message->d.self, "Convert Template", &throwAwayMenuData, 0, &fConvertTemplateAIMenu);
	if (error)
		goto error;
	throwAwayMenuData.itemText = ai::UnicodeString(kMenuPlayCreateArtworkString);
	error = sAIMenu->AddMenuItem(message->d.self, "Create Artwork", &throwAwayMenuData, 0, &fCreateArtworkAIMenu);
	if (error)
		goto error;
	throwAwayMenuData.itemText = ai::UnicodeString(kMenuPlayCreateTemplateString);
	error = sAIMenu->AddMenuItem(message->d.self, "Create Template", &throwAwayMenuData, 0, &fCreateTemplateAIMenu);

error:
	return error;
}

/*
*/
ASErr MenuPlayPlugin::GoMenuItem(AIMenuMessage* message)
{

	ASErr error = kNoErr;
	// Compare the menuItem selected (in the message) with our stored values to see if we do anything
	if (message->menuItem == this->fAboutPluginMenu) {
		// Pop this plug-in's about box.
		SDKAboutPluginsHelper aboutPluginsHelper;
		aboutPluginsHelper.PopAboutBox(message, "About MenuPlay", kSDKDefAboutSDKCompanyPluginsAlertString);
	}
	else if (message->menuItem == this->fConvertTemplateAIMenu)
	{
		ConvertTemplate();
	}
	else if (message->menuItem == this->fCreateArtworkAIMenu)
	{
		CreateArtwork();
	}
	else if (message->menuItem == this->fCreateTemplateAIMenu)
	{
		CreateTemplate();
	}
	return error;
}

void MenuPlayPlugin::CreateTemplate()
{
	Logger::Info("MenuPlayPlugin", "CreateTemplate", "");
	do
	{
		try
		{
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& oException)
		{
			break;
		}
		ArtworkUtil oUtil;
		string strConFigFilePath = oUtil.GetUserFolder();
		strConFigFilePath.append(kCreateTemplateConfigFileName);
		string strContent = oUtil.GetFileContent(strConFigFilePath);
		Logger::Info("MenuPlayPlugin", "CreateTemplate - strContent", strContent);
		string strErrorString = "";
		string strTemplateID = GenerateArtworkID(strErrorString);
		string strGraphicXML = GetGraphicXML(strContent, strTemplateID);
		Logger::Info("MenuPlayPlugin", "CreateTemplate - New Template ID", strTemplateID);
		ArticleJob oArticleJob(strGraphicXML, fPanelController);
		oArticleJob.SetTemplateId(strTemplateID);
		oArticleJob.SetWidth(kDefaultPageWidth);
		oArticleJob.SetHeight(kDefaultPageHeight);
        TemplateLog oTemplateLog;
		oArticleJob.CreateDocument(true, oTemplateLog);
		XMLPlatformUtils::Terminate();
	} while (false);
}

void MenuPlayPlugin::CreateArtwork()
{
	do
	{
		try
		{
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& oException)
		{
			break;
		}
		Logger::Info("MenuPlayPlugin", "CreateArtwork", "");
		std::vector<string> arrArtworks = ExtractConfigFileContents(kArtworkConfigFileName);
		Logger::Info("MenuPlayPlugin", "CreateArtwork arrArtworks.size : ", to_string(arrArtworks.size()));
		for (int nIndex = 0; nIndex < arrArtworks.size(); nIndex++)
		{
			string strArtworkInfo = arrArtworks[nIndex];
			Logger::Info("MenuPlayPlugin", "CreateArtwork strArtworkInfo : ", strArtworkInfo);
			string strLayerInfo = strArtworkInfo;
			string strTemplateID = "";

			char * poTemplate;
			poTemplate = strtok((char*)strArtworkInfo.c_str(), ":");
			if (poTemplate != NULL)
				strTemplateID.append(poTemplate);
			if (strTemplateID.size() == 0)
				continue;
			CreateArtwork(strTemplateID, strLayerInfo);
		}
	} while (false);
}

void MenuPlayPlugin::CreateArtwork(string strTemplateID, string strLayerInfo)
{

}

string MenuPlayPlugin::GenerateArtworkID(string &strErrorString)
{
	FerreroServerFacade oFerreroServerFacade;
	string strArtworkID = "";
	string strGenerateArtworkIDParam = "function=getprojectid";
	strArtworkID = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strGenerateArtworkIDParam, strErrorString);
	return strArtworkID;
}

void MenuPlayPlugin::ConvertTemplate()
{
	Logger::Info("MenuPlayPlugin", "ConvertTemplate", "");
	do
	{
		try
		{
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& oException)
		{
			break;
		}
		std::vector<string> arrTemplates = ExtractConfigFileContents(kTemplateConfigFileName);
		for (int nIndex = 0; nIndex < arrTemplates.size(); nIndex++)
			ConvertTemplate(arrTemplates.at(nIndex));
		XMLPlatformUtils::Terminate();
	} while (false);
}

void MenuPlayPlugin::ConvertTemplate(string strTemplateInfo)
{
	FerreroServerFacade oFerreroServerFacade;
	ArtworkUtil oUtil;
	string strTemplateID = oUtil.GetComponent(strTemplateInfo, 1);
	Logger::Info("MenuPlayPlugin", "ConvertTemplate template id : ", strTemplateID);
	AIReal nWidth = oUtil.GetComponent(strTemplateInfo, 2).size() > 0 ? std::stod(oUtil.GetComponent(strTemplateInfo, 2)) : 215.9;
	Logger::Info("MenuPlayPlugin", "ConvertTemplate Page width : ", to_string(nWidth));
	AIReal nHeight = oUtil.GetComponent(strTemplateInfo, 3).size() > 0 ? std::stod(oUtil.GetComponent(strTemplateInfo, 3)) : 279.9;
	Logger::Info("MenuPlayPlugin", "ConvertTemplate Page height : ", to_string(nHeight));
	string strDownloadTemplateParam = "function=getConvertTemplateZip&projectid=";
	strDownloadTemplateParam.append(strTemplateID);
	string strErrorString = "";
	string oResponse = oUtil.GetTemplateXML(strTemplateID, strErrorString);
    Logger::Info("MenuPlayPlugin", "ConvertArtwork - XML Response : ", oResponse);
    ArticleJob oArticleJob(oResponse, fPanelController);
	oArticleJob.SetTemplateId(strTemplateID);
	oArticleJob.SetWidth(nWidth);
	oArticleJob.SetHeight(nHeight);
	oFerreroServerFacade.download(kFerreroWFESS2URL, "/SapRequestServlet", strDownloadTemplateParam, strTemplateID, strErrorString);
    TemplateLog oTemplateLog;
	oArticleJob.CreateDocument(false, oTemplateLog);
}

std::vector<string>  MenuPlayPlugin::ExtractConfigFileContents(string strConfigFileName)
{
	std::vector<string> arrArtworks;
	ArtworkUtil oUtil;
	string strConFigFilePath = oUtil.GetUserFolder();
	strConFigFilePath.append(strConfigFileName);
	string strContent = oUtil.GetFileContent(strConFigFilePath);
	char * poArtwork;
	poArtwork = strtok((char*)strContent.c_str(), ";");
	while (poArtwork != NULL)
	{
		string strArtworkID = "";
		strArtworkID.append(poArtwork);
		arrArtworks.push_back(strArtworkID);
		poArtwork = strtok(NULL, ";");
	}
	return arrArtworks;
}

bool MenuPlayPlugin::ParseResponse(string strResponse, string strArtworkID)
{
	bool bSuccess = false;
	// Other terminations and cleanup.
	return bSuccess;
}

ASErr MenuPlayPlugin::UpdateMenuItem(AIMenuMessage* message)
{
	ASErr error = kNoErr;

	// This is only valid when kSelectorAIUpdateMenuItem is received, which is why we're here
	ai::int32 inArtwork = 0, isSelected = 0, isTrue = 0;

	error = sAIMenu->GetUpdateFlags(&inArtwork, &isSelected, &isTrue);
	if (error)
		goto error;

	// Do something based on the result.  It's easier than GetMatchingArt for this example, and can
	// also be used for a quick check before more processing.
	if (isSelected & kIfGuide) // If a guide is selected
	{
		sAIMenu->SetItemText(fManualUpdateAIMenu, ai::UnicodeString("Do Something To Selected Guides"));
	}
	else if (inArtwork & kIfGuide) // If there are guides in the document
	{
		sAIMenu->SetItemText(fManualUpdateAIMenu, ai::UnicodeString("Do Something To All Guides"));
	}
	else // There aren't any guides in the document
	{
		sAIMenu->SetItemText(fManualUpdateAIMenu, ai::UnicodeString("Manually updated for guides"));
	}

error:
	return error;
}


ASErr MenuPlayPlugin::PostStartupPlugin()
{
	ASErr error = kNoErr;
	try
	{
		// Acquire suites we could not get on plug-in startup.
		error = this->AcquirePostStartupSuites();
		if (error)
		{
			error = kNoErr;
		}
	}
	catch (ai::Error ex)
	{
		error = ex;
	}
	return error;
}

/*
*/
ASErr MenuPlayPlugin::AcquirePostStartupSuites()
{
	ASErr error = kNoErr;
	for (int i = 0; gPostStartupSuites[i].name != nullptr; ++i) {
		if (gPostStartupSuites[i].suite != nullptr) {
			ASErr tmperr = sSPBasic->AcquireSuite(gPostStartupSuites[i].name,
				gPostStartupSuites[i].version,
				(const void **)gPostStartupSuites[i].suite);
			SDK_ASSERT_MSG_NOTHROW(!tmperr,
				aisdk::format_args("AcquireSuite failed for suite=%s version=%d",
					gPostStartupSuites[i].name,
					gPostStartupSuites[i].version));
			if (tmperr && !error) {
				// A suite could not be acquired - note first error encountered for later return to caller then carry on.
				error = tmperr;
			}
		}
	}
	return error;
}

/*
*/
ASErr MenuPlayPlugin::ReleasePostStartupSuites()
{
	ASErr error = kNoErr;

	for (int i = 0; gPostStartupSuites[i].name != nullptr; ++i) {
		if (gPostStartupSuites[i].suite != nullptr) {
			void **s = (void **)gPostStartupSuites[i].suite;
			if (*s != nullptr) {
				ASErr tmperr = sSPBasic->ReleaseSuite(gPostStartupSuites[i].name, gPostStartupSuites[i].version);
				*s = nullptr;
				SDK_ASSERT_MSG_NOTHROW(!tmperr,
					aisdk::format_args("ReleaseSuite failed for suite=%s version=%d",
						gPostStartupSuites[i].name,
						gPostStartupSuites[i].version));
				if (tmperr && !error) {
					// A suite could not be released - note first error encountered for later return to caller then carry on.
					error = tmperr;
				}
			}
		}
	}

	return error;
}

/*
*/
ASErr MenuPlayPlugin::AddNotifier(SPInterfaceMessage *message)
{
	ASErr result = kNoErr;
	try 
	{
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kAIArtSelectionChangedNotifier, &fNotifySelectionChanged);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kAIArtObjectsChangedNotifier, &fNotifyArtObjectChanged);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kAIClearCommandPreNotifierStr, &fNotifyBeforeClear);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kConvertTemplatNotifier, &fNotifyConvertTemplate);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kAddSwatchNotifier, &fNotifyAddSwatch);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kOpenTemplateNotifier, &fNotifyOpenTemplate);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kOpenArtworkNotifier, &fNotifyOpenArtwork);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kZipPreviewFolderNotifier, &fNotifyZipPreviewFolder);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kPlaceNewTemplateFilesNotifier, &fNotifyPlaceNewTemplateFiles);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kAIDocumentOpenedNotifier, &fNotifyOpenDocument);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kProcessDownLoadCompleteNotifier, &fNotifyUploadDocument);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kProcessZipArtworkNotifier, &fNotifyProcessArtworkzip);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kProcessSaveTemplateXMLNotifier, &fNotifyProcessSaveTemplateXML);
		aisdk::check_ai_error(result);
		result = sAINotifier->AddNotifier(fPluginRef, "MenuPlayPlugin", kFinalizeArtworkCompleteNotifier, &fNotifyProcessFinalizeArtworkComplete);
		aisdk::check_ai_error(result);
		
	}
	catch (ai::Error& ex) 
	{
		result = ex;
	}
	catch (...)
	{
		result = kCantHappenErr;
	}
	return result;
}

/*
*/
ASErr MenuPlayPlugin::Notify(AINotifierMessage* message)
{
	ASErr result = kNoErr;
	ArtworkUtil oArtworkUtil;
	do
	{
		if (message->notifier == fPlugPlugSetupCompleteNotifier)
		{
			if (fPanelController != NULL)
				fPanelController->RegisterCSXSEventListeners();
		}
		if (message->notifier == fNotifyConvertTemplate)
		{
			string *poTemplate = (string*)message->notifyData;
			ConvertTemplate(*poTemplate, fPanelController);

		}
		if (message->notifier == fNotifyOpenTemplate)
		{
			string *poTemplate = (string*)message->notifyData;
			OpenTemplate(*poTemplate, fPanelController);
		}
		if (message->notifier == fNotifyAddSwatch)
		{
			string *poTemplate = (string*)message->notifyData;
			AddSwatches(*poTemplate, fPanelController);
		}
		if (message->notifier == fNotifyOpenArtwork)
		{
			string *poTemplate = (string*)message->notifyData;
			OpenArtwork(*poTemplate, fPanelController);
		}
		if (message->notifier == fNotifyZipPreviewFolder)
		{
			string *poTemplate = (string*)message->notifyData;
			ZipPreviewFolder(*poTemplate, fPanelController);
		}
		if (message->notifier == fNotifyPlaceNewTemplateFiles)
		{
			string *poTemplate = (string*)message->notifyData;
			PlaceNewTemplateFiles(*poTemplate, fPanelController);
		}
		if (message->notifier == fNotifyOpenDocument)
		{
			ProcessOpenDocument(message->notifyData);
		}
		if (message->notifier == fNotifyUploadDocument)
		{
			ProcessUploadDocument(message->notifyData);
		}
		if (message->notifier == fNotifyProcessArtworkzip)
		{
			string* poArtworkFile = (string*)message->notifyData;
			ZipArtworkFolder(*poArtworkFile, fPanelController);
		}
		if (message->notifier == fNotifyProcessSaveTemplateXML)
		{
			string* poTemplateXML = (string*)message->notifyData;
			SaveTemplateXML(*poTemplateXML, fPanelController);
		}
		if (message->notifier == fNotifyProcessFinalizeArtworkComplete)
		{
			oArtworkUtil.HandleFinilizeArtworkCompleteEvent();
		}
		
		string strTemplateID = oArtworkUtil.GetTemplateInfo(kTemplateIDkey);
		if (strTemplateID.length() == 0)
			break;
		try
		{
			if (message->notifier == fNotifyBeforeClear || message->notifier == fNotifyArtObjectChanged)
			{
				ai::ArtObjectsChangedNotifierData *poMessageData = (ai::ArtObjectsChangedNotifierData *)message->notifyData;
				if (poMessageData->artObjsChangedData.removedObjList.GetCount () > 0)
					VerifyDelete(poMessageData);
			}
			if (message->notifier == fNotifySelectionChanged)
			{
				Logger::Info("MenuPlayPlugin", "Notify", "Selection changed");
				AIArtHandle **matchingArt = NULL;
				ai::int32 numMatches;
				result = sAIMatchingArt->GetSelectedArt(&matchingArt, &numMatches);
				short nType = -1;
				for (int nIndex = 0; nIndex < numMatches; nIndex++)
				{
					AIArtHandle oHandle = matchingArt[0][nIndex];
					sAIArt->GetArtType(oHandle, &nType);
					if (nType == kPlacedArt)
						result = VerifyRules(oHandle);
				}
				//			sAIMdMemorySuite->MdMemoryDisposeHandle(*matchingArt);
				aisdk::check_ai_error(result);
			}
		}
		catch (ai::Error& ex)
		{
			result = ex;
		}
		catch (...)
		{
			result = kCantHappenErr;
		}
	} while (false);
	return result;
}

ASErr MenuPlayPlugin::VerifyRules(AIArtHandle oHandle)
{
	ASErr tResult = kNoErr;
	do
	{
		try
		{

			ArtworkUtil oUtil;
			string strRulesValue = GetDictionaryValue(oHandle);
			string strKey = GetDictionaryKey(oHandle);
			string strGraphicKey = kGraphicKeyPrefix;
			int nFound = strKey.find_first_of("_");
			if (nFound > 0)
				strGraphicKey.append(strKey.substr(nFound + 1));
			string strGraphicValue = oUtil.GetValueFromDocumentDictionary(strGraphicKey);
			GraphicItem oGraphicItem(strGraphicValue);
			RuleData oRuleData(strRulesValue);
			tResult = oRuleData.VerifyRules(oHandle, oGraphicItem);
		}
		catch (ai::Error& ex)
		{
			tResult = ex;
		}
		catch (...)
		{
			tResult = kCantHappenErr;
		}
	} while (false);
	return tResult;
}

ASErr MenuPlayPlugin::VerifyDelete(ai::ArtObjectsChangedNotifierData *poMessageData)
{
	ASErr tResult = kNoErr;
	do
	{
		if (poMessageData->artObjsChangedData.removedObjList.GetCount() <= 0)
			break;
		if (IsAnyTemplateItem (poMessageData))
		{
			// The removedObjList is an array of UUID.
			// Get the AIArtHandle of the removed object using AIUUIDSuite for each object in the removedObjList.
			// Hopefully, you should get proper AIArtHandle. Then, you can do the procedure below to determine if the delete can happen. 
			// as of now, just giving delete alert.

			ai::UnicodeString strResizeRuleFailedMessage("Should not delete template object!");
			sAIUserSuite->MessageAlert(strResizeRuleFailedMessage);
		}
	} while (false);
/*
	AIArtHandle oPlacedArt = oHandle;
	short nType;
	sAIArt->GetArtType(oPlacedArt, &nType);
	if (nType != kPlacedArt)
		oPlacedArt = GetPlacedArt(oPlacedArt);
	if (oPlacedArt != NULL && IsTemplateArt(oPlacedArt))
		tResult = kCantHappenErr;
*/
	return tResult;
}

bool MenuPlayPlugin::IsAnyTemplateItem(ai::ArtObjectsChangedNotifierData *poMessageData)
{
/*
	AutoBuffer<uuid> arrRemovedUUID = poMessageData->artObjsChangedData.removedObjList;
	bool bIsTemplateArt = false;
	for (int nIndex = 0; !bIsTemplateArt && nIndex < arrRemovedUUID.size(); nIndex++)
	{
		ai::uuid poUUID = arrRemovedUUID[nIndex].mData;
		AIArtHandle oHandle;
		sAIUUID->GetArtHandle(poUUID, oHandle);
		short nType;
		sAIArt->GetArtType(oHandle, &nType);
		if (nType == kPlacedArt)
		{
			string strkey = GetDictionaryKey(oHandle);
			bIsTemplateArt = strkey.length() > 0;
			string strUUID((const char *)poUUID.mData);
			ArtworkUtil oUtil;
			string strValue = oUtil.GetValueFromDocumentDictionary(strUUID);
			bIsTemplateArt = strValue.length() > 0;
		}
	}
	return bIsTemplateArt;
*/
	return false;
}

bool MenuPlayPlugin::IsTemplateArt(AIArtHandle oPlacedArt)
{
	string strDictionaryValue = GetDictionaryValue(oPlacedArt);
	return strDictionaryValue.length() > 0;
}

AIArtHandle MenuPlayPlugin::GetPlacedArt(AIArtHandle oArt)
{
	short nType;
	AIArtHandle oPlacedArt;
	sAIArt->GetArtParent(oArt, &oPlacedArt);
	if (oPlacedArt != NULL)
		sAIArt->GetArtFirstChild(oPlacedArt, &oPlacedArt);
	do
	{
		if (oPlacedArt == NULL)
			break;
		sAIArt->GetArtType(oPlacedArt, &nType);
		if (nType != kPlacedArt)
			sAIArt->GetArtSibling(oPlacedArt, &oPlacedArt);
	} while (nType != kPlacedArt);
	return oPlacedArt;
}

string MenuPlayPlugin::GetDictionaryValue(AIArtHandle oPlacedArt)
{
	string strDictionaryValue = "";
	ArtworkUtil oUtil;
	string strKey = GetDictionaryKey(oPlacedArt);
	strDictionaryValue = oUtil.GetValueFromDocumentDictionary(strKey);
	return strDictionaryValue;
}

string MenuPlayPlugin::GetDictionaryKey(AIArtHandle oPlacedArt)
{
	ai::FilePath oFile;
	AILayerHandle oLayer;
	sAIPlaced->GetPlacedFileSpecification(oPlacedArt, oFile);
	ai::UnicodeString strName = oFile.GetFileNameNoExt();
	string strFilename = strName.as_Platform();
	sAIArt->GetLayerOfArt(oPlacedArt, &oLayer);
	ai::UnicodeString strTitle;
	sAILayer->GetLayerTitle(oLayer, strTitle);
	string strLayerTitle = strTitle.as_Platform();
	int nIndex = strLayerTitle.find_first_of("-");
	if (nIndex > 0)
		strLayerTitle = strLayerTitle.substr(0, strLayerTitle.find_first_of("-"));
	string strKey = strFilename;
	strKey.append("_");
	strKey.append(strLayerTitle);
	return strKey;
}

string MenuPlayPlugin::GetGraphicXML(string strContent, string strTemplateID)
{
	Logger::Info("MenuPlayPlugin", "GetGraphicXML", "strContent : "+ strContent);
	string strGraphicXML = "";
	TemplateData oTemplateData(strContent);
	oTemplateData.SetProjectID(strTemplateID);
	strGraphicXML = oTemplateData.CreateDBTemplate();
	Logger::Info("MenuPlayPlugin", "GetGraphicXML", "strGraphicXML : " + strGraphicXML);
	return strGraphicXML;
}

void MenuPlayPlugin::ConvertTemplate(string strTemplateString, CEPTestPanelController* panelController)
{
	CommonUtil oUtil;
	ArtworkUtil oArtworkUtil;
	oUtil.ConvertTemplate(strTemplateString, panelController);
}

void MenuPlayPlugin::OpenTemplate(string strTemplateString, CEPTestPanelController* panelController)
{
	ArtworkUtil oArtworkUtil;
	oArtworkUtil.OpenTemplate(strTemplateString);
}

void MenuPlayPlugin::OpenArtwork(string strTemplateString, CEPTestPanelController* panelController)
{
	ArtworkUtil oArtworkUtil;
	oArtworkUtil.OpenArtwork(strTemplateString);
}

void MenuPlayPlugin::ZipPreviewFolder(string strTemplateString, CEPTestPanelController* panelController)
{
	ArtworkUtil oArtworkUtil;
	oArtworkUtil.ZipPreviewFolder(strTemplateString);
}

void MenuPlayPlugin::ZipArtworkFolder(string strArtworkFileName, CEPTestPanelController* panelController)
{
	ArtworkUtil oArtworkUtil;
	oArtworkUtil.ZipArtworkFolder(strArtworkFileName);
}

void MenuPlayPlugin::SaveTemplateXML(string strTemplateXML, CEPTestPanelController* panelController)
{
	ArtworkUtil oArtworkUtil;
	oArtworkUtil.SaveTemplateXML(strTemplateXML);
}

void MenuPlayPlugin::ProcessOpenDocument(void* poNotifyData)
{
	ai::FilePath oFile;
	ArtworkUtil oArtworkUtil;
	do
	{
		try
		{
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& oException)
		{
			Logger::Info("ProcessOpenDocument", "Error Initializing XMLPlatforUtils : ", "");
			break;
		}
		string strTemplateID = "";
		sAIDocument->GetDocumentFileSpecification(oFile);
		if (!IsTemplateDocument(oFile, strTemplateID))
			break;
		string strErrorString = "";
		string oResponse = oArtworkUtil.GetDBTemplateXML(strTemplateID, strErrorString);
		if (!strErrorString.empty())
		{
			Logger::Error("MenuPlayPlugin", "ProcessOpenDocument", strErrorString);
			oArtworkUtil.showAlert(strErrorString);
			break;
		}
		CEPTestPanelController* panelController = new CEPTestPanelController();
		ArticleJob oArticleJob(oResponse, panelController);
		oArticleJob.SetTemplateId(strTemplateID);
		oArticleJob.ProcessTemplate();
	} while (false);
	try
	{
		XMLPlatformUtils::Terminate();
	}
	catch (const XMLException& oException)
	{
	}
}

void MenuPlayPlugin::ProcessUploadDocument(void* poNotifyData)
{
	ArtworkUtil oUtil;
	string strTemplateId = oUtil.GetTemplateIDFromCurrentDocument();
	string strMessage = "templateid=" + strTemplateId;
	oUtil.SendEventMessageToExtension("com.ferrero.uploadtemplate", strMessage);
}

#ifdef _WINDOWS
	bool MenuPlayPlugin::IsTemplateDocument(ai::FilePath& oFile, string& strTemplateId)
#else
    Boolean MenuPlayPlugin::IsTemplateDocument(ai::FilePath& oFile, string& strTemplateId)
#endif 
{
#ifdef _WINDOWS
	bool bIsValidTemplate = false;
#else
	Boolean bIsValidTemplate = false;
#endif
	do
	{
		ArtworkUtil oUtil;
		ai::FilePath oParentFolder = oFile.GetParent();
		string strParentFolder = oParentFolder.GetFullPath().as_Platform();
		string strParentFolderName = oParentFolder.GetFileName().as_Platform();
		string strFileName = oFile.GetFileName().as_Platform();
		if (strFileName.find(kFerreroPrefix) < 0)
			break;
		std::vector<string> arrFileComponent = oUtil.Split(strFileName, ".");
		if (arrFileComponent.size() == 1)
			break;
		arrFileComponent = oUtil.Split(arrFileComponent.at(0), "_");
		if (arrFileComponent.size() == 1)
			break;
		string strID = arrFileComponent.at(1);
		bIsValidTemplate = (strID == strParentFolderName);
		if (bIsValidTemplate)
			strTemplateId = strID;
	} while (false);
	return bIsValidTemplate;
}

void MenuPlayPlugin::PlaceNewTemplateFiles(string strTemplateString, CEPTestPanelController* panelController)
{
	ArtworkUtil oArtworkUtil;
	oArtworkUtil.PlaceNewTemplateFiles(strTemplateString);
}

void MenuPlayPlugin::AddSwatches(string strTemplateString, CEPTestPanelController* panelController)
{
	CommonUtil oUtil;
	ArtworkUtil oArtworkUtil;
	oUtil.AddSwatches(strTemplateString, panelController);
}

ASErr MenuPlayPlugin::SetGlobal(Plugin *plugin)
{
	gPlugin = (MenuPlayPlugin *)plugin;
	return kNoErr;
}
// End MenuPlayPlugin.cpp
