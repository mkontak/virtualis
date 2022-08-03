// $Id: VCFileStructureFilterCD.cs,v 1.2 2007/02/21 21:11:39 gdavidson Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.Collections;
using System.IO;
using System.Windows.Forms;
using System.Xml;
using System.Text.RegularExpressions;

using Viatronix.v3D.UI;
using Viatronix.v3D.Core;

namespace Viatronix.v3D.Console
{
  /// <summary>
  /// Filters a directory using a VC cd layout.
  /// </summary>
  public class VCFileStructureFilterCD
  {

    #region fields

    /// <summary>
    /// Default text added to denote a report.
    /// </summary>
    private const string ReportText = "(VC Report)";

    /// <summary>
    /// xml directory
    /// </summary>
    private const string XmlDirectory = "XML";

    /// <summary>
    /// series file name
    /// </summary>
    private const string SeriesFileName = "Datasets.xml";

    /// <summary>
    /// patient file name
    /// </summary>
    private const string PatientFileName = "Patient.xml";

    /// <summary>
    /// study file name
    /// </summary>
    private const string StudyFileName = "Studies.xml";

    /// <summary>
    /// Extension for the Marking Volume file.
    /// </summary>
    private const string MarkingVolume = ".mkvl";

    /// <summary>
    /// Extension for the VC Session file.
    /// </summary>
    private const string VCSession = ".vcsx";

    /// <summary>
    /// Extension for the VC Report file.
    /// </summary>
    private const string ReportFile = ".vrx";

    /// <summary>
    /// Volume extension
    /// </summary>
    private const string Volume = ".vol";

    /// <summary>
    /// Dicom extension
    /// </summary>
    private const string Dicom = ".dcm";

    /// <summary>
    /// Image extension
    /// </summary>
    private const string Image =".png";

    /// <summary>
    /// Dicom to Volume 
    /// </summary>
    private const string DicomToVolume = "DcmToVol.txt";

    /// <summary>
    /// Filter for isolating supine files.
    /// </summary>
    private const string SupineRegex = @"^\w+_\ds";

    #endregion

    #region methods
    
    /// <summary>
    /// Reads a value from xml
    /// </summary>
    /// <param name="node">xml node</param>
    /// <param name="key">key</param>
    /// <param name="type">type</param>
    /// <returns>object</returns>
    private object ReadValue( XmlNode node, string key, Type type )
    {
      XmlNode childNode = node.SelectSingleNode( key );
      if( childNode == null )
      {
        if( type == typeof( DateTime ))
          return DateTime.MinValue;

        if( type == typeof( int ) || type == typeof( long ))
          return 0;

        if( type == typeof( float ))
          return 0.0f;

        if( type == typeof( double ))
          return 0.0d;

        return string.Empty;
      }

      if( type == typeof( DateTime ))
        return Convert.ToDateTime( childNode.InnerText );

      if( type == typeof( int  ))
        return Convert.ToInt32( childNode.InnerText );

      if( type == typeof( long ))
        return Convert.ToInt64( childNode.InnerText );

      if( type == typeof( float ))
        return float.Parse( childNode.InnerText );

      if( type == typeof( double ))
        return Convert.ToDouble( childNode.InnerText );

      return childNode.InnerText;
    } // ReadValue( node, key, type )


    /// <summary>
    /// Creates a Patient
    /// </summary>
    /// <param name="directory">patient directory</param>
    /// <returns>Patient</returns>
    private Patient CreatePatient( DirectoryInfo directory )
    { 
      XmlDocument doc = new XmlDocument();
      doc.Load( directory.FullName + PatientFileName );

      XmlNode node = doc.SelectSingleNode( "//Table" );
      if( node == null )
        throw new ApplicationException( "The Patient.xml file is improperly formatted." );

      Patient patient = new Patient();
      patient.Id = (int) ReadValue( node, "ID", typeof( int ));
      patient.MedicalId = (string) ReadValue( node, "MEDICAL_ID", typeof( string ));

      patient.Name.FirstName = (string) ReadValue( node, "FIRST_NAME", typeof( string ));
      patient.Name.LastName = (string) ReadValue( node, "LAST_NAME", typeof( string ));
      patient.Name.MiddleName = (string) ReadValue( node, "MI", typeof( string ));
      patient.Name.Prefix = (string) ReadValue( node, "PREFIX", typeof( string ));
      patient.Name.Suffix = (string) ReadValue( node, "SUFFIX", typeof( string ));

      patient.Address.Line1 = (string) ReadValue( node, "ADDRESS", typeof( string ));
      patient.Address.City = (string) ReadValue( node, "CITY", typeof( string ));
      patient.Address.State = (string) ReadValue( node, "STATE", typeof( string ));
      patient.Address.ZipCode = (string) ReadValue( node, "ZIP", typeof( string ));

      patient.Gender = Patient.ConvertToGender( (string) ReadValue( node, "SEX", typeof( string )));
      patient.Weight = (int) ReadValue( node, "WEIGHT", typeof( int ));
      patient.Dob = Convert.ToDateTime( ReadValue( node, "DOB", typeof( string )));

      return patient;
    } // CreatePatient( directory )


