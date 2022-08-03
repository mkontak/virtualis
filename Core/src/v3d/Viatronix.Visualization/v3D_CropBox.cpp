// $Id: v3D_CropBox.cpp,v 1.4.8.1 2008/09/02 15:26:58 dongqing Exp $
//
// Copyright © 2002, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Frank Dachille (frank@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3d_cropbox.h"
#include "vxManagedObserver.h"
#include "vxUtils.h"


// defines
#define FILE_REVISION "$Revision: 1.4.8.1 $"


// namespaces
USING_VISUALIZATION_NS


/** 
 * Initializes CropBox oobject with default values.
 */
CropBox::CropBox()
{
  m_pObserver = new vxManagedObserver( this );
  m_pCropBox = new vxShareableObject< vxCropBox >();
  m_pCropBox->Connect( m_pObserver );
} // CropBox()


/** 
 * destructor.
 */
CropBox::~CropBox()
{
  this->!CropBox();
} // ~CropBox()


/** 
 * Finalizer.
 */
CropBox::!CropBox()
{
  if ( m_pCropBox != NULL )
  {
    m_pCropBox->Disconnect( m_pObserver );
  }

  delete m_pCropBox;   m_pCropBox = NULL;
  delete m_pObserver; m_pObserver = NULL;
} // !CropBox()


/** 
 * Initializes the object on a volume. It retains the full z height, but defaults
 * to half the width in x and y dimensions.
 *
 * @param mpVolume the intensity volume
 */
void CropBox::Initialize( Volume ^ mpVolume )
{
  try
  {
    vxShareableObject< vxBlockVolume <int2> > * pBlockVolume = reinterpret_cast<  vxShareableObject< vxBlockVolume <int2> > * >( mpVolume->GetVolumePtr().ToPointer() );
    const Triplef worldDim(pBlockVolume->GetHeader().GetWorldDim());
    Point<float4> center(worldDim.X()/2.0, worldDim.Y()/2.0, worldDim.Z()/2.0);
    m_pCropBox->CreateCropBox(center, Vector<float4>(worldDim.X()/4.0, worldDim.Y()/4.0,worldDim.Z()/4.0));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Initialize()


/** 
 * Initializes the object to a full volume.  The crop box encloses the entire volume.
 *
 * @param mpVolume the intensity volume
 */
void CropBox::InitializeFull( Volume ^ mpVolume )
{
  try
  {
    vxShareableObject< vxBlockVolume <int2> > * pBlockVolume = reinterpret_cast<  vxShareableObject< vxBlockVolume <int2> > * >( mpVolume->GetVolumePtr().ToPointer() );
    const Triplef worldDim(pBlockVolume->GetHeader().GetWorldDim());
    Point<float4> center(worldDim.X()/2.0, worldDim.Y()/2.0, worldDim.Z()/2.0);
    m_pCropBox->CreateCropBox(center, Vector<float4>(worldDim.X()/2.0, worldDim.Y()/2.0,worldDim.Z()/2.0));
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Initialize()


/** 
 * Connects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void CropBox::Connect( System::IntPtr observer )
{
  m_pCropBox->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void CropBox::Disconnect( System::IntPtr  observer )
{
  m_pCropBox->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/** 
 * Processes modified queue events.
 *
 * @param modifiedList Modified info list.
 */
void CropBox::OnModified( System::IntPtr modifiedList )
{
} // OnModified()


#undef FILE_REVISION


// $Log: v3D_CropBox.cpp,v $
// Revision 1.4.8.1  2008/09/02 15:26:58  dongqing
// change the initial corpbox size smaller = 1/4 linear size
//
// Revision 1.4  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2004/10/29 20:18:33  frank
// formatting
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.22  2004/03/02 01:10:46  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.21.2.1  2004/02/24 20:06:09  michael
// coding standard: assign NULL after deletion and m_pOberserver --> m_pObserver
//
// Revision 1.21  2003/12/16 13:25:50  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.20  2003/05/16 13:09:36  frank
// code formatting
//
// Revision 1.19  2003/05/13 14:05:21  geconomos
// Fixed up after code walkthru.
//
// Revision 1.18  2003/05/13 13:28:44  michael
// code review
//
// Revision 1.17  2003/05/06 15:32:10  geconomos
// More coding standards.
//
// Revision 1.16  2003/05/05 17:13:04  geconomos
// Coding standards.
//
// Revision 1.15  2003/03/28 13:07:10  frank
// Added a function to initialize the crop box to the full extent of the mpVolume
//
// Revision 1.14  2003/01/22 22:07:59  ingmar
// dimension and units are now Triples
//
// Revision 1.13  2002/11/12 21:32:06  geconomos
// Removed modified events in Connect and Disconnect
//
// Revision 1.12  2002/11/04 18:52:13  frank
// Enabling crop box
//
// Revision 1.11  2002/11/02 03:12:11  frank
// Moved crop box into vxEnvironment.
//
// Revision 1.10  2002/10/07 15:20:33  geconomos
// Merged Volume_Int2 and Volume_uint2 into Volume.
//
// Revision 1.9  2002/09/27 15:29:46  dmitry
// Modified is called after a new observer is connected.
//
// Revision 1.8  2002/09/24 16:08:21  geconomos
// Exception handling and formatting.
//
// Revision 1.7  2002/09/20 16:41:08  geconomos
// Changed implemetation of class vxID
//
// Revision 1.6  2002/09/17 17:11:10  dmitry
// vxID used.
//
// Revision 1.5  2002/08/29 20:16:16  dmitry
// Modified on Disconnect.
//
// Revision 1.4  2002/08/21 22:32:51  ingmar
// adjusted default crop box to center 50%
//
// Revision 1.3  2002/08/09 16:20:12  dmitry
// Added Connect and Disconnect methods to IElement.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_CropBox.cpp,v 1.4.8.1 2008/09/02 15:26:58 dongqing Exp $
// $Id: v3D_CropBox.cpp,v 1.4.8.1 2008/09/02 15:26:58 dongqing Exp $