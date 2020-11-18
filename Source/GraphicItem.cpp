#include "IllustratorSDK.h"
#include "ArtworkUtil.h"
#include "GraphicItem.h"
#include "MenuPlayID.h"
#include "Logger.h"
#include "Path.h"
#include <iostream>

GraphicItem::GraphicItem()
{
	m_strCountryid = "";
	m_strGraphic_id = "";
	m_dHeight = 100;
	m_strId = "";
	m_dItemxscale = 1;
	m_dItemyscale = 1;
	m_dPicxscale = 1;
	m_dPicyscale = 1;
	m_strUid = "";
	m_dWidth = 100;
	m_dXpos = 0;
	m_dYpos = 0;
	m_nPicXOffset = 0;
	m_nPicYOffset = 0;
	m_nPicRotateAngle = 0;
	m_nPicShearAngle = 0;
	m_dPicHeight = 0;
	m_dPicWidth = 0;
	m_strFilename = "";
	m_strLayerName = "";
	m_strPath = "";
	m_strType = "";
	m_nOpacity = 100;
	m_strBlendMode = "NORMAL";
	m_strFillColor = "";
	m_strEffectsApplied = "";
	m_strClipGroupId = "";
	m_strCropTo = "";
	m_strParentClipGroupId = "";
	m_bIsNewImage = false;
	m_dPicXpos = 0;
	m_dPicYpos = 0;
}

GraphicItem::GraphicItem(DOMNode *poNode)
{
	ArtworkUtil oUtil;
	string strTagname = XMLString::transcode(poNode->getNodeName());
	strTagname = strTagname;
	DOMNamedNodeMap * poNodeNameMap = poNode->getAttributes();
	if (poNodeNameMap != nullptr)
	{
		std::string::size_type tSize;
		m_strCountryid = ExtractCountryID(oUtil.GetAttributeValue(poNodeNameMap, "layername"));
        if (m_strCountryid == "00" || m_strCountryid == "")
            m_strCountryid = "shared";
		string strGraphicID = oUtil.GetAttributeValue(poNodeNameMap, "graphic_id");
		m_strGraphic_id = strGraphicID.length() > 0 ? strGraphicID : ExtractGraphicID (oUtil.GetAttributeValue(poNodeNameMap, "linkname"));
		m_strFilename = oUtil.GetAttributeValue(poNodeNameMap, "linkname");
		m_strLayerName = oUtil.GetAttributeValue(poNodeNameMap, "layername");
		string strInstanceID = oUtil.GetAttributeValue(poNodeNameMap, "id");
		m_strId = strInstanceID.length() > 0 ? strInstanceID : ExtractInstanceID(oUtil.GetAttributeValue(poNodeNameMap, "linkname"));
		string strHeight = oUtil.GetAttributeValue(poNodeNameMap, "height");
		m_dHeight = strHeight.length () > 0 ? oUtil.GetRealValue(strHeight) : 100;
		string strXscale = oUtil.GetAttributeValue(poNodeNameMap, "itemxscale");
		m_dItemxscale = strXscale.length () > 0 ? oUtil.GetRealValue(strXscale) : 1;
		string strYscale = oUtil.GetAttributeValue(poNodeNameMap, "itemyscale");
		m_dItemyscale = strYscale.length () > 0 ? oUtil.GetRealValue(strYscale) : 1;
		string strPicXScale = oUtil.GetAttributeValue(poNodeNameMap, "graphicxscale");
		string strPicYScale = oUtil.GetAttributeValue(poNodeNameMap, "graphicyscale");
		m_dPicxscale = strPicXScale.length () > 0 ? oUtil.GetRealValue(strPicXScale)/100.0 : 1;
		m_dPicyscale = strPicYScale.length () > 0 ? oUtil.GetRealValue(strPicYScale)/100.0 : 1;
		m_strUid = oUtil.GetAttributeValue(poNodeNameMap, "id");
		string strWidth = oUtil.GetAttributeValue(poNodeNameMap, "width");
		m_dWidth = strWidth.length () > 0 ? oUtil.GetRealValue(strWidth) : 100;
		m_dXpos = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "xpos"));
		m_dYpos = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "ypos"));
		m_nPicXOffset = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "xoffset"));
		m_nPicYOffset = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "yoffset"));
		m_nPicRotateAngle = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "angle"));
		m_dPicWidth = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "picwidth"));
		m_dPicHeight = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "picheight"));
		m_strPath = oUtil.GetAttributeValue(poNodeNameMap, "path");
		m_strType = oUtil.GetAttributeValue(poNodeNameMap, "type");
		m_nOpacity = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "opacity"));
		m_strBlendMode = oUtil.GetAttributeValue(poNodeNameMap, "blendmode");
		m_nPicShearAngle = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "shear"));
		m_strFillColor = oUtil.GetAttributeValue(poNodeNameMap, "fillcolor");
		m_strEffectsApplied = oUtil.GetAttributeValue(poNodeNameMap, "effectsApplied");
		m_strClipGroupId = oUtil.GetAttributeValue(poNodeNameMap, "clipGroupId");
		m_strCropTo = oUtil.GetAttributeValue(poNodeNameMap, "cropTo");
		m_strParentClipGroupId = oUtil.GetAttributeValue(poNodeNameMap, "parentClipGroupId");
		m_strImageName = oUtil.GetAttributeValue(poNodeNameMap, "filename");
		m_dPicXpos = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "picxpos"));
		m_dPicYpos = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "picypos"));
		m_bIsNewImage = (m_dXpos == 0 && m_dYpos == 0);
	}
}

