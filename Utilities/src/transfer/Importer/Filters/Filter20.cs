// $Id: Filter20.cs,v 1.1.2.2 2009/06/04 18:56:07 kchalupa Exp $000000004
//
// Copyright@ 2008, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc.
//
// Owner: [ Kyle R Chalupa ] email: [ kchalupa@viatronix.com ]

using System;
using System.IO;
using System.Xml;
using System.Collections.Generic;
using System.Text;

using Viatronix.Dicom;
using Viatronix.Logging;
using Viatronix.Enterprise.Entities;
using Viatronix.Utilities.Importer.Data;

namespace Viatronix.Utilities.Importer.Filters
{
  /// <summary>
  /// Abstracts the methods that should be performed by filters that accept V3D 2.0 folders.
  /// </summary>
  public abstract class Filter20 : IFilter
  {

    #region methods

    /// <summary>
    /// Indicates whether or not the particular filter in question can parse the current directory's contents.
    /// </summary>
    /// <param name="ioArgs">Path to the directory.</param>
    /// <returns>True if it can parse, otherwise false.</returns>
    public abstract bool CanParse(ImporterArgs ioArgs);

    /// <summary>
    /// Parses the directory pointed to by ioArgs and returns the appropriate ImportManager representation
    /// of the directory's contents.
    /// </summary>
    /// <param name="ioArgs">Path to the directory.</param>
    /// <returns>An appropriate import manager object.</returns>
    public abstract IImporter Parse(ImporterArgs ioArgs);

    /// <summary>
    /// Checks to see if the file in question is a session file.
    /// </summary>
    /// <remarks>
    /// Performs a lower case sensative check.  Caller is responsible for
    /// forcing consistency by using string.ToLower().
    /// </remarks>
    /// <param name="path">The path to the file.</param>
    /// <returns>Whether or not the file is a session file.</returns>
    public abstract bool IsSession(string path);

    /// <summary>
    /// Checks to see if the file in question is a dicom file.
    /// </summary>
    /// <remarks>
    /// Performs a lower case sensative check.  Caller is responsible for 
    /// forcing consistency by using string.ToLower().
    /// </remarks>
    /// <param name="path">The path to the file.</param>
    /// <returns>Whether or not the file is a dicom file.</returns>
    protected virtual bool IsDicom(string path)
    {
      return path.EndsWith(".dcm");
    } // IsDicom(string path)

    /// <summary>
    /// Checks to see if the file in question is a volume file.
    /// </summary>
    /// <remarks>
    /// Performs a lower case sensative check.  Caller is responsible for
    /// forcing consistency by using string.ToLower().
    /// </remarks>
    /// <param name="path">The path to the file.</param>
    /// <returns>Whether or not the file is a volume file.</returns>
    protected virtual bool IsVolume(string path)
    {
      return path.EndsWith(".vol") || path.EndsWith(".ecv") || path.EndsWith("_hq") || path.EndsWith("dcmtovol.txt");
    } // IsVolume(string path)

    /// <summary>
    /// Checks to see if the file in question is a cad file.
    /// </summary>
    /// <remarks>
    /// Performs a lower case sensative check.  Caller is responsible for
    /// forcing consistency by using string.ToLower().
    /// </remarks>
    /// <param name="path">The path to the file.</param>
    /// <returns>Whether or not the file is a cad file.</returns>
    protected virtual bool IsCad(string path)
    {
      return path.EndsWith(".cad.xml") || path.EndsWith(".cad.log.txt");
    } // IsCad(string path)

    /// <summary>
    /// Checks to see if the file in question is a preprocess file.
    /// </summary>
    /// <remarks>
    /// Always returns true, because there is no real set file list for preprocessed files.  As a
    /// result, this function should either be called, or it's results assumed as a last resort.
    /// </remarks>
    /// <param name="path">The path to the file.</param>
    /// <returns>Whether or not the file is a preprocessed file.</returns>
    protected virtual bool IsPreprocess(string path)
    {
      return true;
    } // IsPreprocess(string path)

