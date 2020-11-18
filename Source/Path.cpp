#include "IllustratorSDK.h"
#include "Path.h"
#include "ArtworkUtil.h"

extern "C" AIArtSuite* sAIArt;
extern "C" AIPathSuite* sAIPath;
Path::Path()
{

}

Path::Path(string strPathString)
{
	char * poPathSegment;
	poPathSegment = strtok((char*)strPathString.c_str(), ";");
	std::vector<string> arrSegments;
	while (poPathSegment != NULL)
	{
		string strPathSegment = "";
		strPathSegment.append(poPathSegment);
		arrSegments.push_back(strPathSegment);
		poPathSegment = strtok(NULL, ";");
	}
	// Added additional for loop since if inside while loop above poPathSegment was returning null if we do strtok(NULL, ";") after constructing PathSegment.
	for (int nIndex = 0; nIndex < arrSegments.size(); nIndex++)
	{
		PathSegment oPathSegment(arrSegments[nIndex]);
		m_arrPathSegment.push_back(oPathSegment);
	}
}

ASErr Path::CreatePath(AIArtHandle oHandle)
{
	ASErr tResult = kNoErr;
	for (int nIndex = 0; nIndex < m_arrPathSegment.size(); nIndex++)
	{
		AIPathSegment oSegment = { {0,0},{0,0},{0,0},false };
		oSegment.corner = m_arrPathSegment[nIndex].IsCorner();
		AIRealPoint oAnchorPoint = m_arrPathSegment[nIndex].GetAnchorPoint();
		MMtoPoints(oAnchorPoint);
		AIRealPoint oInPoint = m_arrPathSegment[nIndex].GetInPoint();
		MMtoPoints(oInPoint);
		AIRealPoint oOutPoint = m_arrPathSegment[nIndex].GetOutPoint();
		MMtoPoints(oOutPoint);
		oSegment.p = oAnchorPoint;
		oSegment.in = oInPoint;
		oSegment.out = oOutPoint;
		sAIPath->SetPathSegments(oHandle, nIndex, 1, &oSegment);
	}
	tResult = sAIPath->SetPathClosed(oHandle, true);
	return tResult;
}

void Path::MMtoPoints(AIRealPoint &oPoint)
{
	oPoint.h *= (PPI / MMPI);
	oPoint.v *= (PPI / MMPI);
}

