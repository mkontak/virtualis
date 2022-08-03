// $Id: vxDataset.h,v 1.4 2007/09/19 14:32:46 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Romy Sreedharan (mailto:romy@viatronix.com)

#pragma once

#include "vxShareableObject.h"
#include "vxBlockVolume.h"
#include "vxComponentArray.h"
#include "vxColormap.h"
#include "vxRenderingMode.h"


// forward declaration
class vxPhaseInfo;


// class declaration
class VX_VIEWER_DLL vxDataset
{
// member functions
public:

	//default constructor
  vxDataset() {}

	//destructor
	virtual ~vxDataset() {}

	///Sets the Intensity volume
	void SetIntensityVolume( vxShareableObject<vxBlockVolume<uint2> > * pVolumeIntensity ) { m_pVolumeIntensity = pVolumeIntensity; } 

	///Gets the Intensity volume
	vxShareableObject<vxBlockVolume<uint2> > * GetIntensityVolume() { return m_pVolumeIntensity; }

	///Set the label volume
	void SetLabelVolume( vxShareableObject<vxBlockVolume<uint2> > * pVolumeLabel ) {	m_pVolumeLabel = pVolumeLabel; }

	///Gets the label volume
	vxShareableObject<vxBlockVolume<uint2> > * GetLabelVolume() { return m_pVolumeLabel; }

	///Sets the components array
	void SetComponents( vxShareableObject<vxComponentArray> * pComponents ) { m_pComponents = pComponents; }

	///Gets the component array
	vxShareableObject<vxComponentArray> * GetComponents() { return m_pComponents; }

  /// get pointer to colormap
  vxShareableObject<vxColormap> * GetColormap() const { return m_pColormap; }

  /// set colormap pointer
  void SetColormap(vxShareableObject<vxColormap> * pColormap) { m_pColormap = pColormap; }

  /// get pointer to rendering mode
  vxShareableObject<vxRenderingMode> * GetRenderingMode() const { return m_pRenderingMode; }

  /// set rendering mode point in environment
  void SetRenderingMode(vxShareableObject<vxRenderingMode> * pRenderingMode) { m_pRenderingMode = pRenderingMode; }

  /// get pointer to phase info
  vxPhaseInfo * GetPhaseInfo() const { return m_pPhaseInfo; }

  /// set pointer to phase inf
  void SetPhaseInfo( vxPhaseInfo * pPhaseInfo ) { m_pPhaseInfo = pPhaseInfo; }

private:

	/// intensity volume
  vxShareableObject<vxBlockVolume<uint2> > * m_pVolumeIntensity;  

  /// label volume
  vxShareableObject<vxBlockVolume<uint2> > * m_pVolumeLabel; 

	/// components
  vxShareableObject<vxComponentArray> * m_pComponents;    

  /// colormap
  vxShareableObject<vxColormap> * m_pColormap; 

  /// rendering mode information
  vxShareableObject<vxRenderingMode> * m_pRenderingMode;  

  /// rendering mode information
  vxPhaseInfo * m_pPhaseInfo; 
  
}; // class vxDataset

// $Log: vxDataset.h,v $
// Revision 1.4  2007/09/19 14:32:46  gdavidson
// Added vxPhaseInfo object
//
// Revision 1.3  2007/04/02 21:28:08  gdavidson
// Added ColorMap and RenderingMode
//
// Revision 1.2  2007/03/30 17:42:13  romy
// Phase percentage member added
//
// Revision 1.1  2006/11/08 22:56:22  romy
// initial version
//
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxDataset.h,v 1.4 2007/09/19 14:32:46 gdavidson Exp $
// $Id: vxDataset.h,v 1.4 2007/09/19 14:32:46 gdavidson Exp $
