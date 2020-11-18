#include "IllustratorSDK.h"
#include "Rule.h"
#include "ArtworkUtil.h"

Rule::Rule()
{

}

Rule::Rule(DOMNode *poNode)
{
	string strTagname = XMLString::transcode(poNode->getNodeName());
	DOMNamedNodeMap * poNodeNameMap = poNode->getAttributes();
	ArtworkUtil oUtil;
	if (poNodeNameMap != nullptr)
	{
		std::string::size_type tSize;
		m_nRuleId = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "id"));
		m_nValue1 = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "value1"));
		m_nValue2 = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "value2"));
		m_nValue3 = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "value3"));
	}
	DOMNodeList * poRuleList = poNode->getChildNodes();
	for (int nIndex = 0; nIndex < poRuleList->getLength(); nIndex++)
	{
		DOMNode *poChildNode = poRuleList->item(nIndex);
		string strTagname = XMLString::transcode(poChildNode->getNodeName());
		if (strTagname == "InDesign_method")
			SetMethodName(poChildNode);
		else if (strTagname == "rule_instance")
			SetRuleInstanceInfo(poChildNode);
	}
}

void Rule::SetMethodName(DOMNode *poChildNode)
{
	DOMNamedNodeMap * poNodeNameMap = poChildNode->getAttributes();
	ArtworkUtil oUtil;
	if (poNodeNameMap != nullptr)
	{
		m_strMethod = oUtil.GetAttributeValue(poNodeNameMap, "name");
	}
}

void Rule::SetRuleInstanceInfo(DOMNode *poChildNode)
{
	DOMNamedNodeMap * poNodeNameMap = poChildNode->getAttributes();
	ArtworkUtil oUtil;
	if (poNodeNameMap != nullptr)
	{
		std::string::size_type tSize;
		m_strInstanceId = oUtil.GetAttributeValue(poNodeNameMap, "instance");
		m_nInstanceValue1 = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "value1"));
		m_nInstanceValue2 = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "value2"));
		m_nInstanceValue3 = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "value3"));
		m_strGraphicLinkId = oUtil.GetAttributeValue(poNodeNameMap, "grph_obj_lnkd_id");
		m_strLinkedCountryId = oUtil.GetAttributeValue(poNodeNameMap, "country_id_lnkd");
		m_nItemXscale = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "ItemXScale"));
		m_nItemYscale = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "ItemYScale"));
		m_nGraphicXscale = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "GraphicXScale"));
		m_nGraphicYscale = oUtil.GetRealValue(oUtil.GetAttributeValue(poNodeNameMap, "GraphicYScale"));
	}
}

void Rule::SetRuleId(int nRuleId)
{
	m_nRuleId = nRuleId;
}

int Rule::GetRuleId()
{
	return m_nRuleId;
}

void Rule::SetValue1(AIReal nValue)
{
	m_nValue1 = nValue;
}

AIReal Rule::GetValue1()
{
	return m_nValue1;
}

void Rule::SetValue2(AIReal nValue)
{
	m_nValue2 = nValue;
}

AIReal Rule::GetValue2()
{
	return m_nValue2;
}

void Rule::SetValue3(AIReal nValue)
{
	m_nValue3 = nValue;
}
AIReal Rule::GetValue3()
{
	return m_nValue3;
}
void Rule::SetMethodName(string strMethod)
{
	m_strMethod = strMethod;
}
string Rule::GetMethodName()
{
	return m_strMethod;
}
void Rule::SetInstanceId(string strInstanceId)
{
	m_strInstanceId = strInstanceId;
}
string Rule::GetInstanceId()
{
	return m_strInstanceId;
}
void Rule::SetInstanceValue1(AIReal nValue)
{
	m_nInstanceValue1 = nValue;
}
AIReal Rule::GetInstanceValue1()
{
	return m_nInstanceValue1;
}
void Rule::SetInstanceValue2(AIReal nValue)
{
	m_nInstanceValue2 = nValue;
}
AIReal Rule::GetInstanceValue2()
{
	return m_nInstanceValue2;
}
void Rule::SetInstanceValue3(AIReal nValue)
{
	m_nInstanceValue3 = nValue;
}
AIReal Rule::GetInstanceValue3()
{
	return m_nInstanceValue3;
}
void Rule::SetGraphicLinkId(string strGraphicLinkId)
{
	m_strGraphicLinkId = strGraphicLinkId;
}
string Rule::GetGraphicLinkId()
{
	return m_strGraphicLinkId;
}
void Rule::SetLinkedCountryId(string strLinkedCountryId)
{
	m_strLinkedCountryId = strLinkedCountryId;
}
string Rule::GetLinkedCountryId()
{
	return m_strLinkedCountryId;
}
void Rule::SetItemXscale(AIReal nItemXscale)
{
	m_nItemXscale = nItemXscale;
}
AIReal Rule::GetItemXscale()
{
	return m_nItemXscale;
}
void Rule::SetItemYscale(AIReal nItemYscale)
{
	m_nItemYscale = nItemYscale;
}
AIReal Rule::GetItemYscale()
{
	return m_nItemYscale;
}
void Rule::SetGraphicXscale(AIReal nGraphicXscale)
{
	m_nGraphicXscale = nGraphicXscale;
}
AIReal Rule::GetGraphicXscale()
{
	return m_nGraphicXscale;
}
void Rule::SetGraphicYscale(AIReal nGraphicYscale)
{
	m_nGraphicYscale = nGraphicYscale;
}
AIReal Rule::GetGraphicYscale()
{
	return m_nGraphicYscale;
}

