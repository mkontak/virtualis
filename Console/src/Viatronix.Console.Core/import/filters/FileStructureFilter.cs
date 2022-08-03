// $Id: FileStructureFilter.cs,v 1.3.2.2 2007/06/25 21:22:48 romy Exp $
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )

using System;
using System.IO;
using System.ComponentModel;
using Viatronix.v3D.Dicom;
using Viatronix.v3D.Core;
using Viatronix.v3D.Utilities.IO;

namespace Viatronix.v3D.Console
{
	/// <summary>
	/// Base class for all data filters.
	/// </summary>
	public abstract class FileStructureFilter : IFileStructureFilter
  {

    #region fields

    /// <summary>
    /// Volume series datetime offset.
    /// </summary>
    public const double VolumeTime = 1.0d;

    /// <summary>
    /// Preprocessed series datetime offset.
    /// </summary>
    public const double PreprocessedTime = 2.0d;

    /// <summary>
    /// CAD series datetime offset.
    /// </summary>
    public const double CADTime = 3.0d;

    /// <summary>
    /// Session series datetime offset.
    /// </summary>
    public const double SessionTime = 4.0d;

    /// <summary>
    /// Dicom type
    /// </summary>
    public const string DicomType = "Dicom";

    /// <summary>
    /// Volume type
    /// </summary>
    public const string VolumeType = "Volume";

    /// <summary>
    /// Preprocessed type
    /// </summary>
    public const string PreprocessedType = "Preprocessed";

    /// <summary>
    /// CAD type
    /// </summary>
    public const string CADType = "CAD";

    /// <summary>
    /// Session type
    /// </summary>
    public const string SessionType = "Session";

    /// <summary>
    /// The name of the filter.
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// progress event
    /// </summary>
    public event FilterProgressEventHandler Progress = null; 

    #endregion

    #region methods

    /// <summary>
    /// Extracts the subtype from the path
    ///  in 2.0 all study directories had the following structure
    ///  CS_PATIENT...
    /// </summary>
    /// <param name="path">Path to find subtype of</param>
    /// <returns></returns>
    protected string GetSubType(string path)
    {
      Char[] del = { '_' };
      string[] sections = System.IO.Path.GetFileName(path).Split(del);

      return sections[0].ToLower();

    } // GetSubType(string path)

    /// <summary>
    /// Raises the Progress event.
    /// </summary>
    /// <param name="value"></param>
    /// <param name="max"></param>
    protected void OnProgress( int value, int max )
    {
      if( this.Progress != null )
        this.Progress( value, max );
    } // OnProgress( value, max )


    /// <summary>
    /// Creates an element list from files contained in the specifeid path.
    /// </summary>
    /// <param name="path"></param>
    /// <returns>An element list.</returns>
    protected ElementList CreateElementList( string path )
    {
      return CreateElementList( path, "*" );
    } // CreateElementList( path )


    /// <summary>
    /// Creates an element list from files contained in the specifeid path.
    /// </summary>
    /// <param name="path"></param>
    /// <returns>An element list.</returns>
    protected ElementList CreateElementList( string path, string pattern )
    {
      string[] files = Directory.GetFiles( path, pattern + Utilities.IO.FileExtensions.Dicom );
      if( files.Length == 0 )
      {
        files = Directory.GetFiles( path, pattern + Utilities.IO.FileExtensions.Volume );

        if ( files.Length == 0 )
        {
          files = Directory.GetFiles( path, pattern + Utilities.IO.FileExtensions.Ecv );
        }
      }
      
      if( files.Length > 0 )
        return CreateElementListFromFile( files[0] );
      else
      {
        Viatronix.v3D.Logging.Logger.Err( "No initialization data found. [PATH=" + path + "] [PATTERN=" + pattern + "]" );
        throw new FileNotFoundException( "No initialization data found." );
      }
    } // CreateElementList( path )