    /// <summary>
    /// Checks to see if the file in question is a movie file.
    /// </summary>
    /// <remarks>
    /// Performs a lower case sensative check.  Caller is responsible for
    /// forcing consistency by using string.ToLower().
    /// </remarks>
    /// <param name="path">The path to the file.</param>
    /// <returns>Whether or not the file is a movie file.</returns>
    protected virtual bool IsMovie(string path)
    {
      return path.EndsWith(".avi") || path.EndsWith(".wmv");
    } // IsMovie(string path)


    /// <summary>
    /// Checks to see if the file in question is a snapshot file.
    /// </summary>
    /// <remarks>
    /// Performs a lower case sensative check.  Caller is responsible for
    /// forcing consistency by using string.ToLower().
    /// </remarks>
    /// <param name="path">The path to the file.</param>
    /// <returns>Whether or not the file is a snapshot file.</returns>
    protected virtual bool IsSnapshot(string path)
    {
      return path.EndsWith(".png");
    } // IsSnapshot(string path)


    /// <summary>
    /// Checks to see if the file in question should be ignored.
    /// </summary>
    /// <remarks>
    /// Performs a lower case sensative check.  Caller is responsible for
    /// forcing consistency by using string.ToLower().
    /// </remarks>
    /// <param name="path">The path to the file.</param>
    /// <returns>Whether or not the file is to be ignored.</returns>
    protected virtual bool IsJunk(string path)
    {
      return path.EndsWith(".id") || path.EndsWith(".xml");
    } // IsJunk(string path)


    /// <summary>
    /// Checks for the existance of the xxxxxx_DCM folder and imports the files contained therein
    /// if it exists and it has the same dataSet as the supplied parameter.
    /// </summary>
    /// <param name="parsingPath">The path that may contain a xxxxxxxx_DCM folder.</param>
    /// <param name="parser">The parser that will invoke this method.  "VC", "XP"</param>
    /// <param name="manager">The ImportManager that the dicom files will be added to.</param>
    /// <returns>
    /// A list of strings containing dataSets where an xxxxxx_DCM folder existed
    /// and had Dicom files in it.
    /// </returns>
    protected virtual List<string> ParseDicomFolders(string parsingPath, string parser, ImportManager20 manager)
    {
      List<string> dicomDataSets = new List<string>();
      string[] subDirectories = Directory.GetDirectories(parsingPath);

      // Look for the xxxxxxx_DCM directory with the provided dataSet.
      foreach( string subDirectory in subDirectories )
      {
        string subDirectoryLC = subDirectory.ToLower();

        // Did we find it?
        if( subDirectoryLC.EndsWith("_dcm"))
        {
          // Initialize a counter 
          int i=-1;

          string subDirectoryDataSet = Utils.GetDataSet(subDirectoryLC);

          // If we add at least one file, add the dataset to the list.
          bool addedDicomFile = false;

          // Define a parsed series, this will assigned to so that a call to addFile in the manager isn't needed
          // expensive lookups will be lessened.
          ParsedSeries seriesToAddTo = null;

          foreach( string file in Directory.GetFiles(subDirectory) )
          {
            if( IsDicom(file.ToLower()) )
            {
              if(seriesToAddTo == null)
              {
                string uid = ImageFactory.Create(file, ParameterType.FILE).InstanceUid.Replace(".", "_");

                // Add the file and then preserve the series that was added to.
                manager.AddFile(subDirectoryDataSet, SeriesTypes.Dicom, parser, file, string.Format("dicom\\{0}.dcm", uid), string.Empty);

                seriesToAddTo = manager.GetParsedSeries(subDirectoryDataSet, SeriesTypes.Dicom);
              } // if(seriesToAddTo == null)
              else
              {
                seriesToAddTo.AddFile(file, ( "dicom\\{0}" + "_" + ( ++i ).ToString() + 
                  Path.GetExtension(file) ), string.Empty);
              } // else

              addedDicomFile = true;

            } // if(IsDicom(file.ToLower()))
          } // foreach(string file in Directory.GetFiles(subDirectory))

          // If we added at least one file and the dataSet was not already added, add it to the parsed list.
          if(addedDicomFile && !dicomDataSets.Contains(subDirectoryDataSet))
          {
            dicomDataSets.Add(subDirectoryDataSet);
          } // if(addedDicomFile && !dicomDataSets.Contains(subDirectoryDataSet))
        } // if(subDirectory.ToLower().EndsWith("_dcm"))
      } // foreach(string subDirectory in subDirectories)

      return dicomDataSets;
    } // CheckDicomFolderInsert(string parsingPath, ImportManager manager)


