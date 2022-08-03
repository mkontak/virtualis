// $Id: vxSuspiciousROIoutput4CADpartner.h,v 1.1.2.3 2008/02/28 19:19:09 dongqing Exp $
//
// Copyright © 2007-2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner(s): Dongqing Chen dongqing@viatronix.net


/** 
 *  This class is to output suspicious ROI infomation to a text file for 
 *  CAD partner creating truth. It is not for release product.
 */


// pragmas
#pragma once


// includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Global.h"
#include "volume.h"
#include "vxBlockVolume.h"
#include "Dataset.h"
#include "vxCadROI.h"


// namespaces
namespace ReaderLib
{

  /// output suspicious region information
  class vxSuspiciousROIoutput4CADpartner
  {
    // public functions
    public:

      /// Constructor
      vxSuspiciousROIoutput4CADpartner( std::string & sFileName );

	    /// destructor
	    ~vxSuspiciousROIoutput4CADpartner();

      /// write out the results
      void Write( const Dataset & dataset );

    // private functions
    private:

      /// Compute the bounding box
      void ComputeBoundingBox( const vxCAD::vxCadROI & roi, Vector3D<float4> & vfLow, Vector3D<float4> & vfUp );

      /// convert volume coordinate to DICOM RCS world coordinate
      void ConvertVol2WorldCoordinate( const Vector3D<int4> & viVolCoordinate, Vector3D<float4> & vfWorldCoordinate );


    // private variables
    private: 	

      /// file name
      std::string & m_sFileName;

			/// DICOM RCS offset 
			Vector3D<float4> m_vfWorldRCSdicom;

      /// dimension and units
      Triple<float4> m_vfUnits;
      Triple<int4>   m_viDim;
  
      /// patient orientation string
      std::string m_sPatientOrientation;

  }; // vxSuspiciousROIoutput4CADpartner

} // namespace ReaderLib

// $Log: vxSuspiciousROIoutput4CADpartner.h,v $
// Revision 1.1.2.3  2008/02/28 19:19:09  dongqing
// the DICOM RCS convertion using Image Orientation Patient Cosine
// only now. The HFS stuff is no longer used for that purpose.
//
// Revision 1.1.2.2  2008/01/21 13:47:00  dongqing
// complete 1st time
//
// Revision 1.1.2.1  2008/01/18 22:38:15  dongqing
// initial check in the suspicious ROI output for CAD vendor codes
//
// Revision 1.0  2008/01/17 Dongqing
// comments and standards.
//
// $Header: /CVS/cvsRepository/V3D/src/vxColon/include/vxSuspiciousROIoutput4CADpartner.h,v 1.1.2.3 2008/02/28 19:19:09 dongqing Exp $
// $Id: vxSuspiciousROIoutput4CADpartner.h,v 1.1.2.3 2008/02/28 19:19:09 dongqing Exp $
