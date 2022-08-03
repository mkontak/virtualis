using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Data;
using System.Windows.Forms;
using System.Diagnostics;
using vxDicom = Viatronix.v3D.Dicom;
using vxUI = Viatronix.v3D.UI;
using vxIO = Viatronix.v3D.Utilities.IO;

namespace Viatronix.v3D.Dicom.UI
{
	/// <summary>
	/// Summary description for DicomImageControl.
	/// </summary>
  public class DicomImageControl : System.Windows.Forms.UserControl
  {

    private Point m_prev = new Point(0,0);

    private bool m_stopping = false;
    private bool m_playing = false;
    private System.Threading.Thread m_playThread = null;

    private int m_frameIncrement = 0;
    private int m_frameEnd = 0; 

    /// <summary>
    /// Current frame number
    /// </summary>
    int m_frameNumber = 1;

    /// <summary>
    /// Number of frames in the image
    /// </summary>
    int m_numberOfFrames = 0;

    /// <summary>
    /// Current Picture Box Image
    /// </summary>
    private Bitmap m_image = null;

    /// <summary>
    /// Current Dicom Image
    /// </summary>
    private DicomImage m_dicomImage = null;

    /// <summary>
    /// Current Dicom Image Bitmap
    /// </summary>
    private Bitmap m_bitmap = null;

    /// <summary>
    /// Image Width
    /// </summary>
    private int m_imageWidth = 0;


    /// <summary>
    /// Image Height
    /// </summary>
    private int m_imageHeight = 0;

    private bool m_enablePopupMenu = true;
    private bool m_enableSaveImageAs = true;
    private bool m_enableViewDicomInformation = true;

    private int m_level = 0;
    private int m_window = 0;
    private int m_increment = 1;

    private double m_widthScale = 0;
    private double m_heightScale = 0;

    private bool m_showCoordinates = true;
    private string m_coordinates = string.Empty;

    private bool m_showPatienName = true;
    private string m_patientName = string.Empty;

    private bool m_showDescription = true;
    private string m_description = string.Empty;

    private bool m_showMinMax = true;
    private string m_minmax = string.Empty;

    private string m_imagePosition = string.Empty;

    private bool m_showPixelValue = true;
    private string m_pixelValue = string.Empty;

    private bool m_showWindowLevel = false;
    private string m_windowLevel = string.Empty;

    private Rectangle m_imageRect;

    /// <summary>
    /// Picture box
    /// </summary>
    private System.Windows.Forms.PictureBox m_pictureBox;
    private vxUI.PopupMenu m_menuImage;
    private vxUI.PopupItem m_menuItemSaveImageAs;
    private vxUI.PopupItem m_menuItemSaveImageAsBmp;
    private vxUI.PopupItem m_menuItemSaveImageAsPng;
    private vxUI.PopupItem m_menuItemSaveImageAsJpeg;
    private vxUI.PopupItem m_menuItemViewDicomInformation;
    private vxUI.PopupItem m_menuItemSeperator;
    private vxUI.PopupItem m_menuItemPlayForward;
    private vxUI.PopupItem m_menuItemPlayBackward;
    private vxUI.PopupItem m_menuItemPlayStop;
    private vxUI.PopupItem m_menuItemSaveDicomImageFiles;
    private vxUI.PopupItem m_menuItemViewDicomImageFiles;


    /// <summary> 
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.Container components = null;

    #region initialization

