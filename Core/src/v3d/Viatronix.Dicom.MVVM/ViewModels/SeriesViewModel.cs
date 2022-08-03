using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using Viatronix.Dicom;
using Viatronix.Dicom.Imaging;
using System.Drawing;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Reflection;
using System.Xml;
using System.Windows;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Helpers;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Messaging;
using GalaSoft.MvvmLight.Threading;
using Viatronix.UI.ViewModels;
using System.Windows.Data;



namespace Viatronix.Dicom.MVVM.ViewModels
{





  #region ImageLoadingEventArgs

  

  /// <summary>
  /// Implements the evenet args using in the Image Loading delegate
  /// </summary>
  public class ImageLoadingEventArgs : EventArgs
  {
    private uint m_frame = 0;

    private uint m_numberOfFames = 0;

    private string m_message = string.Empty;

    private string m_uid = string.Empty;

    public ImageLoadingEventArgs(string uid, uint frame, uint numberOfFrames, string message) : base()
    {
      m_uid = uid;
      m_frame = frame;
      m_numberOfFames = numberOfFrames;
      m_message = message;
    }

    public string SeriesUid
    {
      get { return m_uid; }
    }

    public string Message
    {
      get { return m_message; }
    }

    public uint Frame
    {
      get { return m_frame; }
    }

    public uint NumberOfFrames
    {
      get { return m_numberOfFames; }
    }

    
  };  // ImageLoadingEventArgs

  #endregion


  #region delegates

  /// <summary>
  /// Image loading event handle
  /// </summary>
  /// <param name="sender">Object firing the event</param>
  /// <param name="args">Arges</param>
  public delegate void ImageLoadingDelegate(object sender, ImageLoadingEventArgs args);

  #endregion




  public class SeriesViewModel : Viatronix.UI.ViewModels.ViewModelBase
  {

    #region Fields


    /// <summary>
    /// Selected flaf
    /// </summary>
    private bool m_isSelected = false;

 
    /// <summary>
    /// Windowing flag used for updating images
    /// </summary>
    private bool m_updatingWindowing = false;

    /// <summary>
    /// 
    /// </summary>
    private bool m_updatingSelectedImage = false;

    /// <summary>
    /// Busy flag
    /// </summary>
    private bool m_busy = false;


    /// <summary>
    /// Current point
    /// </summary>
    private System.Windows.Point m_currentPoint = new System.Windows.Point(0,0);

    /// <summary>
    /// Viewing image flag
    /// </summary>
    private bool m_isViewingImage = false;

    /// <summary>
    /// Mouse Wheel Command
    /// </summary>
    private RelayCommand<Viatronix.UI.Gestures.MouseWheelAction> m_mouseWheelCommand;
    private RelayCommand m_mouseWheelUpCommand;
    private RelayCommand m_mouseWheelDownCommand;

    /// <summary>
    /// Sort command
    /// </summary>
    private RelayCommand m_sortCommand;

    /// <summary>
    /// Page changed
    /// </summary>
    private RelayCommand<double>  m_changePageCommand;

    /// <summary>
    /// Mouse down command
    /// </summary>
    private RelayCommand<MouseEventArgs> m_mouseDownCommand;

    /// <summary>
    /// Mouse move command
    /// </summary>
    private RelayCommand<UIElement> m_mouseMoveCommand;

    /// <summary>
    /// Command to iew the selected image
    /// </summary>
    private RelayCommand<ImageViewModel> m_viewImageCommand;

    /// <summary>
    /// Double click resets
    /// </summary>
    private RelayCommand<MouseButtonEventArgs> m_doubleClickCommand;

    /// <summary>
    /// Collection of images (May be images or frames)
    /// </summary>
    private ObservableCollection<ImageViewModel> m_viewableImages = new ObservableCollection<ImageViewModel>();

    /// <summary>
    /// Actual images (these will always be images and no frames)
    /// </summary>
    private Dictionary<string, IImage> m_images = new Dictionary<string, IImage>();