string GraphicItem::ExtractCountryID(string strLayerName)
{
	string strCountryID = strLayerName;
	if (strLayerName.find_first_of("-") > 0)
		strCountryID = strLayerName.substr(0, strLayerName.find_first_of("-"));
	return strCountryID;
}

string  GraphicItem::ExtractGraphicID(string strLinkName)
{
	ArtworkUtil oUtil;
	string strGraphicID = "";
	std::vector<string> arrComponents = oUtil.Split(strLinkName, "_");
	if (arrComponents.size() > 1)
		strGraphicID = arrComponents.at(1);
	return strGraphicID;
}

string  GraphicItem::ExtractInstanceID(string strLinkName)
{
	ArtworkUtil oUtil;
	string strInstanceID = "";
	std::vector<string> arrComponents = oUtil.Split(strLinkName, "_");
	if (arrComponents.size() > 2)
	{
		strInstanceID = arrComponents.at(2);
		std::vector<string> arrsplit = oUtil.Split(strInstanceID, ".");
		strInstanceID = arrsplit.at(0);
	}
	return strInstanceID;
}

void GraphicItem::JSONParse(string strGraphicItem)
{
	try
	{
		ArtworkUtil oUtil;
		std::vector<string> arrFields = oUtil.Split(strGraphicItem, ",");
		m_strImageName = oUtil.Split(
			oUtil.Split(arrFields[0], ":")[1], "\"")[0];
		m_strCountryid = oUtil.Split(
			oUtil.Split(arrFields[1], ":")[1], "\"")[0];
		m_strFilePath = oUtil.GetCurrentDocumentPath() + kFilePathSeparator + m_strImageName;
		m_strLayerName = oUtil.GetMatchingLayerName(m_strCountryid);
		m_bIsNewImage = true;
	}
	catch (...)
	{
		Logger::Error("GraphicItem", "JSONParse", "Parsing error!"+ strGraphicItem);
	}
}

string GraphicItem::GetCountryId()
{
	return m_strCountryid;
}

void GraphicItem::SetCountryId(string strCountryId)
{
	m_strCountryid = strCountryId;
}

string GraphicItem::GetGraphic_id()
{
	return m_strGraphic_id;
}

void GraphicItem::SetGraphic_id(string strGraphic_id)
{
	m_strGraphic_id = strGraphic_id;
}

AIReal GraphicItem::GetHeight()
{
	return m_dHeight;
}

void GraphicItem::SetHeight(AIReal dHeight)
{
	m_dHeight = dHeight;
}

string GraphicItem::GetId()
{
	return m_strId;
}

void GraphicItem::SetId(string strId)
{
	m_strId = strId;
}

AIReal GraphicItem::GetItemxscale()
{
	return m_dItemxscale;
}

void GraphicItem::SetItemxscale(AIReal dItemxscale)
{
	m_dItemxscale = dItemxscale;
}

AIReal GraphicItem::GetItemyscale()
{
	return m_dItemyscale;
}

void GraphicItem::SetItemyscale(AIReal dItemyscale)
{
	m_dItemyscale = dItemyscale;
}

AIReal GraphicItem::GetPicxscale()
{
	return m_dPicxscale;
}

void GraphicItem::SetPicxscale(AIReal dPicxscale)
{
	m_dPicxscale = dPicxscale/100.0;
}

AIReal GraphicItem::GetPicyscale()
{
	return m_dPicyscale;
}

void GraphicItem::SetPicyscale(AIReal dPicyscale)
{
	m_dPicyscale = dPicyscale/100.0;
}

string GraphicItem::GetUid()
{
	return m_strUid;
}

void GraphicItem::SetUid(string strUid)
{
	m_strUid = strUid;
}

AIReal GraphicItem::GetWidth()
{
	return m_dWidth;
}

void GraphicItem::SetWidth(AIReal dWidth)
{
	m_dWidth = dWidth;
}

AIReal GraphicItem::GetXpos()
{
	return m_dXpos;
}

void GraphicItem::SetXpos(AIReal dXpos)
{
	m_dXpos = dXpos;
}

AIReal GraphicItem::GetYpos()
{
	return m_dYpos;
}

void GraphicItem::SetYpos(AIReal dYpos)
{
	m_dYpos = dYpos;
}

string GraphicItem::GetObjectKey()
{
	string strObjectKey = "FERRERO_";
	strObjectKey.append(m_strGraphic_id.c_str());
	strObjectKey.append("_");
	strObjectKey.append(m_strId);
	return strObjectKey;
}

void GraphicItem::SetFilePath(string strFilePath)
{
	m_strFilePath = strFilePath;
}

string GraphicItem::GetFilePath()
{
	return m_strFilePath;
}

void GraphicItem::SetFileName(string strFileName)
{
	m_strFilename = strFileName;
}

string  GraphicItem::GetFileName()
{
	return m_strFilename;
}

