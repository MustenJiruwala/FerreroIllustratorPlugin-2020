#include "IllustratorSDK.h"
#include "ArtworkUtil.h"
#include "Logger.h"
#include "EXTLogger.h"
#include "AIDocumentList.h"
#include "AINativeAction.h"
#include "AIDocumentAction.h"
#include "AIObjectAction.h"
#include "AIPDFFormatAction.h"
#include "AITransformArt.h"
#include "FerreroServerFacade.h"
#include "AIPDFFormatAction.h"
#include "ASUserInteraction.h"
#include "ArticleJob.h"
#include "RuleData.h"

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
extern "C" ASUserInteractionSuite*			sAIUserInteractionSuite;
extern "C" 	AIMdMemorySuite*				sAIMdMemory;
extern "C"	AICSXSExtensionSuite*			sAICSXSExtensionSuite;
extern "C"	AIUserSuite*					sAIUserSuite;
extern "C" AIPreferenceSuite*				sAIPreferenceSuite;

ArtworkUtil::ArtworkUtil()
{

}

AIErr ArtworkUtil::CreateLayer(string strLayerName)
{
	AIErr tResult =  kNoErr;
	do
	{
		if (strLayerName.empty())
			break;
		AILayerHandle oLayerHandle = NULL;
		ai::UnicodeString strLayerTitle(strLayerName.c_str());
		sAILayer->GetLayerByTitle(&oLayerHandle, strLayerTitle);
		if (oLayerHandle != NULL)
		{
			tResult = sAILayer->SetCurrentLayer(oLayerHandle);
			break;
		}
		tResult = sAILayer->InsertLayer(NULL, kPlaceAboveAll, &oLayerHandle);
		if (tResult == kNoErr)
		{
			sAILayer->SetLayerTitle(oLayerHandle, strLayerTitle);
			tResult = sAILayer->SetCurrentLayer(oLayerHandle);
		}
	} while (false);
	return tResult;
}

string ArtworkUtil::GetFileContent(string strConFigFilePath)
{
	string strContent = "";
	Logger::Info("ArtworkUtil", "GetFileContent", "strConFigFilePath :" + strConFigFilePath);
	std::ifstream oFileStream;
	string strLine = "";
	oFileStream.open(strConFigFilePath, std::ifstream::in);

	std::getline(oFileStream, strLine);
	strContent.append(strLine);
	while (oFileStream.good())
	{
		std::getline(oFileStream, strLine);
		strContent.append(strLine);
	}
	oFileStream.close();
	Logger::Info("ArtworkUtil", "GetFileContent", "strContent :" + strContent);
	return strContent;
}

string ArtworkUtil::GetUserFolder()
{
	ai::FilePath oUserFolder;
	sAIFolder->FindFolder(kAIMyDocumentsFolderType, true, oUserFolder);
	string strUserFolder  = oUserFolder.GetFullPath().as_Platform();
	strUserFolder.append("ferrero");
	CreateFolder(strUserFolder);
    strUserFolder.append(1, kFilePathSeparator);
	return strUserFolder;
}

int ArtworkUtil::CreateFolder(string strFilePath)
{
	int nError = 0;
#ifdef _WINDOWS
	nError = _mkdir(strFilePath.c_str());
#else
    nError = mkdir(strFilePath.c_str(), S_IRWXO | S_IRWXG | S_IRWXU);
#endif
	return nError;
}

ASErr ArtworkUtil::UnzipFile(string strUnZipFile, string strUnzipFilePath)
{
	ASErr tErrorCode = 1;
	Logger::Info("ArtworkUtil", "UnzipFile", "strUnZipFile :" + strUnZipFile);
	Logger::Info("ArtworkUtil", "UnzipFile", "strUnzipFilePath :" + strUnzipFilePath);
	do
	{
		unzFile oZipfile = unzOpen(strUnZipFile.c_str());
		if (oZipfile == NULL)
		{
			Logger::Info("ArtworkUtil", "UnzipFile", "Null pointer");
			break;
		}
		// Get info about the zip file
		unz_global_info global_info;
		if (unzGetGlobalInfo(oZipfile, &global_info) != UNZ_OK)
		{
			Logger::Info("ArtworkUtil", "UnzipFile", "UNZip NOT OK");
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
					Logger::Info("ArtworkUtil", "UnzipFile", "unzGoToNextFile NOT OK");
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
bool ArtworkUtil::ProcessUnzip(unzFile &oZipfile, string strUnzipFilePath)
{
	Logger::Info("ArtworkUtil", "ProcessUnzip", "strUnzipFilePath :" + strUnzipFilePath);
	bool bSuccess = true;
	do
	{
		// Get info about current file.
		ArtworkUtil oUtil;
		unz_file_info file_info;
		char strFilename[MAX_FILENAME];
		if (unzGetCurrentFileInfo(oZipfile, &file_info, strFilename, MAX_FILENAME, NULL, 0, NULL, 0) != UNZ_OK)
		{
			Logger::Info("ArtworkUtil", "ProcessUnzip", "unzGetCurrentFileInfo NOT OK");
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
			Logger::Info("ArtworkUtil", "ProcessUnzip", "CreateFolder : " + strFilePath + (nError == 0 ? " - OK" : "- Failed!"));
		}
		else
			bSuccess = ExtractFile(oZipfile, strFilePath);
	} while (false);
	Logger::Info("ArtworkUtil", "ProcessUnzip", "bSuccess :" + to_string(bSuccess));
	return bSuccess;
}

//------------------------------------------------------------------------------------------
//	ConstructFilePath
//	Utility method to construct the file path based on platform.
//	strUnzipFilePath [IN] - String consisting of the file path.
//	strFilename [IN] - String consisting of the file name.
//	returns PMString consisting of platform specific file path.
//------------------------------------------------------------------------------------------
string ArtworkUtil::ConstructFilePath(string strUnzipFilePath, string strFilename)
{
	Logger::Info("ArtworkUtil", "ConstructFilePath", "strUnzipFilePath [IN] :" + strUnzipFilePath);
	Logger::Info("ArtworkUtil", "ConstructFilePath", "strFilename [IN] :" + strFilename);
	string strFilePath = strUnzipFilePath;
	strFilePath.append("/");
	strFilePath.append(strFilename);
	string strLastDirectory = strFilePath.substr(0, strFilePath.find_last_of("/\\"));
	ArtworkUtil oUtil;
	oUtil.CreateFolder(strLastDirectory);
	//	strFilePath = Utils <ITEUtils>()->TextReplaceAll(strFilePath, "\\", "/");
	Logger::Info("ArtworkUtil", "ConstructFilePath", "strFilePath [OUT] :" + strFilePath);
	return strFilePath;
}


//------------------------------------------------------------------------------------------
//	ExtractFile
//	This method writes the contents from source file to destination file.
//	oZipfile [IN] - unzFile consisting the source file that needs to be written.
//	strFilePath [IN] - PMString consisting of the destination file path.
//	returns Boolean, kTrue if file was extracted successfully, false otherwise.
//------------------------------------------------------------------------------------------
bool ArtworkUtil::ExtractFile(unzFile &oZipfile, string strFilePath)
{
	Logger::Info("ArtworkUtil", "ConstructFilePath", "strFilePath [IN] :" + strFilePath);
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
		FILE *poOutput = OpenFile(strFilePath);
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
//			m_arrGraphicObjects[GetKey(strFilePath)] = strFilePath;
			fclose(poOutput);
		}
	} while (false);
	Logger::Info("ArtworkUtil", "ConstructFilePath", "bSuccess [OUT] :" + to_string(bSuccess));
	return bSuccess;
}

string ArtworkUtil::GetKey(string strFilePath)
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

//------------------------------------------------------------------------------------------
//	OpenFile
//	This method opens the given file in write binary format.
//	strFilePath [IN] - File to open.
//	returns FILE pointer, if the file was opened successfully, otherwise nil.
//------------------------------------------------------------------------------------------
FILE* ArtworkUtil::OpenFile(string strFilePath)
{
	// Open a file to write out the data.
	string strDirectory = strFilePath.substr(0, strFilePath.find_last_of("/\\"));
	ArtworkUtil oUtil;
	oUtil.CreateFolder(strDirectory);
	FILE *poOutput;
#ifdef _WINDOWS
	fopen_s(&poOutput, strFilePath.c_str(), "wb");
#else
	poOutput = fopen(strFilePath.c_str(), "wb");
#endif
	return poOutput;
}

std::vector<string> ArtworkUtil::GetDocumentlayers()
{
	Logger::Info("ArtworkUtil", "GetDocumentlayers", "");
	std::vector<string> arrCurrentLayers;
	ai::int32 nCount = 0;
	sAILayer->CountLayers(&nCount);
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		AILayerHandle oHandle = NULL;
		ai::UnicodeString strtitle ("");
		sAILayer->GetNthLayer(nIndex, &oHandle);
		sAILayer->GetLayerTitle (oHandle, strtitle);
		Logger::Info("ArtworkUtil", "GetDocumentlayers Layer : ", strtitle.as_Platform ());
		arrCurrentLayers.push_back(strtitle.as_Platform());
	}
	Logger::Info("ArtworkUtil", "GetDocumentlayers Layers count : ", to_string(arrCurrentLayers.size ()));
	return arrCurrentLayers;
}