    /// <summary>
    /// implied  collection of images
    /// </summary>
    private ImageCollection m_implied = ImageCollection.Create();

    /// <summary>
    /// sorted collection of images
    /// </summary>
    private ImageCollection m_sorted = null;

     /// <summary>
    /// Current selected image
    /// </summary>
    private ImageViewModel m_selectedImage;

    /// <summary>
    /// Current window level
    /// </summary>
    private WindowLevel m_windowLevel;

    /// <summary>
    /// Series Uid
    /// </summary>
    private string m_uid = string.Empty;

    /// <summary>
    /// Medical Id
    /// </summary>
    private string m_medicalId = string.Empty;

    /// <summary>
    /// Scan date
    /// </summary>
    private string m_scanDate = string.Empty;

    /// <summary>
    /// Patient Name
    /// </summary>
    private string m_patientName = string.Empty;

    /// <summary>
    /// Orientation
    /// </summary>
    private string m_orientation = string.Empty;

    /// <summary>
    /// Modality
    /// </summary>
    private string m_modality = string.Empty;

    /// <summary>
    /// Thumnbail for this dataset
    /// </summary>
    private System.Drawing.Image m_thumbnail = null;

    /// <summary>
    /// Lock collection
    /// </summary>
    private object m_lock = new object();

    /// <summary>
    /// Pages window
    /// </summary>
    private ObservableCollection<MultiImagePageViewModel> m_pages = new ObservableCollection<MultiImagePageViewModel>();


    /// <summary>
    /// Images per page
    /// </summary>
    private int m_viewableImagesPerPage = 4;

    /// <summary>
    /// Page numbers counter
    /// </summary>
    private int m_pageNumbers = 1;

    /// <summary>
    /// Either sorted or implied
    /// </summary>
    private bool m_isSorted = false;
  
    /// <summary>
    /// Current page
    /// </summary>
    private MultiImagePageViewModel m_currentPage = null;

    private MultiImagePageViewModel m_page = null;

    #endregion

    #region constructor/destructorw

    /// <summary>
    /// Constructor
    /// </summary>
    public SeriesViewModel()
    {

      BindingOperations.EnableCollectionSynchronization(m_viewableImages, m_lock);
      m_page = new MultiImagePageViewModel(m_pageNumbers++);
      m_pages.Add(m_page);
    } // SeriesViewModel()


     #endregion 


    #region events

    public event ImageLoadingDelegate ImageLoading;

    private void OnImageLoading(uint frame, uint numberOfFrames,string message)
    {
      if ( ImageLoading != null )
        ImageLoading(this, new ImageLoadingEventArgs(m_uid, frame, numberOfFrames,message) );
    }

    #endregion

    #region methods

    public void Sort()
    {
      if (!m_isSorted )
      {
        if ( m_sorted == null )
          m_sorted = m_implied.Sort();

        m_pageNumbers = 1;
        m_pages.Clear();
        m_viewableImages.Clear();
        m_page.Images.Clear();
        m_currentPage = null;
        m_page = new MultiImagePageViewModel(m_pageNumbers++);
        m_pages.Add(m_page);

        foreach (IImage image in m_sorted)
        {
          ImageViewModel model = ImageViewModel.Create(image);

          Add(model);

        }

        IsSorted = true;
      }
      else if (m_isSorted)
      {
        m_pageNumbers = 1;
        m_pages.Clear();
        m_viewableImages.Clear();
        m_page.Images.Clear();
        m_currentPage = null;
        m_page = new MultiImagePageViewModel(m_pageNumbers++);
        m_pages.Add(m_page);

        foreach (IImage image in m_implied)
        {

          ImageViewModel model = ImageViewModel.Create(image);

          Add(model);

        }
        IsViewingImage = false;
        IsSorted = false;
      }


    }

    /// <summary>
    /// Resets the series view model
    /// </summary>
    public void Reset()
    {
      if (m_viewableImages.Count > 0)
      {
        foreach (ImageViewModel image in m_viewableImages)
        {
          image.Reset();
        }

        IsViewingImage = false;
        SelectedImage = m_viewableImages[0];
      }

    }

