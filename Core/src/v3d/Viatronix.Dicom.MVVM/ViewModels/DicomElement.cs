using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using Viatronix.Dicom;
using Viatronix.Dicom.Imaging;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Reflection;
using System.Xml;
using System.Windows;
using System.Windows.Media;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Helpers;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Messaging;
using GalaSoft.MvvmLight.Threading;
using Viatronix.Dicom.MVVM.Model;
using System.Windows.Threading;



namespace Viatronix.Dicom.MVVM.ViewModels
{
  public class DicomElement
  {

    #region fields

    private string m_tag;

    private string m_group;

    private string m_VR;

    private string m_value;

    private string m_name;

    private ObservableCollection<DicomElement> m_elements = new ObservableCollection<DicomElement>();

    private int m_level = 0;


    #endregion

    public DicomElement(int level, Element element)
    {
      m_tag = element.Tag;
      m_name = element.Name;
      m_value = element.Value;
      m_VR = element.VR;
      m_group = String.Format("{0:X4}",element.GroupId);
      m_level = level;
    }

    public DicomElement(int level, Group group)
    {
      m_tag = String.Format("{0:X4}", group.Id);
      m_name = group.Description;
      m_value = string.Empty;
      m_VR = string.Empty;
      m_group = m_tag;
      m_level = level;

    }


 
    public string Tag
    {
      get { return m_tag;  }
    }


    public string Group
    {
      get { return m_group;  }
    }
    
    public string VR
    {
      get { return m_VR;  }
    }

    public string Name
    {
      get { return m_name; }
    }

    public string Value
    {
      get { return m_value; }
    }

    public ObservableCollection<DicomElement> Elements
    {
      get { return m_elements; }
    }

    public int Level
    {
      get
      {
          return m_level;
      }
    }



  }
}