AIBoolean GraphicItem::IsNewImage()
{
	return m_bIsNewImage;
}

string GraphicItem::GetImageName()
{
	return m_strImageName;
}

string GraphicItem::Stringify()
{
	string strJsonObject = "{";
	strJsonObject += "\"m_strImageName\" : \"" + m_strImageName + "\",";
	strJsonObject += "\"m_strCountryid\" : \"" + m_strCountryid + "\"";
	strJsonObject += "}";
	return strJsonObject;
}

void GraphicItem::PlaceItem(AIDocumentHandle &oDocument, string strArtworkId, string &strErrorString)
{
	do
	{
		if (m_strType == kEps)
		{
			ArtworkUtil oUtil;
// Commented code to handle template conversion issue 157556020669739736			
/*
			AIArtHandle oGroup;
			AIArtHandle oClipping;
			AIRealRect oImageRect = { m_dXpos, -m_dYpos, m_dXpos + m_dWidth, -(m_dYpos + m_dHeight) };
			MMtoPoints(oImageRect);
			CreateClippingGroup(oGroup, oImageRect, oClipping);
*/			
			oUtil.PlaceEPSItem(strArtworkId, m_strLayerName, m_strFilename);
			break;
		}
		AIArtHandle oArtHandle = NULL;
		string strFileName = m_strFilename == "" ? m_strFilePath.substr(m_strFilePath.find_last_of("/\\") + 1) : m_strFilename;
		if ((m_strType != kTransparency) && strFileName.size() == 0)
		{
			Logger::Info("GraphicItem", "PlaceItem", "File path : "+ m_strFilePath);
			Logger::Error("GraphicItem", "PlaceItem", "File not found Ferrero_" + m_strGraphic_id + "_"+ m_strId);
			break;
		}
		ArtworkUtil oUtil;
		oUtil.CreateLayer(m_strLayerName);
		ai::FilePath oPath = this->GetAssetPath(ai::UnicodeString(strFileName.c_str()), strArtworkId);
		string strPath = oPath.GetFullPath().as_Platform();
		if (oPath.Exists(false))
		{
			ASErr tResult = PlaceFile(ai::UnicodeString(strFileName.c_str()), oArtHandle, strArtworkId, strErrorString);
		}
		else
		{
			strErrorString += " \n Error placing file, File not found :" + strPath;
			Logger::Error("GraphicItem", "PlaceItem", "File not found : " + strPath);
		}
	} while (false);
}