    /// <summary>
    /// Creates a study 
    /// </summary>
    /// <param name="directory">study directory</param>
    /// <param name="patient">Patient</param>
    /// <returns>Study</returns>
    private Study CreateStudy( DirectoryInfo directory, Patient patient )
    {
      XmlDocument doc = new XmlDocument();
      doc.Load( directory.FullName + StudyFileName );

      XmlNode node = doc.SelectSingleNode( "//Table" );
      if( node == null )
        throw new ApplicationException( "The Studies.xml file is improperly formatted." );

      Study study = new Study();
      study.Id = (int) ReadValue( node, "ID", typeof( int ));
      study.Patient.Id = (int) ReadValue( node, "PATIENT_ID", typeof( int ));

      DateTime scanDate = (DateTime) ReadValue( node, "SCAN_DATE", typeof( DateTime ));
      if( scanDate != DateTime.MinValue )
        study.Date = scanDate;

      study.ReferringPhysician = new Viatronix.v3D.Core.PersonName( (string) ReadValue( node, "REFERRING_PHYS", typeof( string )));
      study.UID = new UID( (string) ReadValue( node, "UID", typeof( string )));
      study.Description = (string) ReadValue( node, "DESCRIPTION", typeof( string ));
      study.DicomStudyId = (string) ReadValue( node, "DICOM_STUDY_ID", typeof( string ));
      study.AccessionNumber = (string) ReadValue( node, "ACCESSSION_NUMBER", typeof( string ));   
      return study;
    } // CreateStudy( directory, patient )


    /// <summary>
    /// Creates a collection of series
    /// </summary>
    /// <param name="root">directory</param>
    /// <param name="xmlDir">xml directory</param>
    /// <returns>list of series</returns>
    private ArrayList CreateSeries( DirectoryInfo root, DirectoryInfo xmlDir )
    { 
      XmlDocument doc = new XmlDocument();
      doc.Load( xmlDir.FullName + SeriesFileName );

      ArrayList list = new ArrayList();
      ArrayList exclusion = new ArrayList();
      foreach( XmlNode node in doc.SelectNodes( "//Table" ))
      {
        Series series = new Series();
        series.UID = new UID( (string) ReadValue( node, "UID", typeof( string )));

        if( exclusion.Contains( series.UID ))
          continue;
        else
          exclusion.Add( series.UID );

        series.Id = (int) ReadValue( node, "ID", typeof( int ));
        series.Study.Id = (int) ReadValue( node, "STUDY_ID", typeof( int ));
        series.Type = (string) ReadValue( node, "TYPE_ID", typeof( string ));

        DateTime scanDate = (DateTime) ReadValue( node, "SCAN_DATE", typeof( DateTime ));
        if( scanDate != DateTime.MinValue )
          series.Date = scanDate;
        
        DateTime dateRecieved = (DateTime) ReadValue( node, "FIRST_COMPUTE_DATE", typeof( DateTime ));
        if( dateRecieved != DateTime.MinValue )
          series.DateReceived = dateRecieved;

        series.State = (string) ReadValue( node, "STATE_ID", typeof( string ));

        series.SliceSpacing = (float) ReadValue( node, "SLICE_COUNT", typeof( float ));
        // TODO 
        //series.Parameters = (string) ReadValue( node, "PARAMETERS", typeof( string ));
        series.Modality = (string) ReadValue( node, "MODALITY", typeof( string ));
        series.AcquisitionNumber = Convert.ToInt64( ReadValue( node, "ACQUISITION_NUMBER", typeof( long )));
       
        string filename = (string) ReadValue( node, "FILE_NAME_ROOT", typeof( string ));

        list.AddRange( CreateSeries( root, filename, series ));
      }

      return list;
    } // CreateSeries( root, xmlDir )


    /// <summary>
    /// Creates a collection of series
    /// </summary>
    /// <param name="root">directory</param>
    /// <param name="filename">series file name</param>
    /// <param name="series">series object</param>
    /// <returns>list of series</returns>
    private ArrayList CreateSeries( DirectoryInfo root, string filename, Series series )
    {
      string name = ( Regex.IsMatch( filename, SupineRegex, RegexOptions.IgnoreCase )) ? "Supine" : "Prone";
      Node original = new Node( series.UID + " (" + name + " Original)", series );

      Series processedSeries = (Series) series.Clone();
      processedSeries.UID = UID.GenerateUID();
      Node processed = new Node( processedSeries.UID + " (" + name + " Preprocessed)", processedSeries );

      foreach( FileInfo file in root.GetFiles( filename + "*" ))
      {
        if( IsOrignialFile( file ))
          original.Nodes.Add( new Node( file.Name, file ));

        else
          processed.Nodes.Add( new Node( file.Name, file ));
      }

      ArrayList nodes = new ArrayList();
      if( original.Nodes.Count > 0 )
        nodes.Add( original );

      if( processed.Nodes.Count > 0 )
        nodes.Add( processed );

      return nodes;
    } // CreateSeries( root, filename, series )


