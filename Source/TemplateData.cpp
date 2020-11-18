#include "IllustratorSDK.h"
#include "TemplateData.h"
#include "ArtworkUtil.h"
#include "FerreroServerFacade.h"
#include "Logger.h"

TemplateData::TemplateData()
{

}

TemplateData::TemplateData(string strContent)
{
	do
	{
		XercesDOMParser* poParser = new XercesDOMParser();
		poParser->setValidationScheme(XercesDOMParser::Val_Always);
		poParser->setDoNamespaces(true);    // optional
		ErrorHandler* errHandler = (ErrorHandler*) new HandlerBase();
		poParser->setErrorHandler(errHandler);
		const char *poChars = strContent.c_str();
		XMLByte *pcResponse = (XMLByte *)(poChars);
		xercesc::MemBufInputSource oSource(pcResponse, strContent.length(), "Response XML");
		try
		{
			poParser->parse(oSource);
			xercesc::DOMDocument * poDoc = poParser->getDocument();
			if (poDoc == nullptr)
				break;
			DOMElement * poElement = poDoc->getDocumentElement();
			if (poElement == nullptr)
				break;
			const XMLCh *poImageTag = XMLString::transcode("sap:createtemplate");
			DOMNodeList * poImageList = poElement->getElementsByTagName(poImageTag);
			if (poImageList->getLength() > 0)
				ProcessChildElements(poImageList->item(0));
		}
		catch (...)
		{
			break;
		}
		delete poParser;
		delete errHandler;
	} while (false);
}

string TemplateData::CreateDBTemplate()
{
	Logger::Info("TemplateData", "CreateDBTemplate", "");
	string strGraphicXML = "";
	FerreroServerFacade oFerreroServerFacade;
	string strCreateTemplateParam = "function=createtemplate&projectid=";
	strCreateTemplateParam.append(m_strProjectID);
	strCreateTemplateParam.append("&projectdescription=");
	strCreateTemplateParam.append(oFerreroServerFacade.URLEncode(m_strProjectDescription));
	strCreateTemplateParam.append("&projectnote=");
	strCreateTemplateParam.append(oFerreroServerFacade.URLEncode(m_strProjectNote));
	strCreateTemplateParam.append("&assignedto=");
	strCreateTemplateParam.append(m_strAssignedTo);
	strCreateTemplateParam.append("&presidenzaid=");
	strCreateTemplateParam.append(m_strPresidenza_id);
	strCreateTemplateParam.append("&brandclassid=");
	strCreateTemplateParam.append(m_strBrand_Class_id);
	strCreateTemplateParam.append("&brandsottoclassid=");
	strCreateTemplateParam.append(m_strBrand_Sotto_Class_id);
	strCreateTemplateParam.append("&branddetailid=");
	strCreateTemplateParam.append(m_strBrand_Detail_id);
	strCreateTemplateParam.append("&conditionid=");
	strCreateTemplateParam.append(m_strCondition_id);
	strCreateTemplateParam.append("&packagingid=");
	strCreateTemplateParam.append(m_strPackaging_id);
	strCreateTemplateParam.append("&formatid=");
	strCreateTemplateParam.append(m_strFormat_id);
	strCreateTemplateParam.append("&objtypeid=");
	strCreateTemplateParam.append(m_strObjtype_id);
	strCreateTemplateParam.append("&tasteid=");
	strCreateTemplateParam.append(m_strTaste_id);
	strCreateTemplateParam.append("&countryid=");
	strCreateTemplateParam.append(m_strCountryId);
	strCreateTemplateParam.append("&variantid=");
	strCreateTemplateParam.append(m_strVariant_id);
	strCreateTemplateParam.append("&variantdetailid=");
	strCreateTemplateParam.append(m_strVariant_Detail_id);
	strCreateTemplateParam.append("&canvasid=");
	strCreateTemplateParam.append(m_strCanvass_id);
	strCreateTemplateParam.append("&techdesignid=");
	strCreateTemplateParam.append(m_strTechnicalDesign_id);
	strCreateTemplateParam.append("&copackid=");
	strCreateTemplateParam.append(m_strCopackId);
	Logger::Info("TemplateData", "strCreateTemplateParam :", strCreateTemplateParam);
	string strErrorString = "";
	strGraphicXML = oFerreroServerFacade.CallServlet(kFerreroWFESS2URL, "/SapRequestServlet", strCreateTemplateParam, strErrorString);
	Logger::Info("TemplateData", "strGraphicXML :", strGraphicXML);
	return strGraphicXML;
}

void TemplateData::ProcessChildElements(DOMNode *poNode)
{
	do
	{
		if (!poNode->hasChildNodes())
			break;
		DOMNodeList *poChildNodeList = poNode->getChildNodes();
		for (int nIndex = 0; nIndex < poChildNodeList->getLength(); nIndex++)
		{
			DOMNode *poChildNode = poChildNodeList->item(nIndex);
			string strNodename = XMLString::transcode(poChildNode->getNodeName());
			if (strNodename == "sap:project_description")
				m_strProjectDescription = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:assigned_to")
				m_strAssignedTo = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:campagna_id")
				m_strCanvass_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:flag_copack")
				m_strCopackId = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:project_note")
				m_strProjectNote = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:techdesign_id")
				m_strTechnicalDesign_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:categoria")
				m_strPresidenza_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:brand_class_id")
				m_strBrand_Class_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:brand_sottoclass_id")
				m_strBrand_Sotto_Class_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:brand_detail_id")
				m_strBrand_Detail_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:condition_id")
				m_strCondition_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:packaging_id")
				m_strPackaging_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:format_id")
				m_strFormat_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:objtype_id")
				m_strObjtype_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:taste_id")
				m_strTaste_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:variant_id")
				m_strVariant_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:variant_detail_id")
				m_strVariant_Detail_id = GetElementNodeValue(poChildNode);
			else if (strNodename == "sap:country_id")
				m_strCountryId = GetElementNodeValue(poChildNode);
		}
	} while (false);
}

string TemplateData::GetElementNodeValue(DOMNode* poChildNode)
{
	string strValue = "";
	do
	{
		DOMNode *poValueNode = poChildNode->getFirstChild();
		if (poValueNode == NULL)
			break;
		strValue = XMLString::transcode(poValueNode->getNodeValue());
	} while (false);
	return strValue;
}
void TemplateData::SetProjectID(string strProjectId)
{
	m_strProjectID = strProjectId;
}

string TemplateData::GetProjectID()
{
	return m_strProjectID;
}
