// $Id: v3D_Animator.h,v 1.8 2007/01/31 15:04:05 romy Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// pragmas
#pragma once

// includes
#include "v3D_Environment.h"
#include "v3D_AnimationAction.h"
#include "v3D_AnimationCapture.h"


// unmanaged forward declarations
class vxRenderer;
class vxRenderingContextDIB;
class vxController;

// namespace declaration
OPEN_ANIMATION_NS

// class declaration
public ref class Animator : public System::IDisposable
{

// member functions
public:
  
  /// consturctor from action and capture
  Animator( AnimationAction ^ mpAction, AnimationCapture ^ mpCapture );
  
  /// consttuctor from action, capture and capture size
  Animator( AnimationAction ^ mpAction, AnimationCapture ^ mpCapture, int iWidth, int iHeight );
  
  /// destructor
  virtual ~Animator();

  /// finalizer
  !Animator();
  
  /// get the capture object
  property AnimationCapture ^ Capture { AnimationCapture ^ get() { return m_mpCapture; } }
  
  /// performs the animation
  void Animate();

  /// frees the opengl context
  void Free();
  
  /// adds a renderer to the animator by using the specified factory type
  void AddRenderer( System::Type ^ mpRendererFactoryType );

  /// adds a controller to the animator by using the specified factory type
  void AddController( System::Type ^ mpControllerFactoryType );
  
  /// create from xml
  static Animator ^ FromXml( System::Xml::XmlNode ^ mpParent,
                             Viatronix::Visualization::Environment ^ mpEnvironment,
                             System::String ^ mpFilePrefix );

  /// gets the preview filename
  property System::String ^ PreviewFilename
  { 
    System::String ^ get() { return m_mpPreviewFilename; }
    void set( System::String ^ mpPreviewFilename ) { m_mpPreviewFilename = mpPreviewFilename; }
  } 

// member variables
private:

  /// animation action
  Viatronix::Animations::AnimationAction ^ m_mpAction;

  /// animation capture
  Viatronix::Animations::AnimationCapture ^ m_mpCapture;

  /// rendering context
  vxRenderingContextDIB * m_pContext;
  
  /// list of renderers
  std::list< vxRenderer *  > * m_pRenderers;

  /// list of controllers
  std::list< vxController *  > * m_pControllers;
  
  /// capture width in pixels
  int m_iWidth;
  
  /// capture width in pixels
  int m_iHeight;

  /// supersampling factor (1 for no supersampling)
  int m_iSupersamplingFactor;

  /// output filename of the preview image
  System::String ^ m_mpPreviewFilename;

}; // class Animator

CLOSE_ANIMATION_NS

// $Log: v3D_Animator.h,v $
// Revision 1.8  2007/01/31 15:04:05  romy
// Added a Free method
//
// Revision 1.7  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.6  2004/06/16 19:40:01  frank
// added movie previews
//
// Revision 1.5  2004/06/15 14:53:29  frank
// added controllers to animation
//
// Revision 1.4  2004/05/19 14:15:54  frank
// added image supersampling factor option
//
// Revision 1.3  2004/05/11 14:41:03  frank
// cosmetics
//
// Revision 1.2  2004/05/06 18:22:15  frank
// added movie scripting
//
// Revision 1.1  2004/05/04 17:47:56  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Animator.h,v 1.8 2007/01/31 15:04:05 romy Exp $
// $Id: v3D_Animator.h,v 1.8 2007/01/31 15:04:05 romy Exp $
