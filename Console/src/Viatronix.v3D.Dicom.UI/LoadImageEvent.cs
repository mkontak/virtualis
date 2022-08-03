using System;
using vxDicom = Viatronix.v3D.Dicom;

namespace Viatronix.v3D.Dicom.UI
{

  /// <summary>
  /// LoadImageEvent arguments contains the 
  /// dicom image loaded
  /// </summary>
  public class LoadingImageEventArgs : System.EventArgs
  {
    /// <summary>
    /// Dicom image loaded
    /// </summary>
    private DicomImage m_image;

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="image">image loaded</param>
    public LoadingImageEventArgs(DicomImage image) { m_image = image; }

    /// <summary>
    /// Returns the dicom image loaded
    /// </summary>
    public DicomImage DicomImage 
    {
      get { return m_image; }
    }
  } // class LoadImageEventArgs

  /// <summary>
  /// Load Image Event Handler
  /// </summary>
  public delegate void LoadingImageEventHandler(object sender, LoadingImageEventArgs args);

}
