// $Id: Utilities.cs,v 1.18.2.1 2009/03/20 20:38:15 mkontak Exp $
//
// Copyright © 2001, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owners: George Economos (george@viatronix.com)
// Owners: frank dachille( frank@viatronix.net )

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Windows.Forms;
using System.Resources;

namespace Viatronix.UI
{
  /// <summary>
  /// A set of utilities for manipulating and displaying images
  /// </summary>
  public sealed class Utilities
  {

    #region fields

    /// <summary>
    /// if running on windows XP or greater
    /// </summary>
    private static bool m_winXpOrGreater = false;

    #endregion

    #region properties

    /// <summary>
    /// Gets if the current os is winxp or greater
    /// </summary>
    public static bool WinXpOrGreater
    {
      get { return m_winXpOrGreater; }
    } // WinXpOrGreater

    #endregion

    #region initialization

    /// <summary>
    /// static initialization
    /// </summary>
    static Utilities()
    {
      Version vs = Environment.OSVersion.Version;
      m_winXpOrGreater = ( vs.Major > 5 ) || ( vs.Major == 5 && vs.Minor > 0 );
    } // Utilities()

    #endregion

    #region methods

    #region find next multiple

    /// <summary>
    /// get the next multiple of two numbers
    /// </summary>
    /// <param name="n">the first number</param>
    /// <param name="m">the second number</param>
    /// <returns></returns>
    static public int GetNextMultiple( int n, int m )
    {
      return ( ( n + m ) / m )* m;
    } // GetNextMultiple( int n, int m )

    #endregion
    
    #region interpolate
    
    /// <summary>
    /// Function to interpolate values.  The output value preserves the
    /// ratio:  (a,b,c) : (d,?,e)
    /// </summary>
    /// <param name="a">input parameter a</param>
    /// <param name="b">input parameter b</param>
    /// <param name="c">input parameter c</param>
    /// <param name="d">input parameter d</param>
    /// <param name="e">input parameter e</param>
    /// <returns></returns>
    static public float Interpolate( float a, float b, float c,  float d, float e )
    {
      return d + ( b - a ) / ( c - a ) * ( e - d );
    } // Interpolate( a, b, c, d, e )

    #endregion
    
    #region DrawBackground

    /// <summary>
    /// Draws the background for a control. 
    /// </summary>
    /// <param name="gfx">Graphics object to render into</param>
    /// <param name="control">Control object to draw background for.</param>
    static public void DrawBackground( Graphics gfx, Control control )
    {
      // force the background to be drawn regardless of the control's back color
      DrawBackground( gfx, control, false );
    } // DrawBackground( gfx, control )        

    
    /// <summary>
    /// Draws the background for a control. If the control's BackColor prooperty is set to
    /// Color.Transparent, the parent's background image ( or BackColor ) is filled in the control.
    /// </summary>
    /// <param name="gfx">Graphics object to render into</param>
    /// <param name="control">Control object to draw background for.</param>
    /// <param name="force">true to force the background to be drawn regardless of the control's back color</param>
    static public void DrawBackground( Graphics gfx, Control control, bool force )
    {
      // fill in the background
      if( force || control.BackColor == Color.Transparent )
      {
        Control parent = control.Parent;
        
        // do we have a parent?
        while( parent != null )
        {
          // is the parent using a background image?
          if( parent.BackgroundImage != null )
          {
            Rectangle bounds = parent.RectangleToClient( control.RectangleToScreen( control.ClientRectangle ) );
            gfx.DrawImage( parent.BackgroundImage, 0, 0, bounds, GraphicsUnit.Pixel );
            break;
          } // if( parent.BackgroundImage != null )
          
            // make sure the parent back color is not transparent
          else if( parent.BackColor != Color.Transparent )
          {
            gfx.Clear( parent.BackColor );     
            break;
          } // else if( parent.BackColor != Color.Transparent )
          parent = parent.Parent;
        } // while( parent != null )
      } // if( force || control.BackColor == Color.Transparent )
      else
        gfx.Clear( control.BackColor );     
    } // DrawBackground( gfx, control, force )

    #endregion

    #region DrawImage

