using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Viatronix.Dicom;
using Viatronix.Dicom.Imaging;
using System.Windows.Input;
using System.Drawing;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using System.Windows.Interop;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Windows;
using System.Xml;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Helpers;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Messaging;
using System.Windows.Controls;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using GalaSoft.MvvmLight.Threading;
using Viatronix.Dicom.MVVM.Converters;

namespace Viatronix.Dicom.MVVM.ViewModels
{


  #region enum Actions

  /// <summary>
  /// Implements a list of possible activities the view model can be performing.
  /// </summary>
  public enum Actions
  {
    Normal,              // Normal use to aspecial action
    WindowLeveling,       // Performing window leveling
    Zooming,              // Performing zooming
    Panning               // Performing panning
  };  // Actions

  #endregion


  #region class ImageViewModel

  /// <summary>
  /// Implements a view mode for a dicom image
  /// </summary>
  public class ImageViewModel : ViewModelBase
  {

    #region fields

    private DicomImage m_renderer;

    /// <summary>
    /// Current image position
    /// </summary>
    private System.Windows.Point m_pixelPosition = new System.Windows.Point(0, 0);

    /// <summary>
    ///  Image Index
    /// </summary>
    private int m_imageIndex = 0;

    /// <summary>
    /// Image Count
    /// </summary>
    private int m_imageCount = 0;

    /// <summary>
    /// Sensitivy adjustment
    /// </summary>
    private float m_sensitiviy = 2.0F;

    /// <summary>
    /// Header View Model
    /// </summary>
    private HeaderViewModel m_header;

    /// <summary>
    /// Current activity the view model is performing
    /// </summary>
    private Actions m_currentAction = Actions.Normal;

 
    /// <summary>
    /// Image this view model wraps
    /// </summary>
    private Viatronix.Dicom.IImage m_image = null;

    /// <summary>
    /// Size of zoomed/panned bitmap
    /// </summary>
    private Rectangle m_bitmapRect = new Rectangle(0, 0, 0, 0);


    /// <summary>
    /// Instance UID
    /// </summary>
    private string m_uid = string.Empty;


    /// <summary>
    /// Is Selected flag
    /// </summary>
    private bool m_isSelected = false;

  
    /// <summary>
    /// Position used for determining movement of the mouse
    /// </summary>
    private System.Windows.Point m_position;


    private System.Windows.Point m_panOffset = new System.Windows.Point(0, 0);


    private RelayCommand m_mouseWheelUpCommand = null;
    private RelayCommand m_mouseWheelDownCommand = null;


    /// <summary>
    /// Mouse wheel command
    /// </summary>
    private RelayCommand<Viatronix.UI.Gestures.MouseWheelAction> m_mouseWheelCommand = null;

    /// <summary>
    /// Double click command
    /// </summary>
    private RelayCommand<MouseButtonEventArgs> m_doubleClickCommand = null;
    
    /// <summary>
    /// Key down command 
    /// </summary>
    private RelayCommand<KeyEventArgs> m_keyDownCommand = null;

    /// <summary>
    /// Key up command
    /// </summary>
    private RelayCommand<KeyEventArgs> m_keyUpCommand = null;


    /// <summary>
    /// Mouse up command
    /// </summary>
    private RelayCommand<MouseEventArgs> m_mouseUpCommand = null;


    /// <summary>
    /// Mouse down command
    /// </summary>
    private RelayCommand<MouseEventArgs> m_mouseDownCommand = null;

    /// <summary>
    /// Mouse Leave command
    /// </summary>
    private RelayCommand m_mouseLeaveCommand;

    /// <summary>
    /// Mouse Move Command
    /// </summary>
    private RelayCommand<UIElement> m_mouseMoveCommand;

 
    /// <summary>
    /// Current point 
    /// </summary>
    private System.Windows.Point m_currentPoint = new System.Windows.Point(0, 0);

 

    /// <summary>
    /// Thumbnail Size (Default 128 x 128 )
    /// </summary>
    private System.Windows.Size m_thumbnailSize = new System.Windows.Size(128, 128);
 
    /// <summary>
    /// Current window level
    /// </summary>
    private WindowLevel m_windowLevel;

    /// <summary>
    /// Original window level used for reset
    /// </summary>
    private WindowLevel m_originalWindowLevel;