    public DicomImageControl()
    {
      // This call is required by the Windows.Forms Form Designer.
      InitializeComponent();

      m_menuImage = new vxUI.PopupMenu();
      m_menuItemSaveImageAs = new vxUI.PopupItem();
      m_menuItemSaveImageAs.Text = "Save Image As...";
      m_menuItemSaveImageAs.Enabled = m_enableSaveImageAs;

      m_menuItemSaveImageAsBmp = new vxUI.PopupItem();
      m_menuItemSaveImageAsBmp.Text = "Save As Bitmap";
      m_menuItemSaveImageAsBmp.Click += new System.EventHandler(this.OnSaveImageAsBmpClick);

      m_menuItemSaveImageAsPng = new vxUI.PopupItem();
      m_menuItemSaveImageAsPng.Text = "Save As Png";
      m_menuItemSaveImageAsPng.Click += new System.EventHandler(this.OnSaveImageAsPngClick);

      m_menuItemSaveImageAsJpeg = new vxUI.PopupItem();
      m_menuItemSaveImageAsJpeg.Text = "Save As Jpeg";
      m_menuItemSaveImageAsJpeg.Click += new System.EventHandler(this.OnSaveImageAsJpegClick);

      m_menuItemSaveImageAs.MenuItems.AddRange( new vxUI.PopupItem [] { 
                                                                        m_menuItemSaveImageAsBmp,
                                                                        m_menuItemSaveImageAsPng,
                                                                        m_menuItemSaveImageAsJpeg } );

      m_menuItemViewDicomInformation = new vxUI.PopupItem();
      m_menuItemViewDicomInformation.Text = "View Dicom Information";
      m_menuItemViewDicomInformation.Enabled = m_enableViewDicomInformation;
      m_menuItemViewDicomInformation.Click += new System.EventHandler(this.OnViewDicomInformation);

      m_menuItemViewDicomImageFiles = new vxUI.PopupItem();
      m_menuItemViewDicomImageFiles.Text = "View Embeded File List";
      m_menuItemViewDicomImageFiles.Enabled = m_enableViewDicomInformation;
      m_menuItemViewDicomImageFiles.Click += new System.EventHandler(this.OnViewDicomImageFiles);

      m_menuItemSeperator = new vxUI.PopupItem();
      m_menuItemSeperator.Text = "-";

      m_menuItemPlayForward = new vxUI.PopupItem();
      m_menuItemPlayForward.Text = "Play Forward";
      m_menuItemPlayForward.Click += new System.EventHandler(this.OnPlayForwardClick);

      m_menuItemPlayBackward = new vxUI.PopupItem();
      m_menuItemPlayBackward.Text = "Play Backward";
      m_menuItemPlayBackward.Click += new System.EventHandler(this.OnPlayBackwardClick);

      m_menuItemPlayStop = new vxUI.PopupItem();
      m_menuItemPlayStop.Text = "Play Stop";
      m_menuItemPlayStop.Click += new System.EventHandler(this.OnPlayStopClick);

      m_menuItemSaveDicomImageFiles = new vxUI.PopupItem();
      m_menuItemSaveDicomImageFiles.Text = "Save Images Files to Disk";
      m_menuItemSaveDicomImageFiles.Click += new System.EventHandler(this.OnSaveFilesClick);

      ResizeImage(Width, Height);

    }

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
    }

    #endregion