    /// <summary>
    /// Draws an image into the specified bounds. The aspect ratio is preserved.
    /// </summary>
    /// <param name="gfx">Graphics object to render into.</param>
    /// <param name="image">Image object to draw.</param>
    /// <param name="bounds">Bounds to draw image into</param>
    /// <param name="selected">Indicates if the item should have a selection highlight.</param>
    static public void DrawImage( Graphics gfx, Image image, Rectangle bounds, bool selected )
    {
      // size of source image
      SizeF size = new SizeF( image.Width  / image.HorizontalResolution, image.Height / image.VerticalResolution );

      // scale and keep aspect ratio of source image
      float scale = Math.Min( bounds.Width / size.Width, bounds.Height / size.Height );
      size.Width *= scale;
      size.Height *= scale;

      // image layout in pixels
      RectangleF layout = new RectangleF(   bounds.X + ( bounds.Width  - size.Width )  / 2 ,
        bounds.Y + ( bounds.Height - size.Height ) / 2,
        size.Width, size.Height );

      // shrink the layout by 1 row and 1 column to accomodate selection
      layout.Inflate( -1.0F, -1.0F );
      
      // draw the image into the layout
      gfx.DrawImage( image, layout.X, layout.Y, layout.Width, layout.Height );
      
      // restore the layout 
      layout.Inflate( 1.0F, 1.0F );

      // is the item is selected?
      if( selected )
      {
        Color highlightColorDark = Color.FromArgb( 0, 0, 255 ) ;
        Color highlightColorLight = Color.FromArgb(80, 0, 0, 255);

        using( SolidBrush brush = new SolidBrush( highlightColorLight ) )
          gfx.FillRectangle( brush, layout.X, layout.Y, layout.Width, layout.Height );
                
        using( Pen pen = new Pen( highlightColorDark ) )
          gfx.DrawRectangle( pen, layout.X, layout.Y, layout.Width, layout.Height  );
      } // if( selected )
      else
      {
        using( Pen pen = new Pen( Color.White ) )
          gfx.DrawRectangle( pen, layout.X, layout.Y, layout.Width, layout.Height );
      } // else
    } // DrawImage( gfx, image, bounds, selected )
    
    #endregion

    #region DrawImage

    /// <summary>
    /// Draws an image into the specified bounds. The aspect ratio is perserved.
    /// </summary>
    /// <param name="gfx">Graphics object to render into.</param>
    /// <param name="image">Image object to draw.</param>
    /// <param name="bounds">Bounds to draw image into</param>
    static public void DrawImage( Graphics gfx, Image image, Rectangle bounds )
    {
      // size of source image
      SizeF size = new SizeF( image.Width  / image.HorizontalResolution, image.Height / image.VerticalResolution );

      // scale and keep aspect ratio of source image
      float scale = Math.Min( bounds.Width / size.Width, bounds.Height / size.Height );
      size.Width *= scale;
      size.Height *= scale;

      // image layout in pixels
      RectangleF layout = new RectangleF(   bounds.X + ( bounds.Width  - size.Width )  / 2 ,
        bounds.Y + ( bounds.Height - size.Height ) / 2,
        size.Width, size.Height );

      // draw the image into the layout
      gfx.DrawImage( image, layout.X, layout.Y, layout.Width, layout.Height );
     
    } // DrawImage( gfx, image, bounds, selected )
    
    #endregion

    #region DrawRectangleWithRoundedEdges


    /// <summary>
    /// Gets the graphics path if a rectangle with rouned edges
    /// /// </summary>
    /// <param name="rect">Rectangle to draw</param>
    /// <param name="radius">radioue</param>
    /// <returns>Returns the graphices path of the rectangle</returns>
    static public GraphicsPath GetRectangleWithRounedEdges(Rectangle rect, float radius)
    {

      // Graphic path 
      GraphicsPath gp = new GraphicsPath();

      // Set up locals
      int left = rect.X + rect.Width - 1;
      int right = rect.X;
      int top = rect.Y;
      int bottom = rect.Y + rect.Height - 1;

      // Get the circumfrence
      float circumfrence = (radius * 2);

      // Top
      gp.AddLine(right + radius, top, left - circumfrence, top);

      // Upper Right
      gp.AddArc(left - circumfrence, top, circumfrence, circumfrence, 270, 90);

      // Right 
      gp.AddLine(left, top + radius, left, bottom - circumfrence);

      // Bottom Right
      gp.AddArc(left - circumfrence, bottom - circumfrence, circumfrence, circumfrence, 0, 90);

      // Bottom
      gp.AddLine(left - circumfrence, bottom, right + radius, bottom);

      // Bottom Left
      gp.AddArc(right, bottom - circumfrence, circumfrence, circumfrence, 90, 90);

      // Left
      gp.AddLine(right, bottom - circumfrence, right, top + radius);

      // Upper Left
      gp.AddArc(right, top, circumfrence, circumfrence, 180, 90);

      // returns the graphic path just in case
      return gp;

    } // GetRectangleWithRounedEdges(Rectangle rect. float radius)