    /// <summary>
    /// Pixel value at current position
    /// </summary>
    private string m_pixelValue = "-";


    private uint m_rows;
    private uint m_cols;

    private double m_width = 0.0;
    private double m_maxWidth = 0.0;
    private double m_height = 0.0;
    private double m_maxHeight = 0.0;

    /// <summary>
    /// Minimum zoom allowed
    /// </summary>
    private double m_minimumZoom = 1.0;

    /// <summary>
    /// Maximum zoom allowed
    /// </summary>
    private double m_maximumZoom = 10.0;

    /// <summary>
    /// Actual zoom factor
    /// </summary>
    private double m_zoom = 1.0;

    /// <summary>
    /// Patient name
    /// </summary>
    private string m_patientName = string.Empty;

     /// <summary>
    /// Pressed key
    /// </summary>
    private Key m_pressedKey = Key.None;

    /// <summary>
    /// Modality default window levels
    /// </summary>
    private Dictionary<string, WindowLevel> m_defaults = new Dictionary<string, WindowLevel>();
 
    #endregion

    #region constructors


    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="parent">Parent View Model</param>
    /// <param name="imageNumber">Image Number in list</param>
    /// <param name="dataset">Dicom Dataset</param>
    public ImageViewModel()
    {
      Messenger.Default.Register<int>(this, UpdateImageCount);

      m_defaults.Add("US", new WindowLevel(107, 167));


    } // ImageViewModel( )

 
    #endregion

    #region message handlers


    /// <summary>
    /// Updates the pixel value type
    /// </summary>
    /// <param name="showHU">True indicates the value is HU, PV otherwise</param>
    private void UpdateImageCount(int imageCount)
    {
      ImageCount = imageCount;
    } // UpdatePixelValueType(bool showHU)

    #endregion

    #region methods

    /// <summary>
    /// Creates the ImageViewModel from the dataset
    /// </summary>
    /// <param name="dataset"></param>
    /// <returns></returns>
    public static ImageViewModel Create(IImage image)
    {

      ImageViewModel imageViewModel = new ImageViewModel();

      imageViewModel.m_renderer = DicomImage.Create(image);
      imageViewModel.m_image = image;
      imageViewModel.m_uid = image.InstanceUid;
      imageViewModel.m_patientName = image.PatientName;
      imageViewModel.m_header = HeaderViewModel.Create(imageViewModel, image.GetElements());
      imageViewModel.m_rows = image.Rows;
      imageViewModel.m_cols = image.Columns;

      imageViewModel.m_height = imageViewModel.m_cols;
      imageViewModel.m_width = imageViewModel.m_rows;
      imageViewModel.m_maxHeight = imageViewModel.m_height - imageViewModel.m_rows;
      imageViewModel.m_maxWidth = imageViewModel.m_width - imageViewModel.m_cols;
      imageViewModel.m_bitmapRect = new Rectangle(0, 0, (int)imageViewModel.m_width, (int)imageViewModel.m_height);

      if ( (image.WindowCenter == 0 || image.WindowWidth == 0) )
      {
        if ( imageViewModel.m_defaults.ContainsKey(image.Modality) )
          imageViewModel.m_windowLevel = imageViewModel.m_defaults[image.Modality];
        else if ( ! image.IsMonochrome )
          imageViewModel.m_windowLevel  = new WindowLevel(128.0F, 255.0F);
      }

      imageViewModel.m_originalWindowLevel = imageViewModel.m_windowLevel;

      return imageViewModel;

    } // Create(Dataset)

