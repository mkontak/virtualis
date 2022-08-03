using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Viatronix.Enterprise.Dicom;
using System.Windows.Input;
using System.Drawing;
using System.Windows.Data;
using System.Windows.Media.Imaging;
using System.Windows.Interop;
using System.Runtime.InteropServices; 



namespace Viatronix.Utilities.Dicom.Viewer.ViewModel
{
  public class ElementListViewModel : ViewModelBase
  {

    #region fields

    private ElementList m_elementList;

    private BitmapSource m_bitmapSource;

    private string m_patientName = string.Empty;

    private string m_studyUid = string.Empty;

    private string m_seriesUid = string.Empty;

#endregion

    public ElementListViewModel(ElementList elementList)
    {
      m_elementList = elementList;
      this.Image = BitmapImageConverter.ToBitmapSource(m_elementList.GetBitmap());
      m_patientName = m_elementList.PatientName;
      m_studyUid = m_elementList.StudyUid;
      m_seriesUid = m_elementList.SeriesUid;
    }


    public BitmapSource Image
    {
      get { return m_bitmapSource; }
      set
      {

        m_bitmapSource = value;

        base.OnPropertyChanged("Image");

      }
    }


    public string PatientName
    {
      get { return m_patientName;  }
    }

    public string StudyUid
    {
      get { return m_studyUid;  }
    }

    public string seriesUid
    {

      get { return m_seriesUid;  }
    }

  }
}
