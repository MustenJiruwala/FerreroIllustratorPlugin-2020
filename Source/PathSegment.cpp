#include "IllustratorSDK.h"
#include "PathSegment.h"

PathSegment::PathSegment()
{

}

PathSegment::PathSegment(string strPathString)
{
	char * poPathSegment;
	poPathSegment = strtok((char*)strPathString.c_str(), "~");
	std::vector<string> arrSegments;
	while (poPathSegment != NULL)
	{
		string strPathSegment = "";
		strPathSegment.append(poPathSegment);
		arrSegments.push_back(strPathSegment);
		poPathSegment = strtok(NULL, "~");
	}
	if (arrSegments.size() == 4)
	{
		ExtractPoint(arrSegments[0], m_oAnchorPoint);
		ExtractPoint(arrSegments[1], m_oInPoint);
		ExtractPoint(arrSegments[2], m_oOutPoint);
		m_bIscorner = arrSegments[3] == "CORNER";
	}
}

void PathSegment::ExtractPoint(string strPointString, AIRealPoint &oPoint)
{
	ArtworkUtil oUtil;
	oPoint.h = std::stod(oUtil.GetComponent(strPointString, 1));
	oPoint.v = -std::stod(oUtil.GetComponent(strPointString, 2));
}

AIRealPoint PathSegment::GetAnchorPoint()
{
	return m_oAnchorPoint;
}

AIRealPoint PathSegment::GetInPoint()
{
	return m_oInPoint;
}

AIRealPoint PathSegment::GetOutPoint()
{
	return m_oOutPoint;
}

AIBoolean PathSegment::IsCorner()
{
	return m_bIscorner;
}
