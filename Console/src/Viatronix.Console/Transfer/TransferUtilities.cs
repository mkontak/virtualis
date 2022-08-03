// $Id: TransferUtilities.cs,v 1.5.2.2 2008/10/15 21:13:45 kchalupa Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;

using Viatronix.v3D.Core;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Console
{
  /// <summary>
  /// Provides the Patient and StudyXML files from a series Collection.
  /// </summary>
  public sealed class TransferUtilities
  {

    #region methods
    /// <summary>
    /// Private constructor
    /// </summary>
    private TransferUtilities()
    {
    }//PatientXmlProvider


    /// <summary>
    /// Gets the Patient Files from the source ( e.g.Patient.xml )
    /// </summary>
    /// <param name="seriesCollection">List<Series></param>
    /// <returns>Hashtable</returns>
    static public System.Collections.Hashtable GetPatientXmls( List<Series> seriesCollection )
    {
      System.Collections.Hashtable patientXmlFiles = new System.Collections.Hashtable();
      try
      {
        foreach ( Series series in seriesCollection )
        {
          if ( series.Path == string.Empty )
          {
            Viatronix.Logging.Log.Error(" Cannot transfer Series : " + series.Uid + ", which has an empty path ", "TransferProviderSelector", "GetPatientXmls");
            continue;
          }
        
          string studyFolder   = System.IO.Path.GetDirectoryName( series.Path );
          string patientFolder = System.IO.Path.GetDirectoryName( studyFolder );
          string basePath      = System.IO.Path.GetDirectoryName( patientFolder );
          string relativePath  = patientFolder.Replace( basePath, string.Empty );
        
          string [] files = System.IO.Directory.GetFiles( patientFolder );
        
          //avoid duplicate entries
          foreach ( string file in files )
            patientXmlFiles[file] = System.IO.Path.Combine( relativePath.TrimStart(System.IO.Path.DirectorySeparatorChar), System.IO.Path.GetFileName(file) );
        }
      }
      catch( Exception ex )
      {
        Viatronix.Logging.Log.Error(ex.Message, "TransferProviderSelector", "GetPatientXmls");
        throw ex;
      }
      
      return patientXmlFiles;

    }//GetPatientXmls( List<Series> seriesCollection )


    /// <summary>
    /// Gets the Patient XML file path of a series
    /// </summary>
    /// <param name="series"></param>
    /// <returns></returns>
    static public string GetPatientXml( Series series )
    {
      if ( series.Path == string.Empty )
      {
        Viatronix.Logging.Log.Error(" Cannot Get Patient XML for  : " + series.Uid + ", which has an empty path ", "TransferProviderSelector", "GetPatientXml");
        return string.Empty;
      }

        
      string studyPath   = System.IO.Path.GetDirectoryName( series.Path );
      string patientPath = System.IO.Path.GetDirectoryName(studyPath);

      string[] files = System.IO.Directory.GetFiles(patientPath);

      foreach ( string file in files )
      {
        if ( string.Compare(System.IO.Path.GetFileName(file),"patient.xml", true) == 0  )
          return file;
      }
      
      return string.Empty;

    }//GetPatientXml( Series series )

      
    /// <summary>
    /// Gets the study XML path of a series.
    /// </summary>
    /// <param name="series"></param>
    /// <returns></returns>
    static public string GetStudyXml( Series series )
    {
      if ( series.Path == string.Empty )
      {
        Viatronix.Logging.Log.Error(" Cannot process Series : " + series.Uid + ", which has an empty path ", "TransferProviderSelector", "GetStudyXml");
        return string.Empty;
      }

      string studyFolder   = System.IO.Path.GetDirectoryName( series.Path );
      string [] files = System.IO.Directory.GetFiles( studyFolder );
        
      //avoid duplicate entries
      foreach ( string file in files )
        if ( System.IO.Path.GetFileName( file).ToLower() == "study.xml" )
          return file;
      
      return string.Empty;
    }//GetStudyXml( Series series )


    /// <summary>
    /// Gets the study files from the source collection. ( e.g.Study.xml )
    /// </summary>
    /// <param name="seriesCollection">List<Series></param>
    /// <returns>Hashtable</returns>
    static public System.Collections.Hashtable GetStudyXmls( List<Series> seriesCollection )
    {
      System.Collections.Hashtable studyXmlFiles = new System.Collections.Hashtable();
    
      foreach ( Series series in seriesCollection )
      {
        if ( series.Path == string.Empty )
        {
          Viatronix.Logging.Log.Error(" Cannot Get Study XMls for series : " + series.Uid + ", which has an empty path ", "TransferProviderSelector", "GetStudyXmls");
          continue;
        }

        string studyFolder   = System.IO.Path.GetDirectoryName( series.Path );
        string patientFolder = System.IO.Path.GetDirectoryName( studyFolder );
        string basePath      = System.IO.Path.GetDirectoryName( patientFolder );
        string relativePath  = studyFolder.Replace( basePath, string.Empty );

        string [] files = System.IO.Directory.GetFiles( studyFolder );
        
        //avoid duplicate entries
        foreach ( string file in files )
          studyXmlFiles[file] = System.IO.Path.Combine ( relativePath.TrimStart(System.IO.Path.DirectorySeparatorChar),  System.IO.Path.GetFileName(file)) ;
      }
          
      return studyXmlFiles;
    }//GetStudyXmls( List<Series> seriesCollection )


    /// <summary>
    /// Create the series collection XML file.
    /// </summary>
    /// <param name="seriesCollection">List<Series></param>
    /// <param name="filename">string</param>
    static public void CreateTableOfContentsXml( List<Series> seriesCollection, string filename )
    {
      try
      {
        //Delete old files if any.
        if( System.IO.File.Exists(filename) )
          System.IO.File.Delete(filename);
      }
      catch( System.IO.IOException ex )
      {
        Viatronix.Logging.Log.Error("Failed the Creation of Table of Content XML" + ex.Message, "TransferProviderSelector", "CreateTableOfContentsXml");
        throw ex;
      }

      try
      {
        XmlDocument xmlDoc = new XmlDocument();
        //create a new xml file
        XmlTextWriter xmlWriter = new XmlTextWriter( filename, System.Text.Encoding.UTF8 );
        xmlWriter.Formatting = Formatting.Indented;
        xmlWriter.WriteProcessingInstruction("xml", "version='1.0' encoding='UTF-8'");
        xmlWriter.WriteStartElement( "patients" );
        xmlWriter.Close();
        xmlDoc.Load(filename);
             
        XmlNode root = xmlDoc.DocumentElement;

        string patientXmlPath     = string.Empty;
        string studyXmlPath       = string.Empty ;
        
        XmlNode patientNode       = null;
        XmlElement studiesNode    = null;
        XmlNode studyNode         = null;
        XmlElement collectionNode = null;

        foreach ( Series series in seriesCollection )
        {
          if ( patientXmlPath != TransferUtilities.GetPatientXml ( series ) )
          {
            patientXmlPath = GetPatientXml ( series );

            //Get the Patient Node from patient XML
            XmlDocument patientDoc = new XmlDocument();
            patientDoc.Load( patientXmlPath );
            patientNode = xmlDoc.ImportNode( patientDoc.DocumentElement, true );

            //Add the patient Node
            root.AppendChild ( patientNode );
          }

          if ( studyXmlPath !=  GetStudyXml ( series ) )
          {
            studyXmlPath = TransferUtilities.GetStudyXml ( series );
            studiesNode  = xmlDoc.CreateElement( "studies" );
            patientNode.AppendChild ( studiesNode );

            //Get the study Node from study XML
            XmlDocument studyDoc = new XmlDocument();
            studyDoc.Load( studyXmlPath );
            studyNode = xmlDoc.ImportNode( studyDoc.DocumentElement, true );

            //Add the study Node
            studiesNode.AppendChild ( studyNode );
            //Create Collection Node.
            collectionNode = xmlDoc.CreateElement( "seriescollection" );
            studyNode.AppendChild( collectionNode );
          }
       
          //Add the Series Node
          XmlDocument doc1 = new XmlDocument();
          doc1.Load(Path.Combine(series.Path, "series.xml"));
          XmlNode seriesNode = xmlDoc.ImportNode( doc1.DocumentElement, true );

          XmlAttribute dir = xmlDoc.CreateAttribute("dir");
          dir.Value = series.Directory;
          seriesNode.Attributes.Append(dir);

          collectionNode.AppendChild ( seriesNode );
          
          //Create the files attribute
          XmlElement filesNode  = xmlDoc.CreateElement( "files" );
          seriesNode.AppendChild( filesNode );

          foreach ( string file in series.Files )
          {
            XmlElement fileNode  = xmlDoc.CreateElement( "file" );

            //Create a new attribute.
            XmlAttribute fileAttrib = xmlDoc.CreateAttribute( "fileName" );
            fileAttrib.Value = file;
            fileNode.Attributes.Append ( fileAttrib );
            filesNode.AppendChild( fileNode );
          }
        }
        xmlDoc.Save(filename);
      }
      catch( XmlException ex )
      {
        Viatronix.Logging.Log.Error(" Failed the creation of Table of content XML" + ex.Message, "TransferProviderSelector", "CreateTableOfContentsXml");
        throw new Exception ( " Failed the creation of Table of content XML", ex );
      }
    } // CreateTableOfContentsXml( List<Series> seriesCollection, string filename )

    /// <summary>
    /// Create subFolder Name
    /// </summary>
    /// <param name="series"></param>
    /// <returns></returns>
    public static string CreatesubFolderName( Series series )
    {
      return CreatesubFolderName( series, string.Empty );
    }//CreatesubFolderName
    
    /// <summary>
    /// CreatesubFolderName
    /// </summary>
    /// <param name="series">Series</param>
    /// <param name="childFolder">string</param>
    /// <returns></returns>
    public static string CreatesubFolderName( Series series, string childFolder )
    {
      string folderName = Viatronix.Utilities.IO.PathUtilities.EliminateInvalidFileNameChars(series.Description) + "_" + series.DateProcessed.ToString();
      if ( childFolder != string.Empty )
        folderName = Viatronix.Utilities.IO.PathUtilities.CombinePaths(folderName, childFolder);
      
      return folderName.Replace( " ", "" );
    }//CreatesubFolderName( Series series, string childFolder )
  
  
    #endregion methods

  }//public sealed class PatientXmlProvider
}//namespace Viatronix.Console

