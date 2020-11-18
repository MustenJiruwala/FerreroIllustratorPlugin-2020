#ifndef _ArtworkUtil_
#define _ArtworkUtil_

#include "IllustratorSDK.h"
#include "xercesc/dom/DOM.hpp"
#include "MenuPlayID.h"
#include "zlib/include/zip.h"
#include "zlib/include/unzip.h"
#include "zlib/include/zlib.h"
#include "ArticleJob.h"
using namespace xercesc;
class ArtworkUtil
{
public:
	/**	Constructor.
	*/
	ArtworkUtil();

	/**	Destructor.
	*/
	virtual ~ArtworkUtil() {}
	virtual AIErr CreateLayer(string strLayerName);
	virtual string GetFileContent(string strConFigFilePath);
	virtual string GetUserFolder();
	virtual int CreateFolder(string strFilePath);
	virtual ASErr UnzipFile(string strUnZipFile, string strUnzipFilePath);
	virtual FILE* OpenFile(string strFilePath);
	virtual std::vector<string> GetDocumentlayers();
	virtual AIBoolean IsFound(string strLayer, std::vector<string> arrlayers);
	virtual ASErr Removelayer(string strLayer);
	virtual string BuildNewArtworkPath(string strArtworkID);
	virtual ASErr SaveDocument(AIDocumentHandle oDocument, string strDocPath);
	virtual void RenameFolder(string strMainFolder, string strRenameFolder, string strRenameToFolder);
	virtual void DeleteTemplateFiles(string strUnzipFolder, string strTemplateID, string strArtworkID);
	virtual ASErr CloseDocument(AIDocumentHandle oDocument);
	virtual AIBoolean ZipFolder(string strFolderToZip);
	virtual ASErr SaveDocumentAsPDF(AIDocumentHandle oDocument, string strDocPath);
	virtual void AddToDocumentDictionary(string strKey, string strValue);
	virtual string GetValueFromDocumentDictionary(string strKey);
	virtual string GetTemplateXML(string strTemplateID, string &strErrorString);
	virtual string GetTemplateRulesXML(string strTemplateID, string &strErrorString);
	virtual string GetAttributeValue(DOMNamedNodeMap* poNodeNameMap, string strNodeName);
	virtual AIReal GetRealValue(string strValue);
	virtual ASErr OpenDocument(AIDocumentHandle &oDocument, string strDocPath);
	virtual string GetCurrentlayerName();
	virtual void PlaceLayerEPS(AIDocumentHandle &oDocument, string strTemplateID);
	virtual ai::FilePath GetLayerEPSPath(const ai::UnicodeString& name, string strArtworkId);
	virtual string GetComponent(string strTemplateInfo, int nIndex);
	virtual ASErr PlaceEPSItem(string strArtworkId, string strLayerName, string strFilename);
	virtual ai::FilePath GetEPSPath(string strFilename, string strArtworkId);
	virtual std::vector<string> Split(string strString, string strSeparator);
	int DeleteFolder(string strFilePath);
	void SendEventMessageToPanel(std::string eventName, std::string eventMessage);
	virtual ai::FilePath GetSwatchFilePath(string strArtworkId);
	string GetArtworkDownloadFolder();
	string GetTemplateDownloadFolder();
	void OpenTemplate(string strTemplateID);
	void OpenArtwork(string strArtworkID);
	string GetDBTemplateXML(string strTemplateID, string &strErrorString);
	void SendEventMessageToExtension(std::string eventName, std::string eventMessage);
	void WriteExtensionLog(string strLogMessage);
	AIArtHandle GetArtByClipGroupId(string strClipGroupId);
	ai::int32 GetCropTo(string strCropTo);
	ASErr OpenAiFile(string strTemplateID, string strArtworkID, string strUnzipFolder, AIDocumentHandle &oDocument);
	std::vector<string> ExtractLayers(string strArtworkInfo);
	string GetTemplateInfo(string strKey);
	string ConstructDocumentPath(string strUnzipFolder, string strArtworkID);
	void AddTemplateInfo(string strTemplateID);
	string GetArtworkDetails(string strArtworkID, string &strError);
	void showAlert(string strMessage);
	void ZipPreviewFolder(string strTemplateID);
	void PlaceNewTemplateFiles(string strTemplateID);
	string GetTemplateIDFromCurrentDocument();
	string GetCurrentDocumentPath();
	string GetMatchingLayerName(string strCountryId);
	void ZipArtworkFolder(string strArtworkFileName);
	void DeleteArtworkZip(string strArtworkID);
	string extractCountryIDFromLayer(string strLayerName);
	void SaveTemplateXML(string strTemplateXML);
	void HandleFinilizeArtworkCompleteEvent();
private:
	bool ProcessUnzip(unzFile &oZipfile, string strUnzipFilePath);
	string ConstructFilePath(string strUnzipFilePath, string strFilename);
	bool ExtractFile(unzFile &oZipfile, string strFilePath);
	string GetKey(string strFilePath);
	std::vector<string> GetFolderFiles(string strFolderToZip);
	string GetZipFileName(string strFilePath, string strFolderPath);
	string ReplaceBackslashString(string strMain);
	ASErr PlaceFile(ai::FilePath path, const ai::UnicodeString& name, AIArtHandle& art);
	string ReadXML();
	void ProcessCreateTemplate(string strTemplateID, ArticleJob &oArticleJob);
	void ProcessCreateArtwork(string strArtworkID, string strTemplateID, string strLayerInfo);
	void RemoveLayers(std::vector<string> arrRequiredLayers, std::vector<string> arrDocumentLayers);
	void AddLayers(std::vector<string> arrRequiredLayers, std::vector<string> arrDocumentLayers);
	void AddGraphicDetailsTodictionary(std::vector<GraphicItem> arrGraphicItem);
	void FinalizeArtwork(std::vector<string> arrRequiredLayers);
	void ProcessArtwork(AIDocumentHandle& oDocument, string strArtworkInfo);
	void SetRulesToDictionary(string strRulesXML);
	void CreateArtworkFromTemplate(string strArtworkID, string strTemplateID, string strLayerInfo);
	ASErr UnzipArtwork(string strArtworkID);
	string ConstructArtworkPath(string strArtworkID);
	AIBoolean DoOpenArtwork(string strArtworkID);
	void ConvertTemplateToArtwork(string strArtworkID);
	std::vector<string> GetDestinationLayers(std::vector<string> arrRequiredLayers);
	AIBoolean CanKeep(string strLayer, std::vector<string> arrLayers);
};
#endif // _ArtworkUtil_