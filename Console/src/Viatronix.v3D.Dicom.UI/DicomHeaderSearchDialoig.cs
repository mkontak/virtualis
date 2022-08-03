// $Id: DicomHeaderSearchDialoig.cs,v 1.2 2007/03/08 21:56:13 mkontak Exp $
//
// Copyright © 2007, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: [Mark Kontak] [mkontak@viatronix.com]


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Viatronix.Dicom;

namespace Viatronix.v3D.Dicom.UI
{

  /// <summary>
  /// Implements a dicom header search dialog used to serach for speciied dicom elements
  /// </summary>
  public partial class DicomHeaderSearchDialog : Viatronix.UI.Dialog
  {

    #region fields

    /// <summary>
    /// Element list
    /// </summary>
    private Viatronix.Dicom.Image m_image;

    #endregion


    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="image">Elementlist used for initialization</param>
    public DicomHeaderSearchDialog(Viatronix.Dicom.Image image)
    {
      InitializeComponent();

      m_image = image;

      LoadListView(string.Empty);
    } // DicomHeaderSearchDialog( image )

    #endregion


    #region methods


    /// <summary>
    /// Loads the list view using the search string
    /// </summary>
    /// <param name="search">Search string</param>
    private void LoadListView(string search)
    {
      //m_listViewSearchResults.Items.Clear();

      //ElementList elementList = m_dataset.GetElements();

      //foreach (Element element in elementList.Elements.Values)
      //{
      //  if (search.Length > 0 ? element.Name.IndexOf(search) != -1 : true)
      //  {
      //    ListViewItem item = m_listViewSearchResults.Items.Add(string.Format("{0:X4},{1:X4}",element.GroupId, element.ElementId));
      //    item.SubItems.Add(element.Name);
      //    item.SubItems.Add(element.VR.ToString());
      //    item.SubItems.Add(element.Value);
      //  }
      //}

    } // LoadListView(string search)

    #endregion

    #region event handlers

    /// <summary>
    /// Handler for the search text changed event
    /// </summary>
    /// <param name="sender">Sender object</param>
    /// <param name="e">Event args</param>
    private void OnSearchTextChangedHandler(object sender, EventArgs e)
    {
      if (m_textBoxSearch.Text.Length > 0)
      {
        m_buttonSearchCancel.Text = "Search";
      }
      else
      {
        m_buttonSearchCancel.Text = "Cancel";
      }

    } // OnSearchTextChangedHandler(object sender, EventArgs e)


    /// <summary>
    /// Handler for the click event
    /// </summary>
    /// <param name="sender">Sender object</param>
    /// <param name="e">Event args</param>
    private void OnClickHandler(object sender, EventArgs e)
    {
      if (string.Compare(m_buttonSearchCancel.Text, "Cancel", true) == 0)
      {
        this.Close();
      }
      else
      {
        LoadListView(m_textBoxSearch.Text);
      }
    } // OnClickHandler(object sender, EventArgs e)

    #endregion

  } // class DicomHEaderSearchDialog

} // namespace Viatronix.v3D.Dicom.UI

#region revision history

// $Log: DicomHeaderSearchDialoig.cs,v $
// Revision 1.2  2007/03/08 21:56:13  mkontak
// Coding standards
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.v3D.Dicom.UI/DicomHeaderSearchDialoig.cs,v 1.2 2007/03/08 21:56:13 mkontak Exp $
// $Id: DicomHeaderSearchDialoig.cs,v 1.2 2007/03/08 21:56:13 mkontak Exp $

#endregion