AIBoolean ArtworkUtil::IsFound(string strLayer, std::vector<string> arrlayers)
{
	AIBoolean bIsfound = false;
	string strLayerCountryID = extractCountryIDFromLayer(strLayer).substr(0, 2);
	for (int nIndex = 0; !bIsfound && nIndex < arrlayers.size(); nIndex++)
		bIsfound = (strLayerCountryID == extractCountryIDFromLayer(arrlayers[nIndex]).substr(0, 2));
	return bIsfound;
}

AIBoolean ArtworkUtil::CanKeep(string strLayer, std::vector<string> arrLayers)
{
	AIBoolean bIsfound = false;
	string strDocLayerCountryID = extractCountryIDFromLayer(strLayer);
	for (int nIndex = 0; !bIsfound && nIndex < arrLayers.size(); nIndex++)
	{
		string strRequiredLayerID = extractCountryIDFromLayer(arrLayers[nIndex]);
		bIsfound = (strDocLayerCountryID.size() <= strRequiredLayerID.size() &&
			strRequiredLayerID.substr(0, strDocLayerCountryID.size()) == strDocLayerCountryID);
	}
	return bIsfound;
}

string ArtworkUtil::extractCountryIDFromLayer(string strLayerName)
{
	string strCountryID;
	int nPosNonSpace = strLayerName.find_first_not_of(' ');
	int nPosHyphen = strLayerName.find_first_of('-');

	if (nPosHyphen > nPosNonSpace)
		strLayerName = strLayerName.substr(nPosNonSpace, nPosHyphen - nPosNonSpace);
	for (int nIndex = 0; nIndex < strLayerName.size(); nIndex++)
		strCountryID.push_back(std::tolower(strLayerName.at(nIndex)));
	return strCountryID;
}

ASErr ArtworkUtil::Removelayer(string strLayer)
{
	Logger::Info("ArtworkUtil", "Removelayer Layer Name :", strLayer);
	ASErr tResult = 1;
	AILayerHandle oHandle = NULL;
	sAILayer->GetLayerByTitle(&oHandle, ai::UnicodeString (strLayer.c_str()));
	if(oHandle != NULL)
		tResult = sAILayer->DeleteLayer(oHandle);
	Logger::Info("ArtworkUtil", "Removelayer ErrorCode :", to_string(tResult));
	return tResult;
}

string ArtworkUtil::BuildNewArtworkPath(string strArtworkID)
{
	string strArtworkPath = GetUserFolder();
	strArtworkPath.append(kArtworkFolder);
	strArtworkPath.append(1, kFilePathSeparator);
	strArtworkPath.append(strArtworkID);
	CreateFolder(strArtworkPath);
	return strArtworkPath;
}

/*
*/
ASErr ArtworkUtil::SaveDocument(AIDocumentHandle oDocument, string strDocPath)
{
	Logger::Info("ArtworkUtil", "SaveDocument strDocPath :", strDocPath);
	ASErr tResult = kNoErr;
	try
	{
		SDK_ASSERT(oDocument);
		SDK_ASSERT(sAIDocument);
		sAIDocumentList->Activate(oDocument, true);
		ai::UnicodeString strPath = ai::UnicodeString(strDocPath);
		struct DummyParams
		{
		};
		AIDocumentFileFormatParameters oldParams = nullptr;
		sAIDocument->GetDocumentFileFormatParameters(&oldParams);

		DummyParams* param = nullptr;
		if (oldParams == nullptr)
		{
			sAIMdMemory->MdMemoryNewHandle(sizeof(DummyParams), (AIMdMemoryHandle*)&param);
			sAIDocument->SetDocumentFileFormatParameters((AIDocumentFileFormatParameters)param);
		}
		AIDictionaryRef importExportDict = NULL;
		ASErr result = sAIDictionary->CreateDictionary(&importExportDict);
		AIDictKey kIncludeLinkedFile = sAIDictionary->Key("IncludeLinkedFiles");
		AIDictKey kPDFCompatibleKey = sAIDictionary->Key("kAINativePDFCompatibilityKey");
		result = sAIDictionary->SetBooleanEntry(importExportDict, kIncludeLinkedFile, true);
		result = sAIDictionary->SetBooleanEntry(importExportDict, kPDFCompatibleKey, false);
		ASErr status = sAIDocument->WriteDocumentWithOptions(ai::FilePath(strPath), kAINativeFileFormat, kFileFormatSuppressUI, importExportDict, false);
		sAIDocument->SetDocumentModified(false);
		//It will automatically dispose of the memory allocated into param.
		if (param)
			sAIDocument->SetDocumentFileFormatParameters(nullptr);
		aisdk::check_ai_error(status);
	}
	catch (ai::Error& tError)
	{
		Logger::Info("ArtworkUtil", "SaveDocument error :", tError.what());
	}
	Logger::Info("ArtworkUtil", "SaveDocument tResult :", to_string(tResult));
	return tResult;
}

/*
*/
ASErr ArtworkUtil::CloseDocument(AIDocumentHandle oDocument)
{
	Logger::Info("ArtworkUtil", "CloseDocument", "");
	ASErr tResult = kNoErr;
	try
	{
		SDK_ASSERT(oDocument);
		SDK_ASSERT(sAIDocumentList);
		tResult = sAIDocumentList->Close(oDocument);
		aisdk::check_ai_error(tResult);
	}
	catch (ai::Error& tError)
	{
		tResult = tError;
	}
	Logger::Info("ArtworkUtil", "CloseDocument tResult :", to_string (tResult));
	return tResult;
}

void ArtworkUtil::RenameFolder(string strMainFolder, string strRenameFolder, string strRenameToFolder)
{
	string strOldFolderPath = strMainFolder;
	string strNewFolderPath = strMainFolder;
	strOldFolderPath.append(1, kFilePathSeparator);
	strOldFolderPath.append(strRenameFolder);
	strNewFolderPath.append(1, kFilePathSeparator);
	strNewFolderPath.append(strRenameToFolder);
	Logger::Info("ArtworkUtil", "RenameFolder strOldFolderPath : ", strOldFolderPath);
	Logger::Info("ArtworkUtil", "RenameFolder strNewFolderPath : ", strNewFolderPath);
	int nErrorCode = rename(strOldFolderPath.c_str(), strNewFolderPath.c_str());
	Logger::Info("ArtworkUtil", "RenameFolder strNewFolderPath : ", to_string(nErrorCode));
	if (nErrorCode > 0)
		showAlert("Unable to rename folder : " + strOldFolderPath);
}

