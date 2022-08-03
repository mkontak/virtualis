// $Id: RedactorForm.cs,v 1.2.2.8 2011/05/18 01:57:01 mkontak Exp $ 
//
// Copyright © 2005, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Romy Sreedharan ( mailto:romy@viatronix.com )

using System;
using System.Drawing;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Forms;
using Viatronix.UI;
using System.Threading;


namespace Viatronix.Console.ImageRedactor
{
  /// <summary>
  /// FormRedactor shows all Images for redacting.
  /// </summary>
  public class RedactorForm : Viatronix.UI.Dialog
  {
    #region fields
    /// <summary>
    /// collection of Image files 
    /// </summary>
    private List<string> m_files;


    /// <summary>
    /// Table which holds the Redact Information
    /// </summary>
    private System.Collections.Hashtable m_redactTable = new Hashtable();

    /// <summary>
    /// Redactor Viewer Control object
    /// </summary>
    private RedactorViewer m_redactorViewer;

    /// <summary>
    /// Previewer Control object
    /// </summary>
    private ImagePreviewer m_previewer;

    /// <summary>
    /// Splitter Object
    /// </summary>
    //private System.Windows.Forms.Splitter m_splitter;

    /// <summary>
    /// Redact All button
    /// </summary>
    private Viatronix.UI.Button m_redactAll;

    /// <summary>
    /// Next Button Object
    /// </summary>
    private Viatronix.UI.Button m_next;

    /// <summary>
    /// Prev Button Object
    /// </summary>
    private Viatronix.UI.Button m_prev;

    /// <summary>
    /// Finish button Object
    /// </summary>
    private Viatronix.UI.Button m_finish;

    /// <summary>
    /// index of the previously selected Image
    /// </summary>
    private int m_lastSelectedIndex = 0;

    /// <summary>
    /// destinatipm folder for the redacted images
    /// </summary>
   // private string m_destFolder = string.Empty;

    private System.Windows.Forms.Panel m_previewPanel;


    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;

    #endregion fields

    #region Properties

    public int LastImageIndex
    {
      get { return m_lastSelectedIndex; }
      set { m_lastSelectedIndex = value; }
    }//LastIndex


    #endregion Properties

    #region Methods


    /// <summary>
    /// Show the Dialog with the input image files.
    /// </summary>
    public void RedactImages(List<string> sourcefiles, List<string> destFiles)
    {
      m_files = sourcefiles;

      //Create a dummy table for all files. To fix PACS no source found issue.
      for ( int index=0; index < m_files.Count; index++ )
        m_redactTable [ m_files [ index ] ] = new RedactInformation( destFiles[ index ].ToString() );

      SelectItem(0);
    }
        
    /// <summary>
    /// Show the Form with Image files in it
    /// </summary>
    private void SelectItem(int selectedItem)
    {

      m_previewer.Clear();

      m_previewer.IconSpaceX = 75;
      //m_previewer.IconSpaceY = 75;

      m_previewer.BeginUpdate();
      
      //Iterate through the Redact Informations
      for ( int index = 0; index < m_files.Count ; index ++ )
      {
        System.Drawing.Bitmap image  =  new Bitmap( m_files[index].ToString() );
        m_previewer.Add(image);
      }

      if( m_files.Count > 0 && selectedItem < m_files.Count )
      {
        m_previewer.SelectItem( selectedItem );
        m_redactorViewer.Image = new Bitmap( m_files[selectedItem].ToString() );
      }

      m_previewer.EndUpdate();

    }//Show (int selectedItem)


    /// <summary>
    /// Clone the same area to from the current image to all Images
    /// </summary>
    private void CloneRedactAreaToAll()
    {
      //Apply the same Redact Area to all files.
      foreach ( string file in m_files )
        m_redactorViewer.GetRedactAreaInformation( ( RedactInformation ) m_redactTable [ file ] );

    }//CloneRedactAreaToAll


    /// <summary>
    /// redacts the images
    /// </summary>
    private void Redact()
    {
      this.Cursor = Cursors.WaitCursor;

      try
      {
        ImageRedactor.Redact( m_redactTable );
      }
      catch ( Exception ex )
      {
        Viatronix.UI.MessageBox.ShowError ( "Failed redacting" + ex.Message );
      }
      finally
      {
        this.Cursor = Cursors.Default;
      }
 
    }//Redact()
    

    #endregion Methods

