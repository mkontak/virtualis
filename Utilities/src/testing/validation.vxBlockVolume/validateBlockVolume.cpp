// $Id: VCpipeline.cpp,v 1.0 2011/07/19 dongqing Exp $
//
// Copyright(c) 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Created:  
// Author: Dongqing Chen, dongqing@viatronix.net
//
// Complete History at bottom of file.

#include "stdafx.h"
#include "vxTestBlockVolume.h"

/**
*The one and only main.
*/
int _tmain(int argc, _TCHAR* argv[])
{
	try
  {
		std::string sSourceFilePath = argv[1];
		std::string sDestFilePath   = argv[2];

    vxTestBlockVolume tester( sSourceFilePath, sDestFilePath ); 
		if( !tester.Run() )
			return -1; 
  }
	catch( ... )
	{
		return( -1 );
	}

	return( 0 );
} // main

// $Log: VCpipeline.cpp,v $
// Revision 1.0 2011/07/19  dongqing
//
// $Header: validateBlockVolume.cpp,v 1.0 2011/07/19  dongqing Exp $
// $Id: VCpipeline.cpp,v 1.0 2011/07/19  dongqing Exp $
