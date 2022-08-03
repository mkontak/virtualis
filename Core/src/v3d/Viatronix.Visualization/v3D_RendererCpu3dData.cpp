// $Id: v3D_RendererCpu3dData.cpp,v 1.2 2004/05/12 19:27:08 frank Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_RendererCpu3dData.h"


// namespaces
USING_VISUALIZATION_NS


/**
 * Initializes a RendererCpu3dData object with default values.
 */
RendererCpu3dData::RendererCpu3dData()
{
  m_pRendererCpu3dData = new vxShareableObject<vxRendererCpu3dData>();
}


/**
 * Finalizer
 */
RendererCpu3dData::~RendererCpu3dData()
{
  delete m_pRendererCpu3dData;
  m_pRendererCpu3dData = NULL;
} // ~RendererCpu3dData()


/** 
 * Connects an observer from the object.
 * @param observer Observer interested in events.
 */
void RendererCpu3dData::Connect( System::IntPtr observer )
{
  m_pRendererCpu3dData->Connect( reinterpret_cast< vxObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 * @param observer Observer interested in events.
 */
void RendererCpu3dData::Disconnect( System::IntPtr  observer )
{
  m_pRendererCpu3dData->Disconnect( reinterpret_cast< vxObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events.
 * @param modifiedList Modified info list.
 */
void RendererCpu3dData::OnModified( System::IntPtr modifiedList )
{
} // OnModified()


/** 
 * Set rendering quality level
 */
void RendererCpu3dData::set_RenderingQuality( RenderingQualityEnum quality )
{
  vxRendererCpu3dData * pRendererCpu3dData = dynamic_cast<vxRendererCpu3dData *>(m_pRendererCpu3dData);
  switch (quality)
  {
  case ENDOLUMINAL:
    pRendererCpu3dData->SetJittering(true);
    pRendererCpu3dData->SetPreIntegration(false);
    pRendererCpu3dData->SetSampleEveryPixel(false);
    break;
  case HIGHEST:
    pRendererCpu3dData->SetJittering(false);
    pRendererCpu3dData->SetPreIntegration(true);
    pRendererCpu3dData->SetSampleEveryPixel(true);
    break;
  case DEFAULT:
  default:
    pRendererCpu3dData->SetJittering(false);
    pRendererCpu3dData->SetPreIntegration(true);
    pRendererCpu3dData->SetSampleEveryPixel(false);
    break;
  }

  m_pRendererCpu3dData->Modified(vxModInfo(vxRendererCpu3dData, vxUtils::vxEventRendererCpu3dDataEnum::RENDERERCPU3DDATA_MODIFIED ) );
} // SetQuality()


/** 
 * Gets if shading is enabled
 *
 * @return True if shadows are being displayed; false otherwise.
 */
bool RendererCpu3dData::get_DisplayShading()
{  
  return m_pRendererCpu3dData->IsShading();
} // get_DisplayShadows()


/**
 * Sets if shading is enabled
 *
 * @param   bDisplayShadows   True to display shadows; false otherwise.
 */
void RendererCpu3dData::set_DisplayShading( bool bDisplayShadows )
{
  m_pRendererCpu3dData->SetShading( bDisplayShadows );
  m_pRendererCpu3dData->Modified(vxModInfo(vxRendererCpu3dData, vxUtils::vxEventRendererCpu3dDataEnum::RENDERERCPU3DDATA_MODIFIED ) );
} // set_DisplayShadows( bDisplayShadows )


// Revision History:
// $Log: v3D_RendererCpu3dData.cpp,v $
// Revision 1.2  2004/05/12 19:27:08  frank
// improved quality for off-line rendering
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/03/02 01:10:43  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.3.2.2  2004/02/24 20:11:39  michael
// took out code that was commented out
//
// Revision 1.3.2.1  2004/02/17 16:16:42  geconomos
// Added DisplayShading property.
//
// Revision 1.3  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.2  2003/10/08 19:32:50  frank
// Changed the property to a property syntax
//
// Revision 1.1  2003/09/29 19:26:04  michael
// added managed wrapper RendererCpu3dData
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_RendererCpu3dData.cpp,v 1.2 2004/05/12 19:27:08 frank Exp $
// $Id: v3D_RendererCpu3dData.cpp,v 1.2 2004/05/12 19:27:08 frank Exp $
