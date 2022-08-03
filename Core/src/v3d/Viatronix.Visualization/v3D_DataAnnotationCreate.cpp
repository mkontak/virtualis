// $Id: v3D_DataAnnotationCreate.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Michael Meissner (mailto:michael@viatronix.com)


// includes
#include "StdAfx.h"
#include "v3D_DataAnnotationCreate.h"


// namespaces
USING_VISUALIZATION_NS


/**
 * Initializes a Triplet object with default values.
 */
DataAnnotationCreate::DataAnnotationCreate()
//*******************************************************************
{
  m_pDataAnnotationCreate = new vxShareableObject<vxDataAnnotationCreate>();
}


/**
 * destructor
 */
DataAnnotationCreate::~DataAnnotationCreate()
//*******************************************************************
{
  this->!DataAnnotationCreate();
} // ~DataAnnotationCreate()


/**
 * Finalizer
 */
DataAnnotationCreate::!DataAnnotationCreate()
//*******************************************************************
{
  if (m_pDataAnnotationCreate->m_pAnnotation != NULL)
  {
    delete m_pDataAnnotationCreate->m_pAnnotation;
    m_pDataAnnotationCreate->m_pAnnotation = NULL;
  }

  delete m_pDataAnnotationCreate;
  m_pDataAnnotationCreate = NULL;
} // !DataAnnotationCreate()


/**
 * Adds an annotation using the specified factory object.
 * @param mpFactory Annotation factory object.
 */
void DataAnnotationCreate::AddAnnotation( IAnnotationFactory ^ mpFactory )
//*******************************************************************
{
  try
  {
    // create the unmanaged annotation using the factory
    vxAnnotation * pAnnotation = reinterpret_cast<vxAnnotation *>(mpFactory->CreateAnnotation().ToPointer());

    // add the annotation
    m_pDataAnnotationCreate->m_pAnnotation = pAnnotation;
  }
  catch( ex::VException & e )
  {
    throw gcnew v3DException( &e );
  }
} // Add()


/** 
 * Connects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void DataAnnotationCreate::Connect(System::IntPtr observer)
//*******************************************************************
{
  m_pDataAnnotationCreate->Connect(reinterpret_cast<vxObserver *>(observer.ToPointer()));
} // Connect()


/** 
 * Disconnects an observer from the object.
 * @param observer vxManagedObserver interested in events.
 */
void DataAnnotationCreate::Disconnect(System::IntPtr observer)
//*******************************************************************
{
  m_pDataAnnotationCreate->Disconnect(reinterpret_cast<vxObserver *>(observer.ToPointer()));
} // Disconnect()


/**
 * Processes modified queue events.
 * @param modifiedList Modified info list.
 */
void DataAnnotationCreate::OnModified(System::IntPtr modifiedList)
//*******************************************************************
{
} // OnModified()


// Revision History:
// $Log: v3D_DataAnnotationCreate.cpp,v $
// Revision 1.4  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.3  2006/09/27 17:23:48  geconomos
// renamed Observer to vxManagedObserver
//
// Revision 1.2  2005/05/24 13:50:02  michael
// Introduced concept of data objects to start with v3D_Data..../vxData...
// Has been done for DataColormapEditor, DataAnnotationCreate, and
// DataPlaneObliqueCreate
//
// Revision 1.1  2005/05/23 20:25:21  michael
// Introduced concept of data objects to start with v3D_Data....
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.4  2004/03/02 01:10:44  michael
// merged CONSOLE_2-0 changes back into trunc
//
// Revision 1.3.2.1  2004/02/24 20:11:24  michael
// took out code that was commented out
//
// Revision 1.3  2003/12/16 13:26:59  michael
// Changed the obersver function from Modified() to OnModified()
//
// Revision 1.2  2003/08/22 19:21:30  frank
// formatting
//
// Revision 1.1  2003/08/20 14:41:02  michael
// added managed plane oblique create data class
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_DataAnnotationCreate.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
// $Id: v3D_DataAnnotationCreate.cpp,v 1.4 2006/10/04 18:29:04 gdavidson Exp $
