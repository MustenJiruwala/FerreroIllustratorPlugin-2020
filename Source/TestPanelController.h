//========================================================================================
//  
//  $File: //ai/mainline/devtech/sdk/public/samplecode/Test/Source/TestPanelController.h $
//
//  $Revision: #1 $
//
//  Copyright 2010 Adobe Systems Incorporated. All rights reserved.
//  
//  NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance 
//  with the terms of the Adobe license agreement accompanying it.  If you have received
//  this file from a source other than Adobe, then your use, modification, or 
//  distribution of it requires the prior written permission of Adobe.
//  
//========================================================================================

#ifndef __TestPANELCONTROLLER_H__
#define __TestPANELCONTROLLER_H__

#include "IllustratorSDK.h"
#include "HtmlUIController.h"

class CEPTestPanelController : public HtmlUIController
{
public:
	CEPTestPanelController();
	
	csxs::event::EventErrorCode RegisterCSXSEventListeners();
	
	csxs::event::EventErrorCode RemoveEventListeners();
	
	virtual ASErr SendData() {return kNoErr;}
	
	virtual void ParseData(const char* eventData) { ; }
};

#endif
