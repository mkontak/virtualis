// $Id: CadReportReader.cs,v 1.1.4.2 2010-05-17 19:51:54 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@viatronix.com]

using System;
using System.IO;
using System.Xml;
using System.Configuration;
using System.Collections.Generic;
using System.Text;

using Viatronix.Logging;

namespace Viatronix.Colon.Medicsight.CAD.Validation
{

  #region exceptions

  /// <summary>
  /// Exception for the invalid units error.
  /// </summary>
  public class InvalidUnitsException : Exception
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public InvalidUnitsException() : base()
    {
    } // InvalidUnitsException()


    /// <summary>
    /// Constructor.
    /// </summary>
    /// <param name="message">The message.</param>
    public InvalidUnitsException(string message) : base(message)
    {
    } // InvalidUnitsException( message )

    #endregion

  } // InvalidUnitsException

  #endregion
  
  /// <summary>
  /// Reads in the cad file and (if in validation mode) writes it out in the specified text format.
  /// </summary>
  public class CadReportReader
  {

    #region fields

    /// <summary>
    /// List of CadFindings.
    /// </summary>
    private List<CadFinding> m_cadFindings = new List<CadFinding>();

    /// <summary>
    /// A map from the sphericity value to the findings that belong to it.
    /// </summary>
    private Dictionary<int, List<CadFinding>> m_mapSphericityToFindingsMap = new Dictionary<int,List<CadFinding>>();

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public CadReportReader()
    {
    } // CadConverter()

    #endregion

    #region properties
    
    /// <summary>
    /// Gets or sets the list of findings.
    /// </summary>
    public List<CadFinding> Findings
    {
      get { return m_cadFindings;  }
      set { m_cadFindings = value; }
    } // Findings


    /// <summary>
    /// Gets the map from sphericity value to all the CadFindings that are associated with it, as read from the Cad Findings Report.
    /// </summary>
    public Dictionary<int, List<CadFinding>> SphericityValuesToCadFindings
    {
      get { return m_mapSphericityToFindingsMap; }
    } // SphericityValuesToCadFindings

    #endregion

    #region methods

    /// <summary>
    /// Indicates whether this ICadReportReader can read the report.
    /// </summary>
    /// <param name="filePath">The report to read.</param>
    /// <returns>True if this ICadReportReader can read the report, false otherwise.</returns>
    public bool CanReadReport(string filePath)
    {
      XmlDocument doc = new XmlDocument();
      doc.Load(filePath);

      XmlAttribute vendor = doc.DocumentElement.Attributes["cad_vendor"];
      XmlAttribute version = doc.DocumentElement.Attributes["cad_API_version"];

      if (doc.DocumentElement.Name == "findings" && vendor != null && vendor.Value == "Medicsight" && version != null && version.Value == "4.0")
      {
        return true;
      }

      return false;
    } // CanReadReport(string filePath)


    /// <summary>
    /// Reads a finding node from the Xml and creates a CadFinding object from it.
    /// </summary>
    /// <param name="findingNode">The node to parse.</param>
    /// <returns>A CadFinding based on the XmlNode.</returns>
    public CadFinding ReadFindingFromXml(XmlNode findingNode)
    {
      if (findingNode.Name == "finding" && findingNode.Attributes["version"].Value == "1.1")
      {
        CadFinding finding = new CadFinding();

        finding.Guid = new Guid(findingNode.Attributes["GUID"].Value);

        XmlNode centroidNode = findingNode.SelectSingleNode("centroid");

        if (centroidNode != null && centroidNode.Attributes["units"].Value != "voxels")
        {
          throw new InvalidUnitsException("Finding has units type that is not compatible with this program.");
        }

        try
        {
          finding.Centroid = Point3Df.FromXml(findingNode.SelectSingleNode("centroid"));
        }
        catch (XmlException e)
        {
          Log.Error("Unable to read centroid coordinates; Exception: " + e.Message, "CadReportReader", "ReadFindingFromXml");
          throw new ApplicationException("Unable to read centroid coordinates; Exception: " + e.Message);
        }

        foreach (XmlNode variableNode in findingNode.SelectNodes("vendor_specific_variables/variable"))
        {
          XmlAttribute key = variableNode.Attributes["key"];
          XmlAttribute value = variableNode.Attributes["value"];

          if (key != null && value != null)
          {
            if (variableNode.Attributes["key"].Value.IndexOf("Sphericity") > -1)
            {
              finding.SphericityValues.Add(XmlConvert.ToInt32(variableNode.Attributes["value"].Value));
            }
          }
          else
          {
            Log.Error("Key or Value attributes were missing from variable node.", "CadReportReader", "ReadFindingFromXml");
          }
        }

        return finding;
      }
      else
      {
        Log.Error("Name or version attributes did not match expections for a Cad Finding.", "CadReportReader", "ReadFindingFromXml");
        throw new ApplicationException("Name or version attributes did not match expections for a Cad Finding.");
      }
    } // ReadFindingFromXml(XmlNode findingNode)


    /// <summary>
    /// Finds the relationships between the points and the sphercitities they are related to.
    /// </summary>
    /// <returns>A map of the sphericity value and all the points that are related to that value.</returns>
    public Dictionary<int, List<CadFinding>> FindSphericityFindingsRelationships()
    {
      int[] sphericityValues = Utils.ReadDesiredSphericityValues();
      Dictionary<int, List<CadFinding>> sphericityFindingMap = new Dictionary<int,List<CadFinding>>();

      foreach (int sphericityValue in sphericityValues)
      {
        List<CadFinding> sphericityFindings = new List<CadFinding>();

        foreach (CadFinding finding in m_cadFindings)
        {
          if (finding.SphericityValues.Contains(sphericityValue))
          {
            sphericityFindings.Add(finding);
          }
        }

        List<CadFinding> findings = null;
        if (sphericityFindingMap.TryGetValue(sphericityValue, out findings))
        {
          findings.AddRange(sphericityFindings);
        }
        else
        {
          sphericityFindingMap.Add(sphericityValue, sphericityFindings);
        }
      }

      return sphericityFindingMap;
    } // FindSphericityFindingsRelationships()


    /// <summary>
    /// Reads the Xml file and builds up the list of cad findings.
    /// </summary>
    /// <param name="filePath">The file to read.</param>
    /// <param name="pointConverter">Converts the centroid into DICOM coordinates.</param>
    public void ReadXmlFile(string filePath, Point3DConverter pointConverter)
    {
      try
      {
        XmlDocument doc = new XmlDocument();
        doc.Load(filePath);

        m_cadFindings.Clear();

      //=========================================================================================================
      // Iterate over the finding nodes and read them into memory.
      //=========================================================================================================

        foreach (XmlNode findingNode in doc.DocumentElement.SelectNodes("finding"))
        {
          try
          {
            CadFinding finding = ReadFindingFromXml(findingNode);
            finding.ConvertedCentroid = pointConverter.ConvertVolToDICOMCoordinates(finding.Centroid);
            m_cadFindings.Add(finding);
          }
          catch (XmlException e)
          {
            Log.Error("Unable to parse finding node; Exception: " + e.Message, "CadReportReader", "ReadXmlFile");
          }
        }

        m_mapSphericityToFindingsMap = FindSphericityFindingsRelationships();
      }
      catch(XmlException e)
      {
        Log.Error("Could not read file; Exception: " + e.Message, "CadReportReader", "ReadXmlFile");
        throw new XmlException("Could not read file; Exception: " + e.Message);
      }
    } // ReadXmlFile(string filePath)


    /// <summary>
    /// Saves the data read in out to a text file.
    /// </summary>
    /// <param name="filePath">The file to write out to.</param>
    /// <param name="inputFilePath">The input file that generated this output.</param>
    /// <param name="pointConverter">Helps to write out debug data.</param>
    public void SaveTextFile(string filePath, string inputFilePath)
    {
      try
      {
        //=======================================================================================================
        // Iterate over the sphericityList and find all the Findings that contain the sphericity value.
        //=======================================================================================================

        Comparison<CadFinding> findingComparer = new Comparison<CadFinding>(CadFinding.CompareConvertedCentroidPoints);

        using (TextWriter writer = new StreamWriter(filePath))
        {
          string seriesName = Path.GetFileName(filePath);
          seriesName = Path.GetFileNameWithoutExtension(seriesName);

          writer.WriteLine();
          writer.WriteLine("------------------------------------------------------------------------------");
          writer.WriteLine("Input File: " + inputFilePath);
          writer.WriteLine("------------------------------------------------------------------------------");
          writer.WriteLine();

          // Used for debugging the centroid conversion.
          //writer.WriteLine();
          //writer.WriteLine("Body Position: " + pointConverter.BodyPosition);
          //writer.WriteLine("Reference Point: " + pointConverter.ReferencePoint.ToString());
          //writer.WriteLine("Units: " + pointConverter.Units.ToString());
          //writer.WriteLine("Dimension: " + pointConverter.Dimensions.ToString());
          //writer.WriteLine();
          
          writer.WriteLine();

          foreach (KeyValuePair<int, List<CadFinding>> keyPair in m_mapSphericityToFindingsMap)
          {
            //=======================================================================================================
            // Write out the sphericity value and the findings that were related.
            //=======================================================================================================

            writer.WriteLine("Sphericity=" + keyPair.Key);
            writer.WriteLine(keyPair.Value.Count + " regions found.");
            writer.WriteLine();

            for (int i = 0; i < keyPair.Value.Count; ++i)
            {
              // Used for normal output.
              //writer.WriteLine(keyPair.Value[i].ConvertedCentroid.ToString(6));

              Point3Df centroid = keyPair.Value[i].Centroid;
              Point3Df convCentroid = keyPair.Value[i].ConvertedCentroid;

              // Used for debugging the centroid conversion.
              writer.WriteLine("Volume: " + centroid.X + Utils.GenerateSpacing(centroid.X, 7) + centroid.Y + Utils.GenerateSpacing(centroid.Y, 7) + centroid.Z + Utils.GenerateSpacing(centroid.Z, 7) + " DICOM: " + convCentroid.X.ToString() + Utils.GenerateSpacing(convCentroid.X, 15) + convCentroid.Y.ToString() + Utils.GenerateSpacing(convCentroid.Y, 15) + convCentroid.Z.ToString() + Utils.GenerateSpacing(convCentroid.Z, 15));
            }

            writer.WriteLine();
          } // FOREACH sphericity value.

          writer.Flush();
        }
      }
      catch (Exception e)
      {
        Log.Error("Unable to save file; Exception " + e.Message, "CadReportReader", "SaveTextFile");
        throw new ApplicationException("Unable to save file; Exception " + e.Message);
      }
    } // SaveTextFile(string filePath)

    #endregion

  } // CadReportReader
} // namespace Viatronix.Colon.Medicsight.CAD.Validation

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1.4.1  2009/06/29 13:21:30  kchalupa
// Checkin for CadParser.
//
// Revision 1.1  2009/05/19 13:40:31  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/MedicSight40/CadReportReader.cs,v 1.1.4.2 2010-05-17 19:51:54 kchalupa Exp $
// $Id: CadReportReader.cs,v 1.1.4.2 2010-05-17 19:51:54 kchalupa Exp $

#endregion
