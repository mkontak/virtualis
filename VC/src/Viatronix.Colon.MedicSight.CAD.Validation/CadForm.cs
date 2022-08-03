// $Id: CadForm.cs,v 1.1.4.1 2009-06-29 13:22:24 kchalupa Exp $
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
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using Viatronix.Logging;

namespace Viatronix.Colon.Medicsight.CAD.Validation
{
  /// <summary>
  /// The form for user input, specifiying the file to read and so forth.
  /// </summary>
  public partial class CadForm : Form
  {

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public CadForm()
    {
      InitializeComponent();
    } // CadForm()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles the navigator button clicked event.
    /// </summary>
    /// <param name="sender">Origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnNavigatorClickedHandler(object sender, EventArgs e)
    {
      OpenFileDialog dialog = new OpenFileDialog();
      dialog.Filter = "Xml Files (*.xml)|*.xml";
      DialogResult result = dialog.ShowDialog();

      if(result == DialogResult.OK)
      {
        m_cadReportFileTextBox.Text = dialog.FileName;
      }
    } // OnNavigatorClickedHandler(object sender, EventArgs e)


    /// <summary>
    /// Handles the DICOM path navigate button's clicked event.
    /// </summary>
    /// <param name="sender">Origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnDicomNavigatorClickedHandler(object sender, EventArgs e)
    {
      FolderBrowserDialog dialog = new FolderBrowserDialog();

      if (m_dicomPathTextBox.Text != string.Empty && Directory.Exists(m_dicomPathTextBox.Text))
      {
        dialog.SelectedPath = m_dicomPathTextBox.Text;
      }

      DialogResult result = dialog.ShowDialog();
      if (result == DialogResult.OK)
      {
        m_dicomPathTextBox.Text = dialog.SelectedPath;
      }
    } // OnDicomNavigatorClickedHandler(object sender, EventArgs e)


    /// <summary>
    /// Handles the CadTruth Navigator clicked event.
    /// </summary>
    /// <param name="sender">Origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnCadTruthNavigatorClickedHandler(object sender, EventArgs e)
    {
      OpenFileDialog dialog = new OpenFileDialog();
      dialog.Filter = "CAD Truth Files (*.pef.dcm.txt)|*.pef.dcm.txt";
      DialogResult result = dialog.ShowDialog();

      if (result == DialogResult.OK)
      {
        m_cadTruthTextBox.Text = dialog.FileName;
      }
    } // OnCadTruthNavigatorClickedHandler(object sender, EventArgs e)


    /// <summary>
    /// Handles the output navigator button's clicked event.
    /// </summary>
    /// <param name="sender">origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnOutputNavigatorClickedHandler(object sender, EventArgs e)
    {
      FolderBrowserDialog dialog = new FolderBrowserDialog();

      if (m_outputTextBox.Text != string.Empty && Directory.Exists(m_outputTextBox.Text))
      {
        dialog.SelectedPath = m_outputTextBox.Text;
      }

      DialogResult result = dialog.ShowDialog();
      if (result == DialogResult.OK)
      {
        m_outputTextBox.Text = dialog.SelectedPath;
      }
    } // OnOutputNavigatorClickedHandler(object sender, EventArgs e)


    /// <summary>
    /// Handles the Validation checked changed event.
    /// </summary>
    /// <param name="sender">Origin of the event.</param>
    /// <param name="e">Arguments to take into account.</param>
    private void OnValidationCheckChangedHandler(object sender, EventArgs e)
    {
      m_outputLabel.Enabled = m_validateUtilityCheckBox.Checked;
      m_outputTextBox.Enabled = m_validateUtilityCheckBox.Checked;
      m_outputNavigatorButton.Enabled = m_validateUtilityCheckBox.Checked;
    } // OnValidationCheckChangedHandler(object sender, EventArgs e)


