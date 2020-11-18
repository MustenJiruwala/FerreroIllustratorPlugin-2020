#ifndef _TemplateData_
#define _TemplateData_
#include "IllustratorSDK.h"
#include "xercesc/util/PlatformUtils.hpp"
#include "xercesc/dom/DOM.hpp"
#include "xercesc/parsers/XercesDOMParser.hpp"
#include "xercesc/sax/HandlerBase.hpp"
#include "xercesc/util/XMLString.hpp"
#include "xercesc/framework/MemBufInputSource.hpp"
#include "xercesc/framework/Wrapper4InputSource.hpp"
using namespace xercesc;
class TemplateData
{
public:
	/**	Constructor.
	*/
	TemplateData();
	TemplateData(string strContent);
	/**	Destructor.
	*/
	virtual ~TemplateData() {}
	virtual void SetProjectID(string strProjectId);
	virtual string GetProjectID();
private:
	string m_strProjectID;
	string m_strProjectDescription;
	string m_strProjectNote;
	string m_strAssignedTo;
	string m_strPresidenza_id;
	string m_strBrand_Class_id;
	string m_strBrand_Sotto_Class_id;
	string m_strBrand_Detail_id;
	string m_strCondition_id;
	string m_strPackaging_id;
	string m_strFormat_id;
	string m_strObjtype_id;
	string m_strTaste_id;
	string m_strCountryId;
	string m_strVariant_id;
	string m_strVariant_Detail_id;
	string m_strCanvass_id;
	string m_strTechnicalDesign_id;
	string m_strCopackId;
public:
	string CreateDBTemplate();
private:
	void ProcessChildElements(DOMNode *poNode);
	string GetElementNodeValue(DOMNode* poChildNode);
};
#endif