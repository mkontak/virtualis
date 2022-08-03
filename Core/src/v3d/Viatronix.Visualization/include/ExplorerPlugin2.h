// $Id: ExplorerPlugin2.h,v 1.6 2007/05/03 21:26:09 gdavidson Exp $
//
// Copyright(c)2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.net )


#pragma once
#pragma warning ( disable : 4290 )

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif

// common vxBase headers
#include "vxViewerLibrary.h"
#include "vxEnvironment.h"
#include "vxIDs.h"
#include "vxVisualizationPreset.h"
#include "vxPreset.h"
#include "vxDataAnnotationCreate.h"
#include "vxDataPlaneObliqueCreate.h"
#include "vxDataRenderer.h"
#include "vxVessel.h"
#include "vxDOM.h"
#include "vxCadFindings.h"

#include <vcclr.h>

#ifdef GetObject
#undef GetObject
#endif


#pragma managed

/**
 * Converts a System::String to a std::string
 *
 * @param string input System::String *
 * @return converted std::string
 */
inline const std::string ss( System::String ^ string )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );

  std::string converted = ( const char * ) buffer.ToPointer();
	
  Marshal::FreeHGlobal( buffer );

  return converted;
}

/**
 * Wrapper class for unmanaged objects
 */
template< class SharedObject >
class _native_ptr_t 
{
public:

  _native_ptr_t( System::IntPtr ptr )
  {
    m_p = static_cast< vxShareableObject< SharedObject > *>( ptr.ToPointer() );
  }

  vxShareableObject< SharedObject > * operator->()
  { 
    return m_p; 
  }
  
  operator vxShareableObject< SharedObject > * ()
  { 
    return m_p; 
  }

private:
  
  vxShareableObject< SharedObject > * m_p;
};


inline vxShareableObject< vxBlockVolume< int2 > > * ToNativeVolumeInt2Ptr( System::IntPtr ptr )
{
  return _native_ptr_t< vxBlockVolume< int2 > >( ptr );
}

inline vxShareableObject< vxBlockVolume< uint2 > > * ToNativeVolumeUint2Ptr( System::IntPtr ptr )
{
  return _native_ptr_t< vxBlockVolume< uint2 > >( ptr );
}

inline vxShareableObject< vxCamera > * ToNativeCameraPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxCamera >( ptr );
}

inline vxShareableObject< vxColormap > * ToNativeColormapPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxColormap >( ptr );
}

inline vxShareableObject< vxDataRendererSoftware > * ToNativeDataRendererSoftwarePtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxDataRendererSoftware >( ptr );
}

inline vxShareableObject< vxVisualizationPreset > * ToNativeVisualizationPresetPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxVisualizationPreset >( ptr );
}

inline vxShareableObject< vxPreset > * ToNativePresetPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxPreset >( ptr );
}

inline vxShareableObject< vxEnvironment > * ToNativeEnvironmentPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxEnvironment >( ptr );
}

inline vxShareableObject< vxComponentArray > * ToNativeComponentsPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxComponentArray >( ptr );
}

inline vxShareableObject< vxCropBox > * ToNativeCropBoxPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxCropBox >( ptr );
}

inline vxShareableObject< Triple<float4> > * ToNativeTripletPtr( System::IntPtr ptr )
{
  return _native_ptr_t< Triple<float4> >( ptr );
}

inline vxShareableObject< vxPlaneOrtho<float4> > * ToNativePlaneOrthoPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxPlaneOrtho<float4> >( ptr );
}

inline vxShareableObject< vxPlaneOblique<float4> > * ToNativePlaneObliquePtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxPlaneOblique<float4> >( ptr );
}

inline vxShareableObject< vxPlaneCurved> * ToNativePlaneCurvedPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxPlaneCurved >( ptr );
}

inline vxShareableObject< vxAnnotations> * ToNativeAnnotationsPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxAnnotations >( ptr );
}

inline vxShareableObject< vxCAD::vxCadFindings> * ToNativeCadFindingsPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxCAD::vxCadFindings >( ptr );
}

