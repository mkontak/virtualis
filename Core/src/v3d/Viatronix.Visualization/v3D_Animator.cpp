// $Id: v3D_Animator.cpp,v 1.18 2007/10/24 20:36:42 gdavidson Exp $
//
// Copyright © 2004, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// includes
#include "StdAfx.h"
#include "v3D_Animator.h"
#include "vxRenderer.h"
#include "vxController.h"
#include "vxRenderingContextDib.h"
#include "v3D_AnimationCaptureMovie.h"


// namespaces
USING_ANIMATION_NS
USING_VISUALIZATION_NS
using namespace System::Xml;


// defines
#define FILE_REVISION "$Revision: 1.18 $"


/**
 * Constuctor.
 *
 * @param   mpAction      Animation action.
 * @param   mpCapture     Animation capture.
 */
Animator::Animator( AnimationAction ^ mpAction, AnimationCapture ^ mpCapture ) : 
 m_mpAction( mpAction ),
 m_mpCapture( mpCapture ),
 m_iWidth( 512 ),
 m_iHeight( 512 ),
 m_iSupersamplingFactor( 1 )
{
  m_pContext = new vxRenderingContextDIB();
  m_pRenderers = new std::list< vxRenderer * >();
  m_pControllers = new std::list< vxController * >();
} // Animator( AnimationAction * mpAction, AnimationCapture * mpCapture ) 


/**
 * Constuctor.
 *
 * @param   mpAction      Animation action.
 * @param   mpCapture     Animation capture.
 * @param   iWidth        Capture width in pixles.
 * @param   iHeight        Capture height in pixels.
 */
Animator::Animator( AnimationAction ^ mpAction, AnimationCapture ^ mpCapture, int iWidth, int iHeight ) : 
 m_mpAction( mpAction ),
 m_mpCapture( mpCapture ),
 m_iWidth( iWidth ),
 m_iHeight( iHeight ),
 m_iSupersamplingFactor( 1 )
{
  m_pContext = new vxRenderingContextDIB();
  m_pRenderers = new std::list< vxRenderer *  >();
  m_pControllers = new std::list< vxController *  >();
} // Animator( AnimationAction * mpAction, AnimationCapture * mpCapture, int iWidth, int iHeight ) 


/**
 * Destructor.
 */
Animator::~Animator()
{
  this->!Animator();
} // ~Animator()


/**
 * Finalizer.
 */
Animator::!Animator()
{
  if( m_pRenderers != NULL )
  {
    for( std::list< vxRenderer * >::iterator iter = m_pRenderers->begin(); iter != m_pRenderers->end(); ++iter )
      delete (*iter);
    
    delete m_pRenderers;
    m_pRenderers = NULL;
  }
  
  if( m_pControllers != NULL )
  {
    for( std::list< vxController * >::iterator iter = m_pControllers->begin(); iter != m_pControllers->end(); ++iter )
      delete (*iter);

    delete m_pControllers;
    m_pControllers = NULL;
  }

  if( m_pContext != NULL )
  {
    delete m_pContext;
    m_pContext = NULL;
  }
} // !Animator()


int4 BytesPerLine( int4 iWidth, int4 iBitsPerPixel )
{
  int4 iBitsPerLine( iWidth * iBitsPerPixel );
  int4 iBytesPerLine = ( ( iBitsPerLine + 31 ) & ( ~31) ) / 8;
  return iBytesPerLine;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
  UINT  num = 0;          // number of image encoders
  UINT  size = 0;         // size of the image encoder array in bytes

  Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

  Gdiplus::GetImageEncodersSize(&num, &size);
  if(size == 0)
    return -1;  // Failure

  pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
  if(pImageCodecInfo == NULL)
    return -1;  // Failure

  Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

  for(UINT j = 0; j < num; ++j)
  {
    if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
    {
      *pClsid = pImageCodecInfo[j].Clsid;
      free(pImageCodecInfo);
      return j;  // Success
    }    
  }

  free(pImageCodecInfo);
  return -1;  // Failure
} // GetEncoderClsid()


