#ifndef _PathSegment_
#define _PathSegment_
#include "IllustratorSDK.h"
#include "ArtworkUtil.h"
class PathSegment
{
public:
	/**	Constructor.
	*/
	PathSegment();
	PathSegment(string strPathString);
	AIRealPoint GetAnchorPoint();
	AIRealPoint GetInPoint();
	AIRealPoint GetOutPoint();
	AIBoolean IsCorner();
	/**	Destructor.
	*/
	virtual ~PathSegment() {}
private:
	AIRealPoint	m_oAnchorPoint;
	AIRealPoint m_oInPoint;
	AIRealPoint m_oOutPoint;
	AIBoolean	m_bIscorner;
private:
	void ExtractPoint(string strPointString, AIRealPoint &oPoint);
};
#endif // _PathSegment_