#ifndef _GraphicItem_
#define _GraphicItem_

#include "IllustratorSDK.h"
#include "xercesc/dom/DOM.hpp"
#include "SDKErrors.h"
#include "AIPerspectiveTransform.h"
#include "AITransformArt.h"
#include "AIMask.h"
#include "AIPath.h"
#include "AIGroup.h"
#include "AIUUID.h"
#include "AIPDFFormatAction.h"

using namespace xercesc;
using namespace std; 
extern "C" AIPlacedSuite*	sAIPlaced;
extern "C" AIFilePathSuite*	sAIFilePath;
extern "C" AIArtSuite* sAIArt;
extern "C" AIRealMathSuite* sAIRealMath;
extern "C" AITransformArtSuite* sAITransformArt;
extern "C" AIPerspectiveTransformSuite* sAIPerspectiveTransform;
extern "C" AIMaskSuite *sAIMask;
extern "C" AIPathSuite *sAIPath;
extern "C" AIGroupSuite *sAIGroup;
extern "C" AIUUIDSuite *sAIUUID;
extern "C" AIBlendStyleSuite* sAIBlendStyle;
extern "C" AIPathStyleSuite*  sAIPathStyle;
extern "C" AIPreferenceSuite* sAIPreferenceSuite;


class GraphicItem
{
public:
	/**	Constructor.
	*/
	GraphicItem();
	GraphicItem(DOMNode *poNode);
	GraphicItem(string strGraphicDetails);

	/**	Destructor.
	*/
	virtual ~GraphicItem() {}
	virtual string GetCountryId();
	virtual void SetCountryId(string strCountryId);
	virtual string GetGraphic_id();
	virtual void SetGraphic_id(string strGraphic_id);
	virtual AIReal GetHeight();
	virtual void SetHeight(AIReal dHeight);
	virtual string GetId();
	virtual void SetId(string strId);
	virtual AIReal GetItemxscale();
	virtual void SetItemxscale(AIReal dItemxscale);
	virtual AIReal GetItemyscale();
	virtual void SetItemyscale(AIReal dItemyscale);
	virtual AIReal GetPicxscale();
	virtual void SetPicxscale(AIReal dPicxscale);
	virtual AIReal GetPicyscale();
	virtual void SetPicyscale(AIReal dPicyscale);
	virtual string GetUid();
	virtual void SetUid(string strUid);
	virtual AIReal GetWidth();
	virtual void SetWidth(AIReal dWidth);
	virtual AIReal GetXpos();
	virtual void SetXpos(AIReal dXpos);
	virtual AIReal GetYpos();
	virtual void SetYpos(AIReal dYpos);
	virtual string GetObjectKey();
	virtual void SetFilePath(string strFileName);
	virtual string GetFilePath();
	virtual void SetFileName(string strFileName);
	virtual string GetFileName();
	virtual void SetLayerName(string strLayerName);
	virtual string GetLayerName();
	virtual void SetPath(string strPath);
	virtual string GetPath();
	virtual void SetType(string strType);
	virtual	string GetType();
	void PlaceItem(AIDocumentHandle &oDocument, string strArtworkId, string &strErrorString);
	void SetToDictionary();
	AIRealRect GetDifference(AIArtHandle oArtHandle);
	AIArtHandle GetClippingHandle(AIArtHandle oArtHandle);
	virtual AIReal GetOpacity();
	virtual void SetOpacity(AIReal dOpacity);
	virtual void SetBlendMode(string strBlendMode);
	virtual	string GetBlendMode();
	virtual AIBoolean IsNewImage();
	virtual string GetImageName();
	string GetEffects();
	string Stringify();
	void JSONParse(string strGraphicItem);
private:
	std::string m_strCountryid;
	std::string m_strGraphic_id;
	std::string m_strFilePath;
	std::string m_strFilename;
	std::string m_strLayerName;
	AIReal m_dHeight;
	std::string m_strId;
	AIReal m_dItemxscale;
	AIReal m_dItemyscale;
	AIReal m_dPicxscale;
	AIReal m_dPicyscale;
	std::string m_strUid;
	AIReal m_dWidth;
	AIReal m_dXpos;
	AIReal m_dYpos;
	AIReal m_nPicXOffset;
	AIReal m_nPicYOffset;
	AIReal m_nPicRotateAngle;
	AIReal m_nPicShearAngle;
	AIReal m_dPicHeight;
	AIReal m_dPicWidth;
	std::string m_strPath;
	std::string m_strType;
	AIReal m_nOpacity;
	std::string m_strBlendMode;
	std::string m_strFillColor;
	std::string m_strEffectsApplied;
	std::string m_strClipGroupId;
	std::string m_strCropTo;
	std::string m_strParentClipGroupId;
	AIBoolean m_bIsNewImage;
	std::string m_strImageName;
	AIReal m_dPicXpos;
	AIReal m_dPicYpos;
private:
	ASErr PlaceFile(const ai::UnicodeString& name, AIArtHandle& art, string strArtworkId, string &strErrorString);
	AIArtHandle Place(const ai::UnicodeString &strName, AIArtHandle oBoxArt, string strArtworkId, string &strErrorString);
	ai::FilePath GetAssetPath(const ai::UnicodeString& name, string strArtworkId);
	void MMtoPoints(AIRealRect &oRect);
    AIErr CreateClippingGroup (AIArtHandle &oGroup, AIRealRect oImageRect, AIArtHandle &oClipping);
    AIErr CreateClippingRect (AIArtHandle &oGroup, AIRealRect oImageRect, AIArtHandle &oClipping);
	string BuildGraphicValue();
	string ExtractCountryID(string strLayerName);
	string ExtractGraphicID(string strLinkName);
	string ExtractInstanceID(string strLinkName);
	void GetArtBoxBound(AIRealRect &oPlacedRect, string strArtworkId, string strName);
	void GetScaledRect(AIRealRect &oArtboxRect);
#ifdef _WINDOWS
	bool CanCheckArtboxBound(string strArtworkId, string strName);
	bool IsAngleApplicable(string strArtworkId, string strName);
#else
	Boolean CanCheckArtboxBound(string strArtworkId, string strName);
	Boolean IsAngleApplicable(string strArtworkId, string strName);
#endif
	AIReal GetPDFAngle(string strFilename, string strArtworkId);
	void ExtractColorInfo(AIReal& nCyan, AIReal& nMagenta, AIReal& nYellow, AIReal& nBlack);
	void GetHiResBoxBound(AIRealRect &oBoundRect, string strArtworkId, string strName);
	AIRealPoint GetDelta(AIRealRect oPlacedRect, AIRealRect oArtBoxRect);
};
#endif //_GraphicItem_