void ArtworkUtil::DeleteTemplateFiles(string strUnzipFolder, string strTemplateID, string strArtworkID)
{
	string strTemplateFileBasePath = strUnzipFolder;
	strTemplateFileBasePath.append(1, kFilePathSeparator);
	strTemplateFileBasePath.append(strArtworkID);
	strTemplateFileBasePath.append(1, kFilePathSeparator);
	strTemplateFileBasePath.append(kFerreroPrefix);
	strTemplateFileBasePath.append(strTemplateID);
	string strTemplateInddPath = strTemplateFileBasePath;
	strTemplateInddPath.append(".indd");
	string strTemplateAiPath = strTemplateFileBasePath;
	strTemplateAiPath.append(".ai");
	string strTemplatIdmlPath = strTemplateFileBasePath;
	strTemplatIdmlPath.append(".idml");
	string strTemplatePdfPath = strTemplateFileBasePath;
	strTemplatePdfPath.append(".pdf");
	Logger::Info("ArtworkUtil", "DeleteTemplateFiles File : ", strTemplateInddPath);
	remove(strTemplateInddPath.c_str());
	Logger::Info("ArtworkUtil", "DeleteTemplateFiles File : ", strTemplateAiPath);
	remove(strTemplateAiPath.c_str());
	Logger::Info("ArtworkUtil", "DeleteTemplateFiles File : ", strTemplatIdmlPath);
	remove(strTemplatIdmlPath.c_str());
	Logger::Info("ArtworkUtil", "DeleteTemplateFiles File : ", strTemplatePdfPath);
	remove(strTemplatePdfPath.c_str());
}


AIBoolean ArtworkUtil::ZipFolder(string strFolderToZip)
{
	AIBoolean bSuccess = false;
	Logger::Info("ArtworkUtil", "ZipFolder strFolderToZip : ", strFolderToZip);
	do
	{
		string strzipFilePath = strFolderToZip;
		strzipFilePath.append(".zip");
		zipFile oZipFile = zipOpen(std::string(strzipFilePath.begin(), strzipFilePath.end()).c_str(), APPEND_STATUS_CREATE);
		bSuccess = true;
		if (oZipFile == NULL)
			break;
		std::vector<string> arrPaths = GetFolderFiles(strFolderToZip);
		for (int nIndex = 0; nIndex < arrPaths.size(); nIndex++)
		{
			std::fstream oFileStream(arrPaths[nIndex].c_str(), std::ios::binary | std::ios::in);
			if (oFileStream.is_open())
			{
				oFileStream.seekg(0, std::ios::end);
				long size = oFileStream.tellg();
				oFileStream.seekg(0, std::ios::beg);
				std::vector<char> buffer(size);
				if (size == 0 || oFileStream.read(&buffer[0], size))
				{
					string strFileName = GetZipFileName(arrPaths[nIndex], strFolderToZip);
					if (ZIP_OK == zipOpenNewFileInZip(oZipFile, std::string(strFileName.begin(), strFileName.end()).c_str(), NULL, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION))
					{
						if (zipWriteInFileInZip(oZipFile, size == 0 ? "" : &buffer[0], size))
							bSuccess = false;
						if (zipCloseFileInZip(oZipFile))
							bSuccess = false;
						oFileStream.close();
						continue;
					}
				}
				oFileStream.close();
			}
		}
		zipClose(oZipFile, bSuccess ? "success" : "failure");
	} while (false);
	Logger::Info("ArtworkUtil", "ZipFolder bSuccess : ", to_string(bSuccess));
	return bSuccess;
}

std::vector<string> ArtworkUtil::GetFolderFiles(string strFolderToZip)
{
	std::vector<string>arrFiles;
	string strFfilepath = strFolderToZip;
#ifdef _WINDOWS
	strFolderToZip += kFilePathSeparator;
	strFolderToZip.append("*");
	WIN32_FIND_DATA FindFileData;
	HANDLE tHandle = FindFirstFile(strFolderToZip.c_str(), &FindFileData);
	string strName = FindFileData.cFileName;
	if (tHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
				arrFiles.push_back(strFfilepath + kFilePathSeparator + FindFileData.cFileName);
		} while (FindNextFile(tHandle, &FindFileData));
		FindClose(tHandle);
	}
#else
	DIR *poDir;
	struct dirent *poEntry;
	if ((poDir = opendir(strFolderToZip.c_str())) != NULL)
	{
		while ((poEntry = readdir(poDir)) != NULL)
			arrFiles.push_back(strFfilepath + kFilePathSeparator + poEntry->d_name);
		closedir(poDir);
	}
	else
	{
		perror("open directory failed");
	}
#endif
	return arrFiles;
}

string ArtworkUtil::GetZipFileName(string strFilePath, string strFolderPath)
{
	int nIndex = strFolderPath.find_last_of("/\\") > 0 ? strFolderPath.find_last_of("/\\") : strFolderPath.size();
	strFilePath = strFilePath.substr(nIndex + 1);
	return ReplaceBackslashString(strFilePath);
}

string ArtworkUtil::ReplaceBackslashString(string strMain)
{
	string strReplacedString = strMain;
	int nIndex = strReplacedString.find_first_of("\\");
	while (nIndex > 0)
	{
		strReplacedString = strReplacedString.replace(nIndex, 1, "/");
		nIndex = strReplacedString.find_first_of("\\");
	}
	return strReplacedString;
}

/*
*/
ASErr ArtworkUtil::SaveDocumentAsPDF(AIDocumentHandle oDocument, string strDocPath)
{
	Logger::Info("ArtworkUtil", "SaveDocumentAsPDF strDocPath :", strDocPath);
	ASErr tResult = kNoErr;
	try
	{
		SDK_ASSERT(oDocument);
		SDK_ASSERT(sAIDocument);
		sAIDocumentList->Activate(oDocument, true);
		ai::UnicodeString strPath = ai::UnicodeString(strDocPath);
        AIActionParamValueRef oPVR;
        sAIActionManager->AINewActionParamValue (&oPVR);
        tResult = sAIActionManager->AIActionSetInteger (oPVR, kAIPDFCompatibility17, 0x0017);
        aisdk::check_ai_error(tResult);
        tResult = sAIActionManager->AIActionSetStringUS (oPVR, kAISaveDocumentAsNameKey, ai::UnicodeString (strDocPath.c_str()));
        aisdk::check_ai_error(tResult);
        tResult = sAIActionManager->AIActionSetString(oPVR, kAISaveDocumentAsFormatKey, kAIPDFFileFormat);
        aisdk::check_ai_error(tResult);
		tResult = sAIActionManager->AIActionSetBoolean(oPVR, kAISaveDocumentAsGetParamsKey, false);
        tResult = sAIActionManager->PlayActionEvent(kAISaveDocumentAsAction, kDialogNone, oPVR);
        aisdk::check_ai_error(tResult);
 /*
		ASErr status = sAIDocument->WriteDocumentWithOptions(ai::FilePath(strPath), kAIPDFFileFormat,
			kFileFormatSuppressUI | kFileFormatNoWarningOption | kFileFormatExport,
			NULL, false);
 */
		sAIDocument->SetDocumentModified(false);
		aisdk::check_ai_error(tResult);
	}
	catch (ai::Error& tError)
	{
		tResult = tError;
	}
	Logger::Info("ArtworkUtil", "SaveDocumentAsPDF tResult :", to_string(tResult));
	return tResult;
}

