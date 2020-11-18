//========================================================================================
//  
//  $File: //ai_stream/rel_23_0/devtech/sdk/public/samplecode/MenuPlay/Source/MenuPlayID.h $
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

#ifndef __MENUPLAYID_H__
#define __MENUPLAYID_H__

#define kMenuPlayPluginName			"MenuPlay"

//#define kMenuPlaySDKString			"Ferrero"
#define kMenuPlaySDKString			"Convert"
#define kSDKMenuGroup				"SDK Group"
#define kMenuPlayString				kMenuPlayPluginName
#define kMenuPlayMenuGroup			"Menu Play Group"
#define kMenuPlayGroupString		"If enabled, selection contains a group"
#define kMenuPlayLockString			"If enabled, Art/Layer is unlocked"
#define kMenuPlayGuideString		"Manually updated for guides"
#define kMenuPlayConvertTemplateString		"Convert Template"
#define kMenuPlayCreateArtworkString		"Create Artwork"
#define kMenuPlayCreateTemplateString		"Create Template"
#define kMenuPlayHighlightString	"Disable the above menu item"
#define kTemplateConfigFileName		"template.txt"
#define kArtworkConfigFileName		"artwork.txt"
#define kProxyConfigFileName		"proxy.txt"
#define kCreateTemplateConfigFileName "newtemplate.xml"
#define kDefaultPageWidth 210
#define kDefaultPageHeight 297
#define PPI 72.0
#define MMPI 25.40
#define DTR 0.01745
#define kFerreroWFESS2URL			"https://templateqlt.ferrero.com/WFE_SS2"
//#define kFerreroWFESS2URL			"http://indesignqlt.it.ferrero.com/WFE_SS2"
//#define kFerreroWFESS2URL			"http://xita1801g4.ferreronet.com/WFE_SS2"
#define kTestURL					"http://localhost:8080"
#ifdef _WINDOWS
#define kFilePathSeparator '\\'
#else
#define kFilePathSeparator '/'
#endif
#define kLogFileName "Log.txt"
#define kExtLogFileName "ExtLog.txt"
#define kArtworkFolder "artwork"
#define kPreviewFolderName "preview"
#define CHUNK 16384
#define MAX_FILENAME 512
#define READ_SIZE 8192
#define kFerreroPrefix "FERRERO_"
#define kFerreroDTPrefix "FERRERO_DT_"
#define kGraphicKeyPrefix "Graphic_"
#define kTemplateIDkey "templateid"
#define kTemplateXMLkey "templatexml"
#define kTemplateRulesXMLkey "templaterulesxml"
#define kRuleIsMovable	"isMovable"
#define kRuleIsMovableID	7
#define kRuleIsResizable	"isResizable"
#define kRuleIsResizableId	2
#define kTransparency	"TRANSPARENCY"
#define kEps	"EPS"
#define kArtboxString "<</ArtBox["
#define kCropboxString "CropBox["
#define kConvertTemplatNotifier	"ConverTemplateNotifier"
#define kAddSwatchNotifier "AddSwatchNotifier"
#define kRotateString	"Rotate"
#define kOpenTemplateNotifier "OpenTemplateNotifier"
#define kOpenArtworkNotifier  "OpenArtworkNotifier"
#define kZipPreviewFolderNotifier "ZipPreviewFolderNotifier"
#define kPlaceNewTemplateFilesNotifier "kPlaceNewTemplateFilesNotifier"
#define kProcessDownLoadCompleteNotifier "kProcessDownLoadCompleteNotifier"
#define kProcessZipArtworkNotifier "kProcessZipArtworkNotifier"
#define kProcessSaveTemplateXMLNotifier "kProcessSaveTemplateXMLNotifier"
#define kFinalizeArtworkCompleteNotifier "kFinalizeArtworkCompleteNotifier"
#define kHiresBboxString "%%HiResBoundingBox: "
#define kPDFCropArt "CROP_ART"
#define kPDFCropPDF "CROP_PDF"
#define kPDFCropTrim "CROP_TRIM"
#define kPDFCropBleed "CROP_BLEED"
#define kPDFCropMedia "CROP_MEDIA"
#define kPDFCropVisibleLayers "CROP_CONTENT_VISIBLE_LAYERS"
#define kPDFCropAllLayers "CROP_CONTENT_ALL_LAYERS"
#define kIGMExtensionId "com.emerasoft.ferrero.igm"
#define kConversionExtensionID "com.emerasoft.ferrero"
#endif // End MenuPlayID.h


