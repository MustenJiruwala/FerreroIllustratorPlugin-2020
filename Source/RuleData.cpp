#include "IllustratorSDK.h"
#include "RuleData.h"
#include "ArtworkUtil.h"
extern "C" AIUserSuite*		sAIUserSuite;

RuleData::RuleData()
{

}

RuleData::RuleData(string strRuleValue)
{
	char * poLayer;
	poLayer = strtok((char*)strRuleValue.c_str(), ";");
	while (poLayer != NULL)
	{
		string strRule = "";
		strRule.append(poLayer);
		poLayer = strtok(NULL, ";");
		Rule oRule(strRule);
		m_arrRules.push_back(oRule);
	}
}

RuleData::RuleData(DOMNode *poNode)
{
	string strTagname = XMLString::transcode(poNode->getNodeName());
	DOMNamedNodeMap * poNodeNameMap = poNode->getAttributes();
	ArtworkUtil oUtil;
	if (poNodeNameMap != nullptr)
	{
		m_strCountryId = oUtil.GetAttributeValue(poNodeNameMap, "country_id");
		if (m_strCountryId == "00")
			m_strCountryId = "shared";
		m_strGraphicId = oUtil.GetAttributeValue(poNodeNameMap, "id");
		m_strId = oUtil.GetAttributeValue(poNodeNameMap, "instance");
	}
	DOMNodeList * poRuleList = poNode->getChildNodes();
	for (int nIndex = 0; nIndex < poRuleList->getLength(); nIndex++)
		m_arrRules.insert(m_arrRules.begin(), *(new Rule(poRuleList->item(nIndex))));
	//parse the values
	// create Dictionary Key - FERRERO_{graphic_id}_{instance_id}_{country_id}
	// Set the values - ruleName,value1,value2,value3,itemx,itemy,picx,picy;...
}

void RuleData::SetToDictionary()
{
	ArtworkUtil oUtil;
	string strKey = kFerreroPrefix;
	strKey.append(GetGraphic_id());
	strKey.append("_");
	strKey.append(GetId());
	strKey.append("_");
	strKey.append(GetCountryId());
	string strValue = BuildRuleValue();
	oUtil.AddToDocumentDictionary(strKey, strValue);
}

string RuleData::GetCountryId()
{
	return m_strCountryId;
}

void RuleData::SetCountryId(string strCountryId)
{
	m_strCountryId = strCountryId;
}

string RuleData::GetGraphic_id()
{
	return m_strGraphicId;
}

void RuleData::SetGraphic_id(string strGraphic_id)
{
	m_strGraphicId = strGraphic_id;
}

string RuleData::GetId()
{
	return m_strId;
}

void RuleData::SetId(string strId)
{
	m_strId = strId;
}

std::vector<Rule> RuleData::GetRules(string strKey)
{
	return m_arrRules;
}

string RuleData::BuildRuleValue()
{
	string strRuleValue = "";
	for (int nIndex = 0; nIndex < m_arrRules.size(); nIndex++)
	{
		strRuleValue += strRuleValue.length() > 0 ? ";" : "";
		strRuleValue.append(m_arrRules[nIndex].BuildRuleValue());
	}
	return strRuleValue;
}

ASErr RuleData::VerifyRules(AIArtHandle oHandle, GraphicItem oGraphicItem)
{
	ASErr tResult = kNoErr;
	m_nVerifiedRules = 0;
	for (int nIndex = 0; nIndex < m_arrRules.size() && tResult == kNoErr; nIndex++)
		m_nVerifiedRules |= m_arrRules[nIndex].VerifyRule (oHandle, oGraphicItem);
	if (Moved(oHandle, oGraphicItem) && !(m_nVerifiedRules & Rule::kMoveVerified))
	{
		tResult = kCantHappenErr;
		ai::UnicodeString strMoveRuleFailedMessage ("Move rule failed");
		sAIUserSuite->MessageAlert(strMoveRuleFailedMessage);
	}
	if (Resized(oHandle, oGraphicItem) && !(m_nVerifiedRules & Rule::kResizeVerified))
	{
		tResult = kCantHappenErr;
		ai::UnicodeString strResizeRuleFailedMessage("Resize rule failed");
		sAIUserSuite->MessageAlert(strResizeRuleFailedMessage);
	}
	return tResult;
}

bool RuleData::Moved(AIArtHandle oArtHandle, GraphicItem oGraphicItem)
{
	AIRealRect oDifference = oGraphicItem.GetDifference(oArtHandle);
	return oDifference.left != 0 || oDifference.top != 0;
}

bool RuleData::Resized(AIArtHandle oArtHandle, GraphicItem oGraphicItem)
{
	AIRealRect oDifference = oGraphicItem.GetDifference(oArtHandle);
	return oDifference.right > 101.0 || oDifference.right < 99.0 || oDifference.bottom > 101.0 || oDifference.bottom < 99.0;
}