    /// <summary>
    /// Creates an element list from files contained in the specifeid path.
    /// </summary>
    /// <param name="path"></param>
    /// <returns>An element list.</returns>
    protected ElementList CreateElementListFromFile( string file )
    {
      ElementList elementList = null;
      try
      {
        elementList = new ElementList( file );
      }
      catch ( Exception ex )
      {
        elementList = null;
        Viatronix.v3D.Logging.Logger.Wrn("Failed to create the element list from [" + file + "]  : " + ex.Message ); 
      }

      ////////////////////////////////////////////////////////////////////////////
      /// Make sure the element list has not already been initialized
      ////////////////////////////////////////////////////////////////////////////
      if ( elementList == null )
      {
        Viatronix.v3D.Logging.Logger.Dbg("Attempting to extract information from volume header" ); 

        string extension = Path.GetExtension(file).ToLower();
        if ( extension != ".vol" && extension != ".ecv" )
        {

          string root = Path.GetFileNameWithoutExtension(file);
          string path = Path.GetDirectoryName(file);
          string pattern =  "*" + root;
          string [] files = Directory.GetFiles( path, pattern + Utilities.IO.FileExtensions.Volume );

          if ( files.Length == 0 )
          {
            files = Directory.GetFiles( path, pattern + Utilities.IO.FileExtensions.Ecv );
          }

          if ( files.Length > 0 )
          {
            file = files[0];
          }
          else
          {
            Viatronix.v3D.Logging.Logger.Err("No volume or ecv file found for [PATH=" + path + " ] [ROOT=" + root + "]");
            throw new FileNotFoundException("No volume or ecv file found");
          }
        }

        Viatronix.v3D.Logging.Logger.Dbg("Extract information from volume file [FILE=" + file + "]" ); 

        Viatronix.v3D.Utilities.VolumeHeader header = new Viatronix.v3D.Utilities.VolumeHeader( file );

        /// Log the volume header
        header.ToLogger();

        /// Create a new element list to populate
        elementList = new ElementList();

        /// Populate the element list with the volume header informationtry
        try
        {
          if ( header.SeriesDate.Length > 0 )  elementList.SeriesDate = new Viatronix.v3D.Dicom.Types.Date(header.SeriesDate);
        }
        catch (Viatronix.v3D.Dicom.InvalidDataException ex)
        {
          Viatronix.v3D.Logging.Logger.Wrn("Invalid series date [DATE=" + header.SeriesDate + "], ignoring : " + ex.Message);
        }

        try
        {
          elementList.StudyDate = ( header.StudyDate.Length > 0  ?  new Viatronix.v3D.Dicom.Types.Date(header.StudyDate) : Viatronix.v3D.Dicom.Types.Date.Now );
        }
        catch (Viatronix.v3D.Dicom.InvalidDataException ex)
        {
          Viatronix.v3D.Logging.Logger.Wrn("Invalid study date [DATE=" + header.StudyDate + "], setting to today : " + ex.Message);
          elementList.StudyDate = Viatronix.v3D.Dicom.Types.Date.Now;
        }

        elementList.SeriesTime  = new Viatronix.v3D.Dicom.Types.Time(12,0,0);
        elementList.StudyTime   = new Viatronix.v3D.Dicom.Types.Time(12,0,0);
        elementList.StudyInstanceUID  = ( header.StudyUID.Length == 0 ? UID.GenerateUID().ToString() : header.StudyUID );
        elementList.SeriesInstanceUID = ( header.SeriesUID.Length == 0 ? UID.GenerateUID().ToString()  : header.SeriesUID );
        
        if ( header.PatientAge.Length > 0 )
        {
          try
          {
            int iAge = System.Convert.ToInt32(header.PatientAge);

            if ( iAge > 0 )
            {
              elementList.PatientAge = new PersonAge(header.PatientAge);
            }
          }
          catch ( Exception )
          {
            Viatronix.v3D.Logging.Logger.Wrn("Invalid patient age, ignoring");
          }
        }

        try
        {
          if ( header.PatientDOB.Length > 0 ) elementList.PatientDOB = new Viatronix.v3D.Dicom.Types.Date(header.PatientDOB);
        }
        catch (Viatronix.v3D.Dicom.InvalidDataException ex)
        {
          Viatronix.v3D.Logging.Logger.Wrn("Invalid patient dob [DATE=" + header.PatientDOB + "], ignoring : " + ex.Message);
        }

        elementList.ReferringPhysician = new Viatronix.v3D.Core.PersonName(header.ReferringPysician);
        elementList.PatientName = new Viatronix.v3D.Core.PersonName(header.PatientName);
        elementList.PatientPosition = header.PatientPosition;
        elementList.PatientId = header.PatientID;
        elementList.StudyDescription = "Study generated from " + Path.GetFileName(Path.GetDirectoryName(file));
        elementList.Modality = "CT";      // Default

      } // END ... If the element list has not already been initialized

      /// Return
      return elementList;
    } // CreateElementListFromFile( file )