    #region construction
    /// <summary>
    /// Constructor
    /// </summary>
    public RedactorForm()
    {
      // Required for Windows Form Designer support
      InitializeComponent();

      m_previewer = new ImagePreviewer();
      Controls.Add( m_previewer );
      m_previewer.Location = new System.Drawing.Point( m_previewPanel.Location.X, m_previewPanel.Location.Y );
      m_previewer.Size = m_previewPanel.Size;
      m_previewer.Name = "m_previewer";
      m_previewer.BackColor = Theme.BackColor;//Color.White;
      this.BackColor = Theme.BackColor;

      Controls.SetChildIndex(this.m_previewer, 0);
      m_previewer.SelectedIndexChanged += new EventHandler(this.OnPreviewChangedHandler);

      ToolTip nextButtonTip = new ToolTip();
      nextButtonTip.SetToolTip( m_next, "Next Image" );

      ToolTip prevButtonTip = new ToolTip();
      prevButtonTip.SetToolTip( m_prev, "Previous Image" );

      ToolTip applyButtonTip = new ToolTip();
      applyButtonTip.SetToolTip( m_redactAll, "Apply the redact areas to all images" );

      ToolTip finishButtonTip = new ToolTip();
      finishButtonTip.SetToolTip( m_finish, "Finish redacting" );

      //ToolTip cancelButtonTip = new ToolTip();
      //cancelButtonTip.SetToolTip( m_cancel, "Cancel redacting" );

     }// RedactorForm()


    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose( bool disposing )
    {
      if( disposing )
      {
        if(components != null)
        {
          components.Dispose();
        }
      }
      base.Dispose( disposing );
    }//Dispose( bool disposing )


    #region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      //Viatronix.Console.ImageRedactor.RedactInformation redactInformation2 = new Viatronix.Console.ImageRedactor.RedactInformation();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager( typeof( RedactorForm ) );
      this.m_redactorViewer = new Viatronix.Console.ImageRedactor.RedactorViewer();
      this.m_redactAll = new Viatronix.UI.Button();
      this.m_next = new Viatronix.UI.Button();
      this.m_prev = new Viatronix.UI.Button();
      this.m_finish = new Viatronix.UI.Button();
      this.m_previewPanel = new System.Windows.Forms.Panel();
      this.SuspendLayout();
      // 
      // m_redactorViewer
      // 
      this.m_redactorViewer.BackColor = System.Drawing.SystemColors.GrayText;
      this.m_redactorViewer.Location = new System.Drawing.Point( 20, 10 );
      this.m_redactorViewer.Name = "m_redactorViewer";
      //redactInformation2.Height = 0F;
      //redactInformation2.MaxX = 0F;
      //redactInformation2.MaxY = 0F;
      //redactInformation2.MinX = 0F;
      //redactInformation2.MinY = 0F;
      //redactInformation2.RedactionRects = ( ( System.Collections.ArrayList ) ( resources.GetObject( "redactInformation2.RedactionRects" ) ) );
      //redactInformation2.Width = 0F;
      //this.m_redactorViewer.RedactingAreas = redactInformation2;
      this.m_redactorViewer.Size = new System.Drawing.Size( 520, 520 );
      this.m_redactorViewer.TabIndex = 5;
      // 
      // m_redactAll
      // 
      this.m_redactAll.ForeColor = System.Drawing.Color.Black;
      this.m_redactAll.Location = new System.Drawing.Point( 221, 707 );
      this.m_redactAll.Name = "m_redactAll";
      this.m_redactAll.OldStockButton = false;
      this.m_redactAll.Size = new System.Drawing.Size( 70, 27 );
      this.m_redactAll.TabIndex = 3;
      this.m_redactAll.Text = "Apply to All";
      this.m_redactAll.Click += new System.EventHandler( this.OnApplyAll );
      // 
      // m_next
      // 
      this.m_next.ForeColor = System.Drawing.Color.Black;
      this.m_next.Image = ( ( System.Drawing.Image ) ( resources.GetObject( "m_next.Image" ) ) );
      this.m_next.Location = new System.Drawing.Point( 294, 659 );
      this.m_next.Name = "m_next";
      this.m_next.OldStockButton = false;
      this.m_next.Size = new System.Drawing.Size( 23, 23 );
      this.m_next.TabIndex = 15;
      this.m_next.Text = ">>";
      this.m_next.Click += new System.EventHandler( this.OnNextButtonClick );
      // 
      // m_prev
      // 
      this.m_prev.ForeColor = System.Drawing.Color.Black;
      this.m_prev.Image = ( ( System.Drawing.Image ) ( resources.GetObject( "m_prev.Image" ) ) );
      this.m_prev.Location = new System.Drawing.Point( 266, 658 );
      this.m_prev.Name = "m_prev";
      this.m_prev.OldStockButton = false;
      this.m_prev.Size = new System.Drawing.Size( 24, 24 );
      this.m_prev.TabIndex = 16;
      this.m_prev.Text = "<<";
      this.m_prev.Click += new System.EventHandler( this.OnPrevButtonClick );
      // 
      // m_finish
      // 
      this.m_finish.ForeColor = System.Drawing.Color.Black;
      this.m_finish.Location = new System.Drawing.Point( 292, 707 );
      this.m_finish.Name = "m_finish";
      this.m_finish.OldStockButton = false;
      this.m_finish.Size = new System.Drawing.Size( 58, 27 );
      this.m_finish.TabIndex = 14;
      this.m_finish.Text = "Finish";
      this.m_finish.Click += new System.EventHandler( this.OnFinishButtonClick );
      // 
      // m_previewPanel
      // 
      this.m_previewPanel.BackColor = System.Drawing.Color.FromArgb( ( ( int ) ( ( ( byte ) ( 123 ) ) ) ), ( ( int ) ( ( ( byte ) ( 134 ) ) ) ), ( ( int ) ( ( ( byte ) ( 165 ) ) ) ) );
      this.m_previewPanel.Location = new System.Drawing.Point( 20, 542 );
      this.m_previewPanel.Name = "m_previewPanel";
      this.m_previewPanel.Size = new System.Drawing.Size( 520, 110 );
      this.m_previewPanel.TabIndex = 12;
      // 
      // RedactorForm
      // 
      this.AutoScaleBaseSize = new System.Drawing.Size( 5, 13 );
      this.AutoScroll = true;
      this.ClientSize = new System.Drawing.Size( 568, 756 );
      this.ControlBox = false;
      this.Controls.Add( this.m_previewPanel );
      this.Controls.Add( this.m_finish );
      this.Controls.Add( this.m_redactorViewer );
      this.Controls.Add( this.m_redactAll );
      this.Controls.Add( this.m_next );
      this.Controls.Add( this.m_prev );
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
      this.MaximizeBox = false;
      this.MinimizeBox = false;
      this.Name = "RedactorForm";
      this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
      this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
      this.Text = "Image Redactor";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler( this.OnClosingEventHandler );
      this.Controls.SetChildIndex( this.m_prev, 0 );
      this.Controls.SetChildIndex( this.m_next, 0 );
      this.Controls.SetChildIndex( this.m_redactAll, 0 );
      this.Controls.SetChildIndex( this.m_redactorViewer, 0 );
      this.Controls.SetChildIndex( this.m_finish, 0 );
      this.Controls.SetChildIndex( this.m_previewPanel, 0 );
      this.ResumeLayout( false );

    }
    #endregion

