#ifndef _RuleData_
#define _RuleData_
#include "IllustratorSDK.h"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/sax/HandlerBase.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"
using namespace xercesc;
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include "Rule.h"
#include "GraphicItem.h"
using namespace std;

class RuleData
{
public:
	/**	Constructor.
	*/
	RuleData();
	RuleData(DOMNode *poNode);
	RuleData(string strRuleValue);

	/**	Destructor.
	*/
	virtual ~RuleData() {}
	virtual string GetCountryId();
	virtual void SetCountryId(string strCountryId);
	virtual string GetGraphic_id();
	virtual void SetGraphic_id(string strGraphic_id);
	virtual string GetId();
	virtual void SetId(string strId);
	virtual std::vector<Rule> GetRules(string strKey);
	virtual void SetToDictionary();
	virtual ASErr VerifyRules(AIArtHandle oHandle, GraphicItem oGraphicItem);
private:
	string m_strCountryId;
	string m_strGraphicId;
	string m_strId;
	std::vector<Rule> m_arrRules;
	int	m_nVerifiedRules;
private:
	string BuildRuleValue();
	bool Moved(AIArtHandle oArtHandle, GraphicItem oGraphicItem);
	bool Resized(AIArtHandle oArtHandle, GraphicItem oGraphicItem);
};
#endif // _RuleData_