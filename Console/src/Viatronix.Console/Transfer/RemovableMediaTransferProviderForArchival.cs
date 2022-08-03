// $Id: RemovableMediaTransferProviderForArchival.cs,v 1.3 2006/05/09 18:14:54 romy Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

//using System;
//using Viatronix.v3D.Core;
//using Viatronix.Utilities.RemovableMedia;
//using System.IO;
//using Viatronix.UI;
//using System.Collections;
//using System.Threading;
//using System.Xml;
//using Viatronix.Enterprise.Entities;

//namespace Viatronix.Console
//{
//  /// <summary>
//  /// RemovableMediaTransferProviderForArchival Adds files to the media by keeping -
//  /// Viatronix File System structure.
//  /// </summary>
//  public class RemovableMediaTransferProviderForArchival :  RemovableMediaTransferProvider
//  {

//    #region construction
//    /// <summary>
//    /// Constructor
//    /// </summary>
//    public RemovableMediaTransferProviderForArchival()
//    {
//    } // RemovableMediaTransferProviderForArchival()


//     #endregion

//    #region RemovableMediaTransferProvider Methods
    
//    /// <summary>
//    /// Adds Files to the Removable media.Implementation of base classes abstract method.
//    /// </summary>
//    public override void AddFiles()
//    {
//      //Build the table of content file.
//      try
//      {
//        // ------------------------------------------------
//        // Importer now imports the Archival CD.
//        // ------------------------------------------------

//        //string tocFilename =  System.IO.Path.Combine( m_transferProviderArgs.StageArea, "TOC.xml" );
//        //System.IO.Directory.CreateDirectory ( System.IO.Path.GetDirectoryName( tocFilename ));
//        //TransferUtilities.CreateTableOfContentsXml ( m_transferProviderArgs.List<Series>, tocFilename );

//        //SelectedDevice.Writer.Files.Add( new MediaFile( tocFilename, System.IO.Path.GetFileName( tocFilename )));
//        //Viatronix.Logging.Log.Debug ( "Added TOC.Xml file for Transfer", "RemovableMediaTransferProviderForArchival", "AddFiles");
//      }
//      catch ( Exception ex )
//      {
//        Viatronix.Logging.Log.Error(" Table of content file creation failed " + ex.Message, "RemovableMediaTransferProviderForArchival", "AddFiles");
//        throw new Exception(" Table of content file creation failed ", ex );
//      }

//      IDictionaryEnumerator enumerator = m_transferProviderArgs.TransferData.GetEnumerator();
//      while ( enumerator.MoveNext() )
//      {
//        Series series = (Series)enumerator.Key;
//        foreach( string file in series.Files )
//        {
//          SelectedDevice.Writer.Files.Add( new MediaFile( Path.Combine( series.Path, file ), Path.Combine (series.Directory, file )  ));
//          Viatronix.Logging.Log.Debug(Path.Combine(series.Path, file) + " Selected for Transfer", "RemovableMediaTransferProviderForArchival", "AddFiles");
//        }
//      }

//      //Get the Patient files and add them to the Media. ( e.g.patient.XML)
//      System.Collections.Hashtable patientXmlFiles = TransferUtilities.GetPatientXmls( m_transferProviderArgs.SeriesCollection );
//      IDictionaryEnumerator myEnumerator = patientXmlFiles.GetEnumerator();
//      while ( myEnumerator.MoveNext() )
//      {
//        SelectedDevice.Writer.Files.Add( new MediaFile( myEnumerator.Key.ToString(), myEnumerator.Value.ToString() ));
//        Viatronix.Logging.Log.Debug(myEnumerator.Key + " Selected for Transfer", "RemovableMediaTransferProviderForArchival", "AddFiles");
//      }

//      //Get the Study files and add them to the Media.(e.g.Study.XMl)
//      System.Collections.Hashtable studyXmlFiles = TransferUtilities.GetStudyXmls(m_transferProviderArgs.SeriesCollection);
//      myEnumerator = studyXmlFiles.GetEnumerator();
//      while ( myEnumerator.MoveNext() )
//      {
//        SelectedDevice.Writer.Files.Add( new MediaFile( myEnumerator.Key.ToString(), myEnumerator.Value.ToString() ));
//        Viatronix.Logging.Log.Debug(myEnumerator.Key + " Selected for Transfer", "RemovableMediaTransferProviderForArchival", "AddFiles");
//      }
//    }//AddFiles()
    
    
//    #endregion
          
//  } // class RemovableMediaTransferProviderForArchival
//} // namespace Viatronix.Console
 
#region revision History

// $Log: RemovableMediaTransferProviderForArchival.cs,v $
// Revision 1.3  2006/05/09 18:14:54  romy
// temp area fix
//
// Revision 1.2  2006/02/16 15:10:15  romy
// using ISessionProvider
//
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.1  2005/11/03 19:55:23  romy
// Initial Version
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/RemovableMediaTransferProviderForArchival.cs,v 1.3 2006/05/09 18:14:54 romy Exp $
// $Id: RemovableMediaTransferProviderForArchival.cs,v 1.3 2006/05/09 18:14:54 romy Exp $

#endregion revision History
