// $Id: v3D_SliceShadow.cpp,v 1.5 2006/10/04 18:29:05 gdavidson Exp $
//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManagedObserver.h"
#include "v3D_SliceShadow.h"
#include "v3D_PlaneOrtho.h"
#include "v3D_PlaneOblique.h"
#include "v3d_RenderingMode.h"
#include "Logger.h"


// namespaces
USING_VISUALIZATION_NS


// defines
#define FILE_REVISION "$Revision: 1.5 $"


/**
 * Initializes a SliceShadow object with default values.
 */
SliceShadow::SliceShadow() :
  m_pSliceShadow( new vxShareableObject< vxSliceShadow >() )
{
  m_pObserver = new vxManagedObserver( this );
} // SliceShadow()


/**
 * destructor
 */
SliceShadow::~SliceShadow()
{
  this->!SliceShadow();
} // ~SliceShadow()


/**
 * Finalizer
 */
SliceShadow::!SliceShadow()
{
  delete m_pObserver;
  m_pObserver = NULL;
} // !SliceShadow()


/**
 * adds an ortho plane
 *
 * @param     mpPlaneOrtho      PlaneOrtho object being added
 */
void SliceShadow::AddPlane( Viatronix::v3D::Visualization::PlaneOrtho ^ mpPlaneOrtho )
{
  vxShareableObject< vxPlaneOrtho<float4> > * pPlaneOrtho( ToNativePlaneOrthoPtr( mpPlaneOrtho->GetElementPtr() ) );

  pPlaneOrtho->Connect( m_pObserver );

} // AddPlane( mpPlaneOrtho )


/**
 * removes an ortho plane
 *
 * @param     mpPlaneOrtho      PlaneOrtho object being removed
 */
void SliceShadow::RemovePlane( Viatronix::v3D::Visualization::PlaneOrtho ^ mpPlaneOrtho )
{
  vxShareableObject< vxPlaneOrtho<float4> > * pPlaneOrtho( ToNativePlaneOrthoPtr( mpPlaneOrtho->GetElementPtr() ) );

  pPlaneOrtho->Disconnect( m_pObserver );

} // RemovePlane( mpPlaneOrtho )


/**
 * adds an oblique plane
 *
 * @param     mpPlaneOblique      PlaneOblique object being added
 */
void SliceShadow::AddPlane( Viatronix::v3D::Visualization::PlaneOblique ^ mpPlaneOblique )
{
  vxShareableObject< vxPlaneOblique<float4> > * pPlaneOblique( ToNativePlaneObliquePtr( mpPlaneOblique->GetElementPtr() ) );

  pPlaneOblique->Connect( m_pObserver );
} // AddPlane( mpPlaneOblique )


/**
 * removes an oblique plane
 *
 * @param     mpPlaneOblique      PlaneOrtho object being removed
 */
void SliceShadow::RemovePlane( Viatronix::v3D::Visualization::PlaneOblique ^ mpPlaneOblique )
{
  vxShareableObject< vxPlaneOblique<float4> > * pPlaneOblique( ToNativePlaneObliquePtr( mpPlaneOblique->GetElementPtr() ) );

  pPlaneOblique->Disconnect( m_pObserver );
} // RemovePlane( mpPlaneOblique )


/**
 * adds a rendering mode
 *
 * @param     mpRenderingMode      RenderingMode object being added
 */
void SliceShadow::AddRenderingMode( Viatronix::v3D::Visualization::RenderingMode ^ mpRenderingMode )
{
  vxShareableObject< vxRenderingMode > * pRenderingMode( ToNativeRenderingModePtr( mpRenderingMode->GetElementPtr() ) );

  pRenderingMode->Connect( m_pObserver );
 
} // AddRenderingMode( mpRenderingMode )


/**
 * removes a rendering mode
 *
 * @param     mpRenderingMode      RenderingMode object being removed
 */
void SliceShadow::RemoveRenderingMode( Viatronix::v3D::Visualization::RenderingMode ^ mpRenderingMode )
{
  vxShareableObject< vxRenderingMode > * pRenderingMode( ToNativeRenderingModePtr( mpRenderingMode->GetElementPtr() ) );

  pRenderingMode->Disconnect( m_pObserver );
} // RemoveRenderingMode( mpRenderingMode )


/** 
 * Connects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void SliceShadow::Connect( System::IntPtr observer )
{
  m_pSliceShadow->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void SliceShadow::Disconnect( System::IntPtr observer )
{
  m_pSliceShadow->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Processes modified queue events.
 * @param modifiedList Modified info list.
 */
void SliceShadow::OnModified( System::IntPtr modifiedList )
{
  vxModifiedInfoList * pInfoList( reinterpret_cast< vxModifiedInfoList * >( modifiedList.ToPointer() ));
  for ( const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next() )
  {
    const type_info & elementType = pInfo->GetTypeInfo();    

    // if a vxPlaneOrtho, vxPlaneOblique or vxRenderingMode is modified - raise a vxSliceShadow modified
    if(( elementType == typeid( vxPlaneOrtho< float4 > )) || 
       ( elementType == typeid( vxPlaneOblique< float4 > )) || 
       ( elementType == typeid( vxRenderingMode )))
    {
      m_pSliceShadow->Modified( vxModInfo( vxSliceShadow, vxUtils::SLICESHADOW_MODIFIED ));
      break;
    }
  }  
} // OnModified( modifiedList )


// $Log: v3D_SliceShadow.cpp,v $
// Revision 1.5  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.4  2006/09/27 17:23:49  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.3  2006/04/25 18:40:51  geconomos
// removed dead code
//
// Revision 1.2  2006/04/20 12:33:07  geconomos
// removed AddReference and RemoveReference methods (sorry Greg)
//
// Revision 1.1  2006/03/27 19:41:49  gdavidson
// new build
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Visualization/v3D_SliceShadow.cpp,v 1.5 2006/10/04 18:29:05 gdavidson Exp $
// $Id: v3D_SliceShadow.cpp,v 1.5 2006/10/04 18:29:05 gdavidson Exp $