AIRealPoint GraphicItem::GetDelta(AIRealRect oPlacedRect, AIRealRect oArtBoxRect)
{
	AIRealPoint oArtCenter, oPlacedCenter, oDelta;
	AIRealRect oNormalizedPlacedRect;
	oDelta.h = oDelta.v = 0;
	if (oPlacedRect.left != oArtBoxRect.left ||
		oPlacedRect.right != oArtBoxRect.right ||
		oPlacedRect.top != oArtBoxRect.top ||
		oPlacedRect.bottom != oArtBoxRect.bottom
		)
	{
		oArtCenter.h = ((oArtBoxRect.right - oArtBoxRect.left) / 2) + oArtBoxRect.left;
		oArtCenter.v = ((oArtBoxRect.bottom - oArtBoxRect.top) / 2) + oArtBoxRect.top;
		oNormalizedPlacedRect.left = oPlacedRect.right + oPlacedRect.left;
		oNormalizedPlacedRect.top = oPlacedRect.bottom + oPlacedRect.top;
		oNormalizedPlacedRect.right = oPlacedRect.right - oPlacedRect.left;
		oNormalizedPlacedRect.bottom = abs(oPlacedRect.bottom - oPlacedRect.top);
		oPlacedCenter.h = ((oNormalizedPlacedRect.right - oNormalizedPlacedRect.left) / 2) + oNormalizedPlacedRect.left;
		oPlacedCenter.v = ((oNormalizedPlacedRect.bottom - oNormalizedPlacedRect.top) / 2) + oNormalizedPlacedRect.top;
		oDelta.h = (oPlacedCenter.h - oArtCenter.h);
		oDelta.v = (oPlacedCenter.v - oArtCenter.v);
	}
	if (m_bIsNewImage)
	{
		oDelta.h -= ((oPlacedRect.right - oPlacedRect.left));
		oDelta.v = 0;
	}
	return oDelta;
}
/*
*/
ASErr GraphicItem::PlaceFile(const ai::UnicodeString& strName, AIArtHandle& oArtHandle, string strArtworkId, string &strErrorString)
{
	ASErr tResult = kNoErr;
	AIRealRect oImageRect = { m_dXpos, -m_dYpos, m_dXpos + m_dWidth, -(m_dYpos + m_dHeight)};
	MMtoPoints(oImageRect);
    AIRealPoint oImageCenter;
	oImageCenter.h = oImageRect.left + ((oImageRect.right - oImageRect.left) / 2);
	oImageCenter.v = oImageRect.top + ((oImageRect.bottom - oImageRect.top) / 2);
	do
	{
		try
		{
			AIArtHandle oGroup;
			AIArtHandle oClipping;
			if (!m_bIsNewImage)
				CreateClippingGroup(oGroup, oImageRect, oClipping);
			if (m_strType != kTransparency)
			{
				sAIArt->NewArt(kPlacedArt, kPlaceInsideOnTop, oGroup, &oArtHandle);
				// define the proper scale for the placed image
				AIRealMatrix oImageMatrix;
				oImageMatrix.Init();
				sAIRealMath->AIRealMatrixConcatScale(&oImageMatrix, m_dPicxscale, m_dPicyscale);
				sAIPlaced->SetPlacedMatrix(oArtHandle, &oImageMatrix);
				oArtHandle = Place(strName, oArtHandle, strArtworkId, strErrorString);
				// move it into the right position
				AIRealRect oPlacedRect;
				sAIArt->GetArtBounds(oArtHandle, &oPlacedRect);
				AIRealRect oArtBoxRect{ oPlacedRect.left, oPlacedRect.top, oPlacedRect.right, oPlacedRect.bottom };
				string strFilename = strName.as_Platform();
				int nDTIndex = strFilename.find("FERRERO_DT");
				if (nDTIndex >= 0)
					m_nPicRotateAngle = GetPDFAngle(strFilename, strArtworkId);
				AIRealPoint oDelta = GetDelta(oPlacedRect, oArtBoxRect);
				oImageCenter.h += (oDelta.h * cos(m_nPicRotateAngle*DTR)) - (oDelta.v * sin(m_nPicRotateAngle*DTR));
				oImageCenter.v += (oDelta.h * sin(m_nPicRotateAngle*DTR)) + (oDelta.v * cos(m_nPicRotateAngle*DTR));
				if (m_nPicShearAngle != 0.0)
				{
					oImageMatrix.Init();
					sAIRealMath->AIRealMatrixSetTranslate(&oImageMatrix, -oImageCenter.h, -oImageCenter.v);
					oImageMatrix.c = tan(kAIRealPi / 180 * m_nPicShearAngle);
					sAIRealMath->AIRealMatrixConcatTranslate(&oImageMatrix, oImageCenter.h, oImageCenter.v);
					sAITransformArt->TransformArt(oArtHandle, &oImageMatrix, 1, kTransformObjects);
				}
				oImageMatrix.Init();
				AIRealPoint oPlacedCenter;
				sAIRealMath->AIRealMatrixConcatTranslate(&oImageMatrix, oImageCenter.h, oImageCenter.v);
				sAITransformArt->TransformArt(oArtHandle, &oImageMatrix, 1, kTransformObjects);
				oPlacedCenter.h = (m_nPicXOffset * (PPI / MMPI));
				oPlacedCenter.v = (m_nPicYOffset * (PPI / MMPI));
				if (m_nPicRotateAngle != 0.0)
				{
					oImageMatrix.Init();
					sAIRealMath->AIRealMatrixSetTranslate(&oImageMatrix, -oImageCenter.h, -oImageCenter.v);
					sAIRealMath->AIRealMatrixConcatRotate(&oImageMatrix, (AIReal)(m_nPicRotateAngle * kAIRealPi / 180.0));
					sAIRealMath->AIRealMatrixConcatTranslate(&oImageMatrix, oImageCenter.h, oImageCenter.v);
					sAITransformArt->TransformArt(oArtHandle, &oImageMatrix, 1, kTransformObjects);
				}
				oImageMatrix.Init();
				sAIRealMath->AIRealMatrixConcatTranslate(&oImageMatrix, oPlacedCenter.h, -oPlacedCenter.v);
				sAITransformArt->TransformArt(oArtHandle, &oImageMatrix, 1, kTransformObjects);
			}
		}
		catch (ai::Error& tError)
		{
			tResult = tError;
		}
	} while (false);
	return tResult;
}

#ifdef _WINDOWS
	bool GraphicItem::CanCheckArtboxBound(string strArtworkId, string strName)
#else
	Boolean GraphicItem::CanCheckArtboxBound(string strArtworkId, string strName)
#endif
{
#ifdef _WINDOWS
	bool bCanCheck = false;
#else
	Boolean bCanCheck = false;
#endif
	do
	{
		int nIndex = strName.find("19911_22580");
		int nIndex1 = strName.find("15254_17919");
		int nIndex2 = strName.find("22492_25147");
		int nIndex3 = strName.find("26885_29511");
		int nIndex4 = strName.find("20381_23052");
		int nIndex5 = strName.find("26455_29081");
		int nIndex6 = strName.find("27703_30331");
		if (nIndex2 >= 0 || nIndex3 >= 0 || nIndex4 >=0 || nIndex5 >= 0 || nIndex6 > 0 || (nIndex1 > 0 && (strArtworkId == "147617563721712598" || strArtworkId == "149821164610548265")))
		{
			bCanCheck = true;
			break;
		}
		if (strArtworkId == "147608548235168921" || strArtworkId == "147608619937532932" 
			|| strArtworkId == "147617563721712598" || (strArtworkId == "156222832308712672" && abs(m_nPicRotateAngle) != 90)
			|| strArtworkId == "158100129501960987" || strArtworkId == "149821164610548265")
			break;
		if ((nIndex >= 0 || nIndex1 >= 0) && strArtworkId == "156222832308712672"&& m_dXpos < 100)
			break;
		if ((nIndex >= 0 || nIndex1 >= 0))
			bCanCheck = true;
	} while (false);
	return bCanCheck;
}