/**
 * Performs the animation
 */
void Animator::Animate()
{
  try
  {
    // cache a reference to the environment for easy access
    vxEnvironment & environment = *ToNativeEnvironmentPtr( m_mpAction->Environment->GetEnvironmentPtr() );

    environment.SetViewerType(  reinterpret_cast< vxViewerType*>( m_mpAction->Environment->ViewerType->GetViewerTypePtr().ToPointer() ) );

    // resize the vxWinRect in the vxEnvironment
    vxWinRect winrect = environment.GetWinRect();
    const uint4 iRenderWidth( m_iWidth * m_iSupersamplingFactor );
    const uint4 iRenderHeight( m_iHeight * m_iSupersamplingFactor );
    winrect.SetWindowSize( std::make_pair( iRenderWidth, iRenderHeight ) );
    environment.SetWinRect( winrect );

    // setup fixed size rendering
    environment.GetRenderingMode()->SetFixedImageSizeRendering( true );
    environment.GetRenderingMode()->SetFixedImageSize( std::make_pair( (uint2)iRenderWidth,(uint2) iRenderHeight ) );

    // create the rendering context using the specified width and height
    m_pContext->Create( iRenderWidth, iRenderHeight );

    // set the rendering context as current
    m_pContext->MakeCurrent();
    //environment.SetContext( m_pContext );
    //environment.SetFont( new GLFontGeorge() );
    environment.UpdateFont();

    vxModifiedQueue::Flush();
    environment.GetViewerType()->InitializeView();

    // indicate that the animation is beginning
    m_mpAction->OnBeginAnimation( false );  

    // initialize the all renderers
    for( std::list< vxRenderer * >::iterator iter = m_pRenderers->begin(); iter != m_pRenderers->end(); ++iter )
      (*iter)->Initialize();

    // decide which frame is the keyframe
    int iKeyFrame( Bound( static_cast< int >( 0 ),
                          static_cast< int >( m_mpAction->GetFramesToRender() / 2 ),
                          static_cast< int >( m_mpAction->GetFramesToRender() - 1 ) ) );

    // render each frame 
    environment.SetModified( vxEnvironment::ALL, true );
    for( int iFrame( 0 ); iFrame < m_mpAction->GetFramesToRender(); ++iFrame )
    {
      // allow the animation to update the environment
      m_mpAction->OnPreRenderFrame( false, iFrame );             
      m_mpAction->OnRenderFrame( false, iFrame );

      // process any modifications the animation action may have caused
      vxModifiedQueue::Flush();

      // allow the controllers to update the environment
      for( std::list< vxController * >::iterator iter = m_pControllers->begin(); iter != m_pControllers->end(); ++iter )
      {
        if( (*iter)->IsProcessingNecessary() )
          (*iter)->Process();
      }

      // process any modifications the controller may have caused
      vxModifiedQueue::Flush();

      // clear the color buffer for next frame
      glClear( GL_COLOR_BUFFER_BIT );

      // alow the current viewer type to initialize it's view 
      environment.GetViewerType()->SetupView();

      // PreRender
      for( std::list< vxRenderer * >::iterator iter = m_pRenderers->begin(); iter != m_pRenderers->end(); ++iter )
        (*iter)->PreRender();

      // Render
      for( std::list< vxRenderer * >::iterator iter = m_pRenderers->begin(); iter != m_pRenderers->end(); ++iter )
        (*iter)->Render();

      // PostRender
      for( std::list< vxRenderer * >::iterator iter = m_pRenderers->begin(); iter != m_pRenderers->end(); ++iter )
        (*iter)->PostRender();

      // flush the opengl pipeline
      glFlush();

      if ( m_iSupersamplingFactor != 1 )
      {
        Gdiplus::Bitmap bigBitmap( iRenderWidth, iRenderHeight, BytesPerLine( iRenderWidth, 24 ), PixelFormat24bppRGB, m_pContext->GetDib().GetPixelData() );
        Gdiplus::Bitmap smallBitmap( m_iWidth, m_iHeight, PixelFormat24bppRGB );
        {
          Gdiplus::Graphics * graphics = new Gdiplus::Graphics( & smallBitmap );
          graphics->SetInterpolationMode( Gdiplus::InterpolationModeHighQualityBicubic );
          graphics->DrawImage( & bigBitmap, 0, 0, m_iWidth, m_iHeight );
        }
        Gdiplus::Rect rect( 0, 0, m_iWidth, m_iHeight );
        Gdiplus::BitmapData smallBitmapData;
        smallBitmap.LockBits( & rect, Gdiplus::ImageLockModeRead, PixelFormat24bppRGB, & smallBitmapData );

        // pass the pixel data to the capture object
        m_mpCapture->AddFrame( System::IntPtr( smallBitmapData.Scan0 ) );

        // potentially save the keyframe
        if ( iFrame == iKeyFrame )
        {
          System::Drawing::Bitmap ^ mpBitmap = gcnew System::Drawing::Bitmap( m_iWidth, m_iHeight, BytesPerLine( m_iWidth, 24 ), System::Drawing::Imaging::PixelFormat::Format24bppRgb, System::IntPtr( smallBitmapData.Scan0 ) );
          mpBitmap->RotateFlip( System::Drawing::RotateFlipType::RotateNoneFlipY );
					mpBitmap->Save( m_mpPreviewFilename, System::Drawing::Imaging::ImageFormat::Png );
        }

        smallBitmap.UnlockBits( & smallBitmapData );
      }
      else
      {
        m_mpCapture->AddFrame( System::IntPtr( m_pContext->GetDib().GetPixelData() ) );

        // potentially save the keyframe
        if ( iFrame == iKeyFrame )
        {
          System::Drawing::Bitmap bitmap( iRenderWidth, iRenderHeight, BytesPerLine( iRenderWidth, 24 ), System::Drawing::Imaging::PixelFormat::Format24bppRgb, System::IntPtr( m_pContext->GetDib().GetPixelData() ));
          bitmap.RotateFlip( System::Drawing::RotateFlipType::RotateNoneFlipY );
					bitmap.Save( m_mpPreviewFilename, System::Drawing::Imaging::ImageFormat::Png  );
        }
      }

      environment.SetModified( vxEnvironment::ALL, false );

    } // for( int iFrame( 0 ); iFrame < m_mpAction->GetFramesToRender(); ++iFrame )

    // indicate that the animation has completed
    m_mpAction->OnEndAnimation( false );

  } // try
  catch ( System::Exception ^ e )
  {
    LogDbg( ss( e->ToString() ) + " " + ss( e->Message ), "Animator", "Animate" );
    throw gcnew v3DException();
  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
  catch ( ... )
  {
    throw gcnew v3DException( "unhandled exception in Animator::Animate" );
  }

} // Animate()


/**
 * Frees the opengl context
 */
void Animator::Free()
{
  m_pContext->Destroy();
} // Free


/**
 * Adds a renderer to the animator using the specified factory type
 *
 * @param   mpFactoryType   Type of factory used to create renderer.
 */
void Animator::AddRenderer( System::Type ^ mpFactoryType )
{
  // create an instance of the renderer factory
  IRendererFactory ^ mpFactory = dynamic_cast<IRendererFactory^>( System::Activator::CreateInstance( mpFactoryType ) );
    
  // request that the factory create the actual vxRenderer
  vxRenderer * pRenderer = 
    reinterpret_cast< vxRenderer * >( mpFactory->CreateRenderer(  m_mpAction->Environment->GetEnvironmentPtr()  ).ToPointer() ); 
  
  m_pRenderers->push_back( pRenderer );
} // AddRenderer( System::Type ^ mpFactoryType )


/**
 * Adds a controller to the animator using the specified factory type
 *
 * @param   mpFactoryType   Type of factory used to create controller.
 */
void Animator::AddController( System::Type ^ mpFactoryType )
{

  // create an instance of the controller factory
  IControllerFactory ^ mpFactory = dynamic_cast<IControllerFactory^>( System::Activator::CreateInstance( mpFactoryType ) );
    
  // request that the factory create the actual vxController
  vxController * pController(
    reinterpret_cast< vxController * >( mpFactory->CreateController( m_mpAction->Environment->GetEnvironmentPtr() ).ToPointer() ) ); 
  
  m_pControllers->push_back( pController );

} // AddController( System::Type ^ mpFactoryType )


/**
 * create from xml
 *
 * @param mpParent      the parent xml node
 * @param mpEnvironment the environment
 * @param mpFilePrefix  the output file prefix (path not including .avi)
 *
 * @return a new animator as specified in the xml
 */
Animator ^ Animator::FromXml( System::Xml::XmlNode ^ mpParent,
                              Viatronix::Visualization::Environment ^ mpEnvironment,
                              System::String ^ mpFilePrefix )
{
  try
  {
    //
    // check the version
    //
    XmlNode ^ mpVersionNode( mpParent->Attributes->GetNamedItem( "version" ) );
    if( mpVersionNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "version missing from Animator node", "Animator", "FromXml" ) );
    }
    System::String ^ mpVersion( mpVersionNode->Value );
    if ( mpVersion->CompareTo( "1.0" ) != 0 )
    {
      throw ex::DataCorruptionException( LogRec( "Unknown XML version: " + ss( mpVersion ), "Animator", "FromXml" ) );
    }

    //
    // set the animation action
    //
    XmlNode ^ mpActionNode( mpParent->SelectSingleNode( "action" ) );
    if ( mpActionNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "no action specified", "Animator", "FromXml" ) );
    }
    AnimationAction ^ mpAction( AnimationAction::FromXml( mpActionNode, mpEnvironment ) );

    //
    // set the capture
    //
    XmlNode ^ mpCaptureNode( mpParent->SelectSingleNode( "capture" ) );
    if ( mpCaptureNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "no capture specified", "Animator", "FromXml" ) );
    }
    AnimationCaptureMovie ^ mpCapture( AnimationCaptureMovie::FromXml( mpCaptureNode ) );
    mpCapture->Filename = System::String::Concat( mpFilePrefix, ".avi" );
    mpCapture->Initialize();

    //
    // make the animator
    //
    Animator ^ mpAnimator( gcnew Animator( mpAction, mpCapture, mpCapture->Width, mpCapture->Height ) );
    mpAnimator->PreviewFilename = System::String::Concat( mpFilePrefix, ".png" );
    
    //
    // add all the renderers
    //
    XmlNode ^ mpRenderersNode( mpParent->SelectSingleNode( "renderers" ) );
    if ( mpRenderersNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "no renderers specified", "Animator", "FromXml" ) );
    }
    for ( int i(0); i<mpRenderersNode->ChildNodes->Count; ++i )
    {

      XmlNode ^ mpRendererNode( mpRenderersNode->ChildNodes[ i ] );

      if ( mpRendererNode->Name->CompareTo( "renderer" ) == 0 )
      {

        //
        // check the version
        //
        XmlNode ^ mpVersionNode( mpRendererNode->Attributes->GetNamedItem( "version" ) );
        if ( mpVersionNode == nullptr )
        {
          throw ex::DataCorruptionException( LogRec( "version missing from renderer node", "Animator", "FromXml" ) );
        }
        System::String ^ mpVersion( mpVersionNode->Value );
        if ( mpVersion->CompareTo( "1.0" ) != 0 )
        {
          throw ex::DataCorruptionException( LogRec( "Unknown XML version: " + ss( mpVersion ), "Animator", "FromXml" ) );
        }

        //
        // make the renderer
        //
        XmlNode ^ mpType( mpRendererNode->Attributes->GetNamedItem( "typename" ) );
        if ( mpType == nullptr )
        {
          throw ex::DataCorruptionException( LogRec( "no renderer type specified in node", "Animator", "FromXml" ) );
        }
        mpAnimator->AddRenderer( System::Type::GetType( mpType->Value ) );
      }
      else
      {
        throw ex::DataCorruptionException( LogRec( "expected a renderer node here, got instead: " + ss( mpRenderersNode->Name ), "Animator", "FromXml" ) );
      }
    }

    //
    // add all the controllers
    //
    XmlNode ^ mpControllersNode( mpParent->SelectSingleNode( "controllers" ) );
    if ( mpControllersNode == nullptr )
    {
      LogWrn( "no controllers specified", "Animator", "FromXml" );
    }
    else
    {
      for ( int i(0); i<mpControllersNode->ChildNodes->Count; ++i )
      {

        XmlNode ^ mpControllerNode( mpControllersNode->ChildNodes[ i ] );

        if ( mpControllerNode->Name->CompareTo( "controller" ) == 0 )
        {

          //
          // check the version
          //
          XmlNode ^ mpVersionNode( mpControllerNode->Attributes->GetNamedItem( "version" ) );
          if ( mpVersionNode == nullptr )
          {
            throw ex::DataCorruptionException( LogRec( "version missing from controller node", "Animator", "FromXml" ) );
          }
          System::String ^ mpVersion( mpVersionNode->Value );
          if ( mpVersion->CompareTo( "1.0" ) != 0 )
          {
            throw ex::DataCorruptionException( LogRec( "Unknown XML version: " + ss( mpVersion ), "Animator", "FromXml" ) );
          }

          //
          // make the controller
          //
          XmlNode ^ mpType( mpControllerNode->Attributes->GetNamedItem( "typename" ) );
          if ( mpType == nullptr )
          {
            throw ex::DataCorruptionException( LogRec( "no controller type specified in node", "Animator", "FromXml" ) );
          }
          try
          {
            mpAnimator->AddController( System::Type::GetType( mpType->Value ) );
          }
          catch ( ... )
          {
            throw ex::DataCorruptionException( LogRec( "unable to create specified type: " + ss( mpType->Value ), "Animator", "FromXml" ) );
          }
        }
        else
        {
          throw ex::DataCorruptionException( LogRec( "expected a controller node here, got instead: " + ss( mpControllersNode->Name ), "Animator", "FromXml" ) );
        }
      }

    } // if controllers are specified

    //
    // configure the rendering quality
    //
    XmlNode ^ mpQualityNode( mpParent->SelectSingleNode( "renderingQuality" ) );
    if ( mpQualityNode != nullptr )
    {
      XmlNode ^ mpValueNode( mpQualityNode->Attributes->GetNamedItem( "value" ) );
      if ( mpValueNode == nullptr )
      {
        throw ex::DataCorruptionException( LogRec( "value attribute of rendering quality not specified", "Animator", "FromXml" ) );
      }
      if ( mpValueNode->Value->CompareTo( "default" ) == 0 )
      {
        mpEnvironment->RenderingMode->RenderingQuality = RenderingMode::RenderingQualityEnum::DEFAULT;
        LogDbg( "Default rendering quality set", "", "" );
      }
      else if ( mpValueNode->Value->CompareTo( "endoluminal" ) == 0 )
      {
        mpEnvironment->RenderingMode->RenderingQuality = RenderingMode::RenderingQualityEnum::ENDOLUMINAL;
        LogDbg( "Endoluminal rendering quality set", "", "" );
      }
      else if ( mpValueNode->Value->CompareTo( "highest" ) == 0 )
      {
        mpEnvironment->RenderingMode->RenderingQuality = RenderingMode::RenderingQualityEnum::HIGHEST;
        LogDbg( "Highest rendering quality set", "", "" );
      }
      else
      {
        throw ex::DataCorruptionException( LogRec( "unknown value in rendering quality attribute: " + ss( mpValueNode->Value ), "Animator", "FromXml" ) );
      }

      if ( mpQualityNode->Attributes->GetNamedItem( "supersampleFactor" ) )
      {
        int iFactor( System::Int32::Parse( mpQualityNode->Attributes->GetNamedItem( "supersampleFactor" )->Value ) );
        if ( iFactor < 1 )
        {
          throw ex::DataCorruptionException( LogRec( "supersampling factor attribute must a positive integer, not: " + ToAscii( iFactor ), "Animator", "FromXml" ) );
        }
        mpAnimator->m_iSupersamplingFactor = iFactor;
        LogDbg( "supersampling factor is " + ToAscii( iFactor ), "", "" );
      }
    }
    else
    {
      LogDbg( "rendering quality node not found", "", "" );
    }

    //
    // configure the font info
    //
    XmlNode ^ mpFontNode( mpParent->SelectSingleNode( "font" ) );
    if ( mpFontNode != nullptr )
    {

      // set the font family
      XmlNode ^ mpFamilyNode( mpFontNode->Attributes->GetNamedItem( "family" ) );
      if ( mpFamilyNode != nullptr )
      {
        mpEnvironment->FontInfo->FontFamily = mpFamilyNode->Value;
      }

      // set the font size
      XmlNode ^ mpSizeNode( mpFontNode->Attributes->GetNamedItem( "size" ) );
      if ( mpSizeNode != nullptr )
      {
        int iSize( System::Int32::Parse( mpSizeNode->Value ) );
        mpEnvironment->FontInfo->Size = iSize;
      }
    }

    return mpAnimator;

  } // try
  catch ( System::Exception ^ e )
  {
    LogDbg( ss( e->ToString() ) + " " + ss( e->Message ), "Animator", "FromXml" );
    throw gcnew v3DException();
  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
  catch ( ... )
  {
    throw gcnew v3DException( "unhandled exception in Animator::FromXml" );
  }

} // FromXml()