    #endregion construction

    #region EventHandlers

   
    /// <summary>
    /// handles the Preiew Change event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void OnPreviewChangedHandler( object sender, System.EventArgs args )
    {
      //On Image change save the old Image's redacting areas to the Table.
     ChangeImage();

    }//OnPreviewChangedHandler( object sender, System.EventArgs args )


    /// <summary>
    /// changes the Image in the preview and in the Image Previewer
    /// </summary>
    private void ChangeImage()
    {
      if ( m_previewer.SelectedIndex < 0 ) return;

      m_previewer.EnsureVisible( m_previewer.SelectedIndex );
      if ( LastImageIndex != m_previewer.SelectedIndex )
      {
        if( m_redactorViewer.RedactAreaRectangles.Count > 0 )
        {
          m_redactorViewer.GetRedactAreaInformation( ( RedactInformation ) m_redactTable [ m_files [ LastImageIndex ] ] );
          m_redactorViewer.ClearRedactAreas();
        }
        LastImageIndex = m_previewer.SelectedIndex ;
      }

      //If there are any selected redact areas on this image show them on Redactor Viewer
      if ( m_redactTable.ContainsKey( m_files [ m_previewer.SelectedIndex ] ) )
      {
        RedactInformation redactInfo  = ( RedactInformation ) ( m_redactTable [ m_files [ m_previewer.SelectedIndex ] ] );
        m_redactorViewer.RedactAreaRectangles = redactInfo.RedactionRects;
      }

      string redactFile = m_files [ m_previewer.SelectedIndex ].ToString();
      m_redactorViewer.Image = new Bitmap( redactFile );

    }//ChangeImage()


    /// <summary>
    /// Handles the prev button click
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnPrevButtonClick( object sender, System.EventArgs args )
    {
      //avoid the index range overflow.
      if( m_previewer.SelectedIndex - 1 < 0 )
        m_previewer.SelectItem( m_files.Count - 1 );
      else
        m_previewer.SelectItem( m_previewer.SelectedIndex - 1 );

      ChangeImage();

    }//OnPrevButtonClick( object sender, System.EventArgs args )