    /// <summary>
    /// Determines if the specified file is a session file.
    /// </summary>
    /// <param name="file">The file being checked.</param>
    /// <returns>true if the file is a session file; otherwise false</returns>
    private bool IsSessionFile( FileInfo file )
    {
      return ( file.Extension.ToLower() == MarkingVolume || file.Extension.ToLower() == VCSession );
    } // IsSessionFile( file )


    /// <summary>
    /// Determines if the specified file is an original file.
    /// </summary>
    /// <param name="file">The file being checked.</param>
    /// <returns>true if the file is a session file; otherwise false</returns>
    private bool IsOrignialFile( FileInfo file )
    {
      return ( file.Extension.ToLower() == Volume || file.Extension.ToLower() == Dicom 
            || file.Extension.ToLower() == Image || file.Extension.ToLower() == DicomToVolume );
    } // IsOrignialFile( file )

    #endregion

    #region IFileStructureFilter Members

    /// <summary>
    /// Determines if the specified path matches a VCFileStructureFilter.
    /// </summary>
    /// <param name="path">The file system path.</param>
    /// <returns>true if the path is a VCFileStructureFilter; otherwise false</returns>
    public bool CanProcess( string path )
    {
      DirectoryInfo directory = new DirectoryInfo( path );
      if( !directory.Exists )
        return false;

      DirectoryInfo xmlDirectory = new DirectoryInfo( directory.FullName + "\\" + XmlDirectory + "\\" );
      if( !xmlDirectory.Exists )
        return false;

      if( xmlDirectory.GetFiles( PatientFileName ).Length == 0 )
        return false;

      if( xmlDirectory.GetFiles( StudyFileName ).Length == 0 )
        return false;

      if( xmlDirectory.GetFiles( SeriesFileName ).Length == 0 )
        return false;

      return true;
    } // CanProcess( path )


    /// <summary>
    /// Creates a Tree structure that represents the data contained in the specifeid path.
    /// </summary>
    /// <param name="path">The file system path.</param>
    /// <returns>A Tree structure that represents the data contained in the specifeid path.</returns>
    public Tree Process( string path )
    {
      DirectoryInfo root = new DirectoryInfo( path );
      DirectoryInfo xmlDir = new DirectoryInfo( root.FullName + "\\" + XmlDirectory + "\\" );

      Tree tree = new Tree();

      Patient patient = CreatePatient( xmlDir );
      Node patientNode = new Node( patient.Name.FullName, patient );
      tree.Nodes.Add( patientNode );

      Study study = CreateStudy( xmlDir, patient );

      Node studyNode = new Node( study.UID.ToString(), study );
      patientNode.Nodes.Add( studyNode );

      foreach( Node seriesNode in CreateSeries( root, xmlDir ))
        studyNode.Nodes.Add( seriesNode );

      // report node
      Node reportNode = new Node( ReportText, new Series() );
      foreach( FileInfo file in root.GetFiles( "*" + ReportFile ))
        reportNode.Nodes.Add( new Node( file.Name, file ));

      // add the report node only if it contains a report file
      if( reportNode.Nodes.Count > 0 )
        studyNode.Nodes.Add( reportNode );

      return tree;
    } // Process( path )

    #endregion


  } // class VCFileStructureFilterCD
} // namespace Viatronix.v3D.Console

#region revision history

// $Log: VCFileStructureFilterCD.cs,v $
// Revision 1.2  2007/02/21 21:11:39  gdavidson
// Commented code
//
// Revision 1.1  2006/10/30 15:35:04  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.9  2005/12/06 20:04:22  romy
// removed non referenced local variables
//
// Revision 1.8  2005/11/07 16:43:31  gdavidson
// Modified the IFileStructureFilter interface.
//
// Revision 1.7  2005/08/31 18:57:55  romy
// did add  namesapce to PersonName struct
//
// Revision 1.6  2005/08/18 13:07:32  mkontak
// Remove PatientId from series we will now use Study.Patient.Id
//
// Revision 1.5  2005/08/18 13:00:23  mkontak
// Remove StudyId from series we will now use Series.Study.Id
//
// Revision 1.4  2005/06/03 19:06:06  mkontak
// We need to fix
//
// Revision 1.3  2005/05/20 18:50:41  mkontak
// Removed patient age
//
// Revision 1.2  2005/05/03 18:18:20  frank
// reflected a change in the enumerations
//
// Revision 1.1  2005/04/19 20:16:12  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/filters/VCFileStructureFilterCD.cs,v 1.2 2007/02/21 21:11:39 gdavidson Exp $
// $Id: VCFileStructureFilterCD.cs,v 1.2 2007/02/21 21:11:39 gdavidson Exp $

#endregion