    /// <summary>
    /// Draws the secified rectangle with rounded edges
    /// </summary>
    /// <param name="gfx">Graphics</param>
    /// <param name="rect">Rectangle to draw</param>
    /// <param name="radius">radioue</param>
    /// <param name="drawBorder">Draw border flag</param>
    /// <param name="fill">fill flag</param>
    /// <param name="borderColor">Border color</param>
    /// <param name="fillColor">Fill color</param>
    /// <returns>Returns the graphices path of the rectangle</returns>
    static public GraphicsPath DrawRectangleWithRoundedEdges(Graphics gfx, Rectangle rect, float radius, bool drawBorder, bool fill, Color borderColor, Color fillColor)
    {

      // Graphic path 
      GraphicsPath gp = new GraphicsPath();

      // Set up locals
      int left    = rect.X + rect.Width - 1;
      int right   = rect.X;
      int top     = rect.Y;
      int bottom  = rect.Y + rect.Height - 1;

      // Get the circumfrence
      float circumfrence = (radius * 2);

      // Top
      gp.AddLine(right + radius, top, left - circumfrence, top);

      // Upper Right
      gp.AddArc(left - circumfrence, top, circumfrence, circumfrence, 270, 90);

      // Right 
      gp.AddLine(left, top + radius, left, bottom - circumfrence);

      // Bottom Right
      gp.AddArc(left - circumfrence, bottom - circumfrence, circumfrence, circumfrence, 0, 90);

      // Bottom
      gp.AddLine(left - circumfrence, bottom, right + radius, bottom);

      // Bottom Left
      gp.AddArc(right, bottom - circumfrence, circumfrence, circumfrence, 90, 90);

      // Left
      gp.AddLine(right, bottom - circumfrence, right, top + radius);

      // Upper Left
      gp.AddArc(right, top, circumfrence, circumfrence, 180, 90);




      // If a border us to be drawn then draw it now with the color specified
      if (drawBorder)
        gfx.DrawPath(new Pen(borderColor), gp);

      // If the rectangle is to be filled then fill it now
      if (fill)
        gfx.FillRegion(new SolidBrush(fillColor), new Region(gp));

      // returns the graphic path just in case
      return gp;

    } // DrawRectangleWithRoundedEdges(Graphics gfx, Rectangle rect, float radius, bool drawBorder, bool fill, Color borderColor, Color fillColor)


    #endregion


    #region DrawImageTransparent

    /// <summary>
    /// Draws an image at the specified location.
    /// </summary>
    /// <param name="gfx">the graphics object</param>
    /// <param name="image">the image to draw</param>
    /// <param name="x">the x position</param>
    /// <param name="y">the y position</param>
    /// <param name="opacity">draw with this much opacity</param>
    public static void DrawImageTransparent( Graphics gfx, Image image, int x, int y, float opacity )
    {

      float[][] points = { 
                            new float[] { 1,    0,    0,    0,          0 },
                            new float[] { 0,    1,    0,    0,          0 },
                            new float[] { 0,    0,    1,    0,          0 },
                            new float[] { 0,    0,    0,    opacity,    0 }, 
                            new float[] { 0,    0,    0,    0,          1 } }; 
      
      ColorMatrix matrix = new ColorMatrix( points );
      
      ImageAttributes attributes = new ImageAttributes();
      
      attributes.SetColorMatrix( matrix, ColorMatrixFlag.Default, ColorAdjustType.Bitmap );

      gfx.DrawImage( image, 
        new Rectangle( x, y, image.Width, image.Height ), 
        0, 0, image.Width, image.Height, GraphicsUnit.Pixel, attributes );

    } // DrawImageTransparent( Graphics gfx, Image image, int x, int y, float opacity )

    #endregion
    
    #region CreateShadowImage
    
    /// <summary>
    /// Creates a drop shadow for the original image.
    /// </summary>
    /// <param name="source">Original image object.</param>
    /// <returns></returns>
    static public Bitmap CreateShadowImage( Image  source )
    {
      Bitmap bitmap = new Bitmap( source );

      Color shadowColor = Color.FromArgb( 154, 156, 146 );
      Color transparent = Color.FromArgb( 0, 0, 0, 0 );

      // loop over scanlines
      for( int y = 0; y < bitmap.Height; y++ )
      {

        // loop over pixels in scanline
        for( int x = 0; x < bitmap.Width; ++x )
        {

          // see if the pixel is transparent
          if( bitmap.GetPixel( x, y ) != transparent )
            bitmap.SetPixel( x, y, shadowColor );
      
        } // for each x
      
      } // for each y

      return bitmap;
    } // CreateShadowImage()

    #endregion

    #region MakeShadowImage