    /// <summary>
    /// Perform the pan
    /// </summary>
    /// <returns>Returns new bitmap</returns>
    private Bitmap ZoomPan(Bitmap bitmap)
    {
      Bitmap zoomedBitmap = bitmap;


      if ( m_zoom != 1.0F && m_zoom > 0F)
      {


        // New bitmap ( make the new bitmap the exact size of the image )
        zoomedBitmap = new Bitmap((int)m_cols, (int)m_rows, PixelFormat.Format24bppRgb);

        // Set the resolution
        zoomedBitmap.SetResolution(bitmap.HorizontalResolution, bitmap.VerticalResolution);

        // Graphics
        Graphics graphics = Graphics.FromImage(zoomedBitmap);

        // Clear the background to black
        graphics.Clear(Color.Black);

 
        m_bitmapRect.Width  = (int)(m_cols / m_zoom);
        m_bitmapRect.Height = (int)(m_rows / m_zoom);
        m_bitmapRect.X      = (int)((m_cols - m_bitmapRect.Width) / 2)  + (int)m_panOffset.X;
        m_bitmapRect.Y      = (int)((m_rows - m_bitmapRect.Height) / 2) + (int)m_panOffset.Y;;

        // Set interpolation
        graphics.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.High;

 

       // Draw the new image with the panned rectangle
        graphics.DrawImage(bitmap,
                            new Rectangle(0, 0, (int)m_cols, (int)m_rows),
                            m_bitmapRect,
                            GraphicsUnit.Pixel);

        // Dispose of the graphics
        graphics.Dispose();

      }

      // Return the newly rendered panned image
      return zoomedBitmap;

    } // Pan()


    /// <summary>
    /// Resets the image
    /// </summary>
    public void Reset()
    {
      m_windowLevel = m_originalWindowLevel;
      m_zoom = 1.0;
      RaisePropertyChanged("Image");
    }

    #endregion

    #region command handlers


 
    /// <summary>
    /// DoubleCLick event handler
    /// </summary>
    public void OnDoubleClickHandler(MouseButtonEventArgs args)
    {

      // ========================
      // Must be a double click
      // ========================
      if (args.ClickCount == 2)
      {

        RaisePropertyChanged("MouseDoubleClick");


        // ================================
        // Only launch for Encapsulated PDF
        // ================================
        if (m_image.IsEncapsulatedPdf)
        {

          // Filename is UID based (replace the . with _ )
          string sFilename = m_image.InstanceUid.Replace('.', '_') + ".pdf";

          // Put the file in the users temp
          string sPdfFile = System.IO.Path.Combine(System.IO.Path.GetTempPath(), sFilename);

          // Create a buffer to get the pdf binary
          Viatronix.Dicom.Buffer pdfBuffer = new Viatronix.Dicom.Buffer();

 
          // ==========================================================================================
          // Try to get the Encapsulated Pdf element. If it is found then create a file and start 
          // the Acrobat Viewer or the extension based viewer.
          // ==========================================================================================
          if ( m_image.TryGetValue((ushort)0x0042, (ushort)0x0011, ref pdfBuffer) )
          {

            // Open/Create the temporary PDF file
            System.IO.FileStream stream = new System.IO.FileStream(sPdfFile, System.IO.FileMode.Create);

            // Write the binary to the file
            stream.Write(pdfBuffer.GetByteData(), 0, pdfBuffer.Size);

            // Close the file
            stream.Close();

            // Attempt to start the processes associated with the .pdf extension
            ProcessStartInfo info = new ProcessStartInfo(sPdfFile);
            info.WorkingDirectory = System.IO.Path.GetDirectoryName(sPdfFile);
            Process viewer = Process.Start(info);

            // Wait for the process to finish
            viewer.WaitForExit();

            // Delete the temp file
            System.IO.File.Delete(sPdfFile);
            

          } // END ... If the Encapsulated PDF element was found

        } // END ... If the dataset is encapsulated pdf

      } // END ... If 2 clicks were detected
 
    }  // OnDoubleClickHandler()