void ArtworkUtil::AddToDocumentDictionary(string strKey, string strValue)
{
	AIDictKey oTemplateIDKey = sAIDictionary->Key(strKey.c_str());
	AIDictionaryRef oDocumentDict;
	sAIDocument->GetDictionary(&oDocumentDict);
	sAIDictionary->SetStringEntry(oDocumentDict, oTemplateIDKey, strValue.c_str());
}

string ArtworkUtil::GetValueFromDocumentDictionary(string strKey)
{
	string strValue = "";
	const char* sValue = NULL;
	AIDictKey oTemplateIDKey = sAIDictionary->Key(strKey.c_str());
	AIDictionaryRef oDocumentDict;
	sAIDocument->GetDictionary(&oDocumentDict);
	sAIDictionary->GetStringEntry(oDocumentDict, oTemplateIDKey, &sValue);
	if(sValue != NULL)
		strValue.append(sValue);
	return strValue;
}

string ArtworkUtil::GetTemplateXML(string strTemplateID, string &strErrorString)
{
//	return ReadXML();
	FerreroServerFacade oFerreroServerFacade;
	string strGetTemplateXMLParam = "function=getConvertTemplateXML&projectid=";
	strGetTemplateXMLParam.append(strTemplateID);
	string oResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strGetTemplateXMLParam, strErrorString);
	return oResponse;
}

string ArtworkUtil::GetDBTemplateXML(string strTemplateID, string &strErrorString)
{
	FerreroServerFacade oFerreroServerFacade;
	string strGetTemplateXMLParam = "function=getDBTemplateXML&projectid=";
	strGetTemplateXMLParam.append(strTemplateID);
	string oResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strGetTemplateXMLParam, strErrorString);
	return oResponse;
}

string ArtworkUtil::ReadXML()
{
	string strXMLFileName = "144472972605467773_new.xml";
	string strXMLFilePath = GetUserFolder();
	strXMLFilePath.append(strXMLFileName);
	return GetFileContent(strXMLFilePath);
}

string ArtworkUtil::GetTemplateRulesXML(string strTemplateID, string &strErrorString)
{
	FerreroServerFacade oFerreroServerFacade;
	string strGetTemplateXMLParam = "getrulesxml=";
	strGetTemplateXMLParam.append(strTemplateID);
	string oResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strGetTemplateXMLParam, strErrorString);
	return oResponse;
}

string ArtworkUtil::GetAttributeValue(DOMNamedNodeMap * poNodeNameMap, string strNodeName)
{
	string strValue = "";
	DOMNode *poCountry = poNodeNameMap->getNamedItem(XMLString::transcode(strNodeName.c_str()));
	if (poCountry != nullptr)
		strValue = XMLString::transcode(poCountry->getNodeValue());
	return strValue;
}

AIReal ArtworkUtil::GetRealValue(string strValue)
{
	AIReal nValue = 0;
	try
	{
		std::string::size_type tSize;
		nValue = std::stod(strValue, &tSize);
	}
	catch (...)
	{

	}
	return nValue;
}

/*
*/
ASErr ArtworkUtil::OpenDocument(AIDocumentHandle &oDocument, string strDocPath)
{
	Logger::Info("ArtworkUtil", "OpenDocument strDocPath :", strDocPath);
	ASErr tResult = kNoErr;
	try
	{
		SDK_ASSERT(sAIDocument);
		sAICSXSExtensionSuite->ShowExtension(kIGMExtensionId, false);
		ai::UnicodeString strPath = ai::UnicodeString(strDocPath);
		tResult = sAIDocumentList->Open (ai::FilePath(strPath), kAICMYKColorModel, kDialogNone, false, &oDocument);
		aisdk::check_ai_error(tResult);
	}
	catch (ai::Error& tError)
	{
		tResult = tError;
	}
	Logger::Info("ArtworkUtil", "OpenDocument tResult :", to_string(tResult));
	return tResult;
}

string ArtworkUtil::GetCurrentlayerName()
{
	AILayerHandle oLayerHandle;
	sAILayer->GetCurrentLayer(&oLayerHandle);
	ai::UnicodeString oTitle("");
	sAILayer->GetLayerTitle(oLayerHandle, oTitle);
	string strLayerName = oTitle.as_Platform();
	return strLayerName;
}

ASErr ArtworkUtil::PlaceEPSItem(string strArtworkId, string strLayerName, string strFilename)
{
	ASErr result = kNoErr;
	CreateLayer(strLayerName);
	ai::FilePath oLayerPath = GetEPSPath(strFilename, strArtworkId);
	ai::UnicodeString oTitle(strLayerName.c_str());
	string strpath = oLayerPath.GetFullPath().as_Platform();
	if (oLayerPath.Exists(false))
	{
		AIArtHandle oArthandle;
		result = PlaceFile(oLayerPath, oTitle, oArthandle);
	}
	return result;
}

void ArtworkUtil::PlaceLayerEPS(AIDocumentHandle &oDocument, string strTemplateID)
{
	int nCount = 0;
	ASErr result = kNoErr;
	sAILayer->CountLayers(&nCount);
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		AILayerHandle oLayer;
		sAILayer->GetNthLayer(nIndex, &oLayer);
		ai::UnicodeString oTitle("");
		sAILayer->GetLayerTitle(oLayer, oTitle);
		CreateLayer(oTitle.as_Platform());
		oTitle.append(ai::UnicodeString(".eps"));
		string strLayerName = oTitle.as_Platform();
		ai::FilePath oLayerPath = GetLayerEPSPath(oTitle, strTemplateID);
		string strpath = oLayerPath.GetFullPath().as_Platform();
		if (oLayerPath.Exists(false))
		{
			AIArtHandle oArthandle;
			result = PlaceFile(oLayerPath, oTitle, oArthandle);
			aisdk::check_ai_error(result);
		}
	}
}

/*
*/
ai::FilePath ArtworkUtil::GetLayerEPSPath(const ai::UnicodeString& name, string strArtworkId)
{
	ai::FilePath oPath;
	ArtworkUtil oUtil;
	ai::UnicodeString abName(oUtil.GetUserFolder().c_str());
	sAIFilePath->Set(abName, true, false, oPath);
	oPath.AddComponent(ai::FilePath(ai::UnicodeString(strArtworkId.c_str())));
	oPath.AddComponent(ai::FilePath(ai::UnicodeString(strArtworkId.c_str())));
	oPath.AddComponent(ai::FilePath(ai::UnicodeString("Layers")));
	oPath.AddComponent(ai::FilePath(name));
	return oPath;
}

ai::FilePath ArtworkUtil::GetEPSPath(string strFilename, string strArtworkId)
{
	ai::FilePath oPath;
	ArtworkUtil oUtil;
	ai::UnicodeString abName(oUtil.GetUserFolder().c_str());
	sAIFilePath->Set(abName, true, false, oPath);
	oPath.AddComponent(ai::FilePath(ai::UnicodeString(strArtworkId.c_str())));
	oPath.AddComponent(ai::FilePath(ai::UnicodeString(strArtworkId.c_str())));
	oPath.AddComponent(ai::FilePath(ai::UnicodeString("Layers")));
	oPath.AddComponent(ai::FilePath(ai::UnicodeString(strFilename.c_str())));
	return oPath;
}

