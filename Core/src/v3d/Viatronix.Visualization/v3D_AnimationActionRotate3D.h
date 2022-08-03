// $Id: v3D_AnimationActionRotate3D.h,v 1.7 2007/07/16 20:01:14 romy Exp $
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


// includes
#include "v3D_AnimationAction.h"


// namespaces
OPEN_ANIMATION_NS


// managed foward declarations
ref class Environment;


// enum declaration
public enum class RotationAxis
{
  x,
  y,
  z,
}; // enum RotationAxis


// class declaration
public ref class AnimationActionRotate3D : 
  public Viatronix::Animations::AnimationAction
{

// member functions
public:

  /// default constructor
  AnimationActionRotate3D();

  /// initializes a AnimationActionRotate3D object the specified environment
  AnimationActionRotate3D( Viatronix::Visualization::Environment ^ mpEnvironment );
  
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
  virtual int GetFramesToRender() override;

  /// gets the rotation axis
  property Viatronix::Animations::RotationAxis RotationAxis
  {
    Viatronix::Animations::RotationAxis get() { return m_eRotationAxis; }
    void set( Viatronix::Animations::RotationAxis eAxis ) { m_eRotationAxis = eAxis; }
  }
  
  /// gets the number of frames in animation
  property int NumberFrames
  {
    int get() { return m_iNumberFrames; }
    void set( int iNumberFrames ) { m_iNumberFrames = iNumberFrames; }
  }

  /// gets the number of degrees rotation
  property int DegreesRotation
  {
    int get() { return m_iDegreesRotation; }
    void set( int iDegreesRotation ) { m_iDegreesRotation = iDegreesRotation; }
  }

  /// intialize further from xml node
  virtual void ConfigureFromXml( System::Xml::XmlNode ^ mpParent ) override;

// member functions
private:

  /// number of frames in animation
  float m_iNumberFrames;
  
  /// axis to rotate about
  Viatronix::Animations::RotationAxis m_eRotationAxis;
  
 
  /// the number of degrees to rotate
  int4 m_iDegreesRotation;

}; // class AnimationActionRotate3D


// namespaces
CLOSE_ANIMATION_NS


// $Log: v3D_AnimationActionRotate3D.h,v $
// Revision 1.7  2007/07/16 20:01:14  romy
// added beating in animation
//
// Revision 1.6  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.5  2006/08/01 15:14:58  gdavidson
// Changed ArrayList to List<>
//
// Revision 1.4  2005/11/18 21:12:28  frank
// Merged from PRE_CON_3-0_to_trunk
//
// Revision 1.3.12.2  2005/07/08 16:45:43  frank
// permitted less than 360 degrees rotation
//
// Revision 1.3.12.1  2005/07/08 15:49:19  frank
// added 4D animation
//
// Revision 1.3  2004/05/06 18:22:15  frank
// added movie scripting
//
// Revision 1.2  2004/05/04 17:47:13  geconomos
// Changed Viewer parameter to type Environment.
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/08/22 03:39:41  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationActionRotate3D.h,v 1.7 2007/07/16 20:01:14 romy Exp $
// $Id: v3D_AnimationActionRotate3D.h,v 1.7 2007/07/16 20:01:14 romy Exp $