    /// <summary>
    /// Creates a Patient object.
    /// </summary>
    /// <param name="elements">ElementList</param>
    /// <returns>a Patient object from the ElementList or default Patient if the element list is invalid.</returns>
    protected Patient CreatePatient( ElementList elements )
    {
      Patient patient = null;
      try
      {
        patient = Dicom.Utilities.CreatePatient( elements );

        //Eliminate all invalid characters from the MEdical ID field.
        //This was causing an exception is importing old Perry's studies.
        patient.MedicalId = Viatronix.v3D.Utilities.IO.PathUtilities.EliminateInvalidFileNameChars(patient.MedicalId);
      }
      catch( Exception e )
      {
        Viatronix.v3D.Logging.Logger.Err( e.Message );

        // build a default patient object, with a GUID for a name
        patient = new Patient();
        patient.Name = new Viatronix.v3D.Core.PersonName( Guid.NewGuid().ToString() );
      }

      return patient;
    } // CreatePatient( elements )


    /// <summary>
    /// Creates a Study object.
    /// </summary>
    /// <param name="elements">ElementList</param>
    /// <param name="patient">The patient object.</param>
    /// <returns>a Study object from the ElementList or default Study if the element list is invalid.</returns>
    protected Study CreateStudy( ElementList elements, Patient patient )
    {
      Study study = null;
      try
      {
        study = Dicom.Utilities.CreateStudy( elements );
      }
      catch( Exception e )
      {
        Viatronix.v3D.Logging.Logger.Err( e.Message );

        // build a default study object
        study = new Study();
        study.Description = patient.Name + "'s Study";
      }

      // assign a UID to the Study
      study.UID = UID.GenerateUID();
      patient.Studies.Add( study );
      study.Patient = patient;

      return study;
    } // CreateStudy( elements, patient )

    /// <summary>
    /// Creates a Series object.
    /// </summary>
    /// <param name="elements">ElementList</param>
    /// <param name="type">series type</param>
    /// <param name="study">study this series belongs to</param>
    /// <param name="generateUID">flags used to determine if we are to generate a UID</param>
    /// <returns>a Series object from the ElementList or default Series if the element list is invalid.</returns>
    protected Series CreateSeries(ElementList elements, string type,  Study study, bool generateUID)
    {
      return CreateSeries(elements, type, string.Empty, null, study, generateUID);
    } // CreateSeries(ElementList elements, string type,  Study study, bool generateUID)

    /// <summary>
    /// Creates a Series object.
    /// </summary>
    /// <param name="elements">ElementList</param>
    /// <param name="type">series type</param>
    /// <param name="subType">series subtype</param>
    /// <param name="study">study this series belongs to</param>
    /// <param name="generateUID">flags used to determine if we are to generate a UID</param>
    /// <returns>a Series object from the ElementList or default Series if the element list is invalid.</returns>
    protected Series CreateSeries(ElementList elements, string type, string subType, Study study, bool generateUID)
    {
      return CreateSeries(elements, type, subType, null, study, generateUID);
    } // CreateSeries(ElementList elements, string type,  string subTypeStudy study, bool generateUID)