#region revision History

// $Log: TransferUtilities.cs,v $
// Revision 1.5.2.2  2008/10/15 21:13:45  kchalupa
// TOC functionality specific to FileSystemTransferProvider was removed.
//
// Revision 1.5.2.1  2008/10/03 19:35:59  kchalupa
// File System Transfer Functionality
//
// Revision 1.5  2007/02/28 19:58:16  romy
// added comments before the code review
//
// Revision 1.4  2007/02/27 21:20:43  mkontak
// Change the create path to include additional data for the series path since we can two series with the same
// description.
//
// Revision 1.3  2006/10/04 17:43:43  mkontak
// Minor changes
//
// Revision 1.2  2006/02/16 19:45:38  romy
// VC Transfer added
//
// Revision 1.1  2006/02/16 14:55:09  romy
// initial version
//
// Revision 1.1  2005/12/13 20:45:59  romy
// re arranged
//
// Revision 1.7  2005/12/02 02:41:43  romy
// coding standards
//
// Revision 1.6  2005/09/30 16:07:30  romy
// new exception added
//
// Revision 1.5  2005/09/26 15:52:22  romy
// removed message boxes
//
// Revision 1.4  2005/09/22 16:18:24  romy
// initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console/Transfer/TransferUtilities.cs,v 1.5.2.2 2008/10/15 21:13:45 kchalupa Exp $
// $Id: TransferUtilities.cs,v 1.5.2.2 2008/10/15 21:13:45 kchalupa Exp $

#endregion revision History

