// $Id: vxNodeDatasetsCycler.h,v 1.3 2007/09/17 18:50:45 geconomos Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos (mailto:geconomos@viatronix.com)

// pragmas
#pragma once


// includes
#include "vxVisualizationGraphNode.h"
#include "vxEnvironment.h"


// forward declarations
template <typename T > class Image;
template <typename T > class PixelRGBA;
class vxViewer;
class vxNodeDatasetsCyclerThread;
class vxCycleSettings;
class vxDataset;


/**
 *
 */
class VX_VIEWER_DLL vxNodeDatasetsCycler : public vxVisualizationGraphNode
{
// enums, typedefs and structs
public:

  /// current buffer state enum
  enum CacheBufferStateEnum { EMPTY, LQ, HQ };

  /// cache buffer information
  struct vxCacheBuffer
  {
    /// low quality buffer
    vxRenderBuffer m_buffer;

    /// state
    CacheBufferStateEnum m_eState;

    /// accumulated bit cache 
    std::bitset< vxEnvironment::ALL > m_bits;
  }; // struct vxCacheBuffer

  /// datasets cache
  typedef  std::vector< std::vector< vxCacheBuffer * > > vxCacheBuffers;

  ///  viewer collection
  typedef  std::vector< vxViewer * > vxViewerCollection;

  /// cycle settings collection
  typedef std::vector< vxCycleSettings * > vxCycleSettingsCollection;

  /// function pointer for the index changed method
  typedef void (__stdcall* IndexChangedFuncPtr )();

// functions
public:

  /// default constructor
  vxNodeDatasetsCycler();

  /// constructor
  vxNodeDatasetsCycler( IndexChangedFuncPtr pFuncPtr );

  /// destructor
  virtual ~vxNodeDatasetsCycler();

  /// current dataset index
  inline const int4 GetDatasetIndex() const { return m_iCurrentDatasetIndex; }

  /// set dataset index
  inline void SetDatasetIndex( const int4 iIndex ) { m_iCurrentDatasetIndex = iIndex; }

  /// initializes the node
  void Initialize();

  /// executes the node
  void Run();

  /// aborts the node
  void Abort();

  /// resets the node
  void Reset();

  /// returns a flag representing if the node is enabled/disabled
  inline const bool IsEnabled() const { return m_bNodeEnabled; }

  /// sets the node enabled disabled
  inline void SetEnabled( const bool bEnabled ) { m_bNodeEnabled = bEnabled; }

  /// gets the cache
  vxCacheBuffers & GetCache() { return m_cache; }

// member variables
private:

  /// cache bufers -  for each viewer/dataset two images are stored (low quality/high quality)
  vxCacheBuffers m_cache;

  /// cache of viewsviewers
  vxViewerCollection m_viewers;

  /// quick cache of cycle settings
  vxCycleSettingsCollection m_cycleSettings;

  /// is node enabled
  bool m_bNodeEnabled;
  
  /// thread that generates high quality images
  vxNodeDatasetsCyclerThread * m_pDatasetsCyclerThread;

  /// current dataset index
  int4 m_iCurrentDatasetIndex;

  /// callback function for when the dataset index changes
  IndexChangedFuncPtr m_pIndexChangedFuncPtr;

  /// indicates if the datasets thread is currently running
  bool m_bThreadRunning;

  /// used for maintaining cycles per second
  Timer m_timer;

}; // class vxNodeDatasetsCycler


// $Log: vxNodeDatasetsCycler.h,v $
// Revision 1.3  2007/09/17 18:50:45  geconomos
// removed double buffers from vxCacheBuffer
//
// Revision 1.2  2007/07/02 18:30:11  geconomos
// added "cycles per minute" support
//
// Revision 1.1  2007/06/22 15:15:49  geconomos
// - renamed vxNodeDatasets => vxNodeDatasetsCycler
// - reverted to RSNA version
//
// $Header: /CVS/cvsRepository/V3D/src/vxViewer/include/vxNodeDatasetsCycler.h,v 1.3 2007/09/17 18:50:45 geconomos Exp $
// $Id: vxNodeDatasetsCycler.h,v 1.3 2007/09/17 18:50:45 geconomos Exp $
