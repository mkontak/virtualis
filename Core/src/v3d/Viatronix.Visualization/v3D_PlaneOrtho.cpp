// $Id: v3D_PlaneOrtho.cpp,v 1.6.2.2 2009/07/29 19:54:06 kchalupa Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:george@viatronix.com)


// includes
#include "stdafx.h"
#include "vxManagedObserver.h"
#include "v3D_PlaneOrtho.h"
#include "v3D_Triplet.h"
#include "v3D_Volume.h"


// namespaces
USING_VISUALIZATION_NS


/**
 * Initializes a PlaneOrtho object with default values.
 */
PlaneOrtho::PlaneOrtho()
{
  CommonInit( Orientations::Axial );
} // PlaneOrtho()


/**
 * Initializes a PlaneOrtho object from the specified orientation.
 *
 * @param eOrientation Plane orientation.
 */
PlaneOrtho::PlaneOrtho( Orientations eOrientation )
{
  CommonInit( eOrientation );
} // PlaneOrtho( eOrientation )


/**
 * Initializes a PlaneOrtho object from the specified orientation and volume
 *
 * @param eOrientation Plane orientation.
 * @param mpVolume Volume object. Plance will be sent to the senter of the volume.
 */
PlaneOrtho::PlaneOrtho( Orientations eOrientation, Volume ^ mpVolume )
{
  CommonInit( eOrientation );
  Initialize( mpVolume );
} // PlaneOrtho( eOrientation, mpVolume )


/**
 * Destructor
 */
PlaneOrtho::~PlaneOrtho()
{
  this->!PlaneOrtho();
} // ~PlaneOrtho()


/**
 * Finalizer
 */
PlaneOrtho::!PlaneOrtho()
{
  m_pPlane->Disconnect( m_pObserver );
  delete  m_pObserver; 
} // !PlaneOrtho()


/**
 * Common initialization for PlaneOrtho object.
 *
 * @param eOrientation Plane orientation.
 */
void PlaneOrtho::CommonInit( Viatronix::Visualization::Orientations eOrientation )
{
  m_pPlane = new vxShareableObject< vxPlaneOrtho< float4 > >();
  m_pObserver = new vxManagedObserver( this );

  m_pPlane->Connect( m_pObserver );
  
  SetNormalToOrientation( eOrientation );
} // CommonInit( eOrientation )

 
/**
 * Initilizes the plane to the center of the specified volume.
 *
 * @param mpVolume Volume object.
 */
void PlaneOrtho::Initialize( Viatronix::Visualization::Volume ^ mpVolume )
{
  Triplet ^ mpUnits = mpVolume->Units;
  Triplet ^ mpDimensions = mpVolume->Dimensions;
  float fDistance( 0.0F );

  Orientations eOrientation = this->Orientation; 
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
} // Initialize( mpVolume )


/**
 * Increments one slice.
 * @return Plane distance in world coordinates.
 */
float PlaneOrtho::Increment()
{
  float fDistance = m_pPlane->Increment();
  m_pPlane->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED) );

  return fDistance;
} // Increment()


/**
 * Decrements one slice.
 * @return Plane distance in world coordinates.
 */
float PlaneOrtho::Decrement()
{
  float fDistance(m_pPlane->Decrement());
  m_pPlane->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED) );

  return fDistance;
} // Decrement()


/** 
 * Connects an observer from the object.
 *
 * @param observer vxManagedObserver interested in events.
 */
void PlaneOrtho::Connect( System::IntPtr observer )
{
  m_pPlane->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect( observer )


/** 
 * Disconnects an observer from the object.
 *
 * @param observer vxManagedObserver interested in events.
 */
void PlaneOrtho::Disconnect( System::IntPtr observer )
{
  m_pPlane->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect( observer )


/**
 * Processes modified queue events.
 *
 * @param modifiedList Modified info list.
 */
void PlaneOrtho::OnModified( System::IntPtr modifiedList )
{
  Changed( this, System::EventArgs::Empty );
} // OnModified( modifiedList )


/**
 * Gets the orientation.
 *
 * return       orientation
 */
Viatronix::Visualization::Orientations PlaneOrtho::Orientation::get()
{
  Normal3Df plane = m_pPlane->GetNormal();
  if( plane == Normal<float4>( 0, 0, -1 ))
  {
    return Orientations::Axial;
  }
  else if( plane == Normal<float4>( -1, 0, 0 ))
  {
    return Orientations::Sagittal;
  }
  else
  {
    return Orientations::Coronal;
  }
} // get_Orientation()


/**
 * Sets the orientation.
 *
 * @param eOrientation Plane orientation.
 */
 void PlaneOrtho::Orientation::set( Viatronix::Visualization::Orientations eOrientation )
{
  SetNormalToOrientation( eOrientation );
  m_pPlane->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::PLANE_ORIENTATION_CHANGED ) );
} // set_Orientation( eOrientation )