inline vxShareableObject< vxRenderingMode > * ToNativeRenderingModePtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxRenderingMode >( ptr );
}

inline vxShareableObject< vxEventTimer > * ToNativeEventTimerPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxEventTimer >( ptr );
}

inline vxShareableObject< Triple< float4 > > * ToNativeTriplePtr( System::IntPtr ptr )
{
  return _native_ptr_t< Triple< float4 > >( ptr );
}

inline vxShareableObject<vxCursor3d> * ToNativeCursor3dPtr( System::IntPtr ptr )
{
  return _native_ptr_t<vxCursor3d>( ptr );
}

inline vxShareableObject< InteractiveSegmentation > * ToNativeInteractiveSegmentationPtr( System::IntPtr ptr )
{
  return _native_ptr_t< InteractiveSegmentation >( ptr );
}


inline vxShareableObject< vxContourList > * ToNativeContourListPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxContourList >( ptr );
}

inline vxShareableObject< vxVessel > * ToNativeVesselPtr( System::IntPtr ptr )
{
  return _native_ptr_t< vxVessel >( ptr );
}

inline vxShareableObject<vxDataPlaneObliqueCreate> * ToNativeDataPlaneObliqueCreatePtr( System::IntPtr ptr )
{
  return _native_ptr_t<vxDataPlaneObliqueCreate>( ptr );
}

inline vxShareableObject<vxDataAnnotationCreate> * ToNativeDataAnnotationCreatePtr( System::IntPtr ptr )
{
  return _native_ptr_t<vxDataAnnotationCreate>( ptr );
}

inline vxShareableObject<vxFontInfo> * ToNativeFontInfoPtr( System::IntPtr ptr )
{
  return _native_ptr_t<vxFontInfo>( ptr );
}

inline vxShareableObject<vxZoomFactor> * ToNativePtrZoomFactor( System::IntPtr ptr )
{
  return _native_ptr_t<vxZoomFactor>( ptr );
}


inline vxShareableObject<Point3Df> * ToNativePoint3DfPtr( System::IntPtr ptr )
{
  return _native_ptr_t<Point3Df>( ptr );
}


inline vxShareableObject< vxMomentum > * ToNativeMomentumPtr( System::IntPtr ptr )
{
  return _native_ptr_t<vxMomentum>( ptr );
}


inline HMODULE GetModuleHandle()
{
  static int dummy = 0;
  
  MEMORY_BASIC_INFORMATION mbi;
  VirtualQuery( &dummy, &mbi, sizeof(mbi) );
  
  return reinterpret_cast<HMODULE>( mbi.AllocationBase );
}

