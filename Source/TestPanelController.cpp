  //========================================================================================
//  
//  $File: //ai/mainline/devtech/sdk/public/samplecode/Test/Source/TestPanelController.cpp $
//
//  $Revision: #1 $
//
//  Copyright 2010 Adobe Systems Incorporated. All rights reserved.
//  
//  NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance 
//  with the terms of the Adobe license agreement accompanying it.  If you have received
//  this file from a source other than Adobe, then your use, modification, or 
//  distribution of it requires the prior written permission of Adobe.
//  
//========================================================================================

#include "IllustratorSDK.h"
#include "TestPanelController.h"
#include "MenuPlayPlugin.h"
#include "AppContext.hpp"
#include "SDKPlugPlug.h"
#include "FerreroServerFacade.h"
#include "CommonUtil.h"
#include "ASUserInteraction.h"

// Our extension's ID
#define TestPanel_UI_EXTENSION "com.example.TestUI.extension1"

// Events we listen for
static const char* BUTTON_PRESSED  = "com.ferrero.selectedcategory";
static const char* CONVERT_TEMPLATE = "com.ferrero.converttemplate";
static const char* ADD_SWATCHES = "com.ferrero.uploadTemplate";

static const char* GET_CATEGORY = "com.ferrero.getcategory";
static const char* GET_TEMPLATEFOLDER = "com.ferrero.gettemplatefolder";
static const char* GET_ARTWORKFOLDER = "com.ferrero.getartworkfolder";
static const char* OPEN_TEMPLATEFILE = "com.ferrero.opentemplatefile";
static const char* OPEN_ARTWORKFILE = "com.ferrero.openartworkfile";
static const char* LOGEXTENSIONOUTPUT = "com.ferrero.outputlog";
static const char* ZIPPREVIEWFOLDER = "com.ferrero.zippreviewfolder";
static const char* PLACE_NEW_IMAGE = "com.ferrero.placenewimage";
static const char* DOWNLOAD_COMPLETE = "com.ferrero.downloadcomplete";
static const char* ZIP_ARTWORK_FOLDER = "com.ferrero.zipartworkfolder";
static const char* SAVE_TEMPLATE_XML = "com.ferrero.savetemplatexml";
static const char* FINALIZEARTWORK_COMPLETE = "com.ferrero.finalizeartworkcomplete";

// Illustrator's CSXS app id
static const char* ILST_APP = "ILST";
extern "C"	AIUserSuite*					sAIUserSuite;
extern "C"	ASUserInteractionSuite*			sAIUserInteractionSuite;


static void ButtonPressedFunc(const csxs::event::Event* const eventParam, void* const context)
{
	ai::UnicodeString strEventReceivedMessage(eventParam->data);
	CEPTestPanelController* panelController = (CEPTestPanelController*)context;
	CommonUtil oUtil;
	string strCategoryString = eventParam->data;
	ArtworkUtil oArtworkUtil;
	std::vector<string> arrCategory = oArtworkUtil.Split(strCategoryString, ",");
	string strCategoryID = arrCategory[0];
	string strCategoryName = arrCategory[1];
	string strErrorString = "";
	string strTemplates = oUtil.GetCategoryTemplates(strCategoryID, panelController, strErrorString);
	std::vector<string> arrTemplates = oUtil.StringToArray(strTemplates, ",");
	string strMessage = "Total number of templates in category " + strEventReceivedMessage.as_Platform() + " is : " + to_string(arrTemplates.size());
	oArtworkUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
	oArtworkUtil.SendEventMessageToPanel("com.ferrero.getcategorytemplate.result", strTemplates + ",");
}

static void GetCategory(const csxs::event::Event* const eventParam, void* const context)
{
	CEPTestPanelController* panelController = (CEPTestPanelController*)context;
	FerreroServerFacade oFerreroServerFacade;
	ArtworkUtil oArtworkUtil;
	string strErrorString = "";
	string strGetCategoriesParam = "function=getCategories";
	string oResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strGetCategoriesParam, strErrorString);
	oArtworkUtil.SendEventMessageToPanel("com.ferrero.getcategory.result", oResponse);
}

static void ConvertTemplate(const csxs::event::Event* const eventParam, void* const context)
{
	string strTemplate = eventParam->data;
	ArtworkUtil oArtworkUtil;
	std::vector<string> arrCategory = oArtworkUtil.Split(strTemplate, ",");
	if (arrCategory.size() > 1)
	{
		string strTemplateID = arrCategory[1];
		string strMessage = "Converting template start : " + strTemplateID;
		oArtworkUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
		sAINotifier->Notify(kConvertTemplatNotifier, &strTemplate);
	}
}

static void AddSwatchAndUploadTemplate(const csxs::event::Event* const eventParam, void* const context)
{
	string strTemplate = eventParam->data;
	ArtworkUtil oArtworkUtil;
	std::vector<string> arrTemplateInfo = oArtworkUtil.Split(strTemplate, "~");
	if (arrTemplateInfo.size() > 1)
	{
		string strTemplateID = arrTemplateInfo[1];
		string strMessage = "Adding swatches start : " + strTemplateID;
		oArtworkUtil.SendEventMessageToPanel("com.ferrero.log", strMessage);
		sAINotifier->Notify(kAddSwatchNotifier, &strTemplate);
	}
}