    #region Component Designer generated code
    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.m_pictureBox = new System.Windows.Forms.PictureBox();
      ((System.ComponentModel.ISupportInitialize)(this.m_pictureBox)).BeginInit();
      this.SuspendLayout();
      // 
      // m_pictureBox
      // 
      this.m_pictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
      this.m_pictureBox.Location = new System.Drawing.Point(0, 0);
      this.m_pictureBox.Name = "m_pictureBox";
      this.m_pictureBox.Size = new System.Drawing.Size(344, 288);
      this.m_pictureBox.TabIndex = 0;
      this.m_pictureBox.TabStop = false;
      this.m_pictureBox.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.OnImageMouseWheel);
      this.m_pictureBox.MouseDown += new System.Windows.Forms.MouseEventHandler(this.OnImageMouseDown);
      this.m_pictureBox.MouseMove += new System.Windows.Forms.MouseEventHandler(this.OnImageMouseMove);
      this.m_pictureBox.MouseUp += new System.Windows.Forms.MouseEventHandler(this.OnImageMouseUp);
      this.m_pictureBox.MouseEnter += new System.EventHandler(this.OnImageMouseEnter);
      // 
      // DicomImageControl
      // 
      this.BackColor = System.Drawing.Color.Black;
      this.Controls.Add(this.m_pictureBox);
      this.Name = "DicomImageControl";
      this.Size = new System.Drawing.Size(344, 288);
      ((System.ComponentModel.ISupportInitialize)(this.m_pictureBox)).EndInit();
      this.ResumeLayout(false);

    }
    #endregion

    #region properties

    /// <summary>
    ///  Sets/Gets the enable save image as menu option
    /// </summary>
    public bool EnablePopupMenu
    {
      get { return m_enablePopupMenu; }
      set { m_enablePopupMenu = value; }
    }

    /// <summary>
    ///  Sets/Gets the enable save image as menu option
    /// </summary>
    public bool EnableSaveImageAs
    {
      get { return m_enableSaveImageAs; }
      set { m_enableSaveImageAs = value; }
    }

    /// <summary>
    ///  Sets/Gets the enable view dicom information as menu option
    /// </summary>
    public bool EnableViewDicomInformation
    {
      get { return m_enableViewDicomInformation; }
      set { m_enableViewDicomInformation = value; }
    }

    /// <summary>
    /// Returns the dicom image
    /// </summary>
    public DicomImage DicomImage
    {
      get { return m_dicomImage; }
    }

    /// <summary>
    /// Returns the current frame
    /// </summary>
    public int CurrentFrameNumber
    {
      get { return m_frameNumber; }
    }

    /// <summary>
    /// Returns the current frame
    /// </summary>
    public int FirstFrameNumber
    {
      get { return 1; }
    }

    /// <summary>
    /// Returns the current frame
    /// </summary>
    public int LastFrameNumber
    {
      get { return m_numberOfFrames; }
    }


    /// <summary>
    /// Returns the current frame
    /// </summary>
    public bool FirstFrame
    {
      get { return m_frameNumber == 1; }
    }

    /// <summary>
    /// Returns the current frame
    /// </summary>
    public bool LastFrame
    {
      get { return m_frameNumber == m_numberOfFrames; }
    }

    /// <summary>
    /// Returns the number of frames in the image
    /// </summary>
    public int NumberOfFrames
    {
      get { return m_numberOfFrames; }
    }

    /// <summary>
    /// Flag to enable/disable the display patient name on the image
    /// </summary>
    public bool ShowPatientName
    {
      get { return m_showPatienName; }
      set { m_showPatienName = value; }
    }

    /// <summary>
    /// Flag to enable/disable the display patient name on the image
    /// </summary>
    public bool ShowCoordinates
    {
      get { return m_showCoordinates; }
      set { m_showCoordinates = value; }
    }

    /// <summary>
    /// Flag to enable/disable the display description on the image
    /// </summary>
    public bool ShowDescription
    {
      get { return m_showDescription; }
      set { m_showDescription = value; }
    }

    /// <summary>
    /// Fllag to enable/disable the display of the min/max value on the image
    /// </summary>
    public bool ShowMinMaxPixelValues
    {
      get { return m_showMinMax; }
      set { m_showMinMax = value; }
    }

    /// <summary>
    /// Flag to enable/disable the display of the pixel value on the image
    /// </summary>
    public bool ShowPixelValue
    {
      get { return m_showPixelValue; }
      set { m_showPixelValue = value; }
    }


    #endregion

    #region event handlers

    /// <summary>
    /// Hanles the Play Forward Click event
    /// </summary>
    /// <param name="sender">sending object</param>
    /// <param name="e">Event args</param>
    private void OnPlayForwardClick(object sender, System.EventArgs e)
    {

      PlayForward();
    } // OnPlayForwardClick()

    private void OnPlayStopClick(object sender, System.EventArgs e)
    {
      PlayStop();
    }

    private void OnPlayBackwardClick(object sender, System.EventArgs e)
    {
      PlayBackward();
    }

    /// <summary>
    /// Handles the Mouse Up Event in the image
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnImageMouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      if ( e.Button == MouseButtons.Right && m_enablePopupMenu && ( m_menuImage != null && m_menuImage.MenuItems.Count > 0 ) )
      {
        m_menuImage.Show(this, new Point(Cursor.Position.X, Cursor.Position.Y));
      }
    } // OnImageMouseUp()


    /// <summary>
    /// Handles the On Save Image As a Bitmap event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnSaveImageAsBmpClick(object sender, System.EventArgs e)
    {
      SaveImageAs(System.Drawing.Imaging.ImageFormat.Jpeg, vxIO.FileExtensions.Bitmap, "Save Image As Bitmap");
    }

    /// <summary>
    /// Save Image As Png click event
    /// </summary>
    /// <param name="sender">Source of event</param>
    /// <param name="e">Event arguements</param>
    private void OnSaveImageAsPngClick(object sender, System.EventArgs e)
    {
      SaveImageAs(System.Drawing.Imaging.ImageFormat.Jpeg, vxIO.FileExtensions.Png, "Save Image As Png");

    }

    /// <summary>
    /// Save Image As Jpeg click event
    /// </summary>
    /// <param name="sender">Source of event</param>
    /// <param name="e">Event arguements</param>
    private void OnSaveImageAsJpegClick(object sender, System.EventArgs e)
    {
      SaveImageAs(System.Drawing.Imaging.ImageFormat.Jpeg, vxIO.FileExtensions.Jpeg, "Save Image As Jpeg");
    }

    /// <summary>
    /// Handles the selection of the file save omenu option
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="args"></param>
    private void OnSaveFilesClick(object sender, EventArgs args)
    {
      if (  m_dicomImage != null  )
      {


        SaveFilesDialog saveFiles = new SaveFilesDialog();
      
        /////////////////////
        // Only save if OK
        /////////////////////
        if ( saveFiles.ShowDialog() == DialogResult.OK )
        {
          if ( saveFiles.Path.Length > 0 )
          {
            m_dicomImage.Write(saveFiles.Path); 
          }
        } // END ... If OK

      }
    }

    /// <summary>
    /// Preforms the saving of the image/
    /// </summary>
    /// <param name="imageFormat">Image format</param>
    /// <param name="extension">Image extension</param>
    /// <param name="title">Title for dialog</param>
    private void SaveImageAs(System.Drawing.Imaging.ImageFormat imageFormat, string extension, string title)
    {
      if (  m_dicomImage != null  )
      {


        ImageSaveAsDialog saveAs = new ImageSaveAsDialog(extension);

        saveAs.Text = title;

        string dicomFilename = vxIO.PathUtilities.GetFullTitle(m_dicomImage.DicomFileName);
        int frame = 1;
      
        // Set the file name
        saveAs.Filename = dicomFilename + extension;

        /////////////////////
        // Only save if OK
        /////////////////////
        if ( saveAs.ShowDialog() == DialogResult.OK )
        {
          if ( saveAs.Filename.Length > 0 )
          {
            string fileName = System.IO.Path.ChangeExtension(saveAs.Filename, extension);
        
            m_dicomImage.WriteImage(fileName, frame, imageFormat); 
          }
        } // END ... If OK

      }
    }


    /// <summary>
    /// Handles the On View Dicom Information Click event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnViewDicomInformation(object sender, System.EventArgs e)
    {
      if ( m_dicomImage != null )
      {
        DicomHeaderDialog info = new DicomHeaderDialog();
        info.SetElements(m_dicomImage);

        info.ShowDialog();
      }
    }

    /// <summary>
    /// Handles the On View Files click event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnViewDicomImageFiles(object sender, System.EventArgs e)
    {
      if ( m_dicomImage != null )
      {
        DicomImageFileListDialog dialog = new DicomImageFileListDialog(m_dicomImage);
        dialog.ShowDialog();
      }
    }


    /// <summary>
    /// Handles the MouseWheel event in the Dicom Image
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnImageMouseWheel(object sender, System.Windows.Forms.MouseEventArgs e)
    {

      int numberOfItemsToMove = e.Delta / -120;

      if (numberOfItemsToMove != 0) 
      {

        if ( m_numberOfFrames > 1 )
        {

          int frameNumber = m_frameNumber + numberOfItemsToMove;

          if ( frameNumber > 0 && frameNumber <= m_numberOfFrames )
          {
            m_frameNumber = frameNumber;
            SetBitmapImage(m_dicomImage.ToBitmap(m_frameNumber));
            SetMultiFrameDescription(m_frameNumber);
            RefreshImage();
          }

        }

      }

    }



    private void OnImageMouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
    {
      if ( e.Button == MouseButtons.Right )
      {
        m_prev.X = e.X;
        m_prev.Y = e.Y;

      }
    }


    /// <summary>
    /// Handles the Image Resize event
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnImageResize(object sender, System.EventArgs e)
    {

      //ResizeImage();

      int maxWidth = Width - 20;
      int maxHeight = Height - 20;

      m_imageWidth = System.Math.Min(maxWidth, maxHeight);
      m_imageHeight = m_imageWidth;

      m_imageRect = new Rectangle((int)((maxWidth - m_imageWidth) / 2 ) + 10, (int)((maxHeight - m_imageHeight) / 2 ) + 10, m_imageWidth, m_imageHeight);

      CalculateImageScales();

      RefreshImage();


    } // OnImageResize()


    /// <summary>
    /// Handles the Mouse move event within the image
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event args</param>
    private void OnImageMouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
    {

      if ( m_bitmap == null || m_playing ||  ! m_showPixelValue ) return;

      switch ( e.Button )
      {
        case MouseButtons.Right:
        {
          if ( m_prev.X < e.X )
          {
            m_level -= m_increment;
          }
          else if ( m_prev.X > e.X )
          {
            m_level += m_increment;
          }
          
          if ( m_prev.Y < e.Y )
          {
            m_window -= m_increment;
            m_window = ( m_window < 0 ? 0 : m_window ); 

          }
          else if ( m_prev.Y > e.Y )
          {
            m_window += m_increment;
          }

          m_prev.X = e.X;
          m_prev.Y = e.Y;

          m_windowLevel = String.Format("W/L: {0}/{1}", m_window, m_level);

        }
        break;

        case MouseButtons.None:
        {

          // X - Row
          // Y - Column 
          uint y = 0;
          uint x = 0;

          int X = e.X - m_imageRect.X + 1;
          int Y = e.Y - m_imageRect.Y + 1;

          bool outsideImage = false;

          if ( Y < 0 || Y > m_imageRect.Width )
          {
            outsideImage = true;
          }
          else
          {
            y = (uint)System.Math.Min((int)((double)Y * m_widthScale), m_bitmap.Width); 
            outsideImage = false;
          }

          if ( X < 0 || X > m_imageRect.Height )
          {
            outsideImage = true;
          }
          else
          {
            x = (uint)System.Math.Min((int)((double)X * m_heightScale), m_bitmap.Height); 
            outsideImage = false;
          }

          if ( outsideImage )
          {
            m_imagePosition = string.Empty;
            m_pixelValue = string.Empty;
          }
          else
          {
            m_imagePosition   = String.Format("( {0} , {1} ) ", x, y);     
            m_pixelValue = string.Format("{0}/{1}", m_dicomImage.GetPixelValue(y, x), m_dicomImage.GetHUValue(y, x));
          }


        }
        break;

        default:

        break;

      } // END ... Button Switch

      RefreshImage();


    } // OnImageMouseMove()

    /// <summary>
    /// Handles the mouse enter event.
    /// Sets the focus to the picture box so that mouse wheel events can be caught
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event args</param>
    private void OnImageMouseEnter(object sender, System.EventArgs e)
    {
      m_pictureBox.Focus();
    } // OnImageMouseEnter()



    #endregion

    #region events

    public event System.EventHandler PlayStarted;

    public event System.EventHandler PlayStopped;

    #endregion

    #region methods

    /// <summary>
    /// Gets the pixel value
    /// </summary>
    /// <param name="x">X - column</param>
    /// <param name="y">Y - Row</param>
    /// <returns>Pixel Value</returns>
    public int GetPixelValue( uint x, uint y)
    {

      ////////////////////////////
      // Check for a dicom image
      ///////////////////////////
      if ( m_dicomImage == null )
      {
        throw new System.NullReferenceException("No dicom image set");
      } // END ... If there is no DICOM IMAGE

      return m_dicomImage.GetPixelValue(y,x);
    } // GetPixelValue(int x, int y)


    /// <summary>
    /// Gets the HU value
    /// </summary>
    /// <param name="x">X - column</param>
    /// <param name="y">Y - Row</param>
    /// <returns>HU Value</returns>
    public int GetHUValue(uint x, uint y)
    {

      ////////////////////////////
      // Check for a dicom image
      ///////////////////////////
      if (m_dicomImage == null)
      {
        throw new System.NullReferenceException("No dicom image set");
      } // END ... If there is no DICOM IMAGE

      return m_dicomImage.GetHUValue(y, x);
    } // GetHUValue(int x, int y)


    /// <summary>
    /// Clears image
    /// </summary>
    public void ClearImage()
    {
      Reset();
      RefreshImage();
    } // Clear()

    /// <summary>
    /// Dispose of Image and Clears
    /// </summary>
    public void DisposeImage()
    {
      if ( m_dicomImage != null )
      {
        m_dicomImage.Dispose();
      }

      ClearImage();
    } // DisposeImage()

    private void InitializeImageMenu()
    {

      this.m_menuImage.MenuItems.Clear();

      if ( m_dicomImage != null )
      {
        if ( m_numberOfFrames > 1 )
        {
          this.m_menuImage.MenuItems.AddRange(new vxUI.PopupItem[] {
                                                                     this.m_menuItemSaveImageAs,
                                                                     this.m_menuItemSeperator,
                                                                     this.m_menuItemViewDicomInformation,
                                                                     this.m_menuItemSeperator,
                                                                     this.m_menuItemPlayForward,
                                                                     this.m_menuItemPlayBackward,
                                                                     this.m_menuItemPlayStop } );

          this.m_menuItemPlayStop.Enabled = false;
          this.m_menuItemPlayForward.Enabled = true;
          this.m_menuItemPlayBackward.Enabled = false;
        }
        else
        {
          if ( m_dicomImage.FileImage )
          {
            this.m_menuImage.MenuItems.AddRange(new vxUI.PopupItem[] {
                                                                       this.m_menuItemSaveImageAs,
                                                                       this.m_menuItemSaveDicomImageFiles,
                                                                       this.m_menuItemSeperator,
                                                                       this.m_menuItemViewDicomImageFiles,
                                                                       this.m_menuItemSeperator,
                                                                       this.m_menuItemViewDicomInformation } );

          }
          else
          {
            this.m_menuImage.MenuItems.AddRange(new vxUI.PopupItem[] {
                                                                       this.m_menuItemSaveImageAs,
                                                                       this.m_menuItemSeperator,
                                                                       this.m_menuItemViewDicomInformation } );
          }
        }                                                                                          
      }

    }

    /// <summary>
    /// Sets the window level (Not Implemented)
    /// </summary>
    /// <param name="window">New window</param>
    /// <param name="level">New level</param>
    public void SetWindowLevel(int window, int level)
    {
      m_window = window;
      m_level = level;

      m_windowLevel = String.Format("W/L:  {0}/{1}", m_window, m_level);

      RefreshImage();

    }

    /// <summary>
    /// Resets the image
    /// </summary>
    private void Reset()
    {

      SetBitmapImage(null);

      m_dicomImage    = null;

      m_patientName   = string.Empty;
      m_imagePosition = string.Empty;
      m_minmax        = string.Empty;
      m_description   = string.Empty;
      m_pixelValue    = string.Empty;
      m_coordinates   = string.Empty;

    } // Reset()

    /// <summary>
    /// Sets the bitmap image
    /// </summary>
    /// <param name="bitmap"></param>
    private void SetBitmapImage(Bitmap bitmap)
    {
      if ( m_bitmap != null )
      {
        m_bitmap.Dispose();
      }

      m_bitmap = bitmap;
    } // SetBitmapImage()

    /// <summary>
    /// Sets the dicom image with no image position
    /// </summary>
    /// <param name="dicomImage">Dicom image to be viewed</param>
    public void SetDicomImage(DicomImage dicomImage )
    {
      SetDicomImage(dicomImage, string.Empty);
    } // SetDicomImage()

    /// <summary>
    /// Sets the dicom image with no image position
    /// </summary>
    /// <param name="dicomFile">Dicom file viewed</param>
    public void SetDicomImage(string dicomFile )
    {
      SetDicomImage(dicomFile, string.Empty);
    } // SetDicomImage()

    /// <summary>
    /// Sets the dicom image with no image position
    /// </summary>
    /// <param name="dicomFile">Dicom file viewed</param>
    /// <param name="description">Image position</param>
    public void SetDicomImage(string dicomFile, string description )
    {
      SetDicomImage(new DicomImage(dicomFile), description);
    } // SetDicomImage()

    /// <summary>
    /// Sets the dicom image to be viewes
    /// </summary>
    /// <param name="dicomImage">Dicom image to be viewed</param>
    /// <param name="description">Image position</param>
    public void SetDicomImage(DicomImage dicomImage,  string description )
    {
 
      Reset();

      if (m_dicomImage == null || (m_dicomImage != null && m_dicomImage.InstanceUID != dicomImage.InstanceUID))
      {

        m_dicomImage = dicomImage;

        if (m_dicomImage != null)
        {
          SetBitmapImage(m_dicomImage.ToBitmap());

          m_numberOfFrames = m_dicomImage.NumberOfFrames;
          m_description = (m_numberOfFrames == 1 ? description : SetMultiFrameDescription(1));


          m_coordinates = m_dicomImage.Coordinates.ToString().Trim();
          m_patientName = m_dicomImage.PatientName.ToString().Trim();
          m_minmax = String.Format("{0} / {1} ", m_dicomImage.MinimumPixelValue.ToString(), m_dicomImage.MaximumPixelValue.ToString());


          CalculateImageScales();

          //SetWindowLevel( (m_dicomImage.WindowWidth == 0 ? 660 : m_dicomImage.WindowWidth), ( m_dicomImage.WindowCenter == 0 ? 108 : m_dicomImage.WindowCenter ));
          RefreshImage();

          InitializeImageMenu();

        }
        else
        {
          //SetWindowLevel( (m_dicomImage.WindowWidth == 0 ? 660 : m_dicomImage.WindowWidth), ( m_dicomImage.WindowCenter == 0 ? 108 : m_dicomImage.WindowCenter ));
          RefreshImage();
        }
      }

    } // SetDicomImage()


    protected override void OnResize(EventArgs e)
    {
      int width = Width;
      int height = Height;


      if ( Parent != null )
      {
        width = Parent.Width;
        height = Parent.Height;
      }


      ResizeImage(width, height);

      int maxWidth = width - 20;
      int maxHeight = height - 20;

      m_imageWidth = System.Math.Min(maxWidth, maxHeight);
      m_imageHeight = m_imageWidth;

      m_imageRect = new Rectangle((int)((maxWidth - m_imageWidth) / 2 ) + 10, (int)((maxHeight - m_imageHeight) / 2 ) + 10, m_imageWidth, m_imageHeight);

      CalculateImageScales();

      RefreshImage();


      base.OnResize (e);
    }

    /// <summary>
    /// Resize the picture boxes image
    /// </summary>
    private void ResizeImage(int width, int height)
    {
      
      if ( m_image != null )
      {
        m_image.Dispose();
        m_image = null;
      }


      if ( width > 0 && height > 0 )
      {
        m_image = new Bitmap(width, height);
        m_pictureBox.Image = m_image;
      }


    } // ResizeImage()

    public override void Refresh()
    {
      base.Refresh ();
      RefreshImage();
    }


    /// <summary>
    /// Refreshes the master image
    /// </summary>
    private void RefreshImage()
    {
    
      if ( m_image == null ) return;

      Graphics gfx = Graphics.FromImage(m_image);

      gfx.Clear(this.BackColor);

      if ( m_bitmap != null )
      {
        //gfx.DrawImage(m_dicomImage.ToBitmap(m_window, m_level), m_imageRect);
        gfx.DrawImage(m_bitmap, m_imageRect);
      

        ////////////////////
        // Left Hand Text
        ////////////////////
      
        StringFormat  format = new StringFormat();
        format.Alignment = StringAlignment.Near;

        int y = 0;

        int textWidth = 300;
        int textWidthOffset = 310;
        Size textSize = new Size(textWidth,20);


      
        if ( m_showPatienName )
        {
          gfx.DrawString(m_patientName, Font, System.Drawing.Brushes.White, new Rectangle(new Point(0,y), textSize), format);
          y += 21;
        }

        //y = this.Height - 15;
        if ( m_showCoordinates )
        {
          gfx.DrawString(m_coordinates, Font, System.Drawing.Brushes.White, new Rectangle(new Point(0,y), textSize), format);
          y += 21;
        }

        if ( m_showDescription )
        {
          gfx.DrawString(m_description, Font, System.Drawing.Brushes.White, new Rectangle(new Point(0,y), textSize), format);
          y += 21;
        }
  

        if ( m_showWindowLevel && ! m_playing )
        {
          gfx.DrawString(m_windowLevel, Font, System.Drawing.Brushes.White, new Rectangle(new Point(0,y), textSize), format);
          y += 21;
        }

        ////////////////////////
        // Right Hand Text
        //////////////////////

        format.Alignment = StringAlignment.Far;
        int width = Width; 

        y = 0;

        if ( m_showMinMax )
        {
          gfx.DrawString(m_minmax, Font, System.Drawing.Brushes.White, new Rectangle(new Point(width - textWidthOffset,y), textSize), format);
          y += 21;
        }

        if ( m_showPixelValue && ! m_playing )
        {
          gfx.DrawString(m_imagePosition, Font, System.Drawing.Brushes.White, new Rectangle(new Point(width - textWidthOffset,y), textSize), format);
          y += 21;

          gfx.DrawString(m_pixelValue, Font, System.Drawing.Brushes.White, new Rectangle(new Point(width - textWidthOffset,y), textSize), format);
          y += 21;
        }

        m_pictureBox.Refresh();
      }

    } // RefreshImage()

    /// <summary>
    /// Calculates the image scales 
    /// </summary>
    private void CalculateImageScales()
    {
      if ( m_bitmap != null )
      {
        m_heightScale = (double)m_bitmap.Height / (double)m_imageRect.Height ;
        m_widthScale  = (double)m_bitmap.Width  / (double)m_imageRect.Width ;
      }
    }

    /// <summary>
    /// Play the frames in the forward direction
    /// </summary>
    public void PlayForward()
    {
      if ( m_numberOfFrames > 1 )
      {
        m_frameIncrement = 1;
        m_frameEnd = m_numberOfFrames + 1;

        m_menuItemPlayBackward.Enabled = false;

        StartPlayThread();
      }
       
    } // PlayForward()

    /// <summary>
    /// Play the frames in the backward direction
    /// </summary>
    public void PlayBackward()
    {
      if ( m_numberOfFrames > 1 )
      {
        m_frameIncrement = -1;
        m_frameEnd = 0;

        m_menuItemPlayForward.Enabled = false;

        StartPlayThread();
      }
    } // PlayBackward()

    /// <summary>
    /// Stop playing
    /// </summary>
    public void PlayStop()
    {
      m_stopping = true;

      if ( this.PlayStopped != null )
      {
        PlayStopped(this, System.EventArgs.Empty);
      }

    } // PlayStop()


    /// <summary>
    /// Plays the images as a movie
    /// </summary>
    private void Play()
    {
      try
      {

        if ( m_numberOfFrames > 1 )
        {

          for ( ;  ! m_stopping && m_frameNumber != m_frameEnd; m_frameNumber += m_frameIncrement )
          {
            SetBitmapImage(m_dicomImage.ToBitmap(m_frameNumber));
            SetMultiFrameDescription(m_frameNumber);  
            RefreshImage();
          }

          m_frameNumber -= m_frameIncrement;


        }
        
      }
      catch ( Exception  )
      {
        
      }
      finally
      {
        m_playing = false;
        m_stopping = false;
        m_menuItemPlayStop.Enabled = false;
        m_menuItemPlayForward.Enabled = true;
        m_menuItemPlayBackward.Enabled = true;

        if ( m_frameNumber == 1 )
        {
          m_menuItemPlayForward.Enabled = true;
          m_menuItemPlayBackward.Enabled = false;
        }
        else if ( m_frameNumber == m_numberOfFrames )
        {
          m_menuItemPlayForward.Enabled = false;
          m_menuItemPlayBackward.Enabled = true;

        }

        if ( PlayStopped != null )
        {
          PlayStopped(this, System.EventArgs.Empty);
        }
      }

    } // Play()

    /// <summary>
    /// Starts the play thread
    /// </summary>
    private void StartPlayThread()
    {

      m_menuItemPlayStop.Enabled   = true;


      m_playing  = true;
      m_stopping = false;

      if ( this.PlayStarted != null )
      {
        PlayStarted(this, System.EventArgs.Empty);
      }

      m_playThread = new System.Threading.Thread(new System.Threading.ThreadStart(this.Play));

      m_playThread.Start();

    } // 

    /// <summary>
    /// Sets the multi frame description
    /// </summary>
    /// <param name="frameNumber"></param>
    private string SetMultiFrameDescription(int frameNumber)
    {
      m_description = ( m_numberOfFrames == 1 ? @"" : "Frame " + frameNumber.ToString() + " of " + m_numberOfFrames.ToString() );
      return m_description;
    }

    #endregion





 
	}
}