    /// <summary>
    /// Mouse leave event handler
    /// </summary>
    public void OnMouseLeaveHandler()
    {

      // clear the pixel position and value properties
      m_pixelPosition.X = 0;
      m_pixelPosition.Y = 0;

      RaisePropertyChanged("PixelPosition");

      PixelValue = "-";

      CurrentAction = Actions.Normal;


     }  // OnMouseLeaveHandler()

 
    /// <summary>
    /// Mouse Move Handler
    /// </summary>
    /// <param name="control">Control</param>
    public void OnMouseMoveHandler(UIElement element)
    {

 
      // Find the dicom Image control
      //System.Windows.Controls.Image image = (System.Windows.Controls.Image)control.FindName("dicomImage");

      // Get the mouse point from the image panel
      System.Windows.Point point = Mouse.GetPosition(element);




      
      // =======================================================================
      // Interpret the mouse move base on the current action being performed.
      // =======================================================================
      switch (m_currentAction)
      {

        case Actions.WindowLeveling:
          {


            if (point.X != 0 && point.Y != 0)
            {
              // Determine the Delta of the point using the sensitivity adjustment
              System.Drawing.Point delta = new System.Drawing.Point((int)((m_currentPoint.X - point.X) * m_sensitiviy), (int)((m_currentPoint.Y - point.Y) * m_sensitiviy));

              // Adjust the current window level with the delta values calculated above
              m_windowLevel.Center -= delta.Y;
              m_windowLevel.Width  -= delta.X;

              // Save the new current point
              m_currentPoint = point;

              // Set the window level
              this.WindowLevel = m_windowLevel;
            }

          }
          break;

        case Actions.Panning:
          {

            if (point.X != 0 && point.Y != 0)
            {

              // Determine the Delta of the point using the sensitivity adjustment
              System.Drawing.Point delta = new System.Drawing.Point((int)((m_currentPoint.X - point.X) * m_sensitiviy), (int)((m_currentPoint.Y - point.Y) * m_sensitiviy));


              //===================================================
              // Caculate the panning offsets which are cumulativent
              //==================================================
              m_panOffset.X   += delta.X;
              m_panOffset.Y   += delta.Y;

              // Reset the current poi
              m_currentPoint = point;

              // Indicate an image change has occurred
              RaisePropertyChanged("Image");
            } // END ... If the current point is not 0,0

          }
          break;

        case Actions.Zooming:
          {

            bool controlKeyDown = Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl);

            if (controlKeyDown)
            {

              if (point.X != 0 && point.Y != 0)
              {


                // Determine the Delta of the point using the sensitivity adjustment
                System.Drawing.Point delta = new System.Drawing.Point((int)((m_currentPoint.X - point.X) * m_sensitiviy), (int)((m_currentPoint.Y - point.Y) * m_sensitiviy));

                /// Calculate the new zoom factor
                double zoom = Math.Round(m_zoom + (delta.Y < 0 ? -0.10F : 0.10F), 2);

                /// Reset the current point
                m_currentPoint = point;

                /// ===============================================================
                /// Only force an image desiplay if the zomm os greater then 0
                /// ================================================================
                if (zoom > 0)
                {
                  // Set the ZOOM factor
                  this.Zoom = zoom;

                  /// Indicate that the image has changed
                  RaisePropertyChanged("Image");
                }

              }

            }
            else
              CurrentAction = Actions.Normal;
          }
          break;

        default:
          {

            
            //================================================================================================================
            // Get the offset value of the mouse point and convert to a coordinate based on the original dicom images size. 
            // This is done because only the size of the original is displayed at one time while the rest is hidden until
            // it is panned into view.
            //================================================================================================================
            double X = point.X * (m_bitmapRect.Width / element.RenderSize.Width);
            double Y = point.Y * (m_bitmapRect.Height / element.RenderSize.Height);


            this.m_pixelPosition.X = (int)(X + m_bitmapRect.X);
            this.m_pixelPosition.Y = (int)(Y + m_bitmapRect.Y);

            RaisePropertyChanged("PixelPosition");

         
            //=================================================================================================
            // Get the pixel value or HU base on the m_showHU flag. The default is to display the HU value
            //================================================================================================
            try
            {
              this.PixelValue = m_image.GetHUValue((uint)m_pixelPosition.X, (uint)m_pixelPosition.Y).ToString();
            }
            catch (Exception)
            {
              this.PixelValue = "-";
            }

          }
          break;
      }

        

    } // OnMouseMoveHandler(UserControl control)



    #endregion

    #region Commands

    /// <summary>
    /// Double Click Command. This is executed when the mouse leaves the dicom image.
    /// </summary>
    public RelayCommand<MouseButtonEventArgs> DoubleClickCommand
    {
      get
      {
        if (m_doubleClickCommand == null)
          m_doubleClickCommand = new RelayCommand<MouseButtonEventArgs>(param => OnDoubleClickHandler(param), param => true);

        return m_doubleClickCommand;
      }
    } // DoubleClickCommand

    /// <summary>
    /// Mouse Leave Command. This is executed when the mouse leaves the dicom image.
    /// </summary>
    public RelayCommand MouseLeaveCommand
    {
      get
      {
        if (m_mouseLeaveCommand == null)
          m_mouseLeaveCommand = new RelayCommand(() => OnMouseLeaveHandler(), () => true );

        return m_mouseLeaveCommand;
      }
    } // MouseLeaveCommand