#ifdef _WINDOWS
	bool GraphicItem::IsAngleApplicable(string strArtworkId, string strName)
#else
	Boolean GraphicItem::IsAngleApplicable(string strArtworkId, string strName)
#endif
{
#ifdef _WINDOWS
	bool bIsApplicable = false;
#else
	Boolean bIsApplicable = false;
#endif
	do
	{
		if (strArtworkId == "157002118089987698")
		{
			bIsApplicable = abs(m_nPicRotateAngle) > 90;
		}
	} while (false);
	return bIsApplicable;
}

AIArtHandle GraphicItem::Place(const ai::UnicodeString &strName, AIArtHandle oBoxArt, string strArtworkId, string &strErrorString)
{
	ai::FilePath oPath = this->GetAssetPath(strName, strArtworkId);
	AIPlaceRequestData oPlaceReqData;
	oPlaceReqData.m_lPlaceMode = kForceReplace; // places the file and returns new art handle
	oPlaceReqData.m_disableTemplate = false; // disable Template placement
	oPlaceReqData.m_filemethod = 1; // place the file
	oPlaceReqData.m_hNewArt = NULL; // new art handle
	oPlaceReqData.m_hOldArt = oBoxArt; // art to be replaced
	oPlaceReqData.m_lParam = kPathArt; // type of art to create
	oPlaceReqData.m_pFilePath = NULL; // if NULL, shows File Place dialog
	oPlaceReqData.m_PlaceTransformType = kAIPlaceTransformTypeNone;
	string strPath = oPath.GetFullPath().as_Platform();
	if (oPath.Exists(false) && strName.size() != 0) {
		oPlaceReqData.m_pFilePath = &oPath; // place the given file (prompt for file to be placed otherwise)
	}

	// Place the file.
	ArtworkUtil oUtil;
	sAIPreferenceSuite->PutIntegerPreference(NULL, "plugin/PDFImport/CropTo", oUtil.GetCropTo(m_strCropTo));
	AIErr tResult = sAIPlaced->ExecPlaceRequest(oPlaceReqData);
	if (tResult != 0)
	{
		strErrorString += "\n Error placing file " + strName.as_Platform() + (tResult == 129 ? " Invalid file" : ", Error Code : " + to_string(tResult));
	}

	// Verify that the handle of the placed art is returned.
	SDK_ASSERT(oPlaceReqData.m_hNewArt);
	return oPlaceReqData.m_hNewArt;
}


/*
*/
ai::FilePath GraphicItem::GetAssetPath(const ai::UnicodeString& name, string strArtworkId)
{
	ai::FilePath oPath;
	ArtworkUtil oUtil;
	ai::UnicodeString abName(oUtil.GetUserFolder ().c_str());
	sAIFilePath->Set(abName, true, false, oPath);
	oPath.AddComponent(ai::FilePath(ai::UnicodeString (strArtworkId.c_str())));
	oPath.AddComponent(ai::FilePath(ai::UnicodeString(strArtworkId.c_str())));
	oPath.AddComponent(ai::FilePath(name));
	return oPath;
}

void GraphicItem::MMtoPoints(AIRealRect &oRect)
{
	oRect.left *= (PPI / MMPI);
	oRect.top *= (PPI / MMPI);
	oRect.bottom *= (PPI / MMPI);
	oRect.right *= (PPI / MMPI);
}

AIErr GraphicItem::CreateClippingGroup (AIArtHandle &oGroup, AIRealRect oImageRect, AIArtHandle &oClipping)
{
    AIErr tResult = kNoErr;
	ArtworkUtil oUtil;
	oGroup = oUtil.GetArtByClipGroupId(m_strClipGroupId);
// Commented code to handle template conversion issue 157556020669739736				
//	AIArtHandle oParentGroup = oUtil.GetArtByClipGroupId(m_strParentClipGroupId);
	if (oGroup == NULL)
	{
		ai::UnicodeString strArtName("ClipGroup_" + m_strClipGroupId);
//		sAIArt->NewArt(kGroupArt, kPlaceDefault, oParentGroup, &oGroup);
		sAIArt->NewArt(kGroupArt, kPlaceDefault, nullptr, &oGroup);
		sAIArt->SetArtName(oGroup, strArtName);
		tResult = sAIGroup->SetGroupClipped(oGroup, true);
		CreateClippingRect(oGroup, oImageRect, oClipping);
	}
	else if (m_strType == kTransparency)
		CreateClippingRect(oGroup, oImageRect, oClipping);
    return tResult;
}