/*
*/
ASErr ArtworkUtil::PlaceFile(ai::FilePath oPath, const ai::UnicodeString& strFilename, AIArtHandle& oArtHandle)
{
	ASErr result = kNoErr;
	do
	{
		try {
			AIPlaceRequestData placeReqData;
			placeReqData.m_lPlaceMode = kVanillaPlace; // places the file and returns new art handle
			placeReqData.m_disableTemplate = true; // disable Template placement
			placeReqData.m_filemethod = 1; // place the file
			placeReqData.m_hNewArt = NULL; // new art handle
			placeReqData.m_hOldArt = NULL; // art to be replaced
			placeReqData.m_lParam = kPlacedArt; // type of art to create
			placeReqData.m_pFilePath = NULL; // if NULL, shows File Place dialog
			if (oPath.Exists(false) && strFilename.size() != 0) {
				placeReqData.m_pFilePath = &oPath; // place the given file (prompt for file to be placed otherwise)
			}
			sAIPreferenceSuite->PutIntegerPreference(NULL, "plugin/PDFImport/CropTo", kAIPDFMediaBox);
			// Place the file.
			result = sAIPlaced->ExecPlaceRequest(placeReqData);
			aisdk::check_ai_error(result);

			// Verify that the handle of the placed art is returned.
			SDK_ASSERT(placeReqData.m_hNewArt);

			// Pass handle to placed art back to caller.
			oArtHandle = placeReqData.m_hNewArt;
			AIRealRect oBoundBox;
			sAIArt->GetArtBounds(oArtHandle, &oBoundBox);
			AIRealMatrix oMatrix;
			oMatrix.Init();
			sAIRealMath->AIRealMatrixConcatTranslate(&oMatrix, -oBoundBox.left, -oBoundBox.top);
			sAITransformArt->TransformArt(oArtHandle, &oMatrix, 1.0, kTransformObjects);
			AIArtHandle oNative;
			result = sAIPlaced->MakePlacedObjectNative(oArtHandle, &oNative, false);
			if (result != 0)
			{
				sAIArt->DisposeArt(oArtHandle);
				break;
			}
			result = sAIArt->SetArtUserAttr(oArtHandle, kArtSelected, kArtSelected);
			AIActionParamValueRef oPVR;
			sAIActionManager->AINewActionParamValue(&oPVR);
			result = sAIActionManager->PlayActionEvent(kAIUngroupSelectionAction, kDialogNone, oPVR);
		}
		catch (ai::Error& ex) {
			result = ex;
		}
	} while (false);
	return 0;
}

string ArtworkUtil::GetComponent(string strTemplateInfo, int nIndex)
{
	int nIterator = 1;
	string strComponent = "";
	do
	{
		char * poArtwork;
		poArtwork = strtok((char*)strTemplateInfo.c_str(), ",");
		if (nIndex == nIterator)
		{
			strComponent.append(poArtwork);
			break;
		}
		while (poArtwork != NULL)
		{
			if (nIndex == nIterator)
			{
				strComponent.append(poArtwork);
				break;
			}
			poArtwork = strtok(NULL, " ,");
			nIterator++;
		}

	} while (false);
	return strComponent;
}

std::vector<string> ArtworkUtil::Split(string strString, string strSeparator)
{
	Logger::Info("ArtworkUtil", "Split strString :", strString);
	Logger::Info("ArtworkUtil", "Split strSeparator :", strSeparator);
	std::vector<string>arrComponents;
	do
	{
		char * poSplitString;
		poSplitString = strtok((char*)strString.c_str(), strSeparator.c_str());
		while (poSplitString != NULL)
		{
			arrComponents.push_back(poSplitString);
			poSplitString = strtok(NULL, strSeparator.c_str());
		}
	} while (false);
	Logger::Info("ArtworkUtil", "Split arrComponents.size :", to_string(arrComponents.size()));
	return arrComponents;
}

int ArtworkUtil::DeleteFolder(string strFilePath)
{
	Logger::Info("ArtworkUtil", "DeleteFolder :", strFilePath);
	int nError = 0;
	string a = "rmdir /s /q " + strFilePath;
	char char_array[100];
	strcpy(char_array, a.c_str()); //Converting a string into Character Array
	nError = system(char_array);
	Logger::Info("ArtworkUtil", "DeleteFolder nError :", to_string(nError));
	if (nError > 0)
		showAlert("error deleting folder : "+ strFilePath);
	return nError;
}

void ArtworkUtil::SendEventMessageToPanel(std::string eventName, std::string eventMessage)
{
	sAICSXSExtensionSuite->SendEventToExtension(kConversionExtensionID, eventName.c_str(), "ILST", eventMessage.c_str());
	sAICSXSExtensionSuite->LaunchExtension(kConversionExtensionID);
}

void ArtworkUtil::SendEventMessageToExtension(std::string eventName, std::string eventMessage)
{
	sAICSXSExtensionSuite->SendEventToExtension(kIGMExtensionId, eventName.c_str(), "ILST", eventMessage.c_str());
}

ai::FilePath ArtworkUtil::GetSwatchFilePath(string strArtworkId)
{
	ai::FilePath oPath;
	ArtworkUtil oUtil;
	ai::UnicodeString abName(GetUserFolder().c_str());
	sAIFilePath->Set(abName, true, false, oPath);
	oPath.AddComponent(ai::FilePath(ai::UnicodeString(strArtworkId.c_str())));
	oPath.AddComponent(ai::FilePath(ai::UnicodeString(strArtworkId.c_str())));
	string strFilename = "FERRERO_" + strArtworkId + ".ase";
	oPath.AddComponent(ai::FilePath(ai::UnicodeString(strFilename.c_str())));
	return oPath;
}

void ArtworkUtil::OpenTemplate(string strTemplateID)
{
	do
	{
		try
		{
			XMLPlatformUtils::Initialize();
		}
		catch (const XMLException& oException)
		{
			Logger::Info("ProcessCreateTemplate", "Error Initializing XMLPlatforUtils : ", "");
			break;
		}
		string strDocPath = "";
		string strTemplateZipFile = GetTemplateDownloadFolder();
		strTemplateZipFile.append(strTemplateID);
		strTemplateZipFile.append(".zip");
		string strUnzipFilePath = GetTemplateDownloadFolder();
		strUnzipFilePath.append(strTemplateID);
		strDocPath = strUnzipFilePath;
		CreateFolder(strUnzipFilePath);
		string strErrorString = "";
		string oResponse = GetDBTemplateXML(strTemplateID, strErrorString);
		if (!strErrorString.empty())
		{
			Logger::Error("ArtworkUtil", "ProcessCreateTemplate", strErrorString);
			showAlert(strErrorString);
			break;
		}
		CEPTestPanelController* panelController = new CEPTestPanelController();
		ArticleJob oArticleJob(oResponse, panelController);
		ASErr tErrorCode = oArticleJob.UnzipFile(strTemplateZipFile, strUnzipFilePath, strErrorString);
		string strMessage = "";
		if (tErrorCode != 0)
		{
			strMessage = "Error unzipping file :" + strTemplateZipFile;
			SendEventMessageToPanel("com.ferrero.error", strMessage);
			break;
		}
		AIDocumentHandle oDocument = NULL;
		strDocPath.append(1, kFilePathSeparator);
		strDocPath.append(strTemplateID);
		strDocPath.append(1, kFilePathSeparator);
		strDocPath.append(kFerreroPrefix);
		strDocPath.append(strTemplateID);
		strDocPath.append(".ai");
		ai::UnicodeString strPath = ai::UnicodeString(strDocPath);
		ai::FilePath oDocpath = ai::FilePath(strPath);
		if (oDocpath.Exists(false))
		{
			tErrorCode = OpenDocument(oDocument, strDocPath);
			if (tErrorCode != 0)
			{
				strMessage = "Error opening template :" + strDocPath;
				SendEventMessageToPanel("com.ferrero.error", strMessage);
				break;
			}
		}
		else
		{
			ProcessCreateTemplate(strTemplateID, oArticleJob);
		}
		try
		{
			XMLPlatformUtils::Terminate();
		}
		catch (const XMLException& oException)
		{
		}
	} while (false);
}

