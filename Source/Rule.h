#ifndef _Rule_
#define _Rule_
#include "IllustratorSDK.h"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/sax/HandlerBase.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"
#include "GraphicItem.h"
using namespace xercesc;
class Rule
{
public:
	enum RuleMask
	{
		kNoRuleVerified = 0,
		kMoveVerified = 1,
		kResizeVerified = 2
	};
	/**	Constructor.
	*/
	Rule();
	Rule(DOMNode *poNode);
	Rule(string strRule);
	/**	Destructor.
	*/
	virtual ~Rule() {}
	virtual void SetRuleId(int nRuleId);
	virtual int GetRuleId();
	virtual void SetValue1(AIReal nValue);
	virtual AIReal GetValue1();
	virtual void SetValue2(AIReal nValue);
	virtual AIReal GetValue2();
	virtual void SetValue3(AIReal nValue);
	virtual AIReal GetValue3();
	virtual void SetMethodName(string strMethod);
	virtual string GetMethodName();
	virtual void SetInstanceId(string strInstanceId);
	virtual string GetInstanceId();
	virtual void SetInstanceValue1(AIReal nValue);
	virtual AIReal GetInstanceValue1();
	virtual void SetInstanceValue2(AIReal nValue);
	virtual AIReal GetInstanceValue2();
	virtual void SetInstanceValue3(AIReal nValue);
	virtual AIReal GetInstanceValue3();
	virtual void SetGraphicLinkId(string strGraphicLinkId);
	virtual string GetGraphicLinkId();
	virtual void SetLinkedCountryId(string strLinkedCountryId);
	virtual string GetLinkedCountryId();
	virtual void SetItemXscale(AIReal nItemXscale);
	virtual AIReal GetItemXscale();
	virtual void SetItemYscale(AIReal nItemYscale);
	virtual AIReal GetItemYscale();
	virtual void SetGraphicXscale(AIReal nGraphicXscale);
	virtual AIReal GetGraphicXscale();
	virtual void SetGraphicYscale(AIReal nGraphicYscale);
	virtual AIReal GetGraphicYscale();
	virtual string BuildRuleValue();
	RuleMask VerifyRule(AIArtHandle oHandle, GraphicItem oGraphicItem);
private:
	int m_nRuleId;
	AIReal m_nValue1;
	AIReal m_nValue2;
	AIReal m_nValue3;
	string m_strMethod;
	string m_strInstanceId;
	AIReal m_nInstanceValue1;
	AIReal m_nInstanceValue2;
	AIReal m_nInstanceValue3;
	string m_strGraphicLinkId;
	string m_strLinkedCountryId;
	AIReal m_nItemXscale;
	AIReal m_nItemYscale;
	AIReal m_nGraphicXscale;
	AIReal m_nGraphicYscale;
private:
	void SetMethodName(DOMNode *poChildNode);
	void SetRuleInstanceInfo(DOMNode *poChildNode);
	RuleMask VerifyResize(AIRealRect oDifference);
};
#endif // !_Rule_
