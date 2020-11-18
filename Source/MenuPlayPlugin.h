//========================================================================================
//  
//  $File: //ai_stream/rel_23_0/devtech/sdk/public/samplecode/MenuPlay/Source/MenuPlayPlugin.h $
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
#ifndef __MENUPLAYPLUGIN_H__
#define __MENUPLAYPLUGIN_H__

#include "MenuPlaySuites.h"
#include "Plugin.hpp"
#include "AIMenuGroups.h"
#include "SDKDef.h"
#include "SDKAboutPluginsHelper.h"
#include "MenuPlayID.h"
#include "GraphicItem.h"
#include "IAIAutoBuffer.h"
#include "AIArt.h"
#include "TestPanelController.h"
/**	Creates a new MenuPlayPlugin.
	@param pluginRef IN unique reference to this plugin.
	@return pointer to new MenuPlayPlugin.
*/
Plugin* AllocatePlugin(SPPluginRef pluginRef);

/**	Reloads the MenuPlayPlugin class state when the plugin is 
	reloaded by the application.
	@param plugin IN pointer to plugin being reloaded.
*/
void FixupReload(Plugin* plugin);

/**	Provides a plugin which demonstrates adding menu items.
*/
class MenuPlayPlugin : public Plugin
{
public:

	/**	Constructor.
		@param pluginRef IN reference to this plugin.
	*/
	MenuPlayPlugin(SPPluginRef pluginRef);

	/**	Destructor.
	*/
	virtual ~MenuPlayPlugin(){}

	/**	Restores state of MenuPlayPlugin during reload.
	*/
	FIXUP_VTABLE_EX(MenuPlayPlugin, Plugin);
public:
	void ConvertTemplate(string strTemplateString, CEPTestPanelController* panelController);
	void AddSwatches(string strTemplateString, CEPTestPanelController* panelController);
	void OpenTemplate(string strTemplateString, CEPTestPanelController* panelController);
	void OpenArtwork(string strTemplateString, CEPTestPanelController* panelController);
	void ZipPreviewFolder(string strTemplateString, CEPTestPanelController* panelController);
	void PlaceNewTemplateFiles(string strTemplateString, CEPTestPanelController* panelController);
	void ZipArtworkFolder(string strArtworkFileName, CEPTestPanelController* panelController);
	void SaveTemplateXML(string strTemplateXML, CEPTestPanelController* panelController);
	ASErr SetGlobal(Plugin *plugin);
#ifdef _WINDOWS
	bool IsTemplateDocument(ai::FilePath& oFile, string& strTemplateId);
#else
	Boolean IsTemplateDocument(ai::FilePath& oFile, string& strTemplateId);
#endif

	/** Calls Plugin::Message and handles any errors returned.
		@param caller IN sender of the message.
		@param selector IN nature of the message.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr Message(char* caller, char* selector, void *message);

	/**	Calls Plugin::Startup and initialisation functions, such as 
		AddMenus.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr StartupPlugin(SPInterfaceMessage* message);

	/**	Performs plugin tasks that could not be performed until
	the application was started.
	@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr PostStartupPlugin();

	/**	Performs actions required for menu item selected.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr GoMenuItem(AIMenuMessage* message);

	/**	Updates state of menu items.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr UpdateMenuItem(AIMenuMessage* message);

	/**	Handles notifiers this plugin is registered for when they
		are received.
		@param message IN message data.
		@return kNoErr on success, other ASErr otherwise.
	*/
	virtual ASErr Notify(AINotifierMessage* message);

	/**	Registers this plug-in to receive the selection changed
	notifier.
	@param message IN message data.
	@return kNoErr on success, other ASErr otherwise.
*/
	ASErr AddNotifier(SPInterfaceMessage* message);

private:
	/**	Menu item handle for Manually updated for guides menu item.
	*/
	AIMenuItemHandle fManualUpdateAIMenu;

	/**	Menu item handle for disabling and enabling the Check me menu item.
	*/
	AIMenuItemHandle fMultiDemoAIMenu;

	/**	Menu item handle for toggling the menu item check icon.
	*/
	AIMenuItemHandle fConvertTemplateAIMenu;
	AIMenuItemHandle fCreateArtworkAIMenu;
	AIMenuItemHandle fCreateTemplateAIMenu;
	/**	Menu item handle for the about plugin dialog.
	*/
	AIMenuItemHandle fAboutPluginMenu;

	/** Used to toggle the Check me menu item check icon.
	*/
	ASBoolean fCheckMeChecked;

	/** Used to toggle disabling and enabling of Check me menu item.
	*/
	ASBoolean fCheckMeEnabled;

	/** Handle for the selection changed notifier.
	*/
	AINotifierHandle	fNotifySelectionChanged;
	AINotifierHandle	fNotifyArtObjectChanged;
	AINotifierHandle	fNotifyBeforeClear;
	AINotifierHandle fPlugPlugSetupCompleteNotifier;
	AINotifierHandle fNotifyConvertTemplate;
	AINotifierHandle fNotifyAddSwatch;
	AINotifierHandle fNotifyOpenTemplate;
	AINotifierHandle fNotifyOpenArtwork;
	AINotifierHandle fNotifyZipPreviewFolder;
	AINotifierHandle fNotifyPlaceNewTemplateFiles;
	AINotifierHandle fNotifyOpenDocument;
	AINotifierHandle fNotifyUploadDocument;
	AINotifierHandle fNotifyProcessArtworkzip;
	AINotifierHandle fNotifyProcessSaveTemplateXML;
	AINotifierHandle fNotifyProcessFinalizeArtworkComplete;
	CEPTestPanelController* fPanelController;

	/**	Adds the menu items for this plugin to the application UI.
		@param message IN pointer to plugin and call information.
		@return kNoErr on success, other ASErr otherwise.
	*/
	ASErr AddMenus(SPInterfaceMessage* message);
private:
	void ConvertTemplate();
	void ConvertTemplate(string strArtworkID);
	void CreateArtwork();
	void CreateTemplate();
	bool ParseResponse(string strResponse, string strArtworkID);
	ASErr AcquirePostStartupSuites();
	ASErr ReleasePostStartupSuites();
	std::vector<string> ExtractConfigFileContents(string strConfigFileName);
	string GenerateArtworkID(string &strErrorString);
	void CreateArtwork(string strTemplateID, string strLayerInfo);
	ASErr VerifyRules(AIArtHandle oHandle);
	ASErr VerifyDelete(ai::ArtObjectsChangedNotifierData *poMessageData);
	AIArtHandle GetPlacedArt(AIArtHandle oArt);
	bool IsTemplateArt(AIArtHandle oPlacedArt);
	string GetDictionaryValue(AIArtHandle oPlacedArt);
	string GetDictionaryKey(AIArtHandle oPlacedArt);
	bool IsAnyTemplateItem(ai::ArtObjectsChangedNotifierData *poMessageData);
	string GetGraphicXML(string strContent, string strTemplateID);
	void ProcessOpenDocument(void* poNotifyData);
	void ProcessUploadDocument(void* poNotifyData);
};

#endif // End MenuPlayPlugin.h