    /// <summary>
    /// Adds image to list
    /// </summary>
    /// <param name="dicomFile">Dicom file to read image from</param>
    public void Load(IImage image)
    {
      string uid = image.InstanceUid;

      if (!m_images.ContainsKey(uid))
      {

        m_images.Add(uid, image);


        uint numberOfFames = image.NumberOfFrames;

        // ===================================================
        // If multi-framed then generate the images for each
        // ====================================================
        if (numberOfFames > 0)
        {
          OnImageLoading(0,numberOfFames,string.Format("Loading {0} ... Extracting frames ({1})", image.Filename, numberOfFames));

          ImageCollection  collections = image.GetFrames();

          uint frame = 1;

          foreach (IImage item in collections)
          {

            OnImageLoading(frame, numberOfFames, string.Format("Loading {0} ... Frame {1} of {2}", item.Filename, frame, numberOfFames));

            Add(ImageViewModel.Create(item));

            m_implied.Add(item);


            frame++;

          }

        }
        else
        {
         
          OnImageLoading(0,numberOfFames,string.Format("Loading {0}", image.Filename));

          Add(ImageViewModel.Create(image));

          m_implied.Add(image);

        }

      }



    } // Load(string dicomFile)



    /// <summary>
    /// Determines if the image is contained in the list
    /// </summary>
    /// <param name="instanceUid"></param>
    /// <returns></returns>
    public bool ContainsImage(string instanceUid)
    {

      return m_images.ContainsKey(instanceUid);
    } // ContainsImage

    public bool CanExecute()
    {

      return true;
    }

    /// <summary>
    /// Gets the hash code
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return base.GetHashCode();
    } // GetHashCode()

    /// <summary>
    /// Determines euqlaity between the object and the series view model
    /// </summary>
    /// <param name="obj"></param>
    /// <returns></returns>
    public override bool Equals(object obj)
    {
      if (obj is SeriesViewModel)
        return Equals((SeriesViewModel)obj);
      else
        return false;
    } //  Equals(object obj)

    /// <summary>
    /// Determines equality between the series using the UID
    /// </summary>
    /// <param name="series">Series to compare against</param>
    /// <returns>true is equal, false otherwise</returns>
    public bool Equals(SeriesViewModel series)
    {
      return (series.Uid == m_uid ? true : false);
    } // Equals(SeriesViewModel series)

    /// <summary>
    /// Creates a new series view model
    /// </summary>
    /// <param name="image">Dicom image</param>
    /// <returns>Newly created series view model</returns>
    public static SeriesViewModel Create(IImage image)
    {
      SeriesViewModel series = new SeriesViewModel();

      series.m_uid          = image.SeriesInstanceUid;
      series.m_patientName  = image.PatientName;
      series.m_modality     = image.Modality;
      series.m_medicalId    = image.MedicalId;
      series.m_scanDate     = image.StudyDate;

      List<double> imageOrientation = image.ImageOrientation;

      series.m_orientation = Viatronix.Dicom.Utility.GetImageOrientation(imageOrientation).ToString();

      return series;
    }   // Create(Image image)


    /// <summary>
    /// Adds imge to list and raises the ImageCount property
    /// </summary>
    /// <param name="image"></param>
    public void Add(ImageViewModel image)
    {

      // Set the series window level
      if (m_windowLevel != null)
        image.WindowLevel = m_windowLevel;

      image.PropertyChanged += this.OnImagePropertyChanged;

      // Add image to image list
      m_viewableImages.Add(image);

      /// Set the images index
      image.ImageIndex = m_viewableImages.Count;

      // ======================
      // Add image to pages
      // ======================
      if (m_page.Images.Count >= m_viewableImagesPerPage)
      {
        m_page = new MultiImagePageViewModel(m_pageNumbers++);
        m_pages.Add(m_page);
      }

      m_page.Images.Add(image);

      // =========================
      // Select the first image
      // ========================
      if (m_selectedImage == null)
      {
        this.m_currentPage = m_page;
        this.SelectedImage = image;
        this.Thumbnail = image.Thumbnail;
        this.m_windowLevel = image.WindowLevel;
      }

      Messenger.Default.Send<Int32>(m_viewableImages.Count, null);

    } // AddImage();

