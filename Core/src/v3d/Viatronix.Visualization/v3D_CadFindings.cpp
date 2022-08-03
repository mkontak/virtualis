// $Id: v3D_CadFindings.cpp,v 1.3 2007/04/11 20:56:25 romy Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson (mailto:gdavidson@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_CadFindings.h"
#include "v3D_Elements.h"
#include "vxCadFindings.h"
#include "vxCadFinding.h"
#include "vxCadVector.h"
#include "vxManagedObserver.h"
#include "v3D_EventArgs.h"
#include "v3D_Volume.h"
#include "v3D_Environment.h"




// namespaces
USING_VISUALIZATION_NS
using namespace vxCAD;


/**
 * constructor
 */
CadFindings::CadFindings() 
{
  m_pCadFindings = new vxShareableObject< vxCadFindings >();
  
  // create and connect the observer
  m_pObserver = new vxManagedObserver ( this );
  m_pCadFindings->Connect(m_pObserver);
} // CadFindings()


/**
 * destructor
 */
CadFindings::~CadFindings()
{
  this->!CadFindings();
} // ~CadFindings()


/**
 * finalizer
 */
CadFindings::!CadFindings()
{
  delete m_pCadFindings; m_pCadFindings = NULL;
  delete m_pObserver;   m_pObserver = NULL;
} // !CadFindings()


/**
 * Creates a new object that is a copy of the current instance.
 *
 * @return  A new object that is a copy of this instance.
 */
System::Object ^ CadFindings::Clone()
{
  // create a new instance of the annotations class
  CadFindings ^ mpCadFindings = gcnew CadFindings();

  // get the underlying implementation pointer  
  vxCadFindings * pCadFindings = dynamic_cast< vxCadFindings *>( mpCadFindings->m_pCadFindings );
  
  // make a copy of the underying implementations
  *pCadFindings = *dynamic_cast< vxCadFindings *>( m_pCadFindings );  
  
  return mpCadFindings;
} // Clone()


/**
 * Get the id
 *
 * @param     iIndex          CadFinding index
 * @return    Id for the finding
 */
System::String ^ CadFindings::GetID( int4 iIndex )
{
  const vxCadFinding & cadFinding = m_pCadFindings->GetFinding( iIndex );
  return gcnew System::String( cadFinding.GetId().c_str() );
} // GetID( iIndex )


/**
 * Get the centroid (in MM)
 *
 * @param     iIndex          CadFinding index
 * @return    ElementVector for the centroid
 */
ElementVector ^ CadFindings::GetCentroidMM( int4 iIndex )
{
  const vxCadFinding & cadFinding = m_pCadFindings->GetFinding( iIndex );
  const vxCadVector & cadVector = cadFinding.GetCentroidMM();

  ElementVector ^ elementVector = gcnew ElementVector();
  elementVector->Units = ElementVector::VectorUnits::Millimeter;
  vxElementVector<float4> * pVector = static_cast< vxElementVector<float4> * >( elementVector->GetElementPtr().ToPointer() );
  pVector->SetValue( cadVector.GetVector() );

  return elementVector;
} // GetCentroidMM( iIndex )


/**
 * Loads cad findings from file.
 *
 * @param       mpPath            Cad file
 */
void CadFindings::Load( System::String ^ mpPath )
{
  *m_pCadFindings = vxCadFindings::FromFile( ss( mpPath ));
} // FromFile( mpPath )


/**
 * Sets the selected Index.
 * @param index int4
 */
void CadFindings::SetSelectedIndex( const int4 index)
{
  m_pCadFindings->SetSelectedIndex( index ); 
  m_pCadFindings->Modified( vxModIndInfo( vxCadFindings , index, vxEnvironment::CAD_FINDINGS ) );

  vxModifiedQueue::Flush();
}//SetSelectedIndex( const int4 index)

/**
 * Processes modified queue events.
 * 
 * @param modifiedList Modified info list.
 */
void CadFindings::OnModified( System::IntPtr modifiedList )
{
  vxModifiedInfoList * pInfoList = reinterpret_cast< vxModifiedInfoList * >( modifiedList.ToPointer() );
  
  for (const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next())
  {  
    uint4 uFlags = pInfo->GetFlag();

    uint4 uIndex( 0 );
    
    const vxModifiedIndexedInfo * pInfoExt =  dynamic_cast< const vxModifiedIndexedInfo * >( pInfo );
    
    if (pInfoExt != NULL)
      uIndex = pInfoExt->GetIndex();

    if (uFlags & vxEnvironment::CAD_FINDINGS )
      SelectionChanged( this, gcnew IntegerArg( uIndex ) );

  } // for (const vxModifiedInfo * pInfo = pInfoList->Begin(); pInfo != NULL; pInfo = pInfoList->Next())
} // OnModified()

 
/** 
 * Connects an observer to the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void CadFindings::Connect( System::IntPtr observer )
{
  m_pCadFindings->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Connect()


/** 
 * Disconnects an observer from the object.
 *
 * @param observer vxManagedObserver object interested in events.
 */
void CadFindings::Disconnect( System::IntPtr  observer )
{
  m_pCadFindings->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );
} // Disconnect()




// $Log: v3D_CadFindings.cpp,v $
// Revision 1.3  2007/04/11 20:56:25  romy
// made it as a vxShareableObject
//
// Revision 1.2  2007/04/06 18:02:50  romy
// fixed the cad findings load
//
// Revision 1.1  2007/04/06 14:14:10  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_CadFindings.cpp,v 1.3 2007/04/11 20:56:25 romy Exp $
// $Id: v3D_CadFindings.cpp,v 1.3 2007/04/11 20:56:25 romy Exp $