    /// <summary>
    /// Casts a semi-transparent shadow behind a semi-translucent bitmap.
    /// </summary>
    /// <param name="srcImg">The original image.</param>
    /// <param name="translation">The distance to the lower right for the cast shadow.</param>
    /// <param name="blurRadius">How much the shadow spreads out.</param>
    /// <returns>The shadowed image.</returns>
    static public Bitmap MakeShadowImage( Bitmap srcImg, int translation, int blurRadius )
    {
      // create a new bitmap big enough to hold the original image plus the shadow
      Bitmap destImage = new Bitmap( srcImg.Width + translation + blurRadius, srcImg.Height + translation + blurRadius );

      // a constant used to test if we are within the blurred circle
      int innerCircle = blurRadius * blurRadius;

      // lock the bits in the bitmap
      Rectangle rect = new Rectangle( 0, 0, srcImg.Width, srcImg.Height );
      BitmapData srcData = srcImg.LockBits( rect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb );

      // now go really fast
      unsafe
      {

        // get a pointer to the data
        byte * pData = ( byte * ) ( (void *) srcData.Scan0 );

        // loop over scanlines
        for( int y=0; y<destImage.Height; ++y )
        {

          // loop within the scanline
          for( int x=0; x<destImage.Width; ++x )
          {
            // clear the pixel accumulation
            int a=0, r=0, g=0, b=0, count=0;
            int stride = srcData.Stride;

            // loop over the blur area y
            for( int u=-blurRadius; u<=blurRadius; ++u )
            {
              // loop over the blur area x
              for( int v=-blurRadius; v<blurRadius; ++v )
              {
                // get the source pixel location
                int xx = x + u - translation;
                int yy = y + v - translation;

                // see if the radius is within the "circle of confusion"
                int radius = u * u + v * v;
                if( radius <= innerCircle && xx>-1 && xx<srcData.Width && yy>-1 && yy<srcData.Height )
                {
                  // accumulate the pixel
                  int offset = 4 * (yy * srcData.Width + xx);
                  b += pData[offset + 0];
                  g += pData[offset + 1];
                  r += pData[offset + 2];
                  a += pData[offset + 3];
                }
                // keep track of how many samples were taken
                ++count;
              }
            }

            // compute the average
            if( count > 0 )
            {
              a /= count;
              r /= count;
              g /= count;
              b /= count;
            }

            // convert to grayscale
            int gray = ( r + g + b ) / 3;
            r = gray;
            g = gray;
            b = gray;
            
            // write the pixel
            destImage.SetPixel( x, y, Color.FromArgb( a, r, g, b ) );
          } // x
        } // y
      } // unsafe

      srcImg.UnlockBits( srcData );

      // draw the original back over the shadow
      Graphics destGraphics = Graphics.FromImage( destImage );
      destGraphics.DrawImageUnscaled( srcImg, 0, 0 );

      return destImage;

    } // MakeShadowImage( Bitmap srcImg, int translation, int blurRadius )

    #endregion

    #region GetSystemImageList

    public static ImageList GetSystemImageList( bool isSmall, bool isSelected ) 
    { 
      Win32.SHGFI flags = Win32.SHGFI.SHGFI_SYSICONINDEX;
      if( isSmall == true ) 
        flags |= Win32.SHGFI.SHGFI_SMALLICON; 
      else 
        flags |= Win32.SHGFI.SHGFI_LARGEICON; 

      if( isSelected == true ) 
        flags |= Win32.SHGFI.SHGFI_OPENICON; 
      
      ImageList imageList = new ImageList(); 

      Win32.SHFILEINFO info = new Win32.SHFILEINFO(); 
      int size = System.Runtime.InteropServices.Marshal.SizeOf( info ); 
      IntPtr imageHandle = Win32.Shell32.SHGetFileInfo( "c:\\", 0, ref info, (uint)size, flags ); 
      IntPtr iconHandle = IntPtr.Zero; 
      int iconCount = Win32.ComCtl32.ImageList_GetImageCount( imageHandle ); 
      for( int i = 0; i < iconCount; i++ ) 
      { 
        iconHandle = Win32.ComCtl32.ImageList_GetIcon( imageHandle, i, 0 ); 
        imageList.Images.Add( Icon.FromHandle( iconHandle )); 
      }

      return imageList;
    }

    #endregion

    #region GetMimeTypeImageIndex

    public static int GetMimeTypeImageIndex( string path, bool isSmall, bool isSelected, bool isFile )
    {
      Win32.SHGFI flags = Win32.SHGFI.SHGFI_SYSICONINDEX | Win32.SHGFI.SHGFI_USEFILEATTRIBUTES;
      
      if( isSmall ) 
        flags |= Win32.SHGFI.SHGFI_SMALLICON; 
      else 
        flags |= Win32.SHGFI.SHGFI_LARGEICON;

      if( isSelected ) 
        flags |= Win32.SHGFI.SHGFI_OPENICON; 

      Win32.FILE fileFlags = ( isFile ) ? Win32.FILE.FILE_ATTRIBUTE_NORMAL : Win32.FILE.FILE_ATTRIBUTE_DIRECTORY;
      
      Win32.SHFILEINFO info = new Win32.SHFILEINFO(); 
      int size = System.Runtime.InteropServices.Marshal.SizeOf( info ); 
      Win32.Shell32.SHGetFileInfo( path, fileFlags, ref info, (uint)size, flags ); 
      
      return info.iIcon.ToInt32();
    }

    #endregion

    #region GetMimeTypeImage