    /// <summary>
    /// Trnasplanted from XPFileStructureFilter.
    /// Moves the label volume to the dataset directory and creats Components, Annotations and Path xml files from the session.xml
    /// </summary>
    /// <param name="sessionXmlPath">Path to the session.xml file.</param>
    /// <param name="relativeDestination">The relative destination where the files should be placed.</param>
    /// <param name="parsedSeriesToAddTo">The parsed series that the generated files belong to.</param>
    protected virtual void GenerateAnnotationFiles(string sessionXmlSourcePath, string relativeDestinationSessionXml, 
      string relativeDestinationAnnotations, ParsedSeries parsedSeriesToAddTo)
    {
      // Generate new session.xml file as a Virtual file.
      StringBuilder contentBuilder = new StringBuilder();

      XmlWriterSettings settings = new XmlWriterSettings();
      settings.Indent = true;
      settings.IndentChars = "  ";
      settings.Encoding = Encoding.ASCII;

      // write the node out to its own file
      using( XmlWriter writer = XmlWriter.Create(contentBuilder, settings) )
      {
        writer.WriteStartDocument();
        writer.WriteStartElement("session");
        writer.WriteAttributeString("version", "2.1");
        writer.WriteAttributeString("pluginID", parsedSeriesToAddTo.Series.Application.ToLower());
        writer.WriteStartElement("datasets");
        writer.WriteStartElement("seriesGroup");

        foreach( Series referencedSeries in parsedSeriesToAddTo.Series.References )
        {
          writer.WriteStartElement("series");
          writer.WriteAttributeString("uid", referencedSeries.Uid);
          writer.WriteAttributeString("path", string.Empty);
          writer.WriteAttributeString("type", referencedSeries.Type);
          writer.WriteAttributeString("subtype", referencedSeries.Application);
          writer.WriteEndElement();
        } // foreach( Series series in parsedSeriesToAddTo.Series.ReferencedSeries )

        writer.WriteEndDocument();
        writer.Flush();

      } // using( XmlWriter writer = XmlWriter.Create(contentBuilder, settings) )

      string generatedXmlString = contentBuilder.ToString();

      string header = "<?xml version=\"1.0\" encoding=\"utf-16\"?>";
      int position = generatedXmlString.IndexOf(header);

      // Remove the start document using utf-16.
      if( position > -1 )
      {
        // Put a new one in with the correct encoding.
        generatedXmlString = generatedXmlString.Remove(position, header.Length);
        generatedXmlString = "<?xml version=\"1.0\" encoding=\"ASCII\"?>" + generatedXmlString;
      } // if( position > -1 )

      // Add the session.xml as a virtual file.
      parsedSeriesToAddTo.AddVirtualFile(generatedXmlString, "session.xml", string.Empty);

      XmlDocument document = new XmlDocument();
      document.Load(sessionXmlSourcePath);

      // create seperate Components, Annotations and Path xml files from the information in the old *session.xml file
      try
      {
        parsedSeriesToAddTo.AddVirtualFile(WriteXmlContentsToString(document.DocumentElement, "Components"),
          Path.Combine(relativeDestinationAnnotations, "components.xml"), string.Empty);
      } // try
      catch(ApplicationException e)
      {
        Log.Error("File: components.xml could not be created." + 
          "  Components node not found in session.xml.  Exception: " + e.Message, "Filter20", "GenerateAnnotationFiles");
      } // catch(ApplicationException e)

      try
      {
        parsedSeriesToAddTo.AddVirtualFile(WriteXmlContentsToString(document.DocumentElement, "Annotations"),
          Path.Combine(relativeDestinationAnnotations, "annotations.xml"), string.Empty);
      } // try
      catch( ApplicationException e )
      {
        Log.Error("File: annotations.xml could not be created." +
          "  Annotations node not found in session.xml  Exception: " + e.Message, "Filter20", "GenerateAnnotationFiles");
      } // catch(ApplicationException e)

      try
      {
        parsedSeriesToAddTo.AddVirtualFile(WriteXmlContentsToString(document.DocumentElement, "Vessel"),
          Path.Combine(relativeDestinationAnnotations, "path.xml"), string.Empty);
      } // try
      catch(ApplicationException e)
      {
        Log.Error("File: path.xml could not be created." +
          "  Vessel node not found in session.xml  Exception: " + e.Message, "Filter20", "GenerateAnnotationFiles");
      } // catch(ApplicationException e)

      // If it's a vascular subType then try to create the isotropicComponents.xml file.
      if(parsedSeriesToAddTo.Series.Application == "VAS")
      {
        try
        {
          parsedSeriesToAddTo.AddVirtualFile(WriteXmlContentsToString(document.DocumentElement, "//VascularPlugin"),
            Path.Combine(relativeDestinationAnnotations, "isotropicComponents.xml"), string.Empty);
        } // try
        catch( ApplicationException e )
        {
          Log.Error("File: isotropicComponents.xml could not be created." +
          "  VascularPlugin node not found in session.xml  Exception: " + e.Message, "Filter20", "GenerateAnnotationFiles");
        } // catch(ApplicationException e)
      } // if(parsedSeriesToAddTo.Series.SubType == "VAS")
    } // GenerateSessionFiles( ioArgs, session )
    

