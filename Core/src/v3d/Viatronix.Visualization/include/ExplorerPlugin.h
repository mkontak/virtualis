// $Id: ExplorerPlugin.h,v 1.3 2007/05/03 21:26:09 gdavidson Exp $
//
// Copyright(c)2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: George Economos ( mailto:george@viatronix.net )
//
// Complete history at bottom of file.

#pragma once
#pragma warning ( disable : 4290 )

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
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
#include "vxRibbon.h"
#include "vxDataRendererSoftware.h"


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
inline const std::string ss( System::String * string )
{
  using namespace System::Runtime::InteropServices;

  System::IntPtr buffer = Marshal::StringToHGlobalAnsi( string );

  std::string converted = ( const char* ) buffer.ToPointer();
	
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

inline vxShareableObject< vxRibbon > * ToNativePtrRibbon( System::IntPtr ptr )
{
  return _native_ptr_t< vxRibbon >( ptr );
}

inline vxShareableObject< vxDataRendererSoftware > * ToNativePtrDataRendererSoftware( System::IntPtr ptr )
{
  return _native_ptr_t< vxDataRendererSoftware >( ptr );
}

inline vxShareableObject<vxMomentum> * ToNativeMomentumPtr( System::IntPtr ptr )
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
public __gc class  managedClassName :\
  public IElement,\
  public IObserver\
{\
public:\
  System::IntPtr GetElementPtr() { return System::IntPtr( m_pObject ); }\
  void Connect( System::IntPtr observer )\
  {\
    m_pObject->Connect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );\
  }\
  void Disconnect( System::IntPtr observer )\
  {\
    m_pObject->Disconnect( reinterpret_cast< vxManagedObserver * >( observer.ToPointer() ) );\
  }\
  /*__event System::EventHandler * Modified; <- this macro is buggy - we expand it out ourselves below\
  */void remove_Modified(System::EventHandler * eh)\
  {\
    Modified = static_cast<System::EventHandler*> (System::Delegate::Remove(Modified, eh));\
  }\
  void add_Modified(System::EventHandler * eh)\
  {\
    Modified = static_cast<System::EventHandler*> (System::Delegate::Combine(Modified, eh));\
  }\
public:\
  void OnModified( System::IntPtr ptrInfo ) {}\
public:\
  managedClassName( )\
  {\
    m_pObject = new vxShareableObject< vx##managedClassName >();\
    m_pObserver = new vxManagedObserver( this );\
    m_pObject->Connect( m_pObserver );\
  }\
  ~managedClassName()\
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
 * The preferred way to define a managed class wrapper for a vxVisualizationGraphNode.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_SIMPLE_NODE( managedClassName, unmanagedClassName ) \
public __gc class  managedClassName :\
  public Viatronix::Visualization::Graph::IVisualizationGraphNode\
{\
public:\
  ~managedClassName()\
  {\
    if( m_pObject != NULL )\
      delete m_pObject;\
    m_pObject = NULL;\
  }\
  virtual __property System::String * get_ID() { return NodeIDs::managedClassName; } }\
  virtual __property Viatronix::Visualization::Graph::VisualizationGraph * get_VisualizationGraph()\
  {\
    return m_mpVisualizationGraph;\
  }\
  virtual __property void set_VisualizationGraph( Viatronix::Visualization::Graph::VisualizationGraph * m_mpGraph )\
  {\
    m_mpVisualizationGraph = mpGraph;\
  }\
  virtual System::IntPtr GetNodePtr() { return System::IntPtr( m_pObject ); }\
public private:\
  managedClassName() :\
    m_pObject( new unmanagedClassName() ),\
    m_mpVisualizationGraph( NULL )\
  {\
  }\
private:\
  Viatronix::Visualization::Graph::VisualizationGraph * m_mpVisualizationGraph;\
  unmanagedClassName * m_pObject;\
};


/*
 * The preferred way to define non-primary renderer
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_RENDERER( MANAGED_CLASS_NAME )\
public __gc class MANAGED_CLASS_NAME : public IRendererFactory\
{\
public:\
  System::IntPtr CreateRenderer( System::IntPtr env )\
  {\
    return System::IntPtr( new vx##MANAGED_CLASS_NAME( *reinterpret_cast< vxEnvironment * >( env.ToPointer() ) ) );\
  }\
};

/*
 * The preferred way to define a manipulator.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_MANIPULATOR( MANAGED_CLASS_NAME )\
public __gc class MANAGED_CLASS_NAME : public IManipulatorFactory\
 {\
 public:\
   System::IntPtr CreateManipulator( System::IntPtr env )\
   {\
     return System::IntPtr( new vx##MANAGED_CLASS_NAME( *reinterpret_cast< vxEnvironment * >( env.ToPointer() ) ) );\
   }\
 };

/**
 * The preferred way to define a controller.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_LOCAL_CONTROLLER( MANAGED_CLASS_NAME )\
public __gc class MANAGED_CLASS_NAME : public IControllerFactory\
 {\
 public:\
   System::IntPtr CreateController( System::IntPtr env  )\
   {\
     return System::IntPtr( new vx##MANAGED_CLASS_NAME( *reinterpret_cast< vxEnvironment * >( env.ToPointer() ) ) );\
   }\
 };



/**
 * The preferred way to define a controller.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_GLOBAL_CONTROLLER( MANAGED_CLASS_NAME )\
public __gc class MANAGED_CLASS_NAME : public IGlobalControllerFactory\
 {\
 public:\
   System::IntPtr CreateController()\
   {\
     return System::IntPtr( new vx##MANAGED_CLASS_NAME() );\
   }\
 };


/*
 * The preferred way to define a annotation.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_ANNOTATION( MANAGED_CLASS_NAME )\
public __gc class MANAGED_CLASS_NAME : public IAnnotationFactory\
 {\
 public:\
   System::IntPtr CreateAnnotation(  )\
   {\
     return System::IntPtr( new vx##MANAGED_CLASS_NAME() );\
   }\
 };

/*
 * The preferred way to define a viewer type.
 * The unmanaged class name is the managed class name prefixed by "vx".
 */
#define DEFINE_VIEWER_TYPE( MANAGED_CLASS_NAME )\
public __gc class MANAGED_CLASS_NAME : public System::IDisposable , public Viatronix::Visualization::IViewerType\
{\
public:\
  MANAGED_CLASS_NAME( Viatronix::Visualization::Environment * env )\
  {\
    m_pimpl =  new vx##MANAGED_CLASS_NAME( * ToNativeEnvironmentPtr( env->GetEnvironmentPtr() ) );\
  }\
  ~MANAGED_CLASS_NAME()\
  {\
    Dispose( false );\
  }\
  void Dispose()\
  {\
    Dispose( true );\
    System::GC::SuppressFinalize( this );\
  }\
  System::IntPtr GetViewerTypePtr()\
  {\
    return System::IntPtr( m_pimpl );\
  }\
  void InitializeView()\
  {\
    m_pimpl->InitializeView();\
  }\
  __property bool get_Is2D()\
  {\
    return m_pimpl->Is2D();\
  }\
  __property bool get_Is3D()\
  {\
    return m_pimpl->Is3D();\
  }\
  System::String * get_ToString()\
  {\
    return m_pimpl->GetClassString().c_str();\
  }\
protected:\
  virtual void Dispose( bool bDisposing )\
  {\
    if( m_pimpl != NULL)\
    {\
      delete m_pimpl;\
      m_pimpl = NULL;\
    }\
  }\
private:\
    vx##MANAGED_CLASS_NAME * m_pimpl;\
 };