static void GetTemplateDownloadFolder(const csxs::event::Event* const eventParam, void* const context)
{
	ArtworkUtil oArtworkUtil;
	string strTemplateFolder = oArtworkUtil.GetTemplateDownloadFolder();
	oArtworkUtil.SendEventMessageToPanel("com.ferrero.templatefolder.result", strTemplateFolder);
}

static void GetArtworkDownloadFolder(const csxs::event::Event* const eventParam, void* const context)
{
	ArtworkUtil oArtworkUtil;
	string strArtworkFolder = oArtworkUtil.GetArtworkDownloadFolder();
	oArtworkUtil.SendEventMessageToPanel("com.ferrero.artworkfolder.result", strArtworkFolder);
}

static void OpenTemplateFile(const csxs::event::Event* const eventParam, void* const context)
{
	string strTemplateID = eventParam->data;
	sAINotifier->Notify(kOpenTemplateNotifier, &strTemplateID);
}

static void OpenArtworkFile(const csxs::event::Event* const eventParam, void* const context)
{
	string strArtworkID = eventParam->data;
	sAINotifier->Notify(kOpenArtworkNotifier, &strArtworkID);
}

static void ZipPreviewFolder(const csxs::event::Event* const eventParam, void* const context)
{
	string strTemplateID = eventParam->data;
	sAINotifier->Notify(kZipPreviewFolderNotifier, &strTemplateID);
}

static void PlaceNewTemplateFiles(const csxs::event::Event* const eventParam, void* const context)
{
	string strTemplateID = eventParam->data;
	sAINotifier->Notify(kPlaceNewTemplateFilesNotifier, &strTemplateID);
}

static void ProcessDownLoadComplete(const csxs::event::Event* const eventParam, void* const context)
{
    string strParam = "";
	sAINotifier->Notify(kProcessDownLoadCompleteNotifier, &strParam);
}

static void ProcessZipArtwork(const csxs::event::Event* const eventParam, void* const context)
{
	string strArtworkFileName = eventParam->data;
	sAINotifier->Notify(kProcessZipArtworkNotifier, &strArtworkFileName);
}

static void ProcessSaveTemplateXML(const csxs::event::Event* const eventParam, void* const context)
{
	string strTemplateXML = eventParam->data;
	sAINotifier->Notify(kProcessSaveTemplateXMLNotifier, &strTemplateXML);
}

static void HandleFinilizeArtworkCompleteEvent(const csxs::event::Event* const eventParam, void* const context)
{
	sAINotifier->Notify(kFinalizeArtworkCompleteNotifier, NULL);
}

static void LogExtensionOutput(const csxs::event::Event* const eventParam, void* const context)
{
	string strLogMessage = eventParam->data;
	ArtworkUtil oArtworkUtil;
	oArtworkUtil.WriteExtensionLog(strLogMessage);
}

CEPTestPanelController::CEPTestPanelController()
:HtmlUIController(TestPanel_UI_EXTENSION)
{}

csxs::event::EventErrorCode CEPTestPanelController::RegisterCSXSEventListeners()
{
	csxs::event::EventErrorCode result = csxs::event::kEventErrorCode_Success;
	do
	{
		result = htmlPPLib.AddEventListener(BUTTON_PRESSED, ButtonPressedFunc, this);
 		if (result != csxs::event::kEventErrorCode_Success) 
			break;
		result = htmlPPLib.AddEventListener(GET_CATEGORY, GetCategory, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(CONVERT_TEMPLATE, ConvertTemplate, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(ADD_SWATCHES, AddSwatchAndUploadTemplate, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(GET_TEMPLATEFOLDER, GetTemplateDownloadFolder, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(GET_ARTWORKFOLDER, GetArtworkDownloadFolder, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(OPEN_TEMPLATEFILE, OpenTemplateFile, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(OPEN_ARTWORKFILE, OpenArtworkFile, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(LOGEXTENSIONOUTPUT, LogExtensionOutput, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(ZIPPREVIEWFOLDER, ZipPreviewFolder, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(PLACE_NEW_IMAGE, PlaceNewTemplateFiles, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(DOWNLOAD_COMPLETE, ProcessDownLoadComplete, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(ZIP_ARTWORK_FOLDER, ProcessZipArtwork, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(SAVE_TEMPLATE_XML, ProcessSaveTemplateXML, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
		result = htmlPPLib.AddEventListener(FINALIZEARTWORK_COMPLETE, HandleFinilizeArtworkCompleteEvent, this);
		if (result != csxs::event::kEventErrorCode_Success)
			break;
	}
	while (false);
	
	return result;
}

csxs::event::EventErrorCode CEPTestPanelController::RemoveEventListeners()
{
	csxs::event::EventErrorCode result = csxs::event::kEventErrorCode_Success;
	do
	{
		result = htmlPPLib.RemoveEventListener(BUTTON_PRESSED, ButtonPressedFunc, this);
		if (result != csxs::event::kEventErrorCode_Success) 
		{
			break;
		}
	}
	while (false);
	
	return result;
}

