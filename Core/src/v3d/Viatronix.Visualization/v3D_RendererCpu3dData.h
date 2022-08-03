// $Id: v3D_RendererCpu3dData.h,v 1.3 2004/05/13 19:18:26 frank Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)

// pragmas
#pragma once

// includes
#include "v3D_Interfaces.h"
#include "vxShareableObject.h"
#include "vxRendererCpu3dData.h"

// forward declarations
class Observer;

// namespaces
OPEN_VISUALIZATION_NS

// class definition
public __gc class RendererCpu3dData : public IObserver, public IElement
{
// enums
public:

  // events  
  __value enum RenderingQualityEnum
  {
    DEFAULT,
    ENDOLUMINAL,
    HIGHEST
  }; // enum RenderingQualityEnum

// IElement implementation
public:

  /// gets a pointer to the extended element
  System::IntPtr GetElementPtr() { return System::IntPtr( m_pRendererCpu3dData ); }

  /// connects an observer to the object
  void Connect( System::IntPtr observer );

  /// disconnects an observer from the object
  void Disconnect( System::IntPtr observer );

// IObserver implementation
public:
  
  /// processes modified queue events
  void OnModified( System::IntPtr modifiedList );

/// functions
public:

  /// initializer
  RendererCpu3dData();

  /// finalizer
  ~RendererCpu3dData();

/// properties
public:

  /// get quality level scale factor
  __property float get_QualityScaleFactor() { return m_pRendererCpu3dData->GetQualityScaleFactor(); }

  /// set quality level scale factor
  __property void set_QualityScaleFactor( float fFactor ) { m_pRendererCpu3dData->SetQualityScaleFactor(fFactor); }

  /// set quality level
  __property void set_RenderingQuality( RenderingQualityEnum quality );
  
  /// gets if shading is enabled
  __property bool get_DisplayShading();

  /// sets if shading is enabled
  __property void set_DisplayShading( bool bDisplayShadows );

// data
private:

  vxShareableObject<vxRendererCpu3dData> * m_pRendererCpu3dData;

}; // RendererCpu3dData


CLOSE_VISUALIZATION_NS


// $Log: v3D_RendererCpu3dData.h,v $
// Revision 1.3  2004/05/13 19:18:26  frank
// formatting
//
// Revision 1.2  2004/05/12 19:27:08  frank
// improved quality for off-line rendering
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.5  2004/03/02 01:10:43  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.4.2.1  2004/02/17 16:16:41  geconomos
// Added DisplayShading property.
//
// Revision 1.4  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.3  2003/11/16 23:39:02  michael
// added ability to controle the rendering speed/quality ...
//
// Revision 1.2  2003/10/08 19:32:50  frank
// Changed the property to a property syntax
//
// Revision 1.1  2003/09/29 19:26:04  michael
// added managed wrapper RendererCpu3dData
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_RendererCpu3dData.h,v 1.3 2004/05/13 19:18:26 frank Exp $
// $Id: v3D_RendererCpu3dData.h,v 1.3 2004/05/13 19:18:26 frank Exp $
