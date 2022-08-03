// $Id: ThinClientLoadParams.C,v 1.2 2007/03/09 00:11:04 jmeade Exp $
//
// Copyright© 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Greg Davidson (gdavidson@viatronix.com)

#include "StdAfx.h"
#include "ThinClientLoadParams.h"
#include "vxDOMDocument.h"
#include "vxDOMElement.h"
#include "vxDOMNodeList.h"
#include "vxDOMNode.h"
#include "User.h"


/**
 * constructor
 */
ThinClientLoadParams::ThinClientLoadParams( const std::string & sFilename ) :
  m_sFilename( sFilename )
{
} // constructor()


/**
 * Parses the thin client load parameters.
 *
 * @return        true if the load parameters are valid; otherwise false
 */
bool ThinClientLoadParams::ParseParams()
{
  vxDOMDocument doc = vxDOMDocument::FromFile( m_sFilename );
  const std::string sVersion = doc.GetDocumentElement().GetAttribute( "version" );
  if ( sVersion == "1.0" )
  {
    // get datasets
    vxDOMNodeList datasetsList = doc.GetDocumentElement().GetElementsByTagName( "datasets" ).Item( 0 ).ToElement().GetElementsByTagName( "dataset" );
      
    // get session 
    m_params.SetSessionDirectory( doc.GetDocumentElement().GetElementsByTagName( "session" ).Item( 0 ).ToElement().GetNodeValue() );

    // primary dataset
    vxDOMElement primaryElement = datasetsList.Item(0).ToElement();
    m_params.GetPrimaryDataset().SetVolumeDirectory( primaryElement.GetElementsByTagName( "volume" ).Item( 0 ).ToElement().GetNodeValue() );
    m_params.GetPrimaryDataset().SetPreprocessedDirectory( primaryElement.GetElementsByTagName( "preprocessed" ).Item( 0 ).ToElement().GetNodeValue() );
    m_params.GetPrimaryDataset().SetCadDirectory( primaryElement.GetElementsByTagName( "cad" ).Item( 0 ).ToElement().GetNodeValue() );
    m_params.GetPrimaryDataset().SetPrefix( ReaderLib::DatasetLoadParams::GeneratePrefix( m_params.GetPrimaryDataset().GetVolumeDirectory(), "s" ) );

    // secondary dataset
    vxDOMElement secondaryElement = datasetsList.Item(1).ToElement();
    m_params.GetSecondaryDataset().SetVolumeDirectory( secondaryElement.GetElementsByTagName( "volume" ).Item( 0 ).ToElement().GetNodeValue() );
    m_params.GetSecondaryDataset().SetPreprocessedDirectory( secondaryElement.GetElementsByTagName( "preprocessed" ).Item( 0 ).ToElement().GetNodeValue() );
    m_params.GetSecondaryDataset().SetCadDirectory( secondaryElement.GetElementsByTagName( "cad" ).Item( 0 ).ToElement().GetNodeValue() );
    m_params.GetSecondaryDataset().SetPrefix( ReaderLib::DatasetLoadParams::GeneratePrefix( m_params.GetSecondaryDataset().GetVolumeDirectory(), "p" ) );

    // set the validity of the datasets
    uint4 uFlags( 0 );
    if( m_params.GetPrimaryDataset().GetVolumeDirectory().length() && m_params.GetPrimaryDataset().GetPreprocessedDirectory().length() )
      uFlags |=  ReaderLib::StudyLoadParams::PRIMARY_DATASET_VALID;

    if( m_params.GetSecondaryDataset().GetVolumeDirectory().length() && m_params.GetSecondaryDataset().GetPreprocessedDirectory().length() )
      uFlags |=  ReaderLib::StudyLoadParams::SECONDARY_DATASET_VALID;

    m_params.SetFlags( uFlags );

    // load the user information
    vxDOMElement userElement = doc.GetDocumentElement().GetElementsByTagName( "user" ).Item( 0 ).ToElement();
    //User::GetCurrentUser().LoadRights( userElement );

    return true;
  }

  return false;
} // ParseParams()


// $Log: ThinClientLoadParams.C,v $
// Revision 1.2  2007/03/09 00:11:04  jmeade
// code standards.
//
// Revision 1.1  2006/11/11 01:36:42  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Viewer.Colon/ThinClientLoadParams.C,v 1.2 2007/03/09 00:11:04 jmeade Exp $
// $Id: ThinClientLoadParams.C,v 1.2 2007/03/09 00:11:04 jmeade Exp $