    /// <summary>
    /// Changes the selected image to the next image available in the list
    /// </summary>
    public void NextImage()
    {
      int index = m_selectedImage.ImageIndex;
      if (index < m_viewableImages.Count)
      {
        this.SelectedImage = m_viewableImages[index];

        if (m_windowLevel != null)
          this.SelectedImage.WindowLevel = m_windowLevel;

        int page = (int)Math.Round((double)((m_selectedImage.ImageIndex - 1) / m_viewableImagesPerPage));

        System.Diagnostics.Debug.WriteLine("Index: {0}   Page: {1}", m_selectedImage.ImageIndex, page);

        CurrentPage = m_pages[page];

        System.Diagnostics.Debug.WriteLine(string.Format("Next Image {0}", index));

      }

    } // NextImage()


    /// <summary>
    /// Changes the selected image to the previous image in the list
    /// </summary>
    public void PrevImage()
    {

      int index = m_selectedImage.ImageIndex - 2;
      if (index > -1)
      {

        this.SelectedImage = m_viewableImages[index];
        System.Diagnostics.Debug.WriteLine(string.Format("Prev Image {0}", index));

        if (m_windowLevel != null)
          this.SelectedImage.WindowLevel = m_windowLevel;

        int page = (int)Math.Round((double)((m_selectedImage.ImageIndex - 1) / m_viewableImagesPerPage));

        System.Diagnostics.Debug.WriteLine("Index: {0}   Page: {1}", m_selectedImage.ImageIndex, page);

        CurrentPage = m_pages[page];

      }

    } // PrevImage()



    #endregion


    #region Event Handling Methods

    /// <summary>
    /// Sort command toggle
    /// </summary>
    public RelayCommand SortCommand
    {
      get
      {
        if (m_sortCommand == null)
          m_sortCommand = new RelayCommand(() =>
          {
            Sort();
          }, () => true);

        return m_sortCommand;
      }
    } // DoubleClickCommand

