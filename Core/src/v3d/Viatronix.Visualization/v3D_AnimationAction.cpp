// $Id: v3D_AnimationAction.cpp,v 1.6 2007/07/16 20:01:14 romy Exp $
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
#include "v3D_AnimationAction.h"


// namespaces
USING_ANIMATION_NS
USING_VISUALIZATION_NS
using namespace System::Xml;


// defines
#define FILE_REVISION "$Revision: 1.6 $"


/**
 * Initializes an AnimationAction object with default values.
 *
 * @param   mpViewer    Viewer object to animate.
 */
AnimationAction::AnimationAction( Viatronix::Visualization:: Environment ^ mpEnvironment ) :
 m_mpEnvironment( mpEnvironment ),
	m_bCycleDatasets( false ),
	m_iDatasetIndex( 0 ),
	m_mpDatasetList( gcnew System::Collections::Generic::List< Viatronix::Visualization::Dataset ^ >() )
{
} // AnimationAction()


/**
 * default constructor
 */
AnimationAction::AnimationAction() :
  m_mpEnvironment( nullptr )
{
} // AnimationAction()


/**
 * Called when the environment is to be initialized.
 */
void AnimationAction::OnInitializeEnvironment()
{
  InitializeEnvironment( this, System::EventArgs::Empty );
} // OnInitializeEnvironment()


/**
 * Called when the environments's state is to be restored.
 */
void AnimationAction::OnRestoreEnvironment()
{
  RestoreEnvironment( this, System::EventArgs::Empty );
} // OnRestoreEnvironment()


/**
 * Called before the first frame is rendered.
 *
 * @param   bPreview    Indeicates if this is a preview run.
 */
void AnimationAction::OnBeginAnimation( bool bPreview )
{
  AnimationBegin( this, System::EventArgs::Empty );
} // OnBeginAnimation()


/**
 * Called after the last frame is rendered.
 *
 * @param   bPreview    Indeicates if this is a preview run.
 */
void AnimationAction::OnEndAnimation( bool bPreview )
{
  AnimationEnd( this, System::EventArgs::Empty );
} // OnEndAnimation()


/**
 * create from xml
 *
 * @param parent the parent xml node
 *
 * @return a new animation action as specified in the xml
 */
AnimationAction ^ AnimationAction::FromXml( System::Xml::XmlNode ^ mpParent, Viatronix::Visualization::Environment ^ mpEnvironment )
{

  try
  {
    //
    // check the version
    //
    XmlNode ^ mpVersionNode = mpParent->Attributes->GetNamedItem( "version" );
    if( mpVersionNode == nullptr )
      throw ex::DataCorruptionException( LogRec( "version missing from Animator node", "AnimationAction", "FromXml" ) );

    System::String ^ mpVersion = mpVersionNode->Value;
    if ( mpVersion->CompareTo( "1.0" ) != 0 )
      throw ex::DataCorruptionException( LogRec( "Unknown XML version: " + ss( mpVersion ), "AnimationAction", "FromXml" ) );

    //
    // get typename
    //
    XmlNode ^ mpTypenameNode = mpParent->Attributes->GetNamedItem( "typename" );
    if( mpTypenameNode == nullptr )
      throw ex::DataCorruptionException( LogRec( "typename attribute missing", "AnimationAction", "FromXml" ) );

    System::String ^ mpTypename = mpTypenameNode->Value;

    //
    // activate
    //
    AnimationAction ^ mpAction = nullptr;
    try
    {
      array< System::Object ^ > ^ mpArguments = gcnew array< System::Object ^ > { mpEnvironment };
      System::Type ^ mpType = System::Type::GetType( mpTypename );
      mpAction = dynamic_cast< AnimationAction ^ >( System::Activator::CreateInstance( mpType, mpArguments ) );
    }
    catch ( System::Exception ^ e )
    {
      throw ex::DataCorruptionException( LogRec( ss( e->Message ), "AnimationAction", "FromXml" ) );
    }

    if( mpAction == nullptr )
      throw ex::DataCorruptionException( LogRec( "Unable to activate instance of class: " + ss( mpTypename ), "AnimationAction", "FromXml" ) );

    mpAction->Environment = mpEnvironment;

    //
    // configure further
    //
    mpAction->ConfigureFromXml( mpParent );

    return mpAction;
  }
  catch ( ex::VException & e )
  {
    throw gcnew v3DException( & e );
  }
  catch ( ... )
  {
    throw gcnew v3DException( "unhandled exception in AnimationAction::FromXml" );
  }

} // FromXml()


// undefines
#undef FILE_REVISION


// $Log: v3D_AnimationAction.cpp,v $
// Revision 1.6  2007/07/16 20:01:14  romy
// added beating in animation
//
// Revision 1.5  2006/10/04 18:29:03  gdavidson
// Ported to C++/CLI
//
// Revision 1.4  2004/10/29 20:18:07  frank
// formatting
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
// Revision 1.2  2003/09/26 11:40:06  geconomos
// Added events
//
// Revision 1.1  2003/08/22 03:39:41  geconomos
// Initial revision.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Visualization/v3D_AnimationAction.cpp,v 1.6 2007/07/16 20:01:14 romy Exp $
// $Id: v3D_AnimationAction.cpp,v 1.6 2007/07/16 20:01:14 romy Exp $
