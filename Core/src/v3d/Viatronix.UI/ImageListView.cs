using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Reflection;
using System.Drawing;
using System.Drawing.Text;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace Viatronix.UI
{


  #region enumerators

  /// <summary>
  /// Scroll direction
  /// </summary>
  public enum ScrollDirections
  {
    LeftRight,
    UpDown
  };  // ScollDirection

  /// <summary>
  /// Paging Type
  /// </summary>
  public enum PagingTypes
  {
    SingleImage,
    FullPage
  };  // PagingTypes


  public enum Styles
  {
    Standard,
    Rounded,
    Square
  };

  #endregion


  /// <summary>
  /// ImageListView control displays a list of images
  /// </summary>
  public partial class ImageListView : UserControl
  {

    #region ListViewImage class

    /// <summary>
    /// Object to wrap the image and the cpation together
    /// </summary>
    public class ListViewImage
    {

      #region fields


      /// <summary>
      /// Tag object 
      /// </summary>
      private object m_tag = null;

      /// <summary>
      /// Image to be displayed
      /// </summary>
      private Image m_image = null;


      /// <summary>
      /// Caption for the image
      /// </summary>
      private string m_caption = string.Empty;

      #endregion


      #region constructor

      /// <summary>
      /// Constructor for the dispay image
      /// </summary>
      /// <param name="image">Image to be displayed</param>
      /// <param name="caption">Caption for the image</param>
      /// <param name="tag">tag object</param>
      public ListViewImage(Image image, string caption, object tag)
      {
        m_image = image;
        m_caption = caption;
        m_tag = tag;
      } // ListViewImage(Image image, string caption, object tag)

      /// <summary>
      /// Constructor for the dispay image
      /// </summary>
      /// <param name="image">Image to be displayed</param>
      /// <param name="caption">Caption for the image</param>
      public ListViewImage(Image image, string caption)
      {
        m_image = image;
        m_caption = caption;
      } // ListViewImage(Image image, string caption)


      #endregion


      #region properties

      /// <summary>
      /// Sets/Gets the tag object
      /// </summary>
      public object Tag
      {
        get { return m_tag; }
        set { m_tag = value; }
      } // Tag

      /// <summary>
      /// Gets the Image to be displayed
      /// </summary>
      public Image Image
      {
        get { return m_image; }
      } // Image


      /// <summary>
      /// Gets the images caption
      /// </summary>
      public string Caption
      {
        get { return m_caption; }
      } // Caption 

      #endregion


    };  // class ListViewImage

    #endregion


    #region DisplayedImage class

    /// <summary>
    /// Internal class used to wrap the image with its location in the control. Identifies that 
    /// the image is visible.
    /// </summary>
    private class DisplayedImage
    {

      #region fields

      /// <summary>
      /// Index of image in the list
      /// </summary>
      private int m_index = 0;

      /// <summary>
      /// Display image
      /// </summary>
      private ListViewImage m_listViewImage = null;

      /// <summary>
      /// Main rectangle
      /// </summary>
      private Rectangle m_rect;

      /// <summary>
      /// Image rectang;e
      /// </summary>
      private Rectangle m_imageRect;

      #endregion

      #region constructors

      /// <summary>
      /// Constructor or the displayed image object
      /// </summary>
      /// <param name="displayImage">Display image object being displayed</param>
      /// <param name="rect">Main rectanngle</param>
      /// <param name="index">Image index in the main list</param>
      public DisplayedImage(ListViewImage listViewImage, Rectangle rect, int index)
      {
        int pointOffset = 3;
        int sizeOffset = pointOffset * 2;

        m_listViewImage = listViewImage;
        m_rect  = rect;
        m_index = index;
        m_imageRect = new Rectangle(rect.X + pointOffset, rect.Y + pointOffset, rect.Width - sizeOffset, rect.Height - sizeOffset);
      } // DisplayedImage(ListViewImage displayImage, Rectangle rect, int index)

      #endregion

      #region properties

      /// <summary>
      /// Gets the image number
      /// </summary>
      public int ImageNumber
      {
        get { return m_index + 1; }
      } // ImageNumber

      /// <summary>
      /// Gets the ListViewImage image
      /// </summary>
      public ListViewImage ListViewImage
      {
        get { return m_listViewImage; }

      } // ListViewImage

      /// <summary>
      /// Gets the image rectangle
      /// </summary>
      public Rectangle ImageRectangle
      {
        get { return m_imageRect; }

      } // ImageRectangle

      /// <summary>
      /// Gets the main retangle
      /// </summary>
      public Rectangle Rectangle
      {
        get { return m_rect; }

      } // Rectangle

      /// <summary>
      /// Gets hge images index
      /// </summary>
      public int Index
      {
        get { return m_index; }
      } // Index

      #endregion


    };  // class DiplayedImage

    #endregion



    #region structures


    /// <summary>
    /// Image Display Info structure contains the 
    /// max images and the spacing.
    /// </summary>
    private struct RenderInfo
    {
      public RenderInfo(int maxImages, int imageSize, int spacing)
      {
        MaxImages = maxImages;
        ImageSize = imageSize;
        Spacing = spacing;

      }

      public int MaxImages;
      public int ImageSize;
      public int Spacing;
    };  // struct RenderInfo

    #endregion


    #region constants

    /// <summary>
    /// Offset of image that is displayed on button
    /// </summary>
    private const int m_buttonImageOffset = 4;

    #endregion

    #region fields

    /// <summary>
    /// Current image
    /// </summary>
    private DisplayedImage m_dragImage = null;

    /// <summary>
    /// List View Image being dragged
    /// </summary>
    private int m_dragIndex = -1;

    /// <summary>
    /// Marhgin Size
    /// </summary>
    private int m_marginSize = 4;

    /// <summary>
    /// Redraw button flag used when selecting and deselecting the panl buttons
    /// </summary>
    private bool m_redrawButton = true;

    /// <summary>
    /// Selected Button used for the Mouse event in the panel
    /// </summary>
    private string m_selectedButton = string.Empty;

    /// <summary>
    /// Button image size
    /// </summary>
    private Size m_buttonImageSize;

    /// <summary>
    /// Bitmap images
    /// </summary>
    private Dictionary<ScrollDirections, Dictionary<string, List<Bitmap>>> m_buttonImages = new Dictionary<ScrollDirections, Dictionary<string, List<Bitmap>>>();

    /// <summary>
    /// Assembly
    /// </summary>
    private Assembly m_assembly = Assembly.GetExecutingAssembly();

    /// <summary>
    /// Standar style
    /// </summary>
    private Styles m_style = Styles.Standard;

    /// <summary>
    /// Radious used to round corners
    /// </summary>
    private float m_radius = 10.0F;

    /// <summary>
    /// Color to be used for the box
    /// </summary>
    private Color m_imageBorderColor = Color.Gray;

    /// <summary>
    /// Flag to indicate that the image is to be boxed
    /// </summary>
    private bool m_enableImageBorder = false;

    /// <summary>
    /// Enable image selection
    /// </summary>
    private bool m_enableImageSelection = false;

    /// <summary>
    /// Caption spacing
    /// </summary>
    private int m_captionSpacing = 3;

    /// <summary>
    /// Display caption flag
    /// </summary>
    private bool m_displayCaption = true;


    /// <summary>
    /// Button image visible flag
    /// </summary>
    private bool m_buttonImageVisible = true;

    /// <summary>
    /// Display image number flag
    /// </summary>
    private bool m_displayImageNumber = true;

    /// <summary>
    /// Number of images per page 
    /// </summary>
    private int m_imagesPerPage = 0;

    /// <summary>
    /// Multiple rows flag
    /// </summary>
    private bool m_multipleRows = false;

    /// <summary>
    /// Minimum image size used in conjunction with the multiple rows
    /// </summary>
    private int m_minimumImageSize = -1;

    /// <summary>
    /// Selected index
    /// </summary>
    private int m_selectedIndex = -1;

    /// <summary>
    /// Paging type
    /// </summary>
    private PagingTypes m_pagingType = PagingTypes.SingleImage;

    /// <summary>
    /// Image list
    /// </summary>
    private List<ListViewImage> m_imageList = new List<ListViewImage>();

    /// <summary>
    /// Starting index used for rendering
    /// </summary>
    private int m_startingIndex = 0;

    /// <summary>
    /// Ending index 
    /// </summary>
    private int m_endingIndex = 0;

    /// <summary>
    /// Button size either width or height depending on the scoll direction
    /// </summary>
    private int m_buttonSize = 20;

    /// <summary>
    /// Minimum  spacing used between images
    /// </summary>
    private int m_minimumSpacing = 20;

    /// <summary>
    /// Scroll direction
    /// </summary>
    private ScrollDirections m_scrollDirection = ScrollDirections.LeftRight;

    /// <summary>
    /// Displayed image list
    /// </summary>
    private List<DisplayedImage> m_displayedImages = new List<DisplayedImage>();


    #endregion


    #region constructors

    /// <summary>
    /// Constructor
    /// </summary>
    public ImageListView()
    {
      SetStyle(ControlStyles.UserPaint, true);
      SetStyle(ControlStyles.AllPaintingInWmPaint, true);
      SetStyle(ControlStyles.DoubleBuffer, true);
      SetStyle(ControlStyles.ResizeRedraw, true);

      InitializeComponent();


      m_buttonPrev.Enabled  = false;
      m_buttonNext.Enabled  = false;
      m_panelNext.Enabled   = false;
      m_panelPrev.Enabled   = false; 

      m_buttonPrev.Width = m_buttonSize;
      m_buttonPrev.Dock = DockStyle.Left;

      m_panelPrev.Width = m_buttonSize;
      m_panelPrev.Dock = DockStyle.None;

      m_buttonNext.Width = m_buttonSize;
      m_buttonNext.Dock = DockStyle.Right;


      m_panelNext.Width = m_buttonSize;
      m_panelNext.Dock = DockStyle.None;


      // Create the button image list object so that we can read in all the images at the start
      m_buttonImageSize = new Size(m_buttonSize - m_buttonImageOffset, m_buttonSize - m_buttonImageOffset);


      // Left/Right button list
      Dictionary<string, List<Bitmap>> leftRightBitmaps = new Dictionary<string,List<Bitmap>>();
   
      // Left/Right ... Next button list
      List<Bitmap> nextBitmapList = new List<Bitmap>();

      // Right Arrow Image
      nextBitmapList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.rightarrow.png")), m_buttonImageSize));

      // Disabled Right Arrow Image
      nextBitmapList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.disabled_rightarrow.png")), m_buttonImageSize));

      // Selected Right Arrow Image
      nextBitmapList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.selected_rightarrow.png")), m_buttonImageSize));

      // Add the Next button image list to the Left/Right list
      leftRightBitmaps.Add("NEXT", nextBitmapList);


      // Left/Right ... Prev button list
      List<Bitmap> prevBitmapsList = new List<Bitmap>();

      // Left Arrow Image
      prevBitmapsList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.leftarrow.png")), m_buttonImageSize));

      // Disable Left Arrow Image
      prevBitmapsList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.disabled_leftarrow.png")), m_buttonImageSize));

      // Selected Left Arror Image
      prevBitmapsList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.selected_leftarrow.png")), m_buttonImageSize));

      // Add the Prev button image list to the Left/Right list
      leftRightBitmaps.Add("PREV", prevBitmapsList);

     
      // Since the Left/Right button mage list is complete we an add it to the main list which is mapped using the ScrollDirection
      m_buttonImages.Add(ScrollDirections.LeftRight, leftRightBitmaps);
      

      // Create the Up/Down image list
      Dictionary<string, List<Bitmap>> upDownBitmaps = new Dictionary<string, List<Bitmap>>();

      nextBitmapList = new List<Bitmap>();

      nextBitmapList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.downarrow.png")), m_buttonImageSize));
      nextBitmapList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.disabled_downarrow.png")), m_buttonImageSize));
      nextBitmapList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.selected_downarrow.png")), m_buttonImageSize));

      // Add NEXT button images
      upDownBitmaps.Add("NEXT", nextBitmapList);


      prevBitmapsList = new List<Bitmap>();

      prevBitmapsList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.uparrow.png")), m_buttonImageSize));
      prevBitmapsList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.disabled_uparrow.png")), m_buttonImageSize));
      prevBitmapsList.Add(new Bitmap(new Bitmap(m_assembly.GetManifestResourceStream("Viatronix.UI.res.selected_uparrow.png")), m_buttonImageSize));

      // Add PREV button images
      upDownBitmaps.Add("PREV", prevBitmapsList);

      m_buttonImages.Add(ScrollDirections.UpDown, upDownBitmaps);


      m_buttonNext.Image = m_buttonImages[m_scrollDirection]["NEXT"][0];
      m_buttonNext.ImageAlign = ContentAlignment.MiddleRight;

      m_buttonPrev.Image = m_buttonImages[m_scrollDirection]["NEXT"][0];
      m_buttonPrev.ImageAlign = ContentAlignment.MiddleRight;


    } // ImageListView()


    #endregion


    #region methods

    /// <summary>
    /// Clears all images
    /// </summary>
    public void Clear()
    {
      m_imageList.Clear();
      m_startingIndex = 0;
      m_endingIndex = 0;
      this.Invalidate();
    }   // Clear()

    /// <summary>
    /// Gets the fonts 
    /// </summary>
    /// <returns></returns>
    private Font GetFont()
    {
      Font font = this.Font;

      return font;
    } // GetFont()


    /// <summary>
    /// Enables/Disabled the Next button
    /// </summary>
    /// <param name="enable">Enable flag</param>
    private void EnableNextButton(bool enable)
    {

      /// Do nothing if we are already at the setting specified
      if (m_buttonNext.Enabled == enable) return;

      m_buttonNext.Enabled  = enable;
      m_panelNext.Enabled   = enable;


      Size size = new Size(m_buttonSize - m_buttonImageOffset, m_buttonSize - m_buttonImageOffset);

      m_buttonNext.Image = m_buttonImages[m_scrollDirection]["NEXT"][(enable ? 0 : 1)];

    } // EnableNextButton(bool enable)

    /// <summary>
    /// Enables/Disabled the Prev button
    /// </summary>
    /// <param name="enable">Enable flag</param>
    private void EnablePrevButton(bool enable)
    {

      /// Do nothing if we are already at the setting specified
      if (m_buttonPrev.Enabled == enable) return;

      m_buttonPrev.Enabled  = enable;
      m_panelPrev.Enabled   = enable;

      m_buttonPrev.Image = m_buttonImages[m_scrollDirection]["PREV"][(enable ? 0 : 1)];

      // Force a redraw
      m_panelPrev.Invalidate();


    } // EnableNextButton(bool enable)

    /// <summary>
    /// Gets the render informataion
    /// </summary>
    /// <param name="maxSizeAllowed">Maximum size allowed for the images</param>
    /// <param name="imageSize">Image size to use for calcs</param>
    /// <param name="captionSize">caption size</param>
    /// <returns>Render information includoing maximum images and spacing</returns>
    private RenderInfo GetRenderInfo(int maxSizeAllowed, int imageSize, int captionSize)
    {

      // Protect against a image size of 0
      int imageSizeWithCaption = (imageSize == 0 ? 1 : imageSize) + captionSize;

      int adjustedImageSize = (imageSizeWithCaption >= maxSizeAllowed ? (maxSizeAllowed - captionSize) : imageSize);

      int size = Math.Min(maxSizeAllowed, imageSizeWithCaption);

      // Calculate the max images that can be displayed in the allowable space
      int maxImages = maxSizeAllowed / size;

      // Calcumate the total size the image will take up
      int totalSize = Math.Max(maxImages * size, 1);

      /// Total space available
      int totalSpaceAvailable = (maxSizeAllowed - totalSize);

      // Calculate the spacing available with the remaining room
      int spacing = (maxSizeAllowed - totalSize) / (maxImages + 1);

      //=======================================================================
      // Take into account the special condition when we have only one image
      // and the spacing is less then the minimum.
      //=======================================================================
      if (maxImages == 1 && spacing < m_minimumSpacing)
      {
        int difference = m_minimumSpacing - spacing;
        spacing = m_minimumSpacing;
        adjustedImageSize -= (difference * 2);
      }
      else
      {

        //=============================================================================================
        // Adjust sapcing so that we at least have the minimum set by the control. This may include 
        // adjusting the number of images to display in a page.
        //==============================================================================================
        while (maxImages > 1 && spacing < m_minimumSpacing)
        {
          // Display one less image in the list
          --maxImages;

          // Calculate the total size necessary for all images
          totalSize = maxImages * size;

          /// Total space available
          totalSpaceAvailable = (maxSizeAllowed - totalSize);

          // Calculate the new spacing with the extra room afforded to us by removing an image from the page
          spacing = totalSpaceAvailable / (maxImages + 1);

        } // END ... While the spacing is less then the minimum set

      }


      return new RenderInfo(maxImages, adjustedImageSize, spacing);

    } // GetRenderInfo(int maxSizeAllowed, int imageSize)




    /// <summary>
    /// Gets the render informataion
    /// </summary>
    /// <param name="maxSizeAllowed">Maximum size allowed for the images</param>
    /// <param name="imageSize">Image size to use for calcs</param>
    /// <param name="captionSize">caption size</param>
    /// <returns>Render information includoing maximum images and spacing</returns>
    private RenderInfo GetVerticalRenderInfo(Size drawingArea, Size imageSize)
    {

      // Account for a drawing width < the initial image width 
      imageSize.Width = Math.Min(imageSize.Width, drawingArea.Width);

      int maxImages = Math.Min((drawingArea.Width / imageSize.Width), m_imageList.Count);
      int spacing = (drawingArea.Width - (maxImages * imageSize.Width)) / (maxImages + 1);

      if (maxImages == 1 && spacing < m_minimumSpacing)
      {
        int difference = m_minimumSpacing - spacing;
        spacing = m_minimumSpacing;
        imageSize.Width -= (difference * 2);
      }
      else
      {
        while ( spacing < m_minimumSpacing )
        {
          --maxImages;
          spacing = (drawingArea.Width - (maxImages * imageSize.Width)) / (maxImages + 1);
        }
      }


      return new RenderInfo(maxImages, 0, spacing);

    } // GetRenderInfo(int maxSizeAllowed, int imageSize)


    /// <summary>
    /// Gets the render informataion
    /// </summary>
    /// <param name="maxSizeAllowed">Maximum size allowed for the images</param>
    /// <param name="imageSize">Image size to use for calcs</param>
    /// <param name="captionSize">caption size</param>
    /// <returns>Render information includoing maximum images and spacing</returns>
    private RenderInfo GetHorizontalRenderInfo(Size drawingArea, Size imageSize)
    {

      // Account for a drawing width < the initial image width 
      imageSize.Height = Math.Min(imageSize.Height, drawingArea.Height);

      int maxImages = drawingArea.Height / imageSize.Height;
      int totalSpace = (drawingArea.Height - (maxImages * imageSize.Height));
      int spacing = (totalSpace / (maxImages + 1));

      if (maxImages == 1 && spacing < m_minimumSpacing)
      {
        int difference = m_minimumSpacing - spacing;
        spacing = m_minimumSpacing;
        imageSize.Height -= (difference * 2);
      }
      else
      {
        while (spacing < m_minimumSpacing)
        {
          --maxImages;
          totalSpace = (drawingArea.Height - (maxImages * imageSize.Height));
          spacing = totalSpace / (maxImages + 1);
        }
      }


      return new RenderInfo(maxImages, 0, spacing);

    } // GetRenderInfo(int maxSizeAllowed, int imageSize)


    /// <summary>
    /// Allows the selected image to be deselected
    /// </summary>
    public void DeselectImages()
    {
      m_selectedIndex = -1;
      Invalidate();
    } // DeselectImages()

    #endregion


    #region propeties

    /// <summary>
    /// Gets the radius used to draw the rounded edges
    /// </summary>
    public float Radius
    {
      get { return m_radius; }
    } // Radius

    /// <summary>
    /// Sets/Gets the style
    /// </summary>
    public Styles Style
    {
      get { return m_style; }
      set
      {
        if (m_style != value)
        {
          m_style = value;

          if (m_style == Styles.Standard)
          {

            m_panelNext.Visible = false;
            m_panelPrev.Visible = false;
            m_panelPrev.Dock    = DockStyle.None;
            m_panelNext.Dock    = DockStyle.None;

            m_buttonPrev.Enabled  = m_panelPrev.Enabled;
            m_buttonPrev.Width    = m_buttonSize;
            m_buttonPrev.Dock     = ( m_scrollDirection == ScrollDirections.LeftRight ? DockStyle.Left : DockStyle.Top );
            m_buttonPrev.Visible  = true;

            m_buttonNext.Enabled  = m_panelNext.Enabled;
            m_buttonNext.Width    = m_buttonSize;
            m_buttonNext.Dock     = (m_scrollDirection == ScrollDirections.LeftRight ? DockStyle.Right : DockStyle.Bottom);
            m_buttonNext.Visible  = true;



          }
          else
          {
            m_buttonNext.Visible  = false;
            m_buttonPrev.Visible  = false;
            m_buttonPrev.Dock     = DockStyle.None;
            m_buttonNext.Dock     = DockStyle.None;

            m_panelPrev.Enabled   = m_buttonPrev.Enabled;
            m_panelPrev.Width     = m_buttonSize;
            m_panelPrev.Dock      = (m_scrollDirection == ScrollDirections.LeftRight ? DockStyle.Left : DockStyle.Top);
            m_panelPrev.Visible   = true;


            m_panelNext.Enabled   = m_buttonNext.Enabled;
            m_panelNext.Width     = m_buttonSize;
            m_panelNext.Dock      = (m_scrollDirection == ScrollDirections.LeftRight ? DockStyle.Right : DockStyle.Bottom);
            m_panelNext.Visible   = true;


          }

          if (!this.DesignMode)
            Invalidate();
        }
       
      }

    } // Style

    /// <summary>
    /// Sets/Gets Image Outline Color
    /// </summary>
    public Color ImageBorderColor
    {
      get { return m_imageBorderColor; }
      set 
      { 
        m_imageBorderColor = value; 
        if (!this.DesignMode)
          Invalidate();
      }

    } // ImageBorderColor

    /// <summary>
    /// Flag to indicate that the images to be outlined with a box
    /// </summary>
    public bool EnableImageBorder
    {
      get { return m_enableImageBorder; }
      set 
      { 
        m_enableImageBorder = value;
        if (!this.DesignMode)
          Invalidate();
      }
    } // EnableImageBorder

    /// <summary>
    /// Sets/Gets the flag indicating if image selection is implemented
    /// </summary>
    public bool EnableImageSelection
    {
      get { return m_enableImageSelection; }
      set { m_enableImageSelection = value; }
    } // EnableImageSelection

    /// <summary>
    /// Gets the flag indicating that a caption of some sort is to be displayed
    /// </summary>
    private bool IsCaptionsDisplayable
    {
      get { return m_displayCaption || m_displayImageNumber; }
    } // IsCaptionsDisplayable

    /// <summary>
    /// Sets/Gets the caption spacing
    /// </summary>
    public int CaptionSpacing
    {
      get { return m_captionSpacing; }
      set 
      { 
        m_captionSpacing = value;
        if (!this.DesignMode) Refresh();

      }
    } // CaptionSpacing


    /// <summary>
    /// Gets the Image list so that images can be added
    /// </summary>
    public List<ListViewImage> Images
    {
      get { return m_imageList; }
    } // Images

    /// <summary>
    /// Sets/Gets the Multiple rows flag
    /// </summary>
    public bool MultipleRows
    {
      get { return m_multipleRows; }
      set 
      { 
        m_multipleRows = value;
        if (!this.DesignMode) Refresh();
      }
    } // MultipleRows

    /// <summary>
    /// Gets/Sets the Minimum Image Size used in conjunction with the multiple rows flag
    /// </summary>
    public int MinimumImageSize
    {
      get { return m_minimumImageSize; }
      set 
      { 
        m_minimumImageSize = value;
        if (!this.DesignMode) Refresh();

      }
    }  // MinimumImageSize


    /// <summary>
    /// Sets/Gets the display caption flag
    /// </summary>
    public bool DisplayCaptions
    {
      get { return m_displayCaption; }
      set
      {
        m_displayCaption = value;
        if (!this.DesignMode) Refresh();
      }
    } // DisplayCaption

    /// <summary>
    /// Sets/Gets the display image number flag
    /// </summary>
    public bool DisplayImageNumbers
    {
      get { return m_displayImageNumber; }
      set
      {
        m_displayImageNumber = value;
        if (!this.DesignMode) Refresh();
      }
    } // DiplayImageNumber

    /// <summary>
    /// Gets the selected image
    /// </summary>
    public ListViewImage SelectedImage
    {
      get { return (m_selectedIndex == -1 ? null : m_imageList[m_selectedIndex]); }
    }  // SelectedImage

    /// <summary>
    /// Gets/Sets the paging type
    /// </summary>
    public PagingTypes PagingType
    {
      get { return m_pagingType; }
      set 
      { 
        m_pagingType = value;
        if (!this.DesignMode) Invalidate();
      }
    } //  PagingType

    /// <summary>
    /// Gets/Sets the button size
    /// </summary>
    public int ButtonSize
    {
      get { return m_buttonSize; }
      set
      {
        m_buttonSize = value;

        if (m_scrollDirection == ScrollDirections.LeftRight)
        {
          m_buttonPrev.Width = m_buttonSize;
          m_buttonNext.Width = m_buttonSize;
        }
        else
        {
          m_buttonPrev.Height = m_buttonSize;
          m_buttonNext.Height = m_buttonSize;
        }

        if (!this.DesignMode) Invalidate();
      }
    } // ButtonSize

    /// <summary>
    /// Gets/Sets the minimum spacing
    /// </summary>
    public int MinimumSpacing
    {
      get { return m_minimumSpacing; }
      set { m_minimumSpacing = value; }
    } // MinimumSpacing

    /// <summary>
    /// Gets/Sets the scroll direction
    /// </summary>
    public ScrollDirections ScrollDirection
    {
      get { return m_scrollDirection; }
      set
      {
        m_scrollDirection = value;

        Assembly assembly = Assembly.GetExecutingAssembly();

      
        if (m_scrollDirection == ScrollDirections.LeftRight)
        {
          m_buttonPrev.Dock = (m_style == Styles.Standard ? DockStyle.Left : DockStyle.None);
          m_buttonNext.Dock = (m_style == Styles.Standard ? DockStyle.Right : DockStyle.None);
          m_panelPrev.Dock = (m_style != Styles.Standard ? DockStyle.Left : DockStyle.None);
          m_panelNext.Dock = (m_style != Styles.Standard ? DockStyle.Right : DockStyle.None);

          m_buttonPrev.Width  = m_buttonSize;
          m_buttonNext.Width  = m_buttonSize;
          m_panelNext.Width   = m_buttonSize;
          m_panelPrev.Width   = m_buttonSize;

          m_buttonNext.ImageAlign = ContentAlignment.MiddleRight;
          m_buttonPrev.ImageAlign = ContentAlignment.MiddleRight;

        }
        else
        {
          m_buttonPrev.Dock = (m_style == Styles.Standard ? DockStyle.Top : DockStyle.None);
          m_buttonNext.Dock = (m_style == Styles.Standard ? DockStyle.Bottom : DockStyle.None);
          m_panelPrev.Dock  = (m_style != Styles.Standard ? DockStyle.Top : DockStyle.None);
          m_panelNext.Dock  = (m_style != Styles.Standard ? DockStyle.Bottom : DockStyle.None);

          m_buttonPrev.Height = m_buttonSize;
          m_buttonNext.Height = m_buttonSize;
          m_panelNext.Height  = m_buttonSize;
          m_panelPrev.Height  = m_buttonSize;

          m_buttonNext.ImageAlign = ContentAlignment.TopCenter;
          m_buttonPrev.ImageAlign = ContentAlignment.TopCenter;


        }

        m_buttonNext.Image = m_buttonImages[m_scrollDirection]["NEXT"][(m_buttonNext.Enabled ? 0 : 1)];
        m_buttonPrev.Image = m_buttonImages[m_scrollDirection]["PREV"][(m_buttonPrev.Enabled ? 0 : 1)];


        if (! this.DesignMode)
          Invalidate();
      }
    } // ScrollDirection

    #endregion


    #region event handlers

    /// <summary>
    /// Paint handler for the Next panel control
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnNextPaint(object sender, PaintEventArgs e)
    {

      Graphics gfx = e.Graphics;

      gfx.SmoothingMode = SmoothingMode.HighQuality;
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;
      if (this.BackColor != Color.Transparent) gfx.Clear(this.BackColor);

      System.Windows.Forms.Panel panel = (System.Windows.Forms.Panel)sender;

      // Redraw button
      if (m_redrawButton && m_style == Styles.Rounded )
        Viatronix.UI.Utilities.DrawRectangleWithRoundedEdges(gfx, ((System.Windows.Forms.Panel)sender).ClientRectangle, m_radius, false, false, BackColor, BackColor);

      // Only draw the button image if visible
      if (m_buttonImageVisible)
        gfx.DrawImage(m_buttonImages[m_scrollDirection]["NEXT"][(panel.Enabled ? ( m_selectedButton == "NEXT" ? 2 : 0 ) : 1)], 
                        new Point((panel.Width / 2) - (m_buttonImageSize.Width / 2), (panel.Height / 2) - (m_buttonImageSize.Height / 2)));

      m_redrawButton = true;

    } // OnNextPaint(object sender, PaintEventArgs e)

    /// <summary>
    /// Painst the previsou button with the appropriate image
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnPrevPaint(object sender, PaintEventArgs e)
    {
      Graphics gfx = e.Graphics;

      gfx.SmoothingMode     = SmoothingMode.HighQuality;

      if (this.BackColor != Color.Transparent) gfx.Clear(this.BackColor);

      System.Windows.Forms.Panel panel = (System.Windows.Forms.Panel)sender;

      // Draw button
      if (m_redrawButton && m_style == Styles.Rounded)
        Viatronix.UI.Utilities.DrawRectangleWithRoundedEdges(gfx, ((System.Windows.Forms.Panel)sender).ClientRectangle, m_radius, false, false, BackColor, BackColor);

      // Only draw the button image if visible
      if ( m_buttonImageVisible )
        gfx.DrawImage(m_buttonImages[m_scrollDirection]["PREV"][(panel.Enabled ? ( m_selectedButton == "PREV" ? 2 : 0 ) : 1)], 
                        new Point((panel.Width / 2) - (m_buttonImageSize.Width / 2), (panel.Height / 2) - (m_buttonImageSize.Height / 2)));

      m_redrawButton = true;
    } // OnPrevPaint(object sender, PaintEventArgs e)


    /// <summary>
    /// OnResize event handler
    /// </summary>
    /// <param name="sender">Snding object</param>
    /// <param name="e">Enevt args</param>
    protected override void OnResize( EventArgs e)
    {
      base.OnResize(e);
      Invalidate();
    } // OnResize(object sender, EventArgs e)

    /// <summary>
    /// On Previous click handler
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event args</param>
    private void OnPrevClick(object sender, EventArgs e)
    {
      if (m_startingIndex > 0)
      {

        if (m_pagingType == PagingTypes.SingleImage)
        {
          --m_startingIndex;
        }
        else
        {
          m_startingIndex -= m_imagesPerPage;
          if (m_startingIndex < 0) m_startingIndex = 0;
        }

        EnablePrevButton(m_startingIndex == 0);

        Invalidate();

      }
    } // OnPrevClick(object sender, EventArgs e)

    /// <summary>
    /// On next button click handler
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Event parameters</param>
    private void OnNextClick(object sender, EventArgs e)
    {
      if (m_endingIndex < m_imageList.Count)
      {

        int lastIndex = m_imageList.Count - 1;

        if (m_pagingType == PagingTypes.SingleImage)
        {
          ++m_startingIndex;
          ++m_endingIndex;
        }
        else
        {
          m_imagesPerPage = (m_endingIndex - m_startingIndex);
          m_startingIndex += m_imagesPerPage;
          m_endingIndex += m_imagesPerPage;
        }

        if (m_endingIndex >= lastIndex)
        {
          EnableNextButton(false);
          m_endingIndex = lastIndex;
        }
        else
        {
          EnableNextButton(true);
        }

        Invalidate();

      }
    } // OnNextClick(object sender, EventArgs e)


    /// <summary>
    /// Overrides the OnPaint event so that the image viewer can be displayed properly
    /// </summary>
    /// <param name="sender">Sending object</param>
    /// <param name="e">Paint event args</param>
    protected override void OnPaint(PaintEventArgs e)
    {
      base.OnPaint(e);

      if (this.DesignMode) return;

      // Graphice
      Graphics gfx = e.Graphics;

      gfx.SmoothingMode     = SmoothingMode.HighQuality;
      gfx.TextRenderingHint = TextRenderingHint.ClearTypeGridFit;

      if (this.BackColor != Color.Transparent) gfx.Clear(this.BackColor);


      if (m_style == Styles.Rounded)
      {
        BackColor = Color.Transparent;
        Viatronix.UI.Utilities.DrawRectangleWithRoundedEdges(gfx, this.ClientRectangle, m_radius, true, true, Color.Black, Color.Black);
      }

      // Do nothing if there are no images
      if (m_imageList.Count == 0) 
        return;

      /// Brush used for text
      SolidBrush brush  = new SolidBrush(this.ForeColor);
      Pen outlinePen    = new Pen(m_imageBorderColor);

      // Create a small font using the current Font with a size of 5 (This may or maynot work properly based on the Font)
      Font font = this.Font; 

      // Get the size of the string using the fon and a dummy string so we can get the height
      SizeF sizeF = gfx.MeasureString("22", this.Font);


      // total size of both buttons
      int buttonSize = (m_buttonSize * 2);
      int marginSize = (m_marginSize * 2);

      // Calculate the width and height adjustments taking into account the margins and the buttons
      int widthOffset  = ((m_scrollDirection == ScrollDirections.LeftRight ? buttonSize : 0) + marginSize);
      int heightOffset = ((m_scrollDirection == ScrollDirections.LeftRight ? 0 : buttonSize) + marginSize);


      /// Drawing size take into account the buttons and there location
      Size drawingArea = new Size( (this.Width  - widthOffset), this.Height - heightOffset);

      /// Maximum caption lines 
      int maxCaptionLines = (m_scrollDirection == ScrollDirections.LeftRight ? 1 :  2);

      // Caption size (0 if no captions are to be diplayed )
      int captionHeight = (this.IsCaptionsDisplayable ? ((int)sizeF.Height + m_captionSpacing) * maxCaptionLines: 0);


      // Calculate the images size using the controls Height is L/R or Width if U/D.  
      int imageSize = (m_scrollDirection == ScrollDirections.LeftRight ? ( drawingArea.Height - captionHeight) : drawingArea.Width);

      Size imageSz = new Size(imageSize, imageSize + captionHeight );

      int maxImages       = 0;
      int imagesPerRow    = 0;
      int rowSpacing      = 0;
      int colSpacing      = 0;
      int imagesPerCol    = 0;

      if (m_multipleRows && m_minimumImageSize != -1)
      {

        int rowImageSize = Math.Min(m_minimumImageSize, drawingArea.Width);
        int colImageSize = Math.Min(m_minimumImageSize, drawingArea.Height);

        imageSize = Math.Min(rowImageSize, colImageSize);

        RenderInfo info = GetRenderInfo(drawingArea.Height, imageSize, captionHeight);
        imagesPerCol  = info.MaxImages;
        colSpacing    = info.Spacing;
        colImageSize  = info.ImageSize;

        imageSize = Math.Min(rowImageSize, colImageSize);

        info = GetRenderInfo(drawingArea.Width, imageSize, 0);
        imagesPerRow = info.MaxImages;
        rowSpacing = info.Spacing;
        rowImageSize = info.ImageSize;


        imageSize = Math.Min(rowImageSize, colImageSize);

        maxImages =  imagesPerRow * imagesPerCol;

      }
      else
      {

        if (m_scrollDirection == ScrollDirections.LeftRight)
        {

          RenderInfo info = GetVerticalRenderInfo(drawingArea, imageSz);

          maxImages     = info.MaxImages;
          imagesPerRow  = maxImages;
          imagesPerCol  = 1;
          rowSpacing    = info.Spacing;
          colSpacing    = m_marginSize;
        }
        else
        {
          RenderInfo info = GetHorizontalRenderInfo(drawingArea, imageSz);

          maxImages       = info.MaxImages;

          imagesPerRow    = 1;
          imagesPerCol    =  maxImages;
          rowSpacing      = m_marginSize;
          colSpacing      =  info.Spacing;
        }


      }

      m_buttonImageVisible = (maxImages < m_imageList.Count);

      bool captionsDisplayable = this.IsCaptionsDisplayable;

      if (imageSize >= 50 && imageSize < 60)
      {
        font = new Font(Font.FontFamily, 7.0F);
      }
      else if (imageSize >= 40 && imageSize < 50)
      {
        font = new Font(Font.FontFamily, 7.0F);
      }
      else if ( imageSize >= 30 && imageSize < 40 )
      {
        font = new Font(Font.FontFamily, 6.0F);
      }
      else if ( imageSize < 30 )
      {
        font = new Font(Font.FontFamily, 5.0F);
      }
      
      // Set the ending index to the starting index
      m_endingIndex = m_startingIndex;

      // Clear the displayed image list
      m_displayedImages.Clear();


      int x = 0;
      int y = 0;

      if (m_scrollDirection == ScrollDirections.LeftRight)
      {
        // X will be the changing position
        x = m_buttonSize + rowSpacing;

        // Y is constant with the margin offset of 2 from the edge
        y = m_marginSize;
      }
      else
      {
        // X will be the changing position
        x = m_marginSize;

        // Y is constant with the margin offset of 2 from the edge
        y = m_buttonSize + colSpacing;

      }



      int col = 1;

      //=======================================================================================================================
      // Loop though all the images from the starting index to the maximum images allowed to be diplayed and draw each image. 
      //=======================================================================================================================
      for (int i = 0; m_endingIndex < m_imageList.Count && i < maxImages; ++i, ++m_endingIndex)
      {

        // Create the displayed image object
        DisplayedImage image = new DisplayedImage(m_imageList[m_endingIndex], new Rectangle(x, y, imageSize, imageSize), m_endingIndex);

        // Add the displayed image object to the list (Used for OnClick and image selection )
        m_displayedImages.Add(image);


        // Draw the image (Uses the scaling imbedded in the graphics DrawImage() method
        gfx.DrawImage(image.ListViewImage.Image, image.ImageRectangle);

        if (m_dragIndex == m_endingIndex || m_selectedIndex == m_endingIndex)
        {
          float[][] ptsArray ={ new float[] {0.5F, 0, 0, 0, 0},
                                new float[] {0, 0.5F, 0, 0, 0},
                                new float[] {0, 0, 0.5F, 0, 0},
                                new float[] {0, 0, 0, 0.5F, 0}, 
                                new float[] {0.2F, 0.2F, 0.2F, 1.0F, 1.0F}};


          ColorMatrix colorMatrix = new ColorMatrix(ptsArray);

          ImageAttributes imageAttributes = new ImageAttributes();
          imageAttributes.SetColorMatrix(colorMatrix);

          gfx.DrawImage(image.ListViewImage.Image, image.ImageRectangle, 0, 0, image.ListViewImage.Image.Width,image.ListViewImage.Image.Height, GraphicsUnit.Pixel, imageAttributes);

        }
        else
        {
          // Draw the image (Uses the scaling imbedded in the graphics DrawImage() method
          gfx.DrawImage(image.ListViewImage.Image, image.ImageRectangle);

        }

        //==============================================================================
        // If the current index is the actual selected index then draw a box around it
        //==============================================================================
        //if (m_selectedIndex == m_endingIndex)
        if (m_enableImageBorder)
        {
          gfx.DrawRectangle(outlinePen, image.Rectangle);
        } // END ... If the image is selected

        //====================================================================================================================
        // If the display caption flag is set this overrides all and the caption will be doisplayed event is no caption 
        // is present. If no caption is to be displayed then the image number will be displayed.
        //====================================================================================================================
        string caption = ( m_displayCaption ? image.ListViewImage.Caption : ( m_displayImageNumber ? ((int)(m_endingIndex + 1)).ToString() : string.Empty ));
        string [] capionLines = { caption };

        if ( caption.Length > 0 )
        {

          sizeF = gfx.MeasureString(caption, font);

          if (sizeF.Width > (float)(imageSize + 6))
          {
            Char [] space = { ' ' };
            capionLines = caption.Split(space);
          }

          float capX = (float)(image.Rectangle.X + ((imageSize / 2.0F) - (sizeF.Width / 2.0F)));
          float capY = (float)(image.Rectangle.Bottom + m_captionSpacing);

          for (int line = 0; line < capionLines.Length; ++line)
          {
            sizeF = gfx.MeasureString(capionLines[line], font);

            gfx.DrawString(capionLines[line], font, brush, (float)(image.Rectangle.X + ((imageSize / 2.0F) - (sizeF.Width / 2.0F))), capY);
            capY += (sizeF.Height + 2);
          }
          
        }



        if (col++ == imagesPerRow)
        {
          // X will be the changing position
          x = (m_scrollDirection == ScrollDirections.LeftRight ? m_buttonSize + rowSpacing : m_marginSize);
          y += (imageSize + captionHeight + colSpacing);  
          col = 1;
        }
        else
        {
          // Adjust x to the next image position
          x += (imageSize + rowSpacing);
        }

      } // END ... For each image to be displayed





      EnableNextButton((m_imageList.Count > 0 && m_endingIndex < m_imageList.Count));
      EnablePrevButton((m_imageList.Count > 0 && m_startingIndex != 0));


    } // OnPaint(object sender, PaintEventArgs e)

    /// <summary>
    /// On click control event handler. Identifies the image the mouse inn over for selection.
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnClick(object sender, EventArgs e)
    {
      return;

      // Reset the slected index
      m_selectedIndex = -1;

      //==========================================
      // Only select an image if the flag is set
      //==========================================
      if (m_enableImageSelection)
      {

        // Gets the mouse point and convert it to a point in the control
        Point pt = this.PointToClient(Control.MousePosition);

        //===============================================================================================
        // loop though the displayed images to find if the mouse was in on of the images rectangles.
        //===============================================================================================
        foreach (DisplayedImage displayedImage in m_displayedImages)
        {

          //============================================================================================================================
          // If the mouse is in the displayed images area set the selected index to imdex of this image so that it can be highlighted
          //============================================================================================================================
          if (displayedImage.Rectangle.Contains(pt))
          {
            // Set the selected index
            m_selectedIndex = displayedImage.Index;

            //  Refresh
            Invalidate();

            break;
          } // END ... If the mouse is in the image

        } // END ... For each displayed image

        if (m_selectedIndex != -1 && this.SelectedImageChanged != null)
          this.SelectedImageChanged(this, EventArgs.Empty);

      } // END ... If the enable image selection flag is true

    } // OnClick(object sender, EventArgs e)


    /// <summary>
    /// Handles the mouse eneter evenet for the panel buttons
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnMouseEnterButton(object sender, EventArgs e)
    {
      System.Windows.Forms.Panel panel = (System.Windows.Forms.Panel)sender;

      m_selectedButton = (string)panel.Tag;
      m_redrawButton = false;

      if (m_selectedButton == "NEXT")
        m_panelNext.Invalidate();
      else
        m_panelPrev.Invalidate();

    } // OnMouseLeaveButton(object sender, EventArgs e)

    /// <summary>
    /// Handles the mouse leave event for the panel buttons
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnMouseLeaveButton(object sender, EventArgs e)
    {
      System.Windows.Forms.Panel panel = (System.Windows.Forms.Panel)sender;

      string currentSelectedButton = m_selectedButton;
      m_selectedButton = string.Empty;
      m_redrawButton = false;

      if (currentSelectedButton == "NEXT")
        m_panelNext.Invalidate();
      else
        m_panelPrev.Invalidate();

    } // OnMouseLeaveButton(object sender, EventArgs e)


    #endregion


    #region event

    /// <summary>
    /// Event indicating that an item is being dragged
    /// </summary>
    public event ItemDragEventHandler ItemDrag;

    /// <summary>
    /// Event to indicate an image was selected
    /// </summary>
    public event EventHandler SelectedImageChanged;

    #endregion


    #region overrides

    /// <summary>
    /// Overrides the mouse down event tp start the drag event
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseDown(MouseEventArgs e)
    {
      base.OnMouseDown(e);

 
      m_dragImage = null;
      m_dragIndex = -1;
      m_selectedIndex = -1;

      if (m_imageList.Count == 0)
        return;

      //==================================================================
      // Select the image if the left or roght mouse button is pressed
      //===================================================================
      if (e.Button == MouseButtons.Left || e.Button == MouseButtons.Right)
      {
       
        // Gets the mouse point and convert it to a point in the control
        Point pt = this.PointToClient(Control.MousePosition);

        //===============================================================================================
        // loop though the displayed images to find if the mouse was in on of the images rectangles.
        //===============================================================================================
        foreach (DisplayedImage displayedImage in m_displayedImages)
        {

          //============================================================================================================================
          // If the mouse is in the displayed images area set the selected index to imdex of this image so that it can be highlighted
          //============================================================================================================================
          if (displayedImage.Rectangle.Contains(pt))
          {
            // Set the selected index
            if ( m_enableImageSelection ) 
              m_selectedIndex = displayedImage.Index;

            // Set the drag item
            m_dragImage = displayedImage;

            break;
          } // END ... If the mouse is in the image

        } // END ... For each displayed image

        if (m_selectedIndex != -1)
        {
          Invalidate();

          if (this.SelectedImageChanged != null)
            this.SelectedImageChanged(this, EventArgs.Empty);
        }

      }


    } // OnMouseDown(MouseEventArgs e)

    /// <summary>
    /// Overrides the mouse up event to clear the drag rectangle
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseUp( MouseEventArgs e)
    {
      base.OnMouseUp(e);

      // Seve the index to determine if we need to repaint
      int oldIndex = m_dragIndex;

      // Reset the drag index
      m_dragIndex = -1;

      // Clear the drag box
      m_dragImage = null;
      
      // Invalidate since the drag index is reset and we want to deselect the image
      if ( oldIndex != -1 )
        Invalidate();

    } // OnMouseUp( MouseEventArgs e)


    /// <summary>
    /// Overrides the mouse move event to determine if we are dragging
    /// </summary>
    /// <param name="e"></param>
    protected override void OnMouseMove(MouseEventArgs e)
    {

      base.OnMouseMove(e);

      if (m_imageList.Count == 0)
        return;

      //=========================================
      // Check for a left button being held down
      //==========================================
      if ( e.Button == MouseButtons.Left )
      {

        //==============================================================
        // If the mouse moves outside the rectangle, start the drag.
        //=============================================================
        if (  m_dragImage != null ) //&& ! m_dragImage.Rectangle.Contains(e.X, e.Y))
        {

          // Set the drag index
          m_dragIndex = m_dragImage.Index;


          // Highlight the image
          Invalidate();

          //===========================================================================================
          // This is forced so that the DragOver event can be oevridden and the default drag-drop
          // cursor will be displayed in this control
          //===========================================================================================
          this.AllowDrop = true;


          //=====================================================================================
          // Fire the ItemDrag event if there is an actual event to allow someone to perform the 
          // the DoDRagDrop themselves
          //=====================================================================================
          if (ItemDrag != null)
          {
            ItemDrag(this, new ItemDragEventArgs(e.Button, m_imageList[m_dragIndex]));
          }
          else
          {
            // Proceed with the drag-and-drop, passing in the list item.                    
            DragDropEffects dropEffect = this.DoDragDrop(m_imageList[m_dragIndex], DragDropEffects.Move);
          }


        } // END ... If we have moved outside the drag rectangle

     } // END ... If the left mouse button is being help down

    } // OnMouseMove(MouseEventArgs e)


    /// <summary>
    /// Overrides the Query Continue Drag event so that I can check if the Drop action has been performed
    /// </summary>
    /// <param name="e"></param>
    protected override void OnQueryContinueDrag(QueryContinueDragEventArgs e)
    {
      base.OnQueryContinueDrag(e);

      //============================================================================
      // If the object was dropped or canceled then deselect it in this control
      //==========================================================================
      if (e.Action == DragAction.Drop || e.Action == DragAction.Cancel )
      { 
        
        this.AllowDrop = false;


        m_dragImage = null;
        m_dragIndex = -1;
        Invalidate();

      } // END ... If the drag was dropped or cancelled
    } // OnQueryContinueDrag(QueryContinueDragEventArgs e)

    /// <summary>
    /// Overrides the On Drag Over event alloing for the effects to be set and the correct cursor is displayed
    /// </summary>
    /// <param name="e"></param>
    protected override void OnDragOver(DragEventArgs e)
    {
      base.OnDragOver(e);

      // Determine whether string data exists in the drop data. If not, then
      // the drop effect reflects that the drop cannot occur.
      if (m_imageList.Count == 0 || !e.Data.GetDataPresent(typeof(Viatronix.UI.ImageListView.ListViewImage)))
      {

        e.Effect = DragDropEffects.None;

        return;
      }

      // SHIFT KeyState for move.
      e.Effect = DragDropEffects.Move;


    } // OnDragOver(DragEventArgs e)

    #endregion


  } // clase ImageListView


}