    /// <summary>
    /// Trnasplanted from XPFileStructureFilter.
    /// Takes the information located under the specified node and writesit to a seperate file.
    /// </summary>
    /// <param name="parent">The parent xml node.</param>
    /// <param name="destination">The output file</param>
    /// <param name="xpath">The xpath query.</param>
    /// <param name="session">The session series</param>
    protected string WriteXmlContentsToString(XmlNode parent, string xpath)
    {
      StringBuilder contentBuilder = new StringBuilder();

      Encoding encodeASCII = Encoding.ASCII;

      // find the internal node using the nodeName as an XPath query
      XmlNode node = parent.SelectSingleNode(xpath);
      if( node != null )
      {
        XmlWriterSettings settings = new XmlWriterSettings();
        settings.Indent = true;
        settings.IndentChars = "  ";
        settings.Encoding = encodeASCII;

        // write the node out to its own file
        using( XmlWriter writer = XmlWriter.Create(contentBuilder, settings) )
        {
          writer.WriteStartDocument();

          using (XmlNodeReader reader = new XmlNodeReader(node))
          {
            writer.WriteNode(reader, false);
          } // using (XmlNodeReader reader = new XmlNodeReader(node))

          writer.WriteEndDocument();

          writer.Flush();
        } // using( XmlWriter writer = XmlWriter.Create(resultBuilder) )
      } // if( node != null )
      else
      {
        throw new ApplicationException("The supplied XPath query: " + xpath + " didn't yield approrpriate results.");
      } // else

      string result = contentBuilder.ToString();

      string header = "<?xml version=\"1.0\" encoding=\"utf-16\"?>";
      int position = result.IndexOf(header);

      // Remove the start document using utf-16.
      if( position > -1 )
      {
        // Put a new one in with the correct encoding.
        result = result.Remove(position, header.Length);
        result = "<?xml version=\"1.0\" encoding=\"ASCII\"?>" + result;
      } // if( position > -1 )
      
      return result;
    } // WriteFile( parent, destination, nodeName, session )

    #endregion

  } // Filter20
} // namespace Viatronix.Utilities.Importer.Filters

#region revision history

// $Log: Filter20.cs,v $
// Revision 1.1.2.2  2009/06/04 18:56:07  kchalupa
// Coding standards.
//
// Revision 1.1.2.1  2008/09/24 14:31:19  mkontak
// Initial Check in
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Utilities.Importer/Filters/Filter20.cs,v 1.1.2.2 2009/06/04 18:56:07 kchalupa Exp $
// $Date: 2009/06/04 18:56:07 $24 July 2008
// $Id: Filter20.cs,v 1.1.2.2 2009/06/04 18:56:07 kchalupa Exp $000000004

#endregion
