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

  public class MultiImagePageViewModel : Viatronix.UI.ViewModels.ViewModelBase
  {

    #region fields

 
    /// <summary>
    /// MultiImage Page
    /// </summary>
    private ObservableCollection<ImageViewModel> m_images = new ObservableCollection<ImageViewModel>();

    private int m_pageNumber = 1;

    #endregion

    public MultiImagePageViewModel(int pageNumber)
    {
      m_pageNumber = pageNumber;
    }

    public override int GetHashCode()
    {
      return m_pageNumber;
    }

    public override bool Equals(object obj)
    {
      if (obj is MultiImagePageViewModel)
        return Equals((MultiImagePageViewModel)obj);
      else
        return false;
    }


    public bool Equals(MultiImagePageViewModel pvm)
    {
      return (pvm.m_pageNumber == m_pageNumber);
    }

    #region properties

    public int PageNumber
    {
      get { return m_pageNumber; }
      set { Set<int>("PageNumber", ref m_pageNumber, value); }
    }

    public ObservableCollection<ImageViewModel> Images
    {
      get { return m_images; }
    }

 
    #endregion

  } // class 

}