AIErr GraphicItem::CreateClippingRect (AIArtHandle &oGroup, AIRealRect oImageRect, AIArtHandle &oClipping)
{
    AIErr tResult = kNoErr;
	Path oPath(m_strPath);
	if (m_strType != kTransparency)
		tResult = sAIArt->NewArt(kPathArt, kPlaceInsideOnBottom, oGroup, &oClipping);
	else
		tResult = sAIArt->NewArt(kPathArt, kPlaceDefault, NULL, &oClipping);

	if(m_strPath.length() > 0)
		oPath.CreatePath(oClipping);
	else
	{
		tResult = sAIPath->SetPathSegmentCount(oClipping, 4);
		AIPathSegment arrClipping[4];

		arrClipping[0].p.h = oImageRect.left;
		arrClipping[0].p.v = oImageRect.top;
		arrClipping[0].in = arrClipping[0].out = arrClipping[0].p;
		arrClipping[0].corner = true;

		arrClipping[1].p.h = oImageRect.right;
		arrClipping[1].p.v = oImageRect.top;
		arrClipping[1].in = arrClipping[1].out = arrClipping[1].p;
		arrClipping[1].corner = true;

		arrClipping[2].p.h = oImageRect.right;
		arrClipping[2].p.v = oImageRect.bottom;
		arrClipping[2].in = arrClipping[2].out = arrClipping[2].p;
		arrClipping[2].corner = true;

		arrClipping[3].p.h = oImageRect.left;
		arrClipping[3].p.v = oImageRect.bottom;
		arrClipping[3].in = arrClipping[3].out = arrClipping[3].p;
		arrClipping[3].corner = true;

		tResult = sAIPath->SetPathSegments(oClipping, 0, 4, arrClipping);
		tResult = sAIPath->SetPathClosed(oClipping, true);
	}
    // Create Mask using clipping
	if (m_strType != kTransparency)
	{
		AIMaskRef oClipRef;
		int nRefCount = 0;
		tResult = sAIMask->CreateMask(oClipping);
		tResult = sAIMask->GetMask(oClipping, &oClipRef);
		tResult = sAIMask->SetClipping(oClipRef, true);
		nRefCount = sAIMask->AddRef(oClipRef);
		tResult = sAIArt->SetArtUserAttr(oClipping, kArtIsClipMask, kArtIsClipMask);
		if(m_nOpacity > 0 && m_nOpacity < 100)
			tResult = sAIBlendStyle->SetOpacity(oGroup, m_nOpacity/100);
		if (m_strBlendMode == "MULTIPLY")
			tResult = sAIBlendStyle->SetBlendingMode(oGroup, kAIMultiplyBlendingMode);
	}
	else
	{
		AIPathStyle oPathStyle;
		oPathStyle.Init();
		oPathStyle.fillPaint = true;
		oPathStyle.fill.color.kind = kFourColor;
		AIReal nCyan = 0;
		AIReal nMagenta = 0;
		AIReal nYellow = 0;
		AIReal nBlack = 0;
		ExtractColorInfo(nCyan, nMagenta, nYellow, nBlack);
		oPathStyle.fill.color.c.f.cyan = nCyan/100.0;
		oPathStyle.fill.color.c.f.magenta = nMagenta/100.0;
		oPathStyle.fill.color.c.f.yellow = nYellow/100.0;
		oPathStyle.fill.color.c.f.black = nBlack/100.0;
		oPathStyle.stroke.width = 1;
		oPathStyle.stroke.dash.length = 0;
		tResult = sAIPathStyle->SetPathStyle(oClipping, &oPathStyle);
		tResult = sAIBlendStyle->SetOpacity(oClipping, m_nOpacity/100);
	}
    return tResult;
}

void GraphicItem::SetToDictionary()
{
	ArtworkUtil oUtil;
	string strKey = kGraphicKeyPrefix;
	strKey.append(GetGraphic_id());
	strKey.append("_");
	strKey.append(GetId());
	strKey.append("_");
	strKey.append(GetCountryId());
	string strValue = BuildGraphicValue();
	oUtil.AddToDocumentDictionary(strKey, strValue);

}

string GraphicItem::BuildGraphicValue()
{
	string strValue = "";
	strValue.append(m_strCountryid);
	strValue.append(",");
	strValue.append(m_strGraphic_id);
	strValue.append(",");
	strValue.append(m_strId);
	strValue.append(",");
	strValue.append(to_string(m_dHeight));
	strValue.append(",");
	strValue.append(to_string(m_dItemxscale));
	strValue.append(",");
	strValue.append(to_string(m_dItemyscale));
	strValue.append(",");
	strValue.append(to_string(m_dPicxscale));
	strValue.append(",");
	strValue.append(to_string(m_dPicyscale));
	strValue.append(",");
	strValue.append(m_strUid);
	strValue.append(",");
	strValue.append(to_string(m_dWidth));
	strValue.append(",");
	strValue.append(to_string(m_dXpos));
	strValue.append(",");
	strValue.append(to_string(m_dYpos));
	strValue.append(",");
	strValue.append(to_string(m_nPicXOffset));
	strValue.append(",");
	strValue.append(to_string(m_nPicYOffset));
	strValue.append(",");
	strValue.append(to_string(m_nPicRotateAngle));
	strValue.append(",");
	strValue.append(to_string(m_dPicHeight));
	strValue.append(",");
	strValue.append(to_string(m_dPicWidth));
	strValue.append(",");
	strValue.append(to_string(m_nPicShearAngle));
	strValue.append(",");
	strValue.append(to_string(m_dPicXpos));
	strValue.append(",");
	strValue.append(to_string(m_dPicYpos));
	strValue.append(",");
	strValue.append(m_strPath);
	return strValue;
}

