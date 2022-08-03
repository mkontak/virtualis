// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DatabaseLoader.h
//
// Owner: Greg Davidson  gdavidson@viatronix.net

#include "StdAfx.h"
#include "DatabaseLoader.h"
#include "Exception.h"
#include "vxDOMDocument.h"
#include "vxDOMNodeList.h"
#include "vxDOMNamedNodeMap.h"
#include "vxDOMElement.h"
#include "vxDOMAttribute.h"
#include "DatasetLoadParams.h"
#include "User.h"
#include "ServiceLocator.h"
#include "Viatron.h"

/**
 * constructor
 *
 * @param         params            study load parameters
 * @param         callback          load callback
 */
DatabaseLoader::DatabaseLoader( const StudyLoadParamsEx & params, ReaderLib::StudyLoad::LoadCallback callback ) : FileSystemLoader( params, callback )
{
} // DatabaseLoader


/**
 * destructor
 */
DatabaseLoader::~DatabaseLoader()
{
} // ~DatabaseLoader(


/**
 * load operation
 */
void DatabaseLoader::Load()
{
  // populate the GetParams() from the database
  std::string xml = ServiceLocator::GetRetrievalService()->GetLaunchList( ViatronApp::GetViatronApp()->GetContextId(), GetParams().GetUIDs(), "VC",  ViatronApp::GetViatronApp()->GetCommandLine().CreateNewSession() );//( GetParams().GetLoaderStyle() == StudyLoadParamsEx::LOADER_STYLE_CONSOLE ));
  vxDOMDocument doc( vxDOMDocument::FromXml( xml ));

  ViatronApp::GetViatronApp()->SetTransactionId(doc.GetDocumentElement().GetAttribute("tid"));
  GetParams().SetSessionUID( doc.GetDocumentElement().GetAttribute( "uid" ));
  GetParams().SetSessionDirectory( doc.GetDocumentElement().GetAttribute( "url" ));

  std::vector< ReaderLib::DatasetLoadParams * > datasets;
  datasets.push_back( &GetParams().GetPrimaryDataset() );
  datasets.push_back( &GetParams().GetSecondaryDataset() );

  uint4 uFlags( 0 );
  vxDOMNodeList nodes = doc.GetDocumentElement().GetElementsByTagName( "dataset" );
  for( int4 i(0); i < nodes.GetLength(); ++i )
  {
    vxDOMElement datasetElem = nodes.Item(i).ToElement();
    datasets[i]->SetUID( datasetElem.GetAttribute( "uid" ));

    vxDOMNodeList children = datasetElem.GetChildNodes();
    for( int4 iChild(0); iChild < children.GetLength(); ++iChild )
    {
      vxDOMNode child = children.Item(iChild);
      std::string nodeName = tolower( child.GetNodeName() );
      if( !lstrcmpi( nodeName.c_str(), "volume" ))
      {
        datasets[i]->SetVolumeDirectory( child.GetAttributes().GetNamedItem( "url" ).GetNodeValue() );
        datasets[i]->SetPrefix( ReaderLib::DatasetLoadParams::GeneratePrefix( datasets[i]->GetVolumeDirectory() ));
      }
      else if( !lstrcmpi( nodeName.c_str(), "preprocessed" ))
        datasets[i]->SetPreprocessedDirectory( child.GetAttributes().GetNamedItem( "url" ).GetNodeValue() );
      else if( !lstrcmpi( nodeName.c_str(), "cad" ))
        datasets[i]->SetCadDirectory( child.GetAttributes().GetNamedItem( "url" ).GetNodeValue() );
    }

    if( datasets[i]->GetVolumeDirectory().length() && datasets[i]->GetPreprocessedDirectory().length() )
      uFlags |= ( i == 0 ) ? ReaderLib::StudyLoadParams::PRIMARY_DATASET_VALID : ReaderLib::StudyLoadParams::SECONDARY_DATASET_VALID;
  } // for i

  GetParams().SetFlags( uFlags );

  // load the data from file system
  FileSystemLoader::Load();
} // Load()