void ArtworkUtil::ProcessCreateTemplate(string strTemplateID, ArticleJob& oArticleJob)
{
	do
	{
		string strErrorString = "";
		oArticleJob.SetTemplateId(strTemplateID);
		oArticleJob.SetWidth(kDefaultPageWidth);
		oArticleJob.SetHeight(kDefaultPageHeight);
		oArticleJob.CreateNewTemplate();
	} while (false);

}

void ArtworkUtil::ProcessCreateArtwork(string strArtworkID, string strTemplateID, string strLayerInfo)
{
	do
	{
		Logger::Info("ArtworkUtil", "ProcessCreateArtwork", "Related to template :" + strTemplateID);
		ArticleJob oArticleJob;
		if (strTemplateID == "" || strTemplateID == "null")
		{
			Logger::Info("ArtworkUtil", "ProcessCreateArtwork", "Related to template is null or empty creating artwork without template");
			oArticleJob.SetTemplateId(strArtworkID);
			oArticleJob.SetWidth(kDefaultPageWidth);
			oArticleJob.SetHeight(kDefaultPageHeight);
			oArticleJob.CreateNewArtwork(strLayerInfo);
		}
		else
			CreateArtworkFromTemplate(strArtworkID, strTemplateID, strLayerInfo);
	} while (false);
}

void ArtworkUtil::CreateArtworkFromTemplate(string strArtworkID, string strTemplateID, string strLayerInfo)
{
	do
	{
		Logger::Info("ArtworkUtil", "CreateArtworkFromTemplate", "strArtworkID: " + strArtworkID);
		Logger::Info("ArtworkUtil", "CreateArtworkFromTemplate", "Creating artwork using template : "+ strTemplateID);
		AIDocumentHandle oDocument = NULL;
		string strArtworkFolder = GetArtworkDownloadFolder();
		string strDocPath = strArtworkFolder;
		strDocPath.append(1, kFilePathSeparator);
		strDocPath.append(strArtworkID);
		strDocPath.append(1, kFilePathSeparator);
		strDocPath.append(kFerreroPrefix);
		strDocPath.append(strTemplateID);
		strDocPath.append(".ai");
		ai::UnicodeString strPath = ai::UnicodeString(strDocPath);
		ai::FilePath oDocpath = ai::FilePath(strPath);
		Logger::Info("ArtworkUtil", "OpenArtwork", "Artwork file path :" + strDocPath);
		if (oDocpath.Exists(false))
		{
			Logger::Info("ArtworkUtil", "OpenArtwork", "Artwork file path exists opening document :" + strDocPath);
			ASErr tErrorCode = OpenDocument(oDocument, strDocPath);
			if (tErrorCode != 0)
			{
				string strMessage = "Error opening artwork :" + strDocPath;
				Logger::Info("ArtworkUtil", "OpenArtwork", "Error opening artwork :" + strDocPath);
				SendEventMessageToPanel("com.ferrero.error", strMessage);
				showAlert(strMessage);
				break;
			}
		}
		Logger::Info("ArtworkUtil", "CreateArtworkFromTemplate", "Adding template info in ai file ");
		Logger::Info("ArtworkUtil", "CreateArtworkFromTemplate", "Processing layers in ai file ");
		std::vector<string> arrRequiredLayers = ExtractLayers(strLayerInfo);
		ProcessArtwork(oDocument, strLayerInfo);
		AddTemplateInfo(strTemplateID);
		FinalizeArtwork(arrRequiredLayers);
	} while (false);
}

void ArtworkUtil::HandleFinilizeArtworkCompleteEvent()
{
	SendEventMessageToExtension("com.ferrero.cleanartworkfolder", "");
}

string ArtworkUtil::ConstructDocumentPath(string strUnzipFolder, string strArtworkID)
{
	string strDocumentFile = strUnzipFolder;
	strDocumentFile.append(1, kFilePathSeparator);
	strDocumentFile.append(strArtworkID);
	strDocumentFile.append(1, kFilePathSeparator);
	strDocumentFile.append(kFerreroPrefix);
	strDocumentFile.append(strArtworkID);
	return strDocumentFile;
}

void ArtworkUtil::AddTemplateInfo(string strTemplateID)
{
	AddToDocumentDictionary(kTemplateIDkey, strTemplateID);
	string strErrorString = "";
	string strTemplateXML = GetTemplateXML(strTemplateID, strErrorString);
	CEPTestPanelController* panelController = new CEPTestPanelController();
	ArticleJob oArticleJob(strTemplateXML, panelController);
	std::vector<GraphicItem> arrGraphicItem = oArticleJob.GetGraphicItems();
	AddGraphicDetailsTodictionary(arrGraphicItem);
	AddToDocumentDictionary(kTemplateXMLkey, strTemplateXML);
	string strRulesXML = GetTemplateRulesXML(strTemplateID, strErrorString);
	SetRulesToDictionary(strRulesXML);
	AddToDocumentDictionary(kTemplateRulesXMLkey, strRulesXML);
}

void ArtworkUtil::AddGraphicDetailsTodictionary(std::vector<GraphicItem> arrGraphicItem)
{
	for (int nIndex = 0; nIndex < arrGraphicItem.size(); nIndex++)
		arrGraphicItem[nIndex].SetToDictionary();
}

void ArtworkUtil::SetRulesToDictionary(string strRulesXML)
{
	do
	{
		XercesDOMParser* poParser = new XercesDOMParser();
		poParser->setValidationScheme(XercesDOMParser::Val_Always);
		poParser->setDoNamespaces(true);    // optional
		ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
		poParser->setErrorHandler(errHandler);
		const char *poChars = strRulesXML.c_str();
		XMLByte *pcResponse = (XMLByte *)(poChars);
		xercesc::MemBufInputSource oSource(pcResponse, strRulesXML.length(), "Rules XML");
		try
		{
			poParser->parse(oSource);
			xercesc::DOMDocument * poDoc = poParser->getDocument();
			if (poDoc == nullptr)
				break;
			DOMElement * poElement = poDoc->getDocumentElement();
			if (poElement == nullptr)
				break;
			const XMLCh *poImageTag = XMLString::transcode("graphic_object");
			DOMNodeList * poImageList = poElement->getElementsByTagName(poImageTag);
			for (int nIndex = 0; nIndex < poImageList->getLength(); nIndex++)
			{
				RuleData oRuleData(poImageList->item(nIndex));
				oRuleData.SetToDictionary();
			}
		}
		catch (...)
		{

		}
	} while (false);
}

string ArtworkUtil::GetTemplateInfo(string strKey)
{
	ArtworkUtil oUtil;
	string strTemplateValue = oUtil.GetValueFromDocumentDictionary(strKey);
	return strTemplateValue;
}

void ArtworkUtil::ProcessArtwork(AIDocumentHandle &oDocument, string strArtworkInfo)
{
	Logger::Info("ArtworkUtil", "ProcessLayers : ", strArtworkInfo);
	std::vector<string> arrRequiredLayers = ExtractLayers(strArtworkInfo);
	std::vector<string> arrDocumentLayers = GetDocumentlayers();
	arrRequiredLayers.push_back("technical-drawing");
	arrRequiredLayers.push_back("check");
	arrRequiredLayers.push_back("notes");
	arrRequiredLayers.push_back("shared");
	RemoveLayers(arrRequiredLayers, arrDocumentLayers);
	AddLayers(arrRequiredLayers, arrDocumentLayers);
}