    /// <summary>
    /// Key down command
    /// </summary>
    public RelayCommand<KeyEventArgs> KeyDownCommand
    {
      get
      {
        if (m_keyDownCommand == null)
          m_keyDownCommand = new RelayCommand<KeyEventArgs>(param => 
          { 
            m_pressedKey = param.Key; 
          }, param => true);

        return m_keyDownCommand;
      }
    } // KeyDownCommand

    /// <summary>
    /// Key Up command
    /// </summary>
    public RelayCommand<KeyEventArgs> KeyUpCommand
    {
      get
      {
        if (m_keyUpCommand == null)
          m_keyUpCommand = new RelayCommand<KeyEventArgs>(param =>
          { 
            m_pressedKey = Key.None; 
          }, param => true);

        return m_keyUpCommand;
      }
    } // KeyUpCommand


 
    /// <summary>
    /// Command to handle the mouse up event. We only need to disable all activity.
    /// </summary>
    public RelayCommand<MouseEventArgs> MouseUpCommand
    {
      get
      {
        if (m_mouseUpCommand == null)
          m_mouseUpCommand = new RelayCommand<MouseEventArgs>(param => 
          {
            CurrentAction = Actions.Normal; 
          }, param => true);

        return m_mouseUpCommand;
      }
    } // MouseUpCommand