    public static Image GetMimeTypeImage( string path, bool isSmall, bool isSelected, bool isFile )
    {
      Win32.SHGFI flags = Win32.SHGFI.SHGFI_SYSICONINDEX | Win32.SHGFI.SHGFI_USEFILEATTRIBUTES | Win32.SHGFI.SHGFI_ICON;
      
      if( isSmall ) 
        flags |= Win32.SHGFI.SHGFI_SMALLICON; 
      else 
        flags |= Win32.SHGFI.SHGFI_LARGEICON;

      if( isSelected ) 
        flags |= Win32.SHGFI.SHGFI_OPENICON; 

      Win32.FILE fileFlags = ( isFile ) ? Win32.FILE.FILE_ATTRIBUTE_NORMAL : Win32.FILE.FILE_ATTRIBUTE_DIRECTORY;
      
      Win32.SHFILEINFO info = new Win32.SHFILEINFO(); 
      int size = System.Runtime.InteropServices.Marshal.SizeOf( info ); 
      Win32.Shell32.SHGetFileInfo( path, fileFlags, ref info, (uint)size, flags ); 
      
      return Icon.FromHandle( info.hIcon ).ToBitmap();
    }

    #endregion

    #region GetMimeTypeDisplayName

    public static string GetMimeTypeName( string path )
    {     
      Win32.SHFILEINFO info = new Win32.SHFILEINFO(); 
      int size = System.Runtime.InteropServices.Marshal.SizeOf( info ); 
      Win32.Shell32.SHGetFileInfo( path, Win32.FILE.FILE_ATTRIBUTE_NORMAL, ref info, (uint)size, Win32.SHGFI.SHGFI_TYPENAME ); 
      
      return info.szTypeName;
    }

    #endregion

    #region HideFromTaskList

    public static void HideFromTaskList( IntPtr handle )
    {
      Win32.WndProc32.SetWindowLong( handle, (int)Win32.WindowLongConstants.GWL_EXSTYLE, new IntPtr(( Win32.WndProc32.GetWindowLong( handle, (int)Win32.WindowLongConstants.GWL_EXSTYLE ) | (int)Win32.WindowExStyles.WS_EX_TOOLWINDOW ) & ~(int)Win32.WindowExStyles.WS_EX_APPWINDOW )); 
    } // HideFromTaskList( handle )


    #endregion

    #region LoadImage

    public static Image LoadImage( string locationName )
    {
      try
      {
        if( locationName == string.Empty )
          return null;

        string[] resourcePath = locationName.Split( ',' );
        string image = resourcePath[0].Trim();
        string assemblyName = resourcePath[1].Trim();

        System.Reflection.Assembly assembly = System.Reflection.Assembly.Load( assemblyName );
        foreach( string resName in assembly.GetManifestResourceNames() )
        {
          if( resName == image )
            return new Bitmap( System.Reflection.Assembly.Load( assemblyName ).GetManifestResourceStream( image ));
        }

        ResourceManager resource = new ResourceManager( assemblyName, System.Reflection.Assembly.Load( assemblyName ) );
        return resource.GetObject( image ) as Bitmap;
      }
      catch
      {
        return null;
      }
    } // LoadImage( locationName )

    #endregion

    #region Image Aspect Ratio Resizing

    /// <summary>
    /// Resizes an image keeping its aspect ratio
    /// </summary>
    /// <param name="width">new width</param>
    /// <param name="height">new height</param>
    /// <param name="image">Original image</param>
    /// <returns>resized image</returns>
    static public System.Drawing.Bitmap ResizeImage(int width, int height, System.Drawing.Image image)
    {
      ///area where the image aspect ratio is preserved for moving to CO
      RectangleF bounds = new RectangleF(0.0F, 0.0F, width, height);

      // size of source image
      SizeF size = new SizeF(image.Width / image.HorizontalResolution, image.Height / image.VerticalResolution);

      // scale and keep aspect ratio of source image
      float scale = Math.Min(bounds.Width / size.Width, bounds.Height / size.Height);
      size.Width *= scale;
      size.Height *= scale;

      // new image layout in pixels
      RectangleF layout = new RectangleF(bounds.X + (bounds.Width - size.Width) / 2.0F,
        bounds.Y + (bounds.Height - size.Height) / 2.0F,
        size.Width, size.Height);

      // draw a new image into the layout
      System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap(width, height, image.PixelFormat);
      using (System.Drawing.Graphics gfx = System.Drawing.Graphics.FromImage(bitmap))
      {
        gfx.DrawImage(image, layout.X, layout.Y, layout.Width, layout.Height);
      }
      return bitmap;
    }//ResizeImage( int width, int height, System.Drawing.Image image )

    /// <summary>
    /// Resizes an image keeping its aspect ratio
    /// </summary>
    /// <param name="size">Resize to</param>
    /// <param name="image">source image</param>
    /// <returns>resized image</returns>
    static public System.Drawing.Bitmap ResizeImage(Size size, System.Drawing.Image image)
    {
      return ResizeImage(size.Width, size.Height, image);
    }//ResizeImage(Size size, System.Drawing.Image image)

    #endregion

 
    #endregion

    #region CompressedImage class

    /// <summary>
    /// A class used to compress and decompress images for efficient sending over the wire.
    /// </summary>
    public class CompressedImage
    {

      #region fields