std::vector<string> ArtworkUtil::ExtractLayers(string strArtworkInfo)
{
	Logger::Info("ArtworkUtil", "ExtractLayers : ", strArtworkInfo);
	std::vector<string> arrLayers;
	do
	{
		string strlayerInfo = strArtworkInfo;
		int nFound = strArtworkInfo.find_first_of(":");
		if (nFound > 0)
			strlayerInfo = strArtworkInfo.substr(nFound + 1);
		char * poLayer;
		poLayer = strtok((char*)strlayerInfo.c_str(), ",");
		while (poLayer != NULL)
		{
			string strLayerID = "";
			strLayerID.append(poLayer);
			arrLayers.push_back(strLayerID);
			poLayer = strtok(NULL, ",");
		}
	} while (false);
	Logger::Info("ArtworkUtil", "ExtractLayers : arrLayers.size() : ", to_string(arrLayers.size()));
	return arrLayers;
}

void ArtworkUtil::RemoveLayers(std::vector<string> arrRequiredLayers, std::vector<string> arrDocumentLayers)
{
	Logger::Info("ArtworkUtil", "RemoveLayers", "");
	for (int nIndex = 0; nIndex < arrDocumentLayers.size(); nIndex++)
		if (!CanKeep(arrDocumentLayers[nIndex], arrRequiredLayers))
			Removelayer(arrDocumentLayers[nIndex]);
}

void ArtworkUtil::AddLayers(std::vector<string> arrRequiredLayers, std::vector<string> arrDocumentLayers)
{
	Logger::Info("ArtworkUtil", "AddLayers", "");
	for (int nIndex = 0; nIndex < arrRequiredLayers.size(); nIndex++)
		if (!IsFound(arrRequiredLayers[nIndex], arrDocumentLayers))
			CreateLayer(arrRequiredLayers[nIndex]);
}

void ArtworkUtil::FinalizeArtwork(std::vector<string> arrRequiredLayers)
{
	std::vector<string> arrDestinationLayers = GetDestinationLayers(arrRequiredLayers);
	string strDestinationLayers = "[";
	for (int nIndex = 0; nIndex < arrDestinationLayers.size(); nIndex++)
		strDestinationLayers += nIndex == 0 ? ("\"" + arrDestinationLayers[nIndex] + "\"") : (",\"" + arrDestinationLayers[nIndex] + "\"");
	strDestinationLayers += "]";
	SendEventMessageToExtension("com.ferrero.finalizeartwork", strDestinationLayers);
}

std::vector<string> ArtworkUtil::GetDestinationLayers(std::vector<string> arrRequiredLayers)
{
	std::vector<string> arrDestinationLayers;
	for (int nIndex = 0; nIndex < arrRequiredLayers.size(); nIndex++)
	{
		if (arrRequiredLayers[nIndex].find_first_of('-') == 4)
			arrDestinationLayers.push_back(arrRequiredLayers[nIndex]);
	}
	return arrDestinationLayers;
}

ASErr ArtworkUtil::OpenAiFile(string strTemplateID, string strArtworkID, string strUnzipFolder, AIDocumentHandle &oDocument)
{
	ASErr tError = kNoErr;
	Logger::Info("ArtworkUtil", "OpenAiFile Template ID : ", strTemplateID);
	string strDocumentFile = strUnzipFolder;
	strDocumentFile.append(1, kFilePathSeparator);
	strDocumentFile.append(strArtworkID);
	strDocumentFile.append(1, kFilePathSeparator);
	strDocumentFile.append(kFerreroPrefix);
	strDocumentFile.append(strTemplateID);
	strDocumentFile.append(".ai");
	ai::UnicodeString strPath = ai::UnicodeString(strDocumentFile);
	Logger::Info("ArtworkUtil", "OpenAiFile", "Opening the template document path : " + strPath.as_Platform());
	tError = sAIDocumentList->Open(ai::FilePath(strPath), kAICMYKColorModel, kDialogNone, false, &oDocument);
	if (tError > 0)
		showAlert("Unable to open ai file : " + strDocumentFile);
	return tError;
}

void ArtworkUtil::OpenArtwork(string strArtworkID)
{
	Logger::Info("ArtworkUtil", "OpenArtwork", "Opening the artwork : " + strArtworkID);
	do
	{
		try
		{
			XMLPlatformUtils::Initialize();
			ASErr tErrorCode = UnzipArtwork(strArtworkID);
			if (!DoOpenArtwork(strArtworkID))
				ConvertTemplateToArtwork(strArtworkID);
			XMLPlatformUtils::Terminate();
		}
		catch (...)
		{
			Logger::Error("ArtworkUtil", "OpenArtwork", "Error Occurred!");
		}
	} while (false);
}

void ArtworkUtil::ConvertTemplateToArtwork(string strArtworkID)
{
	string strError = "";
	string strArtworkDetail = GetArtworkDetails(strArtworkID, strError);
	Logger::Info("ArtworkUtil", "OpenArtwork", "strArtworkDetail :" + strArtworkDetail);
	std::vector<string> arrArtworkDetail = Split(strArtworkDetail, "~");
	string strLayerInfo = "";
	string strTemplateID = "";
	if (arrArtworkDetail.size() > 0)
		strTemplateID = arrArtworkDetail[0];
	if (arrArtworkDetail.size() > 1)
		strLayerInfo = arrArtworkDetail[1];
	ProcessCreateArtwork(strArtworkID, strTemplateID, strLayerInfo);
}

AIBoolean ArtworkUtil::DoOpenArtwork(string strArtworkID)
{
	AIBoolean bSuccess = false;
	do
	{
		AIDocumentHandle oDocument = NULL;
		string strDocPath = ConstructArtworkPath(strArtworkID);
		ai::UnicodeString strPath = ai::UnicodeString(strDocPath);
		ai::FilePath oDocpath = ai::FilePath(strPath);
		Logger::Info("ArtworkUtil", "OpenArtwork", "Artwork file path :" + strDocPath);
		if (!oDocpath.Exists(false))
			break;
		Logger::Info("ArtworkUtil", "OpenArtwork", "Artwork file path exists opening document :" + strDocPath);
		ASErr tErrorCode = OpenDocument(oDocument, strDocPath);
		bSuccess = tErrorCode == 0;
		if (bSuccess)
			break;
		string strMessage = "Error opening artwork :" + strDocPath;
		Logger::Info("ArtworkUtil", "OpenArtwork", "Error opening artwork :" + strDocPath);
		SendEventMessageToPanel("com.ferrero.error", strMessage);
	} while (false);
	return bSuccess;
}

string ArtworkUtil::ConstructArtworkPath(string strArtworkID)
{
	string strDocPath = GetArtworkDownloadFolder();
	strDocPath.append(1, kFilePathSeparator);
	strDocPath.append(strArtworkID);
	strDocPath.append(1, kFilePathSeparator);
	strDocPath.append(kFerreroPrefix);
	strDocPath.append(strArtworkID);
	strDocPath.append(".ai");
	return strDocPath;
}

ASErr ArtworkUtil::UnzipArtwork(string strArtworkID)
{
	string strArtworkZipFile = GetArtworkDownloadFolder();
	strArtworkZipFile.append(1, kFilePathSeparator);
	strArtworkZipFile.append(strArtworkID);
	strArtworkZipFile.append(".zip");
	Logger::Info("ArtworkUtil", "OpenArtwork", "Unzipping file : " + strArtworkZipFile);
	ASErr tErrorCode = UnzipFile(strArtworkZipFile, GetArtworkDownloadFolder());
	string strMessage = "";
	if (tErrorCode != 0)
	{
		strMessage = "Error unzipping file :" + strArtworkZipFile;
		Logger::Error("ArtworkUtil", "OpenArtwork", "Error unzipping file :" + strArtworkZipFile);
		SendEventMessageToPanel("com.ferrero.error", strMessage);
		throw (strMessage);
	}
	return tErrorCode;
}