    /// <summary>
    /// Command to handle the mouse up event. We only need to disable all activity.
    /// </summary>
    public RelayCommand<MouseEventArgs> MouseDownCommand
    {
      get
      {
        if (m_mouseDownCommand == null)
          m_mouseDownCommand = new RelayCommand<MouseEventArgs>(param => 
          {
            IsSelected = true;

            bool controlKeyDown = Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl);

            if (param.RightButton == MouseButtonState.Pressed && !controlKeyDown)
            {
              m_currentPoint = param.GetPosition((IInputElement)param.Source);
              CurrentAction = Actions.WindowLeveling;
            }
            else if (param.LeftButton == MouseButtonState.Pressed && controlKeyDown )
            {
              m_currentPoint = param.GetPosition((IInputElement)param.Source);
 
              CurrentAction = Actions.Zooming;

            }
            else if (param.LeftButton == MouseButtonState.Pressed && !controlKeyDown)
            {

              if (m_zoom != 1.0)
              {
                m_currentPoint = param.GetPosition((IInputElement)param.Source);
  
                CurrentAction = Actions.Panning;
              }
            }
          }, 
          param => true);

        return m_mouseDownCommand;
      }
    } // MouseUpCommand

 
    /// <summary>
    /// Mouse move command. This is executed when the mouse is moved within the dicom image.
    /// </summary>
    public RelayCommand<UIElement> MouseMoveCommand
    {
      get
      {
        if (m_mouseMoveCommand == null)
          m_mouseMoveCommand = new RelayCommand<UIElement>(param => OnMouseMoveHandler(param), param => true);

        return m_mouseMoveCommand;
      }
    } // MouseMoveCommand

    #endregion

     #region properties


    public Actions CurrentAction
    {
      get { return m_currentAction; }
      set { Set<Actions>("CurrentAction", ref m_currentAction, value); }

    }

    /// <summary>
    /// ImageIndex
    /// </summary>
    public int ImageIndex
    {
      get { return m_imageIndex; }
      set { m_imageIndex = value; }
    }

    /// <summary>
    /// ImageIndex
    /// </summary>
    public int ImageCount
    {
      get { return m_imageCount; }
      set { m_imageCount = value; }
    }



    #region Thumbnail properties

    /// <summary>
    /// Sets/Gets the thumbnail image
    /// </summary>
    public System.Drawing.Image Thumbnail
    {     
      get { return m_renderer.RenderThumbnail((uint)m_thumbnailSize.Width, (uint)m_thumbnailSize.Height, WindowLevel); }

      //get { return Renderer.RenderThumbnail(m_image, (uint)m_thumbnailSize.Width, (uint)m_thumbnailSize.Height, WindowLevel); }
      set
      {
        //m_thumbNail = value;
        RaisePropertyChanged("Thumbnail");
      }
    } // Thumbnail



    /// <summary>
    /// Gets the thumbnail width
    /// </summary>
    public int  ThumbnailWidth
    {
      get { return (int)m_thumbnailSize.Width; }
    } // ThumbnailWidth

    /// <summary>
    /// Gets the thumbnail width
    /// </summary>
    public int ThumbnailHeight
    {
      get { return (int)m_thumbnailSize.Height; }
    } // ThumbnailWidth


    /// <summary>
    /// Gets the thumbnail width
    /// </summary>
    public System.Windows.Size ThumbnailSize
    {
      get { return m_thumbnailSize; }
      set { Set<System.Windows.Size>("ThumbnailSize", ref m_thumbnailSize, value); }
    } // ThumbnailWidth

 
    #endregion


    /// <summary>
    /// Sets/Gets the dicom header
    /// </summary>
    public HeaderViewModel Header
    {
      get { return m_header;  }
      set
      {
        m_header = value;
        RaisePropertyChanged("Header");

      }
    } // Header

    /// <summary>
    /// Gets the maximum zoom
    /// </summary>
    public double MaximumZoom
    {
      get { return m_maximumZoom; }
      set { Set<double>("MaximumZoom", ref m_maximumZoom, value);  }

    } // MaximumZoom

    /// <summary>
    /// Gets the minimum zoom
    /// </summary>
    public double MinimumZoom
    {
      get { return m_minimumZoom; }
      set { Set<double>("MinimumZoom", ref m_minimumZoom, value);  }
    } // MinimumZoom



    /// <summary>
    /// Sets/Gets the image uid
    /// </summary>
    public string Uid
    {
      get { return m_uid; }
      private set { Set<string>("Uid", ref m_uid, value);  }
    } // UID

    /// <summary>
    /// Sets/Gets the patient name
    /// </summary>
    public string PatientName
    {
      get { return m_patientName; }
      private set { Set<string>("PatientName", ref m_patientName, value);  }
    } // PatientName


    /// <summary>
    /// Sets/Gets the Zoom Factor
    /// </summary>
    public double Zoom
    {
      get { return m_zoom; }
      private set { Set<double>("Zoom", ref m_zoom, value);  }
    } // ZoomFactor

 

    /// <summary>
    /// Sets/Gets the pixel value
    /// </summary>
    public string PixelValue
    {
      get { return m_pixelValue; }
      private set { Set<string>("PixelValue", ref m_pixelValue, value);  }
    } // PixelValue


    /// <summary>
    /// Sets/Gets the pixel position
    /// </summary>
    public System.Windows.Point PixelPosition
    {
      get { return m_pixelPosition; }
      private set { Set<System.Windows.Point>("PixelPosition", ref m_pixelPosition, value); }
    } // PixelPosition


    /// <summary>
    /// Gets/sets whether the Image is selected
    /// </summary>
    public bool IsSelected
    {
      get { return m_isSelected; }
      set 
      { 
        Set<bool>("IsSelected", ref m_isSelected, value);
      }
    } // IsSelected

    /// <summary>
    /// Sets/Gets the bitmap source for the image
    /// </summary>
    public Bitmap Image
    {
      get { return ZoomPan(m_renderer.Render(WindowLevel, (float)m_zoom)); }
      set { RaisePropertyChanged("Image"); }
      //get { return ZoomPan(Viatronix.Dicom.Imaging.Renderer.RenderPixelData(m_image, WindowLevel, m_zoom)); }
      //set { RaisePropertyChanged("Image"); }
    } // Image

    /// <summary>
    /// Sets/Gets the window level
    /// </summary>
    public WindowLevel WindowLevel
    {

      get 
      { 
        if ( m_windowLevel == null )
          m_windowLevel = new WindowLevel((int)m_image.WindowCenter, (int)m_image.WindowWidth);

        return m_windowLevel;  
      }
      set
      {
        if (value != null)
        {
          m_windowLevel = value;

          RaisePropertyChanged("WindowLevel");

          RaisePropertyChanged("Thumbnail");

          RaisePropertyChanged("Image");
        }

       }

    } // WindowLevel

    #endregion

  } // class ImageViewModel


  #endregion

} // namespace Viatronix.Utilities.Dicom.Viewer.ViewModel