      /// <summary>
      /// The difference between bytes of the image.
      /// </summary>
      private byte [] m_bytes;

      /// <summary>
      /// The run length encoded byte differences.
      /// </summary>
      private byte [] m_rleBytes;

      /// <summary>
      /// The number of bytes used in the difference image.
      /// </summary>
      private int m_usedBytes;

      /// <summary>
      /// The number of bytes used in the RLE encoded image.
      /// </summary>
      private int m_rleUsedBytes;

      /// <summary>
      /// The size of image chunks.
      /// </summary>
      private int m_chunkSize = 32;

      #endregion

      #region initialization

      /// <summary>
      /// initialize an instance of CompressedImage
      /// </summary>
      public CompressedImage()
      {

        // allocate a small 1x1 image.
        m_bytes = new byte[1];
        m_rleBytes = new byte[1];
        m_usedBytes = 0;
        m_rleUsedBytes = 0;

      } // CompressedImage()
      
      #endregion

      #region methods

      /// <summary>
      /// Allocate all the memory according to the required number of bytes.
      /// </summary>
      /// <param name="requiredBytes">The number of bytes that will be required.</param>
      public void AllocateBytes( int requiredBytes )
      {

        if ( m_bytes.Length < requiredBytes )
          m_bytes = new byte[ requiredBytes ];
        if ( m_rleBytes.Length < requiredBytes )
          m_rleBytes = new byte[ requiredBytes ];
        m_usedBytes = 0;

      } // AllocateBytes()


      /// <summary>
      /// Top level compression procedure.
      /// </summary>
      /// <param name="srcImg">The source image.</param>
      public void CompressImage( Bitmap srcImg )
      {

        // lock the bits
        Rectangle rect = new Rectangle( 0, 0, srcImg.Width, srcImg.Height );
        BitmapData srcBitmapData = srcImg.LockBits( rect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb );

        // go really fast
        unsafe
        {

          // get a pointer to the bytes
          byte * srcBytes = ( byte * ) ( (void *) srcBitmapData.Scan0 );
          int inLengthBytes = 4 * srcBitmapData.Width * srcImg.Height;

          // ensure there is enough space to hold data
          AllocateBytes( 40*inLengthBytes );

          // copy into the new buffer but copy only the difference
          m_bytes[0] = srcBytes[0];
          m_bytes[1] = srcBytes[1];
          m_bytes[2] = srcBytes[2];
          m_bytes[3] = srcBytes[3];

          // copy in the differences and let them wrap around
          for( int i=4; i<inLengthBytes; ++i )
            m_bytes[i] = (byte) ( ( srcBytes[i] - srcBytes[i-4] ) % 256 );
          m_usedBytes = inLengthBytes;

          // here is where you would compress with RLE if it was working

        } // unsafe

        srcImg.UnlockBits( srcBitmapData );

      } // CompressImage( Bitmap srcImg )


      /// <summary>
      /// Top level uncompression procedure
      /// </summary>
      /// <param name="destImg">Destination image.</param>
      public void UncompressImage( Bitmap destImg )
      {

        // lock the bits
        Rectangle rect = new Rectangle( 0, 0, destImg.Width, destImg.Height );
        BitmapData destBitmapData = destImg.LockBits( rect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb );

        // go really fast
        unsafe
        {

          // get a pointer to the bytes
          byte * destBytes = ( byte * ) ( (void *) destBitmapData.Scan0 );
          int destLengthBytes = 4 * destBitmapData.Width * destImg.Height;

          // ensure there is enough room to decompress
          if ( m_usedBytes != destLengthBytes )
          {
            System.Windows.Forms.MessageBox.Show( "Byte lengths do not match!" );
            return;
          }

          // here is where you would decompress the RLE if it was working

          // copy into the new buffer but copy only the difference
          bool bShowRaw = false;
          if ( bShowRaw )
          {

            // useful for debugging
            for( int i=0; i<destLengthBytes; ++i )
            {
              destBytes[i] = m_bytes[i];
              // set alpha to opaque
              if ( i % 4 == 3 )
                destBytes[i] = 255;
            }

          }
          else
          {

            // prime the first pixel
            destBytes[0] = m_bytes[0];
            destBytes[1] = m_bytes[1];
            destBytes[2] = m_bytes[2];
            destBytes[3] = m_bytes[3];
            
            // convert the stream of differences into a stream of actual values (use modulo math)
            for( int i=4; i<destLengthBytes; ++i )
              destBytes[i] = (byte) ( ( m_bytes[i] + destBytes[i-4] ) % 256 );

          } // if show raw

        } // unsafe

        destImg.UnlockBits( destBitmapData );

      } // UncompressImage( Bitmap destImg )