GraphicItem::GraphicItem(string strGraphicDetails)
{
	char * poItem;
	int nCount = 0;
	ArtworkUtil oUtil;
	poItem = strtok((char*)strGraphicDetails.c_str(), ",");

	while (poItem != NULL)
	{
		string strItem = "";
		strItem.append(poItem);
		switch (nCount)
		{
		case 0:
			m_strCountryid = strItem;
			break;
		case 1:
			m_strGraphic_id = strItem;
			break;
		case 2:
			m_strId = strItem;
			break;
		case 3:
			m_dHeight = oUtil.GetRealValue(strItem);
			break;
		case 4:
			m_dItemxscale = oUtil.GetRealValue(strItem);
			break;
		case 5:
			m_dItemyscale = oUtil.GetRealValue(strItem);
			break;
		case 6:
			m_dPicxscale = oUtil.GetRealValue(strItem);
			break;
		case 7:
			m_dPicyscale = oUtil.GetRealValue(strItem);
			break;
		case 8:
			m_strUid = strItem;
			break;
		case 9:
			m_dWidth = oUtil.GetRealValue(strItem);
			break;
		case 10:
			m_dXpos = oUtil.GetRealValue(strItem);
			break;
		case 11:
			m_dYpos = oUtil.GetRealValue(strItem);
			break;
		case 12:
			m_nPicXOffset = oUtil.GetRealValue(strItem);
			break;
		case 13:
			m_nPicYOffset = oUtil.GetRealValue(strItem);
			break;
		case 14:
			m_nPicRotateAngle = oUtil.GetRealValue(strItem);
			break;
		case 15:
			m_dPicHeight = oUtil.GetRealValue(strItem);
			break;
		case 16:
			m_dPicWidth = oUtil.GetRealValue(strItem);
			break;
		case 17:
			m_nPicShearAngle = oUtil.GetRealValue(strItem);
			break;
		case 18:
			m_dPicXpos = oUtil.GetRealValue(strItem);
			break;
		case 19:
			m_dPicXpos = oUtil.GetRealValue(strItem);
			break;
		case 20:
			m_strPath = strItem;
			break;
		}
		nCount++;
		poItem = strtok(NULL, ",");
	}
}

AIRealRect GraphicItem::GetDifference(AIArtHandle oArtHandle)
{
	AIRealRect oDifference = { 0.0, 0.0, 100.0, 100.0 };
	AIArtHandle oClipHandle = GetClippingHandle(oArtHandle);
	// get the clipping art handle fro oArtHandle

	if (oClipHandle != NULL)
	{
		AIRealRect oArtBounds;

		oDifference.left = m_dXpos;
		oDifference.top = -m_dYpos;
		oDifference.right = m_dXpos + m_dWidth;
		oDifference.bottom = -m_dYpos + m_dHeight;
		MMtoPoints(oDifference);

		sAIArt->GetArtBounds(oClipHandle, &oArtBounds);
		AIReal nWidth = abs(oArtBounds.right - oArtBounds.left);
		AIReal nHeight = abs(oArtBounds.bottom - oArtBounds.top);
		AIReal nOriginalWidth = abs(oDifference.right - oDifference.left);
		AIReal nOriginalHeight = abs(oDifference.bottom - oDifference.top);
		int nWidthDiff = abs(nWidth - nOriginalWidth);
		int nHeightDiff = abs(nHeight - nOriginalHeight);
		oDifference.right = nWidthDiff <= 1 ? 100 : round((nWidth / nOriginalWidth) * 100.0);
		oDifference.bottom = nHeightDiff <= 1 ? 100 : round((nHeight / nOriginalHeight) * 100.0);
		oDifference.left = int(oArtBounds.left - oDifference.left);
		oDifference.top = int(oArtBounds.top - oDifference.top);
	}
	return oDifference;
}

AIArtHandle GraphicItem::GetClippingHandle(AIArtHandle oArtHandle)
{
	AIArtHandle oClipHandle;
	sAIArt->GetArtParent (oArtHandle, &oClipHandle);
	sAIArt->GetArtFirstChild(oClipHandle, &oClipHandle);
	while (oClipHandle != NULL && !(sAIArt->IsArtClipping(oClipHandle)))
		sAIArt->GetArtSibling(oClipHandle, &oClipHandle);
	return oClipHandle;
}

void GraphicItem::SetLayerName(string strLayerName)
{
	m_strLayerName = strLayerName;
}

string GraphicItem::GetLayerName()
{
	return m_strLayerName;
}

void GraphicItem::SetPath(string strPath)
{
	m_strPath = strPath;
}

string GraphicItem::GetPath()
{
	return m_strPath;
}

void GraphicItem::SetType(string strType)
{
	m_strType = strType;
}

string GraphicItem::GetType()
{
	return m_strType;
}

AIReal GraphicItem::GetOpacity()
{
	return m_nOpacity;
}
void GraphicItem::SetOpacity(AIReal dOpacity)
{
	m_nOpacity = dOpacity;
}

void GraphicItem::SetBlendMode(string strBlendMode)
{
	m_strBlendMode = strBlendMode;
}

string GraphicItem::GetBlendMode()
{
	return m_strBlendMode;
}

