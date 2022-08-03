// $Id: v3D_NodeGlobalController.cpp,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson(mailto:gdavidson@viatronix.com)


#include "StdAfx.h"
#include "v3D_NodeGlobalController.h"
#include "vxNodeGlobalController.h"
#include "vxControllerGlobal.h"
#include "v3D_Interfaces.h"


// namespaces
USING_GRAPH_NS
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;


/**
 * default constructor
 */
NodeGlobalController::NodeGlobalController() :
  m_mpVisualizationGraph( nullptr ),
  m_mpGlobalControllers( gcnew Dictionary< System::Type ^, IGlobalControllerFactory ^ >() ),
  m_mpFunctionPtr( nullptr ),
  m_pNodeGlobalController( nullptr )
{
  m_mpFunctionPtr = gcnew System::Windows::Forms::MethodInvoker( this, &NodeGlobalController::OnClear );
  m_gcHandle = GCHandle::Alloc( m_mpFunctionPtr );

  System::IntPtr fp = Marshal::GetFunctionPointerForDelegate( m_mpFunctionPtr );
  m_pNodeGlobalController = new vxNodeGlobalController( reinterpret_cast< vxNodeGlobalController::ClearFuncPtr >( fp.ToPointer() ));
} // NodeGlobalController()


/**
 * destructor
 */
NodeGlobalController::~NodeGlobalController()
{
  m_gcHandle.Free();
  this->!NodeGlobalController();
} // ~NodeGlobalController()


/**
 * Finalizer
 */
NodeGlobalController::!NodeGlobalController()
{
  delete m_pNodeGlobalController; 
  m_pNodeGlobalController = NULL;
} // !NodeGlobalController()


/**
 * Adds the global controller to the collection.
 *
 * @param     mpFactoryType       Global Controller type
 */
void NodeGlobalController::AddGlobalController( System::Type ^ mpFactoryType )
{
  IGlobalControllerFactory ^ mpFactory( dynamic_cast< IGlobalControllerFactory ^ > ( System::Activator::CreateInstance( mpFactoryType )));
  vxControllerGlobal * pController( reinterpret_cast< vxControllerGlobal * >( mpFactory->CreateController().ToPointer() ));

  m_mpGlobalControllers->Add( mpFactoryType, mpFactory );
  m_pNodeGlobalController->AddController( ss( mpFactoryType->ToString() ), pController );
} // AddGlobalController( mpFactoryType )


/**
 * Removes the global controller to the collection.
 *
 * @param     mpFactoryType       Global Controller type
 */
void NodeGlobalController::RemoveGlobalController( System::Type ^ mpFactoryType )
{
  m_pNodeGlobalController->RemoveController( ss( mpFactoryType->ToString() ));
  m_mpGlobalControllers->Remove( mpFactoryType );
} // RemoveGlobalController( mpFactoryType )


/**
 * Checks if the global controller type is contained in the collection.
 *
 * @return    true if the global controller is in the collection; otherwise false
 */
bool NodeGlobalController::HasGlobalController( System::Type ^ mpFactoryType )
{
  return m_pNodeGlobalController->HasController( ss( mpFactoryType->ToString() ));
} // HasGlobalController( mpFactoryType )


/**
 * Clears all global controllers.
 */
void NodeGlobalController::ClearGlobalControllers()
{
  m_pNodeGlobalController->ClearControllers();
} // ClearGlobalControllers()


/**
 * clears the global controllers
 */
void NodeGlobalController::OnClear()
{
  m_mpGlobalControllers->Clear();
} // OnClear()


// $Log: v3D_NodeGlobalController.cpp,v $
// Revision 1.1  2007/05/03 21:43:16  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_NodeGlobalController.cpp,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
// $Id: v3D_NodeGlobalController.cpp,v 1.1 2007/05/03 21:43:16 gdavidson Exp $