    /// <summary>
    /// Handles the convert button clicked event.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnVerifyClickedHandler(object sender, EventArgs args)
    {
      try
      {
        Point3DConverter pointConverter;

        CadReportReader reader;
        CadVerifier verifier;

        //==================================================================================================
        // Create the point converter to convert from Volume Coords to DICOM world Coords.
        //==================================================================================================

        try
        {
          pointConverter = new Point3DConverter(m_dicomPathTextBox.Text);
        }
        catch (Exception e)
        {
          Log.Error("Unable to read DICOM information to create pointConverter; Exception: " + e.Message, "CadForm", "OnVerifyClickedHandler");
          throw new ApplicationException("Unable to read DICOM information to create pointConverter; Exception: " + e.Message);
        }

        //==================================================================================================
        // Read the CAD findings report and convert the points.
        //==================================================================================================

        try
        {
          reader = new CadReportReader();
          reader.ReadXmlFile(m_cadReportFileTextBox.Text, pointConverter);

          if (m_validateUtilityCheckBox.Checked)
          {
            string cadReportDumpFilePath = System.IO.Path.Combine(m_outputTextBox.Text, "cadReportDump.txt");

            reader.SaveTextFile(cadReportDumpFilePath, m_cadReportFileTextBox.Text);
          }
        }
        catch (System.Xml.XmlException e)
        {
          Log.Error("Unable to read cad xml file; Exception: " + e.Message, "CadForm", "OnVerifyClickedHandler");
          throw new System.Xml.XmlException("Unable to read cad xml file; Exception: " + e.Message);
        }

        //==================================================================================================
        // Verify the read data against a CAD truth table.
        //==================================================================================================

        try
        {
          verifier = new CadVerifier();
          verifier.ReadTruthFile(m_cadTruthTextBox.Text);

          if (m_validateUtilityCheckBox.Checked)
          {
            string cadTruthDumpFilePath = System.IO.Path.Combine(m_outputTextBox.Text, "truthFileDump.txt");

            verifier.SaveTruthFile(cadTruthDumpFilePath, m_cadTruthTextBox.Text);
          }

          verifier.VerifyFindingsAgainstTruthFile(reader.SphericityValuesToCadFindings, verifier.ComputeThreshold(pointConverter));

          if (m_validateUtilityCheckBox.Checked)
          {
            string validationResultsFilePath = System.IO.Path.Combine(m_outputTextBox.Text, "validationResults.txt");

            verifier.SaveValidationResults(validationResultsFilePath, m_cadTruthTextBox.Text, m_cadReportFileTextBox.Text);
          }
        }
        catch (TruthPointsInvalidRegionCountException ex)
        {
          throw ex;
        }
        catch (Exception e)
        {
          Log.Error("Unable to perform diff; Exception: " + e.Message, "CadForm", "OnVerifyClickedHandler");
          throw new ApplicationException("Unable to perform diff; Exception: " + e.Message);
        }
      }
      catch (System.Xml.XmlException)
      {
        Log.Error("Failed to parse the cad findings file.", "CadForm", "OnVerifyClickedHandler");
        System.Windows.Forms.MessageBox.Show(this, "Failed to read the cad findings file.", "Failed Xml Parsing", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
      catch (InvalidUnitsException)
      {
        Log.Error("Cad Finding had invalid units.  Must be voxels.", "CadForm", "OnVerifyClickedHandler");
        System.Windows.Forms.MessageBox.Show(this, "This utility is not compatible with the units specified in the Cad Findings.  Units must be in V3D World Coordinates.", "Failed Xml Parsing", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
      catch (TruthPointsInvalidRegionCountException ex)
      {
        Log.Error(string.Format("Truth file is invalid.  Listed region count does not match the actual number of regions found.  [ERROR={0}]", ex.Message), "CadForm", "OnVerifyClickedHandler");
        System.Windows.Forms.MessageBox.Show(this, "Truth file is invalid.  Listed region count does not match the actual number of regions found.", "Invalid Region Counts", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
      catch (Exception e)
      {
        Log.Error("Unable to perform conversion; Exception: " + e.Message, "CadForm", "OnVerifyClickedHandler");
        System.Windows.Forms.MessageBox.Show(this, "Unable to perform conversion; Exception: " + e.Message, "Failed Conversion", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }
    } // OnVerifyClickedHandler(object sender, EventArgs args)

    #endregion

  } // CadForm
} // namespace Viatronix.Colon.Medicsight.CAD.Validation

#region revision history

// $Log: not supported by cvs2svn $
// Revision 1.1  2009/05/19 13:39:34  kchalupa
// Initial Revision
//
// $Header: D:\CVS\cvsRepository/V3D/src/Viatronix.v3D.Tools/MedicSightAET/CadForm.cs,v 1.1.4.1 2009-06-29 13:22:24 kchalupa Exp $
// $Id: CadForm.cs,v 1.1.4.1 2009-06-29 13:22:24 kchalupa Exp $

#endregion
