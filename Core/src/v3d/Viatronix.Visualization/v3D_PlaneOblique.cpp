// $Id: v3D_PlaneOblique.cpp,v 1.6 2007/06/05 21:20:23 gdavidson Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Frank Dachille (frank@viatronix.ncom)


// includes
#include "stdafx.h"
#include "vxManagedObserver.h"
#include "v3D_PlaneOblique.h"
#include "v3D_Triplet.h"
#include "v3D_Volume.h"


// namespaces
USING_VISUALIZATION_NS


/**
 * Default constructor
 */
PlaneOblique::PlaneOblique()
{
  CommonInit( Orientations::Axial );
} // PlaneOblique()


/**
 * Constructor from orientation.
 *
 * @param eOrientation the plane orientation
 */
PlaneOblique::PlaneOblique( Orientations eOrientation )
{
  CommonInit( eOrientation );
} // PlaneOblique( eOrientation )


/**
 * constructor from orientation and volume
 *
 * @param eOrientation the plane orientation
 * @param mpVolume     the volume
 */
PlaneOblique::PlaneOblique( Viatronix::Visualization::Orientations eOrientation, Volume ^ mpVolume )
{
  CommonInit( eOrientation );
  Initialize( eOrientation, mpVolume );
} // PlaneOblique( orientation, volume )


/**
 * Common initialization for plane
 *
 * @param eOrientation the plane orientation
 */
void PlaneOblique::CommonInit( Viatronix::Visualization::Orientations eOrientation )
{
  m_pPlaneOblique = new vxShareableObject< vxPlaneOblique<float4>  >();

  m_pObserver = new vxManagedObserver( this );
  m_pPlaneOblique->Connect( m_pObserver );

  SetNormalToOrientation( eOrientation );
} // CommonInit()


/**
 * Destructor
 */
PlaneOblique::~PlaneOblique()
{
  this->!PlaneOblique();
} // ~PlaneOblique()


/**
 * Finalizer
 */
PlaneOblique::!PlaneOblique()
{
  m_pPlaneOblique->Disconnect(m_pObserver);

  delete m_pPlaneOblique; m_pPlaneOblique = NULL;
  delete m_pObserver;     m_pObserver = NULL;
} // !PlaneOblique()


/**
 * Sets the plane's normal to the specified orientation.
 *
 * @param eOrientation the plane orientation
 */
void PlaneOblique::SetNormalToOrientation( Viatronix::Visualization::Orientations eOrientation )
{
  switch( eOrientation )
  {
  case Orientations::Axial:
  {
    m_pPlaneOblique->SetNormal( Normal<float4>( 0, 0, 1 ) );
    break;
  }
  case Orientations::Sagittal:
    {
      m_pPlaneOblique->SetNormal( Normal<float4>( -1, 0, 0 ) );
      break;
    }
  case Orientations::Coronal:
    {
      m_pPlaneOblique->SetNormal( Normal<float4>( 0, -1, 0 ) );
      break;
    }
  }  // switch( eOrientation)
} // SetNormalToOrientation()


/**
 * Initialize the object
 */
void PlaneOblique::Initialize( Viatronix::Visualization::Orientations eOrientation,
                               Viatronix::Visualization::Volume ^ mpVolume )
{
  Triplet ^ mpUnits = mpVolume->Units;
  Triplet ^ mpDimensions = mpVolume->Dimensions;
  float4 fDistance( 0.0F );

  if ( eOrientation == Orientations::Sagittal )
  {
    fDistance = mpUnits->x * ( mpDimensions->x / 2.0F );
  }

  else if ( eOrientation == Orientations::Coronal )
  {
    fDistance = mpUnits->y * ( mpDimensions->y / 2.0F );
  }

  else 
  {
    fDistance = mpUnits->z * ( mpDimensions->z / 2.0F );
  }

  this->Distance = fDistance;
} // Initialize()


/**
 * Sets the distance of the plane from the origin
 *
 * @param fDistance distance from the origin in world coordinates
 */
void PlaneOblique::Distance::set( float fDistance )
{
  m_pPlaneOblique->SetDistance( fDistance ); 
  m_pPlaneOblique->Modified( vxModInfo( vxPlaneOblique<float4>, vxUtils::PLANE_MODIFIED) );
} // set_Distance()


/**
 * Returns distance of the plane from the origin
 *
 * @return distance of the plane from the origin in world coordinates
 */
float PlaneOblique::Distance::get()
{
  return m_pPlaneOblique->GetDistance();
} // get_Distance()


/** 
 * Connects an observer to the object.
 *
 * @param observer the observer
 */
void PlaneOblique::Connect( System::IntPtr observer )
{
  m_pPlaneOblique->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 *
 * @param observer the observer
 */
void PlaneOblique::Disconnect( System::IntPtr  observer )
{
  m_pPlaneOblique->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Process the modified info list
 *
 * @param modifiedList
 */
void PlaneOblique::OnModified( System::IntPtr modifiedList )
{
} // OnModified()


// Revision History:
// $Log: v3D_PlaneOblique.cpp,v $
// Revision 1.6  2007/06/05 21:20:23  gdavidson
// Set the initial distance for an axial plane to the middle of the volume
//
// Revision 1.5  2006/10/04 18:29:05  gdavidson
// Ported to C++/CLI
//
// Revision 1.4  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.3  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.2  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.1.1.1.12.1  2005/08/15 19:32:51  geconomos
// set plane initial postion to 0 (z-axis aligned)
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.11  2004/03/02 01:10:44  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.10.2.1  2004/02/24 20:09:47  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.10  2004/01/06 13:11:15  michael
// fixed typo in comments ...
//
// Revision 1.9  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.8  2003/11/06 02:58:04  michael
// Fixed display of slice number and number of total slices for the oblique MPR,
// added text to be displayed if after rotation the slice is outside the volume,
// and added mechanism so that axis aligned obliques can reach the very first
// and very last slice/facs of volume (numerical precision problem).
//
// Revision 1.7  2003/05/13 17:37:40  frank
// code review
//
// Revision 1.6  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.5  2003/03/31 13:51:49  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.4  2002/11/18 20:15:58  michael
// added distance
//
// Revision 1.3  2002/11/18 17:51:48  michael
// additional properties
//
// Revision 1.2  2002/11/18 16:36:03  michael
// completed plane object renaming
//
// Revision 1.1  2002/11/13 19:26:04  frank
// enabled oblique view renderer and manipulator
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_PlaneOblique.cpp,v 1.6 2007/06/05 21:20:23 gdavidson Exp $
// $Id: v3D_PlaneOblique.cpp,v 1.6 2007/06/05 21:20:23 gdavidson Exp $