/*
 * The preferred way to define a managed class wrapper for a vxElement.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_SIMPLE_ELEMENT( managedClassName ) \
public ref class  managedClassName :\
  IElement,\
  IObserver\
{\
public:\
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pObject ); }\
  virtual void Connect( System::IntPtr observer )\
  {\
    m_pObject->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );\
  }\
  virtual void Disconnect( System::IntPtr observer )\
  {\
    m_pObject->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );\
  }\
  virtual event System::EventHandler ^ Modified;\
public:\
  virtual void OnModified( System::IntPtr ptrInfo ) {}\
public:\
  managedClassName( )\
  {\
    m_pObject = new vxShareableObject< vx##managedClassName >();\
    m_pObserver = new vxManagedObserver( this );\
    m_pObject->Connect( m_pObserver );\
  }\
  ~managedClassName()\
  {\
    this->!managedClassName();\
  }\
  !managedClassName()\
  {\
    m_pObject->Disconnect( m_pObserver );\
    delete m_pObserver;\
    delete m_pObject;\
  }\
private:\
  vxShareableObject< vx##managedClassName > * m_pObject;\
  vxManagedObserver * m_pObserver;\
};\
\
  inline vxShareableObject< vx##managedClassName > * ToNativePtr##managedClassName( System::IntPtr ptr )\
{\
  return _native_ptr_t< vx##managedClassName >( ptr );\
}


/*
 * The preferred way to define a managed class wrapper for a vxElement.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_SIMPLE_ELEMENT2( managedClassName, unmanagedClassName ) \
public ref class  managedClassName :\
  Viatronix::Visualization::IElement,\
  Viatronix::Visualization::IObserver\
{\
public:\
  virtual System::IntPtr GetElementPtr() { return System::IntPtr( m_pObject ); }\
  virtual void Connect( System::IntPtr observer )\
  {\
    m_pObject->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );\
  }\
  virtual void Disconnect( System::IntPtr observer )\
  {\
    m_pObject->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );\
  }\
  virtual event System::EventHandler ^ Modified;\
public:\
  virtual void OnModified( System::IntPtr ptrInfo ) {}\
public:\
  managedClassName( )\
  {\
    m_pObject = new vxShareableObject< unmanagedClassName >();\
    m_pObserver = new vxManagedObserver( this );\
    m_pObject->Connect( m_pObserver );\
  }\
  ~managedClassName()\
  {\
    this->!managedClassName();\
  }\
  !managedClassName()\
  {\
    m_pObject->Disconnect( m_pObserver );\
    delete m_pObserver;\
    delete m_pObject;\
  }\
private:\
  vxShareableObject< unmanagedClassName > * m_pObject;\
  vxManagedObserver * m_pObserver;\
};\
\
inline vxShareableObject< unmanagedClassName > * ToNativePtr##unmanagedClassName( System::IntPtr ptr )\
{\
  return _native_ptr_t< unmanagedClassName >( ptr );\
}


/*
 * The preferred way to define a managed class wrapper for a vxVisualizationGraphNode.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_SIMPLE_NODE( managedClassName, unmanagedClassName ) \
public ref class  managedClassName :\
  Viatronix::Visualization::Graph::IVisualizationGraphNode\
{\
public:\
  ~managedClassName()\
  {\
    this->!managedClassName();\
  }\
  !managedClassName()\
  {\
    if( m_pObject != NULL )\
      delete m_pObject;\
    m_pObject = NULL;\
  }\
  virtual property System::String ^ ID { System::String ^ get() { return NodeIDs::managedClassName; } }\
  virtual property Viatronix::Visualization::Graph::VisualizationGraph ^ VisualizationGraph\
  {\
    Viatronix::Visualization::Graph::VisualizationGraph ^ get() { return m_mpVisualizationGraph; }\
    void set( Viatronix::Visualization::Graph::VisualizationGraph ^ mpGraph ) { m_mpVisualizationGraph = mpGraph; }\
  }\
  virtual System::IntPtr GetNodePtr() { return System::IntPtr( m_pObject ); }\
internal:\
  managedClassName() :\
    m_pObject( new unmanagedClassName() ),\
    m_mpVisualizationGraph( nullptr )\
  {\
  }\
private:\
  Viatronix::Visualization::Graph::VisualizationGraph ^ m_mpVisualizationGraph;\
  unmanagedClassName * m_pObject;\
};


/*
 * The preferred way to define non-primary renderer
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_RENDERER( MANAGED_CLASS_NAME )\
public ref class MANAGED_CLASS_NAME : Viatronix::Visualization::IRendererFactory\
{\
public:\
  virtual System::IntPtr CreateRenderer( System::IntPtr env )\
  {\
    return System::IntPtr( new vx##MANAGED_CLASS_NAME( *reinterpret_cast< vxEnvironment * >( env.ToPointer() ) ) );\
  }\
};

/*
 * The preferred way to define a manipulator.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_MANIPULATOR( MANAGED_CLASS_NAME )\
public ref class MANAGED_CLASS_NAME : Viatronix::Visualization::IManipulatorFactory\
 {\
 public:\
   virtual System::IntPtr CreateManipulator( System::IntPtr env )\
   {\
     return System::IntPtr( new vx##MANAGED_CLASS_NAME( *reinterpret_cast< vxEnvironment * >( env.ToPointer() ) ) );\
   }\
 };

/**
 * The preferred way to define a controller.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_LOCAL_CONTROLLER( MANAGED_CLASS_NAME )\
  public ref class MANAGED_CLASS_NAME : Viatronix::Visualization::IControllerFactory\
 {\
 public:\
   virtual System::IntPtr CreateController( System::IntPtr env  )\
   {\
     return System::IntPtr( new vx##MANAGED_CLASS_NAME( *reinterpret_cast< vxEnvironment * >( env.ToPointer() ) ) );\
   }\
 };


/**
 * The preferred way to define a controller.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_GLOBAL_CONTROLLER( MANAGED_CLASS_NAME )\
public ref class MANAGED_CLASS_NAME : Viatronix::Visualization::IGlobalControllerFactory\
 {\
 public:\
   virtual System::IntPtr CreateController()\
   {\
     return System::IntPtr( new vx##MANAGED_CLASS_NAME() );\
   }\
 };


/*
 * The preferred way to define a annotation.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_ANNOTATION( MANAGED_CLASS_NAME )\
public ref class MANAGED_CLASS_NAME : Viatronix::Visualization::IAnnotationFactory\
 {\
 public:\
   virtual System::IntPtr CreateAnnotation(  )\
   {\
     return System::IntPtr( new vx##MANAGED_CLASS_NAME() );\
   }\
 };

/*
 * The preferred way to define a viewer type.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_VIEWER_TYPE( MANAGED_CLASS_NAME )\
public ref class MANAGED_CLASS_NAME : Viatronix::Visualization::IViewerType\
{\
public:\
  MANAGED_CLASS_NAME( Viatronix::Visualization::Environment ^ env )\
  {\
    m_pimpl =  new vx##MANAGED_CLASS_NAME( * ToNativeEnvironmentPtr( env->GetEnvironmentPtr() ) );\
  }\
  ~MANAGED_CLASS_NAME()\
  {\
    this->!##MANAGED_CLASS_NAME();\
  }\
  !MANAGED_CLASS_NAME()\
  {\
    if( m_pimpl != NULL )\
    {\
      delete m_pimpl;\
      m_pimpl = NULL;\
    }\
  }\
  virtual System::IntPtr GetViewerTypePtr()\
  {\
    return System::IntPtr( m_pimpl );\
  }\
  virtual void InitializeView()\
  {\
    m_pimpl->InitializeView();\
  }\
  virtual property bool Is2D\
  {\
    bool get() { return ( m_pimpl->Is2D() == 1 ) ? true : false; }\
  }\
  virtual property bool Is3D\
  {\
    bool get() { return ( m_pimpl->Is3D() == 1 ) ? true : false; }\
  }\
  virtual System::String ^ ToString() override\
  {\
    return gcnew System::String( m_pimpl->GetClassString().c_str() );\
  }\
private:\
    vx##MANAGED_CLASS_NAME * m_pimpl;\
 };


#pragma region revision history

// $Log: ExplorerPlugin2.h,v $
// Revision 1.6  2007/05/03 21:26:09  gdavidson
// Added DEFINE_SIMPLE_NODE
//
// Revision 1.5  2007/04/11 20:54:42  romy
// ToNativeCadFindingsPtr added
//
// Revision 1.4  2007/03/15 15:12:50  geconomos
// added fully qualified types in macros
//
// Revision 1.3  2007/03/01 20:29:01  gdavidson
// Changed ownership
//
// Revision 1.2  2007/01/16 17:12:46  gdavidson
// Added ToNativeMomentumPtr
//
// Revision 1.1  2006/12/18 16:45:03  gdavidson
// moved
//
// Revision 1.4  2006/11/09 15:08:37  gdavidson
// Added DEFINE_GLOBAL_CONTROLLER
//
// Revision 1.3  2006/11/01 22:33:48  gdavidson
// Added ToNativePoint3DfPtr
//
// Revision 1.2  2006/10/04 18:43:56  gdavidson
// Ported to C++/CLI
//
// Revision 1.1  2006/08/01 15:12:07  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/include/ExplorerPlugin2.h,v 1.6 2007/05/03 21:26:09 gdavidson Exp $
// $Id: ExplorerPlugin2.h,v 1.6 2007/05/03 21:26:09 gdavidson Exp $

#pragma endregion