string ArtworkUtil::GetArtworkDetails(string strArtworkID, string &strError)
{
	FerreroServerFacade oFerreroServerFacade;
	string strGetArtworkDetailParam = "artworkid=";
	strGetArtworkDetailParam.append(strArtworkID);
	string oResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/api/getArtworkDetail", strGetArtworkDetailParam, strError);
	return oResponse;
}

string ArtworkUtil::GetArtworkDownloadFolder()
{
	string strArtworkFolder = GetUserFolder();
	strArtworkFolder.append(kArtworkFolder);
	CreateFolder(strArtworkFolder);
	strArtworkFolder.append(1, kFilePathSeparator);
	return strArtworkFolder;
}

string ArtworkUtil::GetTemplateDownloadFolder()
{
	return GetUserFolder();
}

void ArtworkUtil::WriteExtensionLog(string strLogMessage)
{
	EXTLogger::Log("", "", strLogMessage);
}

AIArtHandle ArtworkUtil::GetArtByClipGroupId(string strClipGroupId)
{
	AIArtHandle oArt = NULL;
	ai::UnicodeString strClipGroupID_U(strClipGroupId);
	ai::UnicodeString strArtName;
	AILayerHandle oLayer;
// Commented code to handle template conversion issue 157556020669739736			
//	if (strClipGroupId.length() > 0)
	{
		sAILayer->GetCurrentLayer(&oLayer);
		sAIArt->GetFirstArtOfLayer(oLayer, &oArt);
		sAIArt->GetArtFirstChild(oArt, &oArt);
		do
		{
			if (oArt == NULL)
				break;
			sAIArt->GetArtName(oArt, strArtName, NULL);
			if (strArtName.find(strClipGroupID_U) != ai::UnicodeString::npos)
				break;
		} while (sAIArt->GetArtSibling(oArt, &oArt) == kNoErr);
	}
	return oArt;
}

ai::int32 ArtworkUtil::GetCropTo(string strCropTo)
{
	ai::int32 nCropTo = kAIPDFMediaBox;
	if (strCropTo == kPDFCropArt)
		nCropTo = kAIPDFArtBox;
	else if (strCropTo == kPDFCropTrim)
		nCropTo = kAIPDFTrimBox;
	else if (strCropTo == kPDFCropBleed)
		nCropTo = kAIPDFBleedBox;
	else if (strCropTo == kPDFCropVisibleLayers || strCropTo == kPDFCropAllLayers)
		nCropTo = kAIPDFBoundingBox;
	return nCropTo;
}

void ArtworkUtil::showAlert(string strMessage)
{
	ai::UnicodeString strAlertMessage(strMessage);
	sAIUserSuite->MessageAlert(strAlertMessage);
}

void ArtworkUtil::ZipPreviewFolder(string strTemplateID)
{
	string strFoderToZip = GetUserFolder();
	strFoderToZip.append(strTemplateID);
	strFoderToZip.append(1, kFilePathSeparator);
	strFoderToZip.append(strTemplateID);
	strFoderToZip += kFilePathSeparator;
	strFoderToZip.append(kPreviewFolderName);
	ZipFolder(strFoderToZip);
	string strMessage = strTemplateID +"*"+strFoderToZip+".zip";
	Logger::Info("ArtworkUtil", "ZipPreviewFolder", "strFoderToZip : "+ strFoderToZip);
	SendEventMessageToExtension("com.ferrero.uploadpreview", strMessage);
}

string ArtworkUtil::GetTemplateIDFromCurrentDocument()
{
	string strTemplateId = "";
	ai::FilePath oFile;
	do
	{
		sAIDocument->GetDocumentFileSpecification(oFile);
		string strFileName = oFile.GetFileName().as_Platform();
		if (strFileName.find(kFerreroPrefix) != 0)
			break;
		std::vector<string> arrComponents = Split(strFileName, "_");
		if (arrComponents.size() == 2)
			strTemplateId = Split(arrComponents[1], ".")[0];
	} while (false);
	return strTemplateId;
}

string ArtworkUtil::GetCurrentDocumentPath()
{
	string strDocPath = "";
	ai::FilePath oFile;
	sAIDocument->GetDocumentFileSpecification(oFile);
	ai::FilePath oParentFolder = oFile.GetParent();
	strDocPath = oParentFolder.GetFullPath().as_Platform();
	return strDocPath;
}

void ArtworkUtil::PlaceNewTemplateFiles(string strGraphicItem)
{
	do
	{
		AIDocumentHandle oDocument;
		sAIDocument->GetDocument(&oDocument);
		string strErrorString = "";
		string strTemplateId = GetTemplateIDFromCurrentDocument();
		GraphicItem oGraphicItem;
		oGraphicItem.JSONParse(strGraphicItem);
		oGraphicItem.PlaceItem(oDocument, strTemplateId, strErrorString);
	} while (false);
}

string ArtworkUtil::GetMatchingLayerName( string strCountryId)
{
	string strlayerName = "";
	int nLayerCount = 0;
	sAILayer->CountLayers(&nLayerCount);
	for (int nIndex = 0; nIndex < nLayerCount; nIndex++)
	{
		AILayerHandle oLayer;
		sAILayer->GetNthLayer(nIndex, &oLayer);
		ai::UnicodeString strTitle;
		sAILayer->GetLayerTitle(oLayer, strTitle);
		if (strTitle.as_Platform().find_first_of(strCountryId) == 0)
			strlayerName = strTitle.as_Platform();
	}
	return strlayerName;
}

void ArtworkUtil::ZipArtworkFolder(string strArtworkFileName)
{
	do
	{
		std::vector<string> arrFileComponents = Split(strArtworkFileName, ".");
		std::vector<string> arrComponents = Split(arrFileComponents[0], "_");
		if (arrComponents.size() < 2)
			break;
		string strArtworkID = arrComponents[1];
		string strArtworkFolder = GetArtworkDownloadFolder();
		strArtworkFolder.append(1, kFilePathSeparator);
		strArtworkFolder.append(strArtworkID);
		DeleteArtworkZip(strArtworkID);
		ZipFolder(strArtworkFolder);
		string strMessage = "artworkid=" + strArtworkID;
		SendEventMessageToExtension("com.ferrero.uploadartworkzip", strMessage);
	} while (false);
}

void ArtworkUtil::SaveTemplateXML(string strTemplateXML)
{
	std::vector<string> arrXMLComponents = Split(strTemplateXML, "~");
	string strTemplateID = arrXMLComponents[0];
	string strXML = arrXMLComponents[1];
	string strErrorString = "";
	FerreroServerFacade oFerreroServerFacade;
	string strSaveTemplateXMLParam = "function=saveConvertedTemplateXML&projectid=";
	strSaveTemplateXMLParam.append(strTemplateID);
	strSaveTemplateXMLParam.append("&xml=");
	strSaveTemplateXMLParam.append(oFerreroServerFacade.URLEncode(strXML));
	string oResponse = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strSaveTemplateXMLParam, strErrorString);
	if (!strErrorString.empty())
	{
		Logger::Error("ArtworkUtil", "SaveTemplateXML", strErrorString);
		string strMessage = "Error saving template XML : " + strErrorString;
		SendEventMessageToPanel("com.ferrero.log", strMessage);
	}
}

void ArtworkUtil::DeleteArtworkZip(string strArtworkID)
{
	string strArtworkZipFile = GetArtworkDownloadFolder();
	strArtworkZipFile.append(1, kFilePathSeparator);
	strArtworkZipFile.append(strArtworkID);
	strArtworkZipFile.append(".zip");
	ai::UnicodeString strPath = ai::UnicodeString(strArtworkZipFile);
	ai::FilePath oZipFile = ai::FilePath(strPath);
	if (oZipFile.Exists(false))
	 	remove(strArtworkZipFile.c_str());
}