      /// <summary>
      /// Perform run-length encoding compression
      /// </summary>
      private void CompressRLE()
      {
        // one byte: 0=end of data, 1=all same pixels in chunk, 2=all different pixels in chunk
        byte [] four = new byte[4];
        for ( int i=0; i<m_usedBytes; i+=m_chunkSize*4 )
        {
          // see if all are the same
          for ( int j=0; j<4; ++j )
            four[j] = m_bytes[i+j];
          bool bSame = true;
          for ( int j=4; j<m_chunkSize*4; ++j )
          {
            byte test = m_bytes[ i+j ];
            byte pattern = four[ ( i + j )% 4 ];
            if ( test != pattern )
            {
              bSame = false;
            }
          }

          if ( bSame )
          {
            m_rleBytes[ m_rleUsedBytes++ ] = 1;
            for ( int j=0; j<4*m_chunkSize; ++j )
              m_rleBytes[ m_rleUsedBytes++ ] = m_bytes[ i + j ];
          }
          else
          {
            m_rleBytes[ m_rleUsedBytes++ ] = 2;
            for ( int j=0; j<4; ++j )
              m_rleBytes[ m_rleUsedBytes++ ] = m_bytes[ i + j ];
          }

        }

        m_rleBytes[ m_rleUsedBytes++ ] = 0;

      } // CompressRLE()


      /// <summary>
      /// Uncompress the run-length encoded image
      /// </summary>
      private void UncompressRLE()
      {
        // one byte: 0=end of data, 1=all same pixels in chunk, 2=all different pixels in chunk
        int bytePos = 0;
        int rleBytePos = 0;
        byte [] four = new byte[4];
        for ( ; rleBytePos<m_rleUsedBytes; )
        {
          byte chunkType = m_rleBytes[ rleBytePos++ ];
          if ( chunkType == 0 )
          {
            // do nothing
          }
          else if ( chunkType == 1 )
          {
            // get pattern
            for ( int j=0; j<4; ++j )
              four[j] = m_rleBytes[ rleBytePos++ ];

            // copy to later bytes
            for ( int j=0; j<4*m_chunkSize; ++j )
              m_bytes[ bytePos++ ] = four[ j % 4 ];
          }
          else if ( chunkType == 2 )
          {
            // just copy
            for ( int j=0; j<4*m_chunkSize; ++j )
              m_bytes[ bytePos++ ] = m_rleBytes[ rleBytePos++ ];
          }
        }

      } // UncompressRLE()

      #endregion

    } // class CompressedImage

    #endregion

    #region Hue-Saturation-Lightness class

    public class HSL
    {
      private float h;
      private float s;
      private float l;

      public float Hue
      {
        get
        {
          return h;
        }
        set
        {
          h = (float)(Math.Abs(value)%360);
        }
      }

      public float Saturation
      {
        get
        {
          return s;
        }
        set
        {
          s = (float)Math.Max(Math.Min(1.0, value), 0.0);
        }
      }

      public float Luminance
      {
        get
        {
          return l;
        }
        set
        {
          l = (float)Math.Max(Math.Min(1.0, value), 0.0);
        }
      }

      private HSL()
      {
      }
      public HSL(float hue, float saturation, float luminance)
      {
        Hue = hue;
        Saturation = saturation;
        Luminance = luminance;
      }


      public Color RGB
      {
        get
        {
          double r=0,g=0,b=0; 

          double temp1,temp2; 

          double normalisedH = h/360.0;

          if(l==0) 
          { 
            r=g=b=0; 
          } 
          else 
          { 
            if(s==0) 
            { 
              r=g=b=l; 
            } 
            else 
            { 
              temp2 = ((l<=0.5) ? l*(1.0+s) : l+s-(l*s)); 

              temp1 = 2.0*l-temp2; 

              double[] t3=new double[]{normalisedH+1.0/3.0,normalisedH,normalisedH-1.0/3.0}; 

              double[] clr=new double[]{0,0,0}; 

              for(int i=0;i<3;++i) 
              { 
                if(t3[i]<0) 
                  t3[i]+=1.0; 

                if(t3[i]>1) 
                  t3[i]-=1.0; 

                if(6.0*t3[i] < 1.0) 
                  clr[i]=temp1+(temp2-temp1)*t3[i]*6.0; 
                else if(2.0*t3[i] < 1.0) 
                  clr[i]=temp2; 
                else if(3.0*t3[i] < 2.0) 
                  clr[i]=(temp1+(temp2-temp1)*((2.0/3.0)-t3[i])*6.0); 
                else 
                  clr[i]=temp1; 

              } 

              r=clr[0]; 
              g=clr[1]; 
              b=clr[2]; 
            } 

          } 
          return Color.FromArgb((int)(255*r),(int)(255*g),(int)(255*b)); 
        }
      }

      private static byte toRGB(float rm1, float rm2, float rh)
      {
        if(rh>360) rh-=360;
        else if(rh<  0) rh+=360;

        if(rh< 60) rm1=rm1+(rm2-rm1)*rh/60;   
        else if(rh<180) rm1=rm2;
        else if(rh<240) rm1=rm1+(rm2-rm1)*(240-rh)/60;      

        return (byte)(rm1*255);
      }


