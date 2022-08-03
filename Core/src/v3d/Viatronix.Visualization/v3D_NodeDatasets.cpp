// $Id: v3D_NodeDatasets.cpp,v 1.4.2.4 2008/11/19 19:10:08 kchalupa Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Greg Davidson(mailto:gdavidson@viatronix.com)


#include "StdAfx.h"
#include "v3D_NodeDatasets.h"
#include "vxNodeDatasetsCycler.h"
#include "v3D_Viewer.h"
#include "v3D_Dataset.h"
#include "v3D_Environment.h"
#include "v3D_VisualizationGraph.h"
#include "v3D_NodeRendering.h"
#include "v3D_VisualizationPreset.h"


// namespaces
using namespace System::Runtime::InteropServices;
USING_VISUALIZATION_NS
USING_GRAPH_NS


#define FILE_REVISION "$Revision: 1.4.2.4 $"


/**
 * default constructor
 */
NodeDatasets::NodeDatasets() :
  m_mpFunctionPtr( nullptr ),
  m_mpVisualizationGraph( nullptr ),
  m_pNodeDatasets( nullptr )
{
  m_mpFunctionPtr = gcnew System::Windows::Forms::MethodInvoker( this, &NodeDatasets::OnIndexChanged );
  m_gcHandle = GCHandle::Alloc( m_mpFunctionPtr );

  System::IntPtr fp = Marshal::GetFunctionPointerForDelegate( m_mpFunctionPtr );
  m_pNodeDatasets = new vxNodeDatasetsCycler( reinterpret_cast< vxNodeDatasetsCycler::IndexChangedFuncPtr >( fp.ToPointer() ));
} // NodeDatasets()


/**
 * destructor
 */
NodeDatasets::~NodeDatasets()
{
  m_gcHandle.Free();

  this->!NodeDatasets();
} // ~NodeDatasets()


/**
 * Finalizer
 */
NodeDatasets::!NodeDatasets()
{
  delete m_pNodeDatasets; 
  m_pNodeDatasets = NULL;
} // !NodeDatasets()


/**
 * raises index changed event
 */
void NodeDatasets::OnIndexChanged()
{
  DatasetChanged( this, System::EventArgs::Empty );
} // OnIndexChanged()


/**
 * syncs the viewer with the current dataset
 *
 * @param         mpViewer                managed viewer
 */
void NodeDatasets::SyncViewer( Viewer ^ mpViewer )
{
  Dataset ^ mpDataset = Dataset::GetDataset( m_pNodeDatasets->GetDatasetIndex() );
  
  SyncViewer( mpViewer, mpDataset );
} // SyncViewer( mpViewer )


/**
 * syncs the viewer with the current dataset
 *
 * @param         mpViewer                managed viewer
 */
void NodeDatasets::SyncViewer( Viewer ^ mpViewer, Dataset ^ mpDataset )
{
  mpViewer->Environment->Components = mpDataset->Components;
  mpViewer->Environment->Annotations = mpDataset->Annotations;
  mpViewer->Environment->AssociatedDataset = mpDataset;
  mpViewer->Environment->Volume = mpDataset->IntensityVolume;
  mpViewer->Environment->LabelVolume = mpDataset->LabelVolume;
  mpViewer->Environment->CadFindings = mpDataset->CadFindings;
  // Causes problems when heartbeat ceases, changes the preset when not explicitly desired.
  // mpViewer->Environment->Colormap = ( mpViewer->Is3D ) ? mpDataset->VisualizationPreset3d->Colormap : mpDataset->VisualizationPreset2d->Colormap;

  // copy over the extended elements
  System::Collections::IDictionaryEnumerator ^ mpEnumerator = mpDataset->ExtendedElements->GetEnumerator();
  while( mpEnumerator->MoveNext() )
  {
    if( dynamic_cast< Viatronix::Visualization::IElement ^ >( mpEnumerator->Value ) )
      mpViewer->Environment->AddElement( safe_cast< System::String ^ >( mpEnumerator->Key ), safe_cast< Viatronix::Visualization::IElement ^ >( mpEnumerator->Value ));
  }
} // SyncViewer( mpViewer, mpDataset )


// $Log: v3D_NodeDatasets.cpp,v $
// Revision 1.4.2.4  2008/11/19 19:10:08  kchalupa
// Issue #6208: Synching of colormap was removed.
//
// Revision 1.4.2.3  2008/11/18 15:18:31  gdavidson
// Sync ColorMaps
//
// Revision 1.4.2.2  2008/11/17 16:05:39  kchalupa
// Undo previous checkin
//
// Revision 1.4.2.1  2008/11/17 15:22:55  kchalupa
// Fix for crash when only one dataset is present.
//
// Revision 1.4  2007/09/19 14:34:37  gdavidson
// Synchronized extended elements when calling SyncViewer
//
// Revision 1.3  2007/08/21 19:41:07  gdavidson
// Set annotations in SyncViewer method
//
// Revision 1.2  2007/06/22 15:16:41  geconomos
// Renamed vxNodeDatasets => vxNodeDatasetsCycler
//
// Revision 1.1  2007/05/03 21:43:16  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_NodeDatasets.cpp,v 1.4.2.4 2008/11/19 19:10:08 kchalupa Exp $
// $Id: v3D_NodeDatasets.cpp,v 1.4.2.4 2008/11/19 19:10:08 kchalupa Exp $
