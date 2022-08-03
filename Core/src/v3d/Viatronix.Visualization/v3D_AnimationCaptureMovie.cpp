// $Id: v3D_AnimationCaptureMovie.cpp,v 1.5 2006/10/04 18:29:03 gdavidson Exp $
//
// Copyright © 2003, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Author: George Economos( mailto:george@viatronix.com )

// includes
#include "stdafx.h"
#include "v3D_AnimationCaptureMovie.h"


// namespaces
USING_ANIMATION_NS
USING_VISUALIZATION_NS
using namespace System::Xml;


// defines
#define FILE_REVISION "$Revision: 1.5 $"


/**
 * Initializes a AnimationCaptureMovieAvi with default values.
 */
AnimationCaptureMovie::AnimationCaptureMovie() 
{
  m_mpFilename = L"untitled.avi";
  m_iFramesPerSecond = 30;
} // AnimationCaptureMovie()


/**
 * destructor.
 */
AnimationCaptureMovie::~AnimationCaptureMovie()
{
} // ~AnimationCaptureMovie()


/**
 * Finalizer.
 */
AnimationCaptureMovie::!AnimationCaptureMovie()
{
} // !AnimationCaptureMovie()



/**
 * create from xml
 *
 * @param mpParent the parent xml node
 *
 * @return a new AnimationCapture object
 */
AnimationCaptureMovie ^ AnimationCaptureMovie::FromXml( System::Xml::XmlNode ^ mpParent )
{

  try
  {
  
    //
    // check the version
    //
    XmlNode ^ mpVersionNode = mpParent->Attributes->GetNamedItem( "version" );
    if ( mpVersionNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "version missing from Animator node", "AnimationCaptureMovie", "FromXml" ) );
    }
    System::String ^ mpVersion = mpVersionNode->Value;
    if ( mpVersion->CompareTo( "1.0" ) != 0 )
    {
      throw ex::DataCorruptionException( LogRec( "Unknown XML version: " + ss( mpVersion ), "AnimationCaptureMovie", "FromXml" ) );
    }

    //
    // get typename
    //
    XmlNode ^ mpTypenameNode = mpParent->Attributes->GetNamedItem( "typename" );
    if ( mpTypenameNode == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "typename attribute missing", "AnimationCaptureMovie", "FromXml" ) );
    }
    System::String ^ mpTypename = mpTypenameNode->Value;

    //
    // activate
    //
    AnimationCaptureMovie ^ mpCapture = nullptr;
    try
    {
      mpCapture = dynamic_cast< AnimationCaptureMovie ^ >( System::Activator::CreateInstance( System::Type::GetType( mpTypename ) ) );
    }
    catch ( System::Exception ^ e )
    {
      throw ex::DataCorruptionException( LogRec( ss( e->Message ), "AnimationCaptureMovie", "FromXml" ) );
    }
    if ( mpCapture == nullptr )
    {
      throw ex::DataCorruptionException( LogRec( "Unable to activate instance of class: " + ss( mpTypename ), "AnimationCaptureMovie", "FromXml" ) );
    }
    
    //
    // configure further
    //
    mpCapture->ConfigureFromXml( mpParent );

    return mpCapture;

  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
  catch ( ... )
  {
    throw gcnew v3DException( "unhandled exception in AnimationCaptureMovie::FromXml" );
  }

} // FromXml()


/**
 * further configure from xml
 *
 * @param mpParent the parent xml node
 */
void AnimationCaptureMovie::ConfigureFromXml( System::Xml::XmlNode ^ mpParent )
{

  // configure the image width
  XmlNode ^ mpImageWidthNode = mpParent->SelectSingleNode( "ImageWidth" );
  if ( mpImageWidthNode != nullptr )
  {
    int4 iWidth = System::Int32::Parse( mpImageWidthNode->InnerText );
    if ( iWidth <= 0 )
    {
      throw ex::IllegalArgumentException( LogRec( "image width must be positive be a positive integer, was " + ToAscii( iWidth ), "AnimationCaptureMovie", "ConfigureFromXml" ) );
    }
    this->Width = iWidth;
  }

  // configure the image height
  XmlNode ^ mpImageHeightNode = mpParent->SelectSingleNode( "ImageHeight" );
  if ( mpImageHeightNode != nullptr )
  {
    int4 iHeight = System::Int32::Parse( mpImageHeightNode->InnerText );
    if ( iHeight <= 0 )
    {
      throw ex::IllegalArgumentException( LogRec( "image height must be positive be a positive integer, was " + ToAscii( iHeight ), "AnimationCaptureMovie", "ConfigureFromXml" ) );
    }
    this->Height = iHeight;
  }

  // configure the image height
  XmlNode ^ mpFramesPerSecondNode = mpParent->SelectSingleNode( "FramesPerSecond" );
  if ( mpFramesPerSecondNode != nullptr )
  {
    int4 iFPS = System::Int32::Parse( mpFramesPerSecondNode->InnerText );
    if ( iFPS <= 0 )
    {
      throw ex::IllegalArgumentException( LogRec( "frames per second must be positive be a positive integer, was " + ToAscii( iFPS ), "AnimationCaptureMovie", "ConfigureFromXml" ) );
    }
    this->FramesPerSecond = iFPS;
  }

} // ConfigureFromXml()


// undefines
#undef FILE_REVISION


// $Log: v3D_AnimationCaptureMovie.cpp,v $
// Revision 1.5  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.4  2004/10/29 20:18:21  frank
// formatting
//
// Revision 1.3  2004/06/16 19:40:01  frank
// added movie previews
//
// Revision 1.2  2004/05/06 18:22:15  frank
// added movie scripting
//
// Revision 1.1.1.1  2004/03/02 03:58:39  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.1  2003/08/22 03:39:41  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationCaptureMovie.cpp,v 1.5 2006/10/04 18:29:03 gdavidson Exp $
// $Id: v3D_AnimationCaptureMovie.cpp,v 1.5 2006/10/04 18:29:03 gdavidson Exp $
