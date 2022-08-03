// $Id: v3D_AnimationAction3DPath.h,v 1.3 2006/10/04 18:29:03 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

#pragma once


// includes
#include "v3D_AnimationAction.h"


// namespaces
OPEN_ANIMATION_NS


// managed foward declarations
ref class Environment;


// class declaration
public ref class AnimationAction3DPath : 
  public Viatronix::Animations::AnimationAction
{

// member functions
public:

  /// default constructor
  AnimationAction3DPath();

  /// initializes a AnimationAction3DPath object the specified environment
  AnimationAction3DPath( Viatronix::Visualization::Environment ^ mpEnvironment );
  
  /// called when the environment is to be initialized
  virtual void OnInitializeEnvironment() override;
  
  /// called when the environment's state is to be restored
  virtual void OnRestoreEnvironment() override;
  
  // called before the first frame is rendered
  virtual void OnBeginAnimation( bool bPreview ) override;

  // called after the last frame is rendered
  virtual void OnEndAnimation( bool bPreview ) override;

  /// called when the before frame is rendered
  virtual void OnPreRenderFrame( bool bPreview, int iFrameNumber ) override;

  /// gets the total number of frames for the animation
  virtual int GetFramesToRender() override { return m_iNumberFrames; }

  /// gets the number of images
  property int NumberFrames
  {
    int get() { return m_iNumberFrames; }
    void set( int iNumberFrames ) { m_iNumberFrames = iNumberFrames; }
  }

  /// gets the forward flight direction property
  property bool ForwardFlight
  {
    bool get() { return m_bForward; }
    void set( bool bForward ) { m_bForward = bForward; }
  }

  /// intialize further from xml node
  virtual void ConfigureFromXml( System::Xml::XmlNode ^ mpParent ) override;

// member functions
private:

  /// number of frames
  float m_iNumberFrames;
  
  /// indicates if the patient info was displaying on initialization
  bool m_bShowPatientInfo;
  
  /// indicates if the image info was displaying on initialization
  bool m_bShowImageInfo;

  /// if the flight should proceed in the forward direction
  bool m_bForward;
    
}; // class AnimationAction3DPath


CLOSE_ANIMATION_NS


// $Log: v3D_AnimationAction3DPath.h,v $
// Revision 1.3  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.2  2004/08/09 12:21:23  frank
// added forward/backward flight
//
// Revision 1.1  2004/06/15 15:11:56  frank
// initial version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationAction3DPath.h,v 1.3 2006/10/04 18:29:03 gdavidson Exp $
// $Id: v3D_AnimationAction3DPath.h,v 1.3 2006/10/04 18:29:03 gdavidson Exp $
