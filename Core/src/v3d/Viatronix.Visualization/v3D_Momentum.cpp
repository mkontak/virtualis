// $Id: v3D_Momentum.cpp,v 1.1 2007/01/16 17:14:19 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)

#include "StdAfx.h"
#include "v3D_Momentum.h"
#include "vxMomentum.h"
#include "vxShareableObject.h"
#include "vxManagedObserver.h"
#include "vxUtils.h"


USING_VISUALIZATION_NS


/**
 * default constructor
 */
Momentum::Momentum()  
{
  // unmanaged types 
  m_pMomentum = new vxShareableObject< vxMomentum >();
  m_pObserver = new vxManagedObserver( this );
  m_pMomentum->Connect( m_pObserver );
} // Cine()


/**
 * destructor
 */
Momentum::~Momentum()
{
  this->!Momentum();
} // ~Cine()


/**
 * finalizer
 */
Momentum::!Momentum()
{
  if( m_pMomentum != NULL )
  {
    if( m_pObserver != NULL )
    {
      m_pMomentum->Disconnect( m_pObserver );
      delete m_pObserver;
    }
    delete m_pMomentum;
  }

  m_pMomentum = NULL;
  m_pObserver = NULL;
} // !Cine()


/** 
 * Receives modified information on connected objects. 
 */
void Momentum::OnModified( System::IntPtr ptrInfo )
{
} // OnModified()


/** 
 * Connects an observer to the object.
 */
void Momentum::Connect( System::IntPtr observer )
{
  m_pMomentum->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 */
void Momentum::Disconnect( System::IntPtr observer )
{
  m_pMomentum->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()


/**
 * Sets the enabled property
 *
 * @param       bEnabled        
 */
void Momentum::Enabled::set( bool bEnabled )
{
  m_pMomentum->SetEnabled( bEnabled );
  if( !bEnabled )
  {
    m_pMomentum->SetInitiated( false );
    m_pMomentum->Modified( vxModInfo( vxMomentum, vxUtils::MOMENTUM_MODIFIED ));
  }
} // Enabled


// $Log: v3D_Momentum.cpp,v $
// Revision 1.1  2007/01/16 17:14:19  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Momentum.cpp,v 1.1 2007/01/16 17:14:19 gdavidson Exp $
// $Id: v3D_Momentum.cpp,v 1.1 2007/01/16 17:14:19 gdavidson Exp $