    /// <summary>
    /// Double Click Command. This is executed when the mouse leaves the dicom image.
    /// </summary>
    public RelayCommand<MouseButtonEventArgs> DoubleClickCommand
    {
      get
      {
        if (m_doubleClickCommand == null)
          m_doubleClickCommand = new RelayCommand<MouseButtonEventArgs>(param =>
        {
          Reset();
        }, param => true);

        return m_doubleClickCommand;
      }
    } // DoubleClickCommand

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
            bool controlKeyDown = Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl);

            if (param.MiddleButton == MouseButtonState.Pressed && !controlKeyDown)
            {
              m_currentPoint = param.GetPosition((IInputElement)param.Source);

            }
          },
          param => true);

        return m_mouseDownCommand;
      }
    } // MouseDownCommand

    /// <summary>
    /// Mouse Move Handler
    /// </summary>
    /// <param name="control">Control</param>
    public void OnMouseMoveHandler(UIElement element)
    {


      if (m_isViewingImage && Mouse.MiddleButton == MouseButtonState.Pressed)
      {
        // Get the mouse point from the image panel
        System.Windows.Point point = Mouse.GetPosition(element);


        if (point.X != 0 && point.Y != 0)
        {
          // Calculate the delta of the mouse movement 
          System.Drawing.Point delta = new System.Drawing.Point((int)(point.X - m_currentPoint.X), (int)(point.Y - m_currentPoint.Y));

          // Save the new current point
          m_currentPoint = point;


          System.Diagnostics.Debug.WriteLine(String.Format("Current({0},{1})   Delta({2},{3})", m_currentPoint.X, m_currentPoint.Y, delta.X, delta.Y));

          if (delta.Y < 0)
            PrevImage();
          else if (delta.Y > 0)
            NextImage();

        }

      }


    } // OnMouseMoveHandler(UserControl control)


    void OnPreviewMouseWheelCommand(Viatronix.UI.Gestures.MouseWheelAction action)
    {
      if (m_isViewingImage)
      {

        if (action == Viatronix.UI.Gestures.MouseWheelAction.WheelDown)
          NextImage();
        else if (action == Viatronix.UI.Gestures.MouseWheelAction.WheelUp)
          PrevImage();
      }
    }

    /// <summary>
    /// Handler for the mouse wheel event
    /// </summary>
    /// <param name="sender">Sender</param>
    /// <param name="args">Mouse wheel event args</param>
    void OnMouseWheelHandler(object sender, MouseWheelEventArgs args)
    {
      if (m_isViewingImage)
      {
        System.Diagnostics.Debug.WriteLine(string.Format("Delta {0}  ", args.Delta));

        if (args.Delta < 0)
          NextImage();
        else if (args.Delta > 0)
          PrevImage();

      }
      args.Handled = true;
     
      
     }  // OnMouseWheelHandler(object sender, MouseWheelEventArgs args)



    /// <summary>
    /// Captures property changed events triggered in the ImageViewModel objects
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    void OnImagePropertyChanged(object sender, PropertyChangedEventArgs e)
    {

    
      if (e.PropertyName == "IsSelected")
      {

        if (!m_updatingSelectedImage)
        {

          ImageViewModel ivm = ((ImageViewModel)sender);

          if (ivm.IsSelected)
          {
            m_updatingSelectedImage = true;

            this.SelectedImage = ((ImageViewModel)sender);

            foreach (ImageViewModel image in m_viewableImages)
            {
              if (image.ImageIndex != m_selectedImage.ImageIndex)
                image.IsSelected = false;
            }

            m_updatingSelectedImage = false;
          }
        }
 
      }
      else if (e.PropertyName == "MouseDoubleClick")
      {
        OnToggleViewImageCommand(((ImageViewModel)sender));
      }

      // ====================================================================================================================
      // If the WindowLevel is changed in one of the images in the collection then set the window level for all the images
      // ====================================================================================================================
      else if (e.PropertyName == "WindowLevel")
      {


        if (!m_updatingWindowing)
        {
          m_updatingWindowing = true;

          /// Set the dataset level window level
          m_windowLevel = ((ImageViewModel)sender).WindowLevel;

          foreach (ImageViewModel image in m_viewableImages)
          {
            if (image.ImageIndex != m_selectedImage.ImageIndex)
              image.WindowLevel = m_windowLevel;
          }

          m_updatingWindowing = false;
        }


      }

    }

    /// <summary>
    /// Handles the image view command
    /// </summary>
    /// <param name="image"></param>
    void OnToggleViewImageCommand(ImageViewModel image)
    {
      SelectedImage = image;
      IsViewingImage = (image == null ? false : !m_isViewingImage);
    }


    #endregion // Event Handling Methods


    #region properties



    #region commands


    /// <summary>
    /// Mouse Leave Command. This is executed when the mouse leaves the dicom image.
    /// </summary>
    public RelayCommand<double> ChangePageCommand
    {
      get
      {
        if (m_changePageCommand == null)
          m_changePageCommand = new RelayCommand<double>(param =>
          {
           if (param >= 0 && param < m_pages.Count)
              CurrentPage = m_pages[(int)param];
          },
          param => true);

        return m_changePageCommand;
      }
      
    } // MouseWheelCommand

    /// <summary>
    /// Mouse Leave Command. This is executed when the mouse leaves the dicom image.
    /// </summary>
    public RelayCommand MouseWheelUpCommand
    {
      get
      {
        if (m_mouseWheelUpCommand == null)
          m_mouseWheelUpCommand = new RelayCommand(() => PrevImage(), () => true);

        return m_mouseWheelUpCommand;
      }
    } // MouseWheelCommand


    /// <summary>
    /// Mouse Leave Command. This is executed when the mouse leaves the dicom image.
    /// </summary>
    public RelayCommand MouseWheelDownCommand
    {
      get
      {
        if (m_mouseWheelDownCommand == null)
          m_mouseWheelDownCommand = new RelayCommand(() => NextImage(), () => true);

        return m_mouseWheelDownCommand;
      }
    } // MouseWheelCommand
      

    /// <summary>
    /// Mouse Leave Command. This is executed when the mouse leaves the dicom image.
    /// </summary>
    public RelayCommand<Viatronix.UI.Gestures.MouseWheelAction> MouseWheelCommand
    {
      get
      {
        if (m_mouseWheelCommand == null)
          m_mouseWheelCommand = new RelayCommand<Viatronix.UI.Gestures.MouseWheelAction>(param => OnPreviewMouseWheelCommand(param), param => true);

        return m_mouseWheelCommand;
      }
    } // MouseWheelCommand


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



    /// <summary>
    /// Mouse Leave Command. This is executed when the mouse leaves the dicom image.
    /// </summary>
    public RelayCommand<ImageViewModel> ToggleViewImageCommand
    {
      get
      {
        if (m_viewImageCommand == null)
          m_viewImageCommand = new RelayCommand<ImageViewModel>(param => OnToggleViewImageCommand(param), param => true);

        return m_viewImageCommand;
      }
    } // ToggleViewImageCommand

    #endregion

    public bool IsSorted
    {
      get { return m_isSorted; }
      set { Set<bool>("IsSorted", ref m_isSorted, value); }
    }

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
    /// Thumbnail
    /// </summary>
    public System.Drawing.Image Thumbnail
    {
      get { return m_thumbnail; }
      set { Set<System.Drawing.Image>("Thumbnail", ref m_thumbnail, value); }
    } // Thumnail

    /// <summary>
    /// Images
    /// </summary>
    public ObservableCollection<ImageViewModel> Images
    {
      get { return m_viewableImages; }
    }

    /// <summary>
    /// Images
    /// </summary>
    public ObservableCollection<MultiImagePageViewModel> Pages
    {
      get { return m_pages; }
    }


    /// <summary>
    /// Current page
    /// </summary>
    public MultiImagePageViewModel CurrentPage
    {
      get { return m_currentPage; }
      set 
      { 
        Set<MultiImagePageViewModel>("CurrentPage",ref m_currentPage, value);

        System.Diagnostics.Debug.WriteLine("Current Page: {0}", m_currentPage.PageNumber);
      }
    }

    /// <summary>
    /// Gets the series uid
    /// </summary>
    public string Uid
    {
      get { return m_uid; }
    } // Uid

    /// <summary>
    /// Sets/Gets the Modality
    /// </summary>
    public string Modality
    {
      get { return m_modality; }
     } // Modality

    /// <summary>
    /// Gets the patient name
    /// </summary>
    public string PatientName
    {
      get { return m_patientName; }
    } // PatientName

    /// <summary>
    /// Gets the scan date
    /// </summary>
    public string ScanDate
    {
      get { return m_scanDate; }
    } // ScanDate

    /// <summary>
    /// Gets the scan date
    /// </summary>
    public string MedicalId
    {
      get { return m_medicalId; }
    } // MedicalId


    /// <summary>
    /// Gets the patient name
    /// </summary>
    public string Orientation
    {
      get { return m_orientation; }
    } // Orientation

 
 
    /// <summary>
    /// Gets/Sets the selected image
    /// </summary>
    public ImageViewModel SelectedImage
    {
      get { return m_selectedImage; }
      set
      {
        m_selectedImage = value;
        RaisePropertyChanged("SelectedImage");


       }
    } // SelectedImage


    public bool IsViewingImage
    {
      get { return m_isViewingImage; }
      set 
      { 
        Set<bool>("IsViewingImage", ref m_isViewingImage, value);
      }
    }


    /// <summary>
    /// Sets/Gets the IsBusy flag
    /// </summary>
    public bool IsBusy
    {
      get { return m_busy; }

      set
      {
        m_busy = value;
        RaisePropertyChanged("IsBusy");
      }
    }

    #endregion

  } // class SeriesViewModel

}