/**
 * Sets the plane's distance in world coordinates.
 *
 * @param fDistance Plane distance in world coordinates.
 */
 void PlaneOrtho::Distance::set( float fDistance )
{
  m_pPlane->SetDistance( fDistance ); 
  m_pPlane->Modified( vxModInfo( vxPlaneOrtho<float4>, vxUtils::PLANE_MODIFIED ) );
} // set_Distance( fDistance )


/**
 * Gets the plane's distance in world coordinates.
 *
 * @return Plane distance in world coordinates.
 */
 float PlaneOrtho::Distance::get()
{
  return m_pPlane->GetDistance();
} // get_Distance()


/**
 * Sets the plane's normal to the specified orientation.
 *
 * @param eOrientation Plane orientation.
 */
void PlaneOrtho::SetNormalToOrientation( Viatronix::Visualization::Orientations eOrientation )
{
  switch( eOrientation )
  {
    case Orientations::Axial:
    {
      m_pPlane->SetNormal( Normal<float4>( 0, 0, -1 ) );
      break;
    }
    case Orientations::Sagittal:
    {
      m_pPlane->SetNormal( Normal<float4>( -1, 0, 0 ) );
      break;
    }
    case Orientations::Coronal:
    {
      m_pPlane->SetNormal( Normal<float4>( 0, -1, 0 ) );
      break;
    }
  }  // switch( eOrientation )
} // SetNormalToOrientation( eOrientation )


// $Log: v3D_PlaneOrtho.cpp,v $
// Revision 1.6.2.2  2009/07/29 19:54:06  kchalupa
// Code Walkthrough
//
// Revision 1.6.2.1  2008/11/18 15:17:26  gdavidson
// Determine orientation enum from plane in native pointer
//
// Revision 1.6  2007/06/05 21:20:24  gdavidson
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
// Revision 1.1.1.1.12.1  2005/08/15 19:32:28  geconomos
// set axial plane initial postion to 0
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.22  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.21  2003/11/10 21:18:36  frank
// Sending the proper events
//
// Revision 1.20  2003/09/29 17:24:02  michael
// when orientation is set, it is like initializing the plane
//
// Revision 1.19  2003/08/20 14:31:11  michael
// added "mailto:"
//
// Revision 1.18  2003/05/16 13:11:04  frank
// formatting
//
// Revision 1.17  2003/05/13 13:28:46  michael
// code review
//
// Revision 1.16  2003/05/12 18:40:41  geconomos
// Coding standards.
//
// Revision 1.15  2003/03/31 13:51:49  michael
// Completed first step of cleaning up the event mechanism. The events are now
// mostly in vxUtils, named vxEvent???Enum and some day we might be able to
// move to event classes that are more flexible ...
//
// Revision 1.14  2002/11/18 17:51:48  michael
// additional properties
//
// Revision 1.13  2002/11/18 16:36:03  michael
// completed plane object renaming
//
// Revision 1.12  2002/11/13 15:03:10  geconomos
// Removed EVENT_FLOAT_DISTANCE
//
// Revision 1.11  2002/11/12 21:31:01  geconomos
// All standard Environment elements now implement IElement.
//
// Revision 1.10  2002/11/11 20:16:43  geconomos
// Fixed initialization problem.
//
// Revision 1.9  2002/11/06 22:57:43  michael
// took out the Get/SetFloatDistance() bool which has nothing to do with this class!!!
//
// Revision 1.8  2002/11/05 20:51:51  geconomos
// Qualified orientation enums.
//
// Revision 1.7  2002/10/03 19:40:42  frank
// Added Increment() and Decrement().
//
// Revision 1.6  2002/09/24 18:17:55  geconomos
// Exception handling and formatting.
//
// Revision 1.5  2002/08/07 21:27:57  geconomos
// Added events.
//
// Revision 1.4  2002/08/07 19:41:55  dmitry
// Set float distance to true on set_distance.
//
// Revision 1.3  2002/07/19 12:49:30  manju
// Moved SetDistance to cpp file.
//
// Revision 1.2  2002/07/03 13:15:55  geconomos
// Continued effort!
//
// Revision 1.1  2002/07/02 20:30:13  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_PlaneOrtho.cpp,v 1.6.2.2 2009/07/29 19:54:06 kchalupa Exp $
// $Id: v3D_PlaneOrtho.cpp,v 1.6.2.2 2009/07/29 19:54:06 kchalupa Exp $
