// $Id: CadVerifier.cs,v 1.1.4.1 2009-06-29 13:21:30 kchalupa Exp $
//
// Copyright © 2009, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Kyle R Chalupa] [kchalupa@gmail.com]

using System;
using System.IO;
using System.Configuration;
using System.Collections.Generic;
using System.Text;

using Viatronix.Logging;

namespace Viatronix.Colon.Medicsight.CAD.Validation
{

  #region exceptions

  /// <summary>
  /// An exception for if a parsing for a point3df from a string has failed.
  /// </summary>
  public class Point3DfParseException : Exception
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public Point3DfParseException() : base()
    {
    } // Point3DfParseException()


    public Point3DfParseException(string message) : base(message)
    {
    } // Point3DfParseException( message )

    #endregion

  } // class Point3DfParseException


  /// <summary>
  /// An exception for if the truth sphericity's "x regions detected" doesn't match the actual points for that spheriticty value.
  /// </summary>
  public class TruthPointsInvalidRegionCountException : Exception
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public TruthPointsInvalidRegionCountException() : base()
    {
    } // TruthPointsInvalidRegionCountException()


    public TruthPointsInvalidRegionCountException(string message) : base(message)
    {
    } // TruthPointsInvalidRegionCountException( message )

    #endregion

  } // class TruthPointsInvalidRegionCountException

  #endregion

  /// <summary>
  /// Compares a sphericity/findings relationship to a text file that contains similar information.
  /// </summary>
  public class CadVerifier
  {

    #region fields

    /// <summary>
    /// Maps a sphericity value to a list of DICOM points.
    /// </summary>
    private Dictionary<int, List<Point3Df>> m_sphericityValueToDICOMPointMap = new Dictionary<int, List<Point3Df>>();

    /// <summary>
    /// Used for storing the validation data.
    /// </summary>
    StringBuilder m_validationBuilder = new StringBuilder();

    #endregion

    #region methods

    /// <summary>
    /// Reads a Point from a string, where the axes are seperated by \t characters.
    /// </summary>
    /// <param name="pointString">The string to read the Point3Df from.</param>
    /// <returns>A Point3Df from the string.</returns>
    private static Point3Df ReadPointFromString(string pointString)
    {
      try
      {
        //================================================================================================
        // Remove multiple tabs to prevent an empty element being added when the split is called.
        // \t\t\t\t will be reduced to \t\t\t then \t\t then \t as the loop runs.
        //================================================================================================

        int doubleTabIndex = -1;
        while ((doubleTabIndex = pointString.IndexOf("\t\t")) > -1)
        {
          pointString = pointString.Remove(doubleTabIndex, 1);
        }

        //================================================================================================
        // Split out the values and create a point.
        //================================================================================================

        string[] components = pointString.Split('\t');

        if (components.Length > 2)
        {
          Point3Df point = new Point3Df();

          point.X = double.Parse(components[0]);
          point.Y = double.Parse(components[1]);
          point.Z = double.Parse(components[2]);

          return point;
        }
        else
        {
          throw new Point3DfParseException("String did not have three components to it, cannot represent a 3D point");
        }
      }
      catch (Exception)
      {
        throw;
      }
    } // ReadPointFromString(string pointString)


    /// <summary>
    /// Reads a CAD Truth File composed of the following.
    /// 
    /// ...
    /// Sphericity=A
    /// B Regions Found
    /// 
    /// ...
    /// X\t\tY\t\tZ
    /// ... 
    ///
    /// ...
    /// 
    /// Where A is a sphericity value, B is the number of regions that have a sphericity value of A.
    /// X, Y and Z are the components of a Point3Df object.
    /// </summary>
    /// <param name="fileName">The truth file's path.</param>
    public void ReadTruthFile(string filePath)
    {
      try
      {
        // The list of sphericity values to the regions that should be there.
        List<KeyValuePair<int, int>> sphericityToFindingsCountMap = new List<KeyValuePair<int, int>>();

        using (TextReader reader = new StreamReader(filePath))
        {
          int sphericityValue = -1;
          List<Point3Df> points = new List<Point3Df>();

          string line = string.Empty;
          while ((line = reader.ReadLine()) != null)
          {
            line = line.ToLower();

            string[] components = line.Split('=');

            if ((line.IndexOf("sphericity") > -1 || line.IndexOf("shericity") > -1) && components.Length > 1)
            {
              int newSphericityValue = int.Parse(components[1].Trim());

              if (sphericityValue != -1)
              {
                List<Point3Df> sphericityPoints = null;
                if (m_sphericityValueToDICOMPointMap.TryGetValue(sphericityValue, out sphericityPoints))
                {
                  sphericityPoints.AddRange(points);
                }
                else
                {
                  sphericityPoints = new List<Point3Df>();
                  sphericityPoints.AddRange(points);
                  m_sphericityValueToDICOMPointMap.Add(sphericityValue, sphericityPoints);
                }
              }

              points.Clear();
              sphericityValue = newSphericityValue;
            }
            else if (line.ToLower().IndexOf(" regions detected") > -1)
            {
              int regionCount = Convert.ToInt32(line.Substring(0, line.ToLower().IndexOf(" regions detected")));
              sphericityToFindingsCountMap.Add(new KeyValuePair<int, int>(sphericityValue, regionCount));
            }
            else
            {
              try
              {
                Point3Df point = ReadPointFromString(line);
                points.Add(point);
              }
              catch (Point3DfParseException e)
              {
                Log.Debug("String " + line + " did not comprise a Point3Df object.", "CadVerifier", "ReadTruthFile");
              }
            }
          } // END WHILE ... read lines.

          // Write out the last sphericity value.
          if (sphericityValue != -1)
          {
            List<Point3Df> sphericityPoints = null;
            if (!m_sphericityValueToDICOMPointMap.TryGetValue(sphericityValue, out sphericityPoints))
            {
              sphericityPoints = new List<Point3Df>();
              sphericityPoints.AddRange(points);
              m_sphericityValueToDICOMPointMap.Add(sphericityValue, sphericityPoints);
            }
            else
            {
              sphericityPoints.AddRange(points);
            }
          }

          if (m_sphericityValueToDICOMPointMap.Count == 0)
          {
            throw new ApplicationException("Unable to parse Cad Truth File.");
          }

          foreach (KeyValuePair<int, int> sphericityRegionsCount in sphericityToFindingsCountMap)
          {
            List<Point3Df> detectedPoints = null;
            if (!m_sphericityValueToDICOMPointMap.TryGetValue(sphericityRegionsCount.Key, out detectedPoints))
            {
              throw new ApplicationException(string.Format("Could not find referenced sphericity value in the map.  [VALUE=\'{0}\']", sphericityRegionsCount.Key));
            }

            if(sphericityRegionsCount.Value != detectedPoints.Count)
            {
              throw new TruthPointsInvalidRegionCountException(string.Format("Invalid truth file.  Sphericity {0} lists {1} regions; found {2}.", sphericityRegionsCount.Key, sphericityRegionsCount.Value, detectedPoints.Count));
            }
          }

        } // END USING ...
      }
      catch (TruthPointsInvalidRegionCountException e)
      {
        throw e;
      }
      catch (Exception e)
      {
        Log.Error("Unable to read CAD Truth File; Exception: " + e.Message, "CadVerifier", "ReadTruthFile");
        throw new ApplicationException("Unable to read CAD Truth File; Exception: " + e.Message);
      }
    } // ReadTruthFile(string filePath)


    /// <summary>
    /// Intended purely to validate the ReadTruthFile method.
    /// </summary>
    /// <param name="filePath">The filePath to write out to.</param>
    /// <param name="inputFilePath">The input file that generated this output.</param>
    public void SaveTruthFile(string filePath, string inputFilePath)
    {
      try
      {
        using (TextWriter writer = new StreamWriter(filePath))
        {
          writer.WriteLine();
          writer.WriteLine("------------------------------------------------------------------------------");
          writer.WriteLine(" Input File: " + inputFilePath);
          writer.WriteLine("------------------------------------------------------------------------------");
          writer.WriteLine();

          foreach (KeyValuePair<int, List<Point3Df>> keyPair in m_sphericityValueToDICOMPointMap)
          {
            writer.WriteLine("Sphericity=" + keyPair.Key);
            writer.WriteLine(keyPair.Value.Count + " regions found.");
            writer.WriteLine();

            foreach (Point3Df point in keyPair.Value)
            {
              writer.WriteLine(Math.Round(point.X, 6) + Utils.GenerateSpacing(Math.Round(point.X, 6), 15) + Math.Round(point.Y, 6) + Utils.GenerateSpacing(Math.Round(point.Y, 6), 15) + Math.Round(point.Z, 6) + Utils.GenerateSpacing(Math.Round(point.Z, 6), 15));
            }
            writer.WriteLine();
          }
          writer.Flush();
        }
      }
      catch (Exception e)
      {
        Log.Error("Unable to write out truth file read results; Exception: " + e.Message, "CadVerifier", "SaveTruthFile");
        throw new ApplicationException("Unable to write out truth file read results; Exception: " + e.Message);
      }
    } // SaveTruthFile(string filePath)


    /// <summary>
    /// Saves the validation results of the utility's functionality.
    /// 
    /// Threshold: a
    /// 
    /// Sphericity: d Count: e
    /// 
    /// Accepted: (x, y, z)\t Report: (u, v, w)\t Z Diff: b\t point validated
    /// Accepted: (x, y, z)\t Report: (u, v, w)\t Z Diff: b\t validation on point failed!
    /// 
    /// ...
    /// </summary>
    /// <remarks>Although not required, it is recommended to call the VerifyFindingsAgainstTruthFile method before this one.</remarks>
    /// <param name="outputFilePath">Where should the file be dumped out to?</param>
    /// <param name="truthPath">Where is the truth file's path?</param>
    /// <param name="reportPath">Where is the Cad report file?</param>
    public void SaveValidationResults(string outputFilePath, string truthPath, string reportPath)
    {
      using (TextWriter writer = new StreamWriter(outputFilePath))
      {
        writer.WriteLine("------------------------------------------------------------------------------");
        writer.WriteLine("Truth File: " + truthPath);
        writer.WriteLine("Report Path: " + reportPath);
        writer.Write(m_validationBuilder.ToString());
        writer.Flush();
      }
    } // SaveValidationInformation(string filePath)


    /// <summary>
    /// Checks to ensure that all sphericity values and coordinates 
    /// </summary>
    /// <param name="sphericityValueToFindingsMap">The dictionary established when reading the findings file.</param>
    /// <param name="threshold">The allowable </param>
    public void VerifyFindingsAgainstTruthFile(Dictionary<int, List<CadFinding>> sphericityValueToFindingsMap, double threshold)
    {
      StringBuilder builder = new StringBuilder();

      IComparer<Point3Df> pointComparer = new Point3DfComparer();
      Comparison<CadFinding> findingComparer = new Comparison<CadFinding>(CadFinding.CompareConvertedCentroidPoints);

      using (TextWriter writer = new StringWriter(m_validationBuilder))
      {
        writer.WriteLine("Threshold: " + threshold);
        writer.WriteLine("------------------------------------------------------------------------------");
        writer.WriteLine();

        try
        {
          foreach (KeyValuePair<int, List<Point3Df>> keyPair in m_sphericityValueToDICOMPointMap)
          {
            //==================================================================================================
            // Make sure CAD finding report has the sphericity value and sort both lists.
            //==================================================================================================

            writer.WriteLine();
            writer.WriteLine("Examining Sphericity: " + keyPair.Key + " Count: " + keyPair.Value.Count);
            writer.WriteLine();

            List<CadFinding> findings = null;
            if (!sphericityValueToFindingsMap.TryGetValue(keyPair.Key, out findings))
            {
              Log.Error("Validation failed; Truth file lists sphericity value of: " + keyPair.Key + " was not found in CAD findings report.", "CadVerifier", "VerifyFindingsAgainstTruthFile");
              builder.Append("Validation failed; Truth file lists sphericity value of: " + keyPair.Key + " was not found in CAD findings report.");
              writer.WriteLine("Validation failed; Truth file lists sphericity value of: " + keyPair.Key + " was not found in CAD findings report.");
              throw new ApplicationException("Validation failed; Truth file lists sphericity value of: " + keyPair.Key + " was not found in CAD findings report.");
            }

            if (keyPair.Value.Count != findings.Count)
            {
              Log.Error("Validation failed; Sphericity " + keyPair.Key + " truth file lists " + keyPair.Value.Count + " findings, CAD report has " + findings.Count + ".", "CadVerifier", "VerifyFindingsAgainstTruthFile");
              builder.AppendLine("Validation failed; Sphericity " + keyPair.Key + " truth file lists " + keyPair.Value.Count + " findings, CAD report has " + findings.Count + ".");
              writer.WriteLine("Validation failed; Sphericity " + keyPair.Key + " truth file lists " + keyPair.Value.Count + " findings, CAD report has " + findings.Count + ".");
              throw new ApplicationException("Validation failed; Sphericity " + keyPair.Key + " truth file lists " + keyPair.Value.Count + " findings, CAD report has " + findings.Count + ".");
            }

            //==================================================================================================
            // Go through and make sure all contents match.
            //==================================================================================================

            //
            // Don't compare point for point for one index, iterate over the truth file and find the nearest point with the Euclidean distance function and make sure it's in the threshold. 
            //
            List<Point3Df> findingPoints = new List<Point3Df>();
            foreach (CadFinding finding in findings)
            {
              findingPoints.Add(finding.ConvertedCentroid);
            }

            // Find the best matches.
            List<KeyValuePair<Point3Df, Point3Df>> matches = TruthFindingMatcher.FindMatches(keyPair.Value, findingPoints);

            for (int i = 0; i < matches.Count; ++i)
            {
              writer.Write("Accepted: " + matches[i].Key.ToString(5) + "Report: " + matches[i].Value.ToString(5) + "Diff: " + matches[i].Key.CalculateDifference(matches[i].Value));

              if (!matches[i].Key.IsApproxSameLocation(matches[i].Value, threshold))
              {
                Log.Error("Validation failed; Sphericity " + keyPair.Key + " had mismatching points at position " + i.ToString() + " Truth Point: " + keyPair.Value[i].ToString(2) + 
                  " Report: " + findings[i].ConvertedCentroid.ToString(2), "CadVerifier", "VerifyCadFindingsAgainstTruthFile");
                builder.AppendLine("Validation failed; Sphericity " + keyPair.Key + " had mismatching points at position " + i.ToString() + " Truth Point: " + keyPair.Value[i].ToString(2) +
                  " Report: " + findings[i].ConvertedCentroid.ToString(2));
                writer.WriteLine("\tvalidation on point failed!");
                throw new ApplicationException("Validation failed; Sphericity " + keyPair.Key + " had mismatching points at position " + i.ToString() + " Truth Point: " + keyPair.Value[i].ToString(2) + 
                  " Report: " + findings[i].ConvertedCentroid.ToString(2));
              }
              else
              {
                writer.WriteLine("\tpoint validated!");
              }
            }
          }

          // Show a message to let the user know there were no errors.
          System.Windows.Forms.MessageBox.Show("No errors were detected in validating CAD report.", "No Errors Found", System.Windows.Forms.MessageBoxButtons.OK,
            System.Windows.Forms.MessageBoxIcon.Information);

          writer.WriteLine();
          writer.WriteLine("No errors were detected in validating CAD report.");
        }
        catch (Exception)
        {
          builder.AppendLine();
          builder.AppendLine("An error occured in validating CAD report.  Please see Log.");

          ErrorForm form = new ErrorForm();
          form.ErrorText = builder.ToString();
          form.ShowDialog();

          writer.WriteLine();
          writer.WriteLine("An error occured in validating CAD report.  Please see Log.");
        }
      }
    } // VerifyFindingsAgainstTruthFile(Dictionary<int, List<CadFinding>> sphericityValueToFindingsMap)


    /// <summary>
    /// Compute the threshold as the furthest neighboring voxel.
    /// </summary>
    /// <param name="converter">The converter to retrieve the PixelSpacing (units) from.</param>
    /// <returns>The maximum threshold.</returns>
    public double ComputeThreshold(Point3DConverter converter)
    {
      double threshold = (float)Math.Sqrt((converter.Units.X * converter.Units.X) + (converter.Units.Y * converter.Units.Y) + (converter.Units.Z * converter.Units.Z)) * 1.05F;
      double configThresh = Convert.ToDouble(ConfigurationManager.AppSettings["threshold"].ToString());
      return configThresh > threshold ? configThresh : threshold;
    } // ComputeThreshold( converter )

    #endregion

  } // CadComparer
} // namespace Viatronix.Colon.Medicsight.CAD.Validation

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1  2009/05/19 13:40:31  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/MedicSight40/CadVerifier.cs,v 1.1.4.1 2009-06-29 13:21:30 kchalupa Exp $
// $Id: CadVerifier.cs,v 1.1.4.1 2009-06-29 13:21:30 kchalupa Exp $

#endregion