string Rule::BuildRuleValue()
{
	string strRule = "";
	strRule.append(to_string(m_nRuleId));
	strRule.append(",");
	strRule.append(to_string(m_nValue1));
	strRule.append(",");
	strRule.append(to_string(m_nValue2));
	strRule.append(",");
	strRule.append(to_string(m_nValue3));
	strRule.append(",");
	strRule.append(m_strMethod);
	strRule.append(",");
	strRule.append(m_strInstanceId);
	strRule.append(",");
	strRule.append(to_string(m_nInstanceValue1));
	strRule.append(",");
	strRule.append(to_string(m_nInstanceValue2));
	strRule.append(",");
	strRule.append(to_string(m_nInstanceValue3));
	strRule.append(",");
	strRule.append(m_strGraphicLinkId);
	strRule.append(",");
	strRule.append(m_strLinkedCountryId);
	strRule.append(",");
	strRule.append(to_string(m_nItemXscale));
	strRule.append(",");
	strRule.append(to_string(m_nItemYscale));
	strRule.append(",");
	strRule.append(to_string(m_nGraphicXscale));
	strRule.append(",");
	strRule.append(to_string(m_nGraphicYscale));
	return strRule;
}

Rule::Rule(string strRule)
{
	char * poLayer;
	int nCount = 0;
	ArtworkUtil oUtil;
	poLayer = strtok((char*)strRule.c_str(), ",");

	while (poLayer != NULL)
	{
		string strRule = "";
		strRule.append(poLayer);
		switch (nCount)
		{
		case 0:
			m_nRuleId = oUtil.GetRealValue(strRule);
			break;
		case 1:
			m_nValue1 = oUtil.GetRealValue(strRule);
			break;
		case 2:
			m_nValue2 = oUtil.GetRealValue(strRule);
			break;
		case 3:
			m_nValue3 = oUtil.GetRealValue(strRule);
			break;
		case 4:
			m_strMethod = strRule;
			break;
		case 5:
			m_strInstanceId = strRule;
			break;
		case 6:
			m_nInstanceValue1 = oUtil.GetRealValue(strRule);
			break;
		case 7:
			m_nInstanceValue2 = oUtil.GetRealValue(strRule);
			break;
		case 8:
			m_nInstanceValue3 = oUtil.GetRealValue(strRule);
			break;
		case 9:
			m_strGraphicLinkId = strRule;
			break;
		case 10:
			m_strLinkedCountryId = strRule;
			break;
		case 11:
			m_nItemXscale = oUtil.GetRealValue(strRule);
			break;
		case 12:
			m_nItemYscale = oUtil.GetRealValue(strRule);
			break;
		case 13:
			m_nGraphicXscale = oUtil.GetRealValue(strRule);
			break;
		case 14:
			m_nGraphicYscale = oUtil.GetRealValue(strRule);
			break;
		}
		nCount++;
		poLayer = strtok(NULL, ",");
	}
}

Rule::RuleMask Rule::VerifyRule(AIArtHandle oHandle, GraphicItem oGraphicItem)
{
	RuleMask tResult = kNoRuleVerified;
	do
	{
		AIRealRect oDifference = oGraphicItem.GetDifference(oHandle);
		if (m_strMethod == kRuleIsMovable)
		{
			tResult = kMoveVerified;
		}
		else if (m_strMethod == kRuleIsResizable)
		{
			tResult = VerifyResize(oDifference);
		}
	} while (false);
	return tResult;
}

Rule::RuleMask Rule::VerifyResize(AIRealRect oDifference)
{
	RuleMask tResult = kNoRuleVerified;
	if (oDifference.right >= m_nValue1 && oDifference.right <= m_nValue2 &&
		oDifference.bottom >= m_nValue1 && oDifference.bottom <= m_nValue2)
		tResult = kResizeVerified;
	return tResult;
}