void GraphicItem::GetArtBoxBound(AIRealRect &oBoundRect, string strArtworkId, string strName)
{
	ArtworkUtil oUtil;
	do
	{
		int nIndex = strName.find(".ai");
		if (nIndex > 0)
		{
			ai::FilePath oPath = this->GetAssetPath(ai::UnicodeString(strName.c_str()), strArtworkId);
			string strPath = oPath.GetFullPath().as_Platform();
			string strContent = oUtil.GetFileContent(strPath);
			string strArtbox = kArtboxString;
			int nStart = strContent.find(strArtbox);
			if (nStart < 0)
				break;
			strArtbox = strContent.substr(nStart + strArtbox.length());
			int nEnd = strArtbox.find_first_of("]");
			if (nEnd < 0)
				break;
			strArtbox = strArtbox.substr(0, nEnd);
			std::vector<string> arrComponents = oUtil.Split(strArtbox, " ");
			if (arrComponents.size() < 4)
				break;
			AIRealRect oArtboxRect{ oUtil.GetRealValue(arrComponents.at(0)) , oUtil.GetRealValue(arrComponents.at(1)), oUtil.GetRealValue(arrComponents.at(2)), oUtil.GetRealValue(arrComponents.at(3)) };
			GetScaledRect(oArtboxRect);
			oBoundRect = oArtboxRect;
		}
	} while (false);
}

void GraphicItem::GetScaledRect(AIRealRect &oArtboxRect)
{
	oArtboxRect.left = oArtboxRect.left*m_dPicxscale;
	oArtboxRect.right = oArtboxRect.right*m_dPicxscale;
	oArtboxRect.top = oArtboxRect.top*m_dPicyscale;
	oArtboxRect.bottom = oArtboxRect.bottom*m_dPicyscale;
}

AIReal GraphicItem::GetPDFAngle(string strFilename, string strArtworkId)
{
	AIReal nAngle = m_nPicRotateAngle;
	if (strArtworkId == "158074386388370041")
		nAngle = -270.0;
	return nAngle;
}

void GraphicItem::ExtractColorInfo(AIReal& nCyan, AIReal& nMagenta, AIReal& nYellow, AIReal& nBlack)
{
	do
	{
		ArtworkUtil oUtil;
		string strColor = m_strFillColor;
		if (strColor.length() > 0)
		{
			int nStart = strColor.find("[");
			if (nStart < 0)
				break;
			int nEnd = strColor.find("]");
			strColor = strColor.substr(nStart + 1 , nEnd - nStart - 1);
			std::vector<string> arrComponents = oUtil.Split(strColor, ",");
			if (arrComponents.size() > 0)
				nCyan = oUtil.GetRealValue(arrComponents[0]);
			if (arrComponents.size() > 1)
				nMagenta = oUtil.GetRealValue(arrComponents[1]);
			if (arrComponents.size() > 2)
				nYellow = oUtil.GetRealValue(arrComponents[2]);
			if (arrComponents.size() > 3)
				nBlack = oUtil.GetRealValue(arrComponents[3]);
		}
	} while (false);
} 

void GraphicItem::GetHiResBoxBound(AIRealRect &oBoundRect, string strArtworkId, string strName)
{
	ArtworkUtil oUtil;
	do
	{
		int nIndex = strName.find(".ai");
		if (nIndex > 0)
		{
			ai::FilePath oPath = this->GetAssetPath(ai::UnicodeString(strName.c_str()), strArtworkId);
			string strPath = oPath.GetFullPath().as_Platform();
			string strContent = oUtil.GetFileContent(strPath);
			string strHiresBoundingbox = kHiresBboxString;
			int nStart = strContent.find(strHiresBoundingbox);
			if (nStart < 0)
				break;
			strHiresBoundingbox = strContent.substr(nStart + strHiresBoundingbox.length());
			int nEnd = strHiresBoundingbox.find_first_of("\r");
			if (nEnd < 0)
				break;
			strHiresBoundingbox = strHiresBoundingbox.substr(0, nEnd);
			std::vector<string> arrComponents = oUtil.Split(strHiresBoundingbox, " ");
			if (arrComponents.size() < 4)
				break;
			AIRealRect oArtboxRect{ (oUtil.GetRealValue(arrComponents.at(0)) / 72) * 25.4, 
				(oUtil.GetRealValue(arrComponents.at(1)) / 72) * 25.4, 
				(oUtil.GetRealValue(arrComponents.at(2)) / 72) * 25.4, 
				(oUtil.GetRealValue(arrComponents.at(3)) / 72) * 25.4 };
			GetScaledRect(oArtboxRect);
			oArtboxRect.left += oBoundRect.left;
			oArtboxRect.right += oBoundRect.left;
			oArtboxRect.top = -(oArtboxRect.top + oBoundRect.bottom);
			oArtboxRect.bottom = -(oArtboxRect.bottom + oBoundRect.bottom);
			oBoundRect = oArtboxRect;
		}
	} while (false);
}

string GraphicItem::GetEffects()
{
	string strEffects = "";
	if (m_strEffectsApplied.length() > 0)
		strEffects = kFerreroPrefix + m_strGraphic_id + "_" + m_strId + "-"+ m_strCountryid + ": "+ m_strEffectsApplied;
	return strEffects;
}