    /// <summary>
    /// Handles the Next Button click event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">EventArgs</param>
    private void OnNextButtonClick( object sender, System.EventArgs args )
    {
      //avoid the index range overflow.
      if( m_previewer.SelectedIndex + 1 >= m_files.Count )
        m_previewer.SelectItem(0);
      else
        m_previewer.SelectItem( m_previewer.SelectedIndex + 1 );

      ChangeImage();
      
    }//OnNextButtonClick(object sender, System.EventArgs args)


    /// <summary>
    /// Handles the Finish button click
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnFinishButtonClick( object sender, System.EventArgs args )
    {
     
      //For the current Item.
      if ( m_redactorViewer.RedactAreaRectangles.Count > 0 && m_previewer.SelectedIndex > -1 )
        m_redactorViewer.GetRedactAreaInformation( ( RedactInformation ) m_redactTable [ m_files [ m_previewer.SelectedIndex ] ] );

      //Warn the user that all files are not selcted for redacting.
      if ( m_redactTable.Count != m_files.Count )
      {
        string verb = m_redactTable.Count > 1 ? " are " : " is ";
        if( Viatronix.UI.MessageBox.ShowQuestion( "Out of " + m_files.Count.ToString() + " file(s), only " + 
          m_redactTable.Count.ToString() + verb +  "selected for redaction. Are you sure you want to continue ?" ) != true )
          return;
      }

      Redact();

      this.Dispose();

    }//private void OnFinishButtonClick( object sender, System.EventArgs args )

 
    /// <summary>
    /// Handles the Apply All event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">EventArgs</param>
    private void OnApplyAll( object sender, System.EventArgs args )
    {
//      if(m_redactorViewer.RedactingAreas.Count <= 0 )
//      {
//        Viatronix.UI.MessageBox.ShowError ( " Draw redact area(s) to apply" );
//        return;
//      }
      CloneRedactAreaToAll();

    }//OnApplyAll( object sender, System.EventArgs args )


    /// <summary>
    /// Handles the thumbnail item click
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="args">EventArgs</param>
    private void OnViewereThumbNailItemClick( object sender, System.EventArgs args )
    {
      ChangeImage();
    }//OnViewereThumbNailItemClick( object sender, System.EventArgs args )


    /// <summary>
    /// Handles the cancecl click event
    /// </summary>
    /// <param name="sender">object</param>
    /// <param name="e">EventArgs</param>
    private void OnCancelClick( object sender, System.EventArgs args )
    {
      if ( Viatronix.UI.MessageBox.ShowQuestion ( " Do you wish to cancel redacting ? " ) == true )
        this.Dispose();

    }//OnCancelClick(object sender, System.EventArgs e)

   
    /// <summary>
    /// Snapshot preview Operation
    /// </summary>
    private void SnapshotsPreview()
    {
      for ( int index = 0; index < m_files.Count ; index ++ )
      {
        m_previewer.SelectItem( index);
        m_redactorViewer.Refresh();
        m_previewer.Refresh();
        System.Threading.Thread.Sleep( 600 );
      }
    }

    private void OnClosingEventHandler(object sender, FormClosingEventArgs e)
    {
      e.Cancel = true;
    }// SnapshotsPreview


    #endregion EventHandlers
   
  }//public class RedactorForm : Viatronix.UI.Dialog
}//namespace ImageRedactor



// $Log: RedactorForm.cs,v $
// Revision 1.2.2.8  2011/05/18 01:57:01  mkontak
// Issue CON-1640: Windows 7 Compliance by removing the SystemSettings
// registry to a global config file. Move the default log directory to the windows
// common application data directory (XP - C:\Document and Settings\All Users\Application Data , Win7 - C:\ProgramData ).
// Merged the Viatronix.v3D.Logger project into the Viatronix.Utilities project and
// signed the Utilities assembly.
// Committed on the Free edition of March Hare Software CVSNT Server.
// Upgrade to CVS Suite for more features and support:
// http://march-hare.com/cvsnt/
//
// Revision 1.2.2.7  2007/04/23 21:56:40  romy
// destination file path is part of RedactInformation.  fixed issue#5603
//
// Revision 1.2.2.6  2007/03/27 13:28:01  romy
// Issue# 5517
//
// Revision 1.2.2.5  2007/03/27 00:52:14  romy
// Issue# 5521
//
// Revision 1.2.2.4  2007/03/26 21:16:03  romy
// fixed issue #5519
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.Console.Core/ImageRedactor/RedactorForm.cs,v 1.2.2.8 2011/05/18 01:57:01 mkontak Exp $
// $Id: RedactorForm.cs,v 1.2.2.8 2011/05/18 01:57:01 mkontak Exp $