// undefines
#undef FILE_REVISION


// $Log: v3D_Animator.cpp,v $
// Revision 1.18  2007/10/24 20:36:42  gdavidson
// Modified the rendering context for the viewers
//
// Revision 1.17  2007/01/31 15:05:04  romy
// Modified the setup of the envirionment
//
// Revision 1.16  2006/10/10 13:31:09  geconomos
// renabled fixed size rendering
//
// Revision 1.15  2006/10/04 18:29:04  gdavidson
// Ported to C++/CLI
//
// Revision 1.14  2006/08/18 19:44:24  romy
// modified to save the files as PNG
//
// Revision 1.13  2006/01/20 18:09:00  geconomos
// continued migration to visual studio 2005
//
// Revision 1.12  2004/10/29 20:18:33  frank
// formatting
//
// Revision 1.11  2004/10/05 14:39:22  frank
// moving the rendering mode out of the data renderer software 3D class
//
// Revision 1.10  2004/08/30 18:19:37  michael
// Data used by renderers is now polymorph but so far only on the vx side of things.
// Need to add the inheritance on the managed V3D side, too. Still to be done!!!
//
// Revision 1.9  2004/06/16 19:40:01  frank
// added movie previews
//
// Revision 1.8  2004/06/16 17:07:08  frank
// fixed controllers
//
// Revision 1.7  2004/06/15 14:53:29  frank
// added controllers to animation
//
// Revision 1.6  2004/05/19 14:15:53  frank
// added image supersampling factor option
//
// Revision 1.5  2004/05/14 19:40:02  frank
// fixed the font
//
// Revision 1.4  2004/05/13 19:17:40  frank
// added rendering quality adjustment to xml file
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
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_Animator.cpp,v 1.18 2007/10/24 20:36:42 gdavidson Exp $
// $Id: v3D_Animator.cpp,v 1.18 2007/10/24 20:36:42 gdavidson Exp $
