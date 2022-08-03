// $Id: v3D_AnimationAction.h,v 1.6 2007/07/16 20:01:14 romy Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// pragmas
#pragma once

// foward declaration
#include "v3D_Environment.h"

// namepaces
OPEN_ANIMATION_NS

/**
 * Provides an interface for an animating a viewer.
 */
public ref class AnimationAction abstract
{  

// events
public:

  /// raised when the environment is to be initialied
  event System::EventHandler ^ InitializeEnvironment;

  /// raised when the environment is to be restored
  event System::EventHandler ^ RestoreEnvironment;
  
  /// raised when the animation has started
  event System::EventHandler ^ AnimationBegin;
  
  /// raised when the animation has ended
  event System::EventHandler ^ AnimationEnd;

// member functions
public:

  /// default constructor
  AnimationAction();

  /// initializes an AnimationAction object with default values
  AnimationAction( Viatronix::Visualization::Environment ^ mpEnvironment );

  /// called when the environment is to be initialized
  virtual void OnInitializeEnvironment();
  
  /// called when the environment's state is to be restored
  virtual void OnRestoreEnvironment();

  // called before the first frame is rendered
  virtual void OnBeginAnimation( bool bPreview );  
  
  // called after the last frame is rendered
  virtual void OnEndAnimation( bool bPreview );

  /// called before the next frame is to be rendered
  virtual void OnPreRenderFrame( bool bPreview, int iFrameNumber ) {}

  /// called when the frame is to be rendered
  virtual void OnRenderFrame( bool bPreview, int iFrameNumber ) {}

  /// gets the total number of frames for the animation
  virtual int GetFramesToRender() { return 0; }

	/// gets the list of datasets to cycle through during animation
  property System::Collections::Generic::List< Viatronix::Visualization::Dataset ^ > ^ DatasetList
  {
    System::Collections::Generic::List< Viatronix::Visualization::Dataset ^ > ^ get() { return m_mpDatasetList; }
    void set( System::Collections::Generic::List< Viatronix::Visualization::Dataset ^ > ^ mpDatasetList ) { m_mpDatasetList = mpDatasetList; }
  }
  
	/// gets whether we should cycle the datasets during animation
  property bool CycleDatasets
  {
    bool get() { return m_bCycleDatasets; }
    void set( bool bCycleDatasets ) { m_bCycleDatasets = bCycleDatasets; }
  }
  
  /// gets or sets the environment for the animation
  property Viatronix::Visualization::Environment ^ Environment
  { 
    Viatronix::Visualization::Environment ^ get() { return m_mpEnvironment; }
    void set( Viatronix::Visualization::Environment ^ mpEnvironment ) { m_mpEnvironment = mpEnvironment; }
  }
  
  /// create from xml
  static AnimationAction ^ FromXml( System::Xml::XmlNode ^ mpParent, Viatronix::Visualization::Environment ^ mpEnvironment );
  
  /// intialize further from xml node
  virtual void ConfigureFromXml( System::Xml::XmlNode ^ mpParent ) {}

// member variables
protected:
		/// if we are to cycle through the known datasets during the animation
  bool m_bCycleDatasets;
	
  /// a collection of datasets to cycle through during animation
  System::Collections::Generic::List< Viatronix::Visualization::Dataset ^ > ^ m_mpDatasetList;

	/// the current dataset index (used to cycle through the datasets)
  int4 m_iDatasetIndex;

private:

  /// environment object to animate
  Viatronix::Visualization::Environment ^ m_mpEnvironment;

}; // class Animation

CLOSE_ANIMATION_NS

// $Log: v3D_AnimationAction.h,v $
// Revision 1.6  2007/07/16 20:01:14  romy
// added beating in animation
//
// Revision 1.5  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.4  2004/05/06 18:22:15  frank
// added movie scripting
//
// Revision 1.3  2004/05/05 13:19:00  frank
// removed helper function so managed c++ assemblies can use this class
//
// Revision 1.2  2004/05/04 17:47:13  geconomos
// Changed Viewer parameter to type Environment.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.2  2003/09/26 11:40:06  geconomos
// Added events
//
// Revision 1.1  2003/08/22 03:39:41  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationAction.h,v 1.6 2007/07/16 20:01:14 romy Exp $
// $Id: v3D_AnimationAction.h,v 1.6 2007/07/16 20:01:14 romy Exp $
