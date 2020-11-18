#ifndef _Path_
#define _Path_
#include "IllustratorSDK.h"
#include "PathSegment.h"

class Path
{
public:
	/**	Constructor.
	*/
	Path();
	Path(string strPathString);

	/**	Destructor.
	*/
	virtual ~Path() {}
	virtual ASErr CreatePath(AIArtHandle oHandle);
private:
	std::vector<PathSegment> m_arrPathSegment;
private:
	void MMtoPoints(AIRealPoint &oPoint);
};
#endif // _Path_