    /// <summary>
    /// Creates a Series object.
    /// </summary>
    /// <param name="elements">ElementList</param>
    /// <param name="type">series type</param>
    /// <param name="study">study this series belongs to</param>
    /// <param name="generateUID">flags used to determine if we are to generate a UID</param>
    /// <returns>a Series object from the ElementList or default Series if the element list is invalid.</returns>
    protected Series CreateSeries( ElementList elements, string type,  string subType, string description, Study study, bool generateUID )
    { 
  
      // Series
      Series series = null;

      ////////////////////////////////////////////////
      // Attempt to initialize from the elements list
      ////////////////////////////////////////////////
      try
      {
        series = Dicom.Utilities.CreateSeries( elements );
      }
      catch( Exception e )
      {
        Viatronix.v3D.Logging.Logger.Err( "Unable to create series from ElementList. [ERROR=" + e.Message + "]" );
        series = new Series();
      }

      // Generate a new UID
      if( generateUID )
        series.UID = UID.GenerateUID();


      if (description != null)
      {
        series.Description = description;
      }
      else
      {
        if ( type != "Dicom" || (series.Description.Length == 0 && type == "Dicom")  )
        {
          series.Description = series.PatientOrientation + (series.PatientOrientation.Length > 0 ? " " : string.Empty) + type + " Series";
        }
      }

      /////////////////////////////////////////////
      // Attempt to set the process correctly
      ////////////////////////////////////////////
      if ( string.Compare(type, "Volume", true ) == 0 )
      {
        series.Process = "Conversion";
      }
      else if ( string.Compare(type, "Preprocessed", true ) == 0 )
      {
        series.Process = "Pipeline";
      }
      else if ( string.Compare(type, "Dicom", true) == 0 )
      {
        series.Process = "Receiving";


      }
      else 
      {
        series.Process = type;
      }

      DateTime now = DateTime.Now;

      series.DateProcessed  = now;
      series.DateReceived   = now;
      series.Type           = type;
      series.PreprocessAs   = subType;
      series.SubType        = subType;
      series.Study          = study;

      study.Series.Add(series);



      // modify the DateProcessed base on the series type (Adjust for appropriate sort)
      if( type == FileStructureFilter.VolumeType )
        series.DateProcessed = series.DateProcessed.AddHours( FileStructureFilter.VolumeTime );
      else if( type == FileStructureFilter.PreprocessedType )
        series.DateProcessed = series.DateProcessed.AddHours( FileStructureFilter.PreprocessedTime );
      else if( type == FileStructureFilter.CADType )
        series.DateProcessed = series.DateProcessed.AddHours( FileStructureFilter.CADTime );
      else if( type == FileStructureFilter.SessionType )
        series.DateProcessed = series.DateProcessed.AddHours( FileStructureFilter.SessionTime );

      /// Is the series is a session do mark as comnplete
      series.State = ( string.Compare(type, "Session", true) == 0 ? "" : "Completed" );

      // Set the location to account for the new system
      series.Location = Viatronix.v3D.Utilities.IO.PathUtilities.CombinePaths(System.IO.Path.GetDirectoryName(series.Location), Series.GenerateDirecrtoryName(series));

      // Set the full path to a TEMP directory in case we plan on copying
      series.FullPath = Directory.CreateDirectory(Path.Combine(Utilities.Registry.SystemSettings.TempDirectory, series.Location)).FullName;


      return series;
    } // CreateSeries( elements )

    #endregion

    #region IFileStructureFilter Members


    /// <summary>
    /// Gets or sets the name.
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { m_name = value; }
    } // Name

    /// <summary>
    /// Determines if the structure returned by the filter is relative to the full paths in the series objects.
    /// </summary>
    public virtual bool IsRelative
    {
      get { throw new NotImplementedException( "IsRelative has not been implemented for this filter." ); }
    } // IsRelative