      public static HSL FromRGB(byte red, byte green, byte blue)
      {
        return FromRGB(Color.FromArgb(red, green, blue));
      }

      public static HSL FromRGB(Color c)
      {
        return new HSL(c.GetHue(), c.GetSaturation(), c.GetBrightness());
      }
    }

    #endregion

  
  } // class Utilties
} // namespace Viatronix.UI

#region revision history

// $Log: Utilities.cs,v $
// Revision 1.18.2.1  2009/03/20 20:38:15  mkontak
// Added ImageListView control
//
// Revision 1.18  2007/06/06 18:58:04  romy
// ResizeImage method added
//
// Revision 1.17  2006/10/30 16:28:41  gdavidson
// Modified LoadImage method
//
// Revision 1.16  2006/03/09 21:34:06  gdavidson
// Removed ShowWindow method
//
// Revision 1.15  2006/01/09 19:43:09  gdavidson
// Added LoadImage method.
//
// Revision 1.14  2005/11/23 03:59:18  geconomos
// viewer launching revisted
//
// Revision 1.13  2005/11/16 21:20:26  geconomos
// added method to show a window using native methods
//
// Revision 1.12  2005/11/16 14:14:34  gdavidson
// Modified the BringToFront method.
//
// Revision 1.11  2005/11/10 19:58:53  gdavidson
// Added a BringToFront method.
//
// Revision 1.10  2005/11/04 21:18:23  frank
// added ability to change the theme color to any nearly any system color.
//
// Revision 1.9  2005/10/26 19:57:58  gdavidson
// Added a method to remove a form from the task list.
//
// Revision 1.8  2001/01/02 12:44:21  gdavidson
// Added methods to retrieve images from the system image list.
//
// Revision 1.7  2005/03/08 19:11:06  frank
// Merged the calcium scoring 1.2 branch into the trunk
//
// Revision 1.6.2.2  2005/02/28 22:09:07  gdavidson
// Code review
//
// Revision 1.6.2.1  2005/02/28 21:10:09  frank
// code review
//
// Revision 1.6  2004/10/20 13:09:03  geconomos
// Added a method to draw images transparently.
//
// Revision 1.5  2004/10/11 23:59:53  geconomos
// Initial "reskinning"
//
// Revision 1.4  2004/08/12 17:22:41  geconomos
// Added new overload of DrawImage
//
// Revision 1.3  2004/08/05 14:16:02  geconomos
// Moved code to check version of windows to utilities.
//
// Revision 1.2  2004/05/13 13:33:30  geconomos
// cleanup
//
// Revision 1.1.1.1  2004/03/02 03:58:38  geconomos
// Moved and restructured code from the old xp module. This helps 
// recompiling everything in one go and also reduces the compile
// time for the individual projects due to the new library structure.
//
// Revision 1.19  2003/05/25 18:12:07  michael
// .net --> .com
//
// Revision 1.18  2003/05/07 17:10:02  frank
// code review
//
// Revision 1.17  2003/04/28 16:41:18  geconomos
// Even more coding standards.
//
// Revision 1.16  2003/04/28 12:48:26  frank
// Comments
//
// Revision 1.15  2003/04/28 01:25:24  geconomos
// A little more coding standards.
//
// Revision 1.14  2003/04/27 23:51:49  geconomos
// More code cleanup.
//
// Revision 1.13  2003/04/25 21:06:44  geconomos
// Endless code reviews.
//
// Revision 1.12  2003/02/28 14:49:36  geconomos
// Clenaed up draing selected image in DrawImage
//
// Revision 1.11  2003/02/13 16:56:46  jmeade
// Merge-to-Main_BeforeBranch_VC_1-3_to_UpdateAfterBranch_VC_1-3.
//
// Revision 1.10  2003/02/07 18:41:34  geconomos
// Created new "selected" loook when draing image.
//
// Revision 1.9.2.1  2003/02/12 23:01:35  jmeade
// Merged changes between main-branch tags "BeforeBranch_VC_1-3" and "UpdateAfterBranch_VC_1-3" that should've been in VC_1-3 branch.
//
// Revision 1.9  2003/01/31 21:05:44  frank
// Added ImageCompression - work in progress
//
// Revision 1.8  2003/01/30 16:58:57  geconomos
// ???
//
// Revision 1.7  2003/01/15 17:12:50  geconomos
// Added Franks' MakeShadowImage method.
//
// Revision 1.6  2003/01/09 19:01:03  geconomos
// Added DrawImage()
//
// Revision 1.5  2002/12/12 15:38:57  geconomos
// Added DrawBackground method.
//
// Revision 1.4  2002/11/18 20:12:32  geconomos
// Headers and footers.
//
// $Header: /CVS/cvsRepository/V3D/src/Viatronix.UI/Utilities.cs,v 1.18.2.1 2009/03/20 20:38:15 mkontak Exp $
// $Id: Utilities.cs,v 1.18.2.1 2009/03/20 20:38:15 mkontak Exp $

#endregion