    /// <summary>
    /// Determines if any temporary files need to be deleted.
    /// </summary>
    public virtual bool RequiresDeletion
    { 
      get { throw new NotImplementedException( "RequiresDeletion has not been implemented for this filter." ); } 
    } // RequiresDeletion


    /// <summary>
    /// Deteremines if the filter is capable of processing the path.
    /// </summary>
    /// <param name="path"></param>
    /// <returns></returns>
    public virtual bool CanProcess( string path )
    {
      throw new NotImplementedException( "CanProcess has not been implemented for this filter." );
    } // CanProcess( path )


    /// <summary>
    /// Creates a patient object containing all information specified in the path.
    /// </summary>
    /// <param name="path"></param>
    /// <param name="args"></param>
    /// <returns></returns>
    public virtual Patient Process( string path, CancelEventArgs args )
    {
      throw new NotImplementedException( "Process has not been implemented for this filter." );
    } // Process( path, args )

    #endregion

  } // class FileStructureFilter
} // namespace Viatronix.v3D.Console

#region revision history

// $Log: FileStructureFilter.cs,v $
// Revision 1.3.2.2  2007/06/25 21:22:48  romy
// added check for invalid characters in MedicalID field. This was causing an error in improting some of Perry's datasets
//
// Revision 1.3.2.1  2007/05/30 18:17:20  mkontak
// Get the current date and time once instead of multiple times
//
// Revision 1.3  2007/03/06 19:14:59  gdavidson
// Added class comments
//
// Revision 1.2  2007/02/28 17:02:04  mkontak
// Check the path for a subtype
//
// Revision 1.1  2006/10/30 15:35:03  gdavidson
// Moved from Viatronix.v3D.Console
//
// Revision 1.18  2006/08/21 12:50:19  mkontak
// Issue 4948: Processing volume.dicom series. Fixes the deletion of
// the preprocess parameters set from the console.
//
// Revision 1.17  2006/08/21 12:37:20  mkontak
// Issue 4948: Processing volume.dicom series. Fixes issue with seroies being marked as complete
// when they should not be.
//
// Revision 1.16  2006/07/18 14:52:35  mkontak
// Fix rights and transferring into the system
//
// Revision 1.15  2006/06/05 18:35:27  gdavidson
// ported to vs2005
//
// Revision 1.14  2006/02/07 18:07:28  gdavidson
// Added a name property to the filters
//
// Revision 1.13  2005/12/02 15:29:53  gdavidson
// Commented code
//
// Revision 1.12  2005/11/25 15:49:15  gdavidson
// Added a specfic number of hours to the date processed of a series.
//
// Revision 1.11  2005/11/24 00:06:06  gdavidson
// Set the date completed and processed in the CreateSeries method
//
// Revision 1.10  2005/11/22 17:49:17  gdavidson
// Modified the FilteStructureFilter's CreateSeries method
//
// Revision 1.9  2005/11/17 19:13:04  gdavidson
// Modified the CreateElementList method.
//
// Revision 1.8  2005/11/16 22:45:45  gdavidson
// Commented code
//
// Revision 1.7  2005/11/16 20:35:56  mkontak
// Check for a age of 0 and ignore it.
//
// Revision 1.6  2005/11/16 18:40:27  mkontak
// Default the modality to CT
//
// Revision 1.5  2005/11/16 17:59:39  mkontak
// Fix for invalid dates
//
// Revision 1.4  2005/11/16 16:19:11  mkontak
// Fixed the import
//
// Revision 1.3  2005/11/15 19:38:43  mkontak
// Modified to report error if we fail to create the element list
//
// Revision 1.2  2005/11/10 19:45:44  gdavidson
// Added a method to create an ElementList from either a dicom or vol file.
//
// Revision 1.1  2005/11/08 21:37:08  gdavidson
// Initial revision
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Console.Core/import/filters/FileStructureFilter.cs,v 1.3.2.2 2007/06/25 21:22:48 romy Exp $
// $Id: FileStructureFilter.cs,v 1.3.2.2 2007/06/25 21:22:48 romy Exp $

#endregion
