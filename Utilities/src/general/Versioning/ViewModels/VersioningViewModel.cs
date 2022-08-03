using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Helpers;
using GalaSoft.MvvmLight.Command;
using System.Xml;
using System.Collections;
using System.Collections.ObjectModel;
using Versioning.Models;

namespace Versioning.ViewModels
{
  public class VersioningViewModel : ViewModelBase
  {

    #region fields

    private VersioningModel m_versioningModel = new VersioningModel();

    private ObservableCollection<ReleaseViewModel> m_revisions = new ObservableCollection<ReleaseViewModel>();
    private ObservableCollection<ProjectViewModel> m_projects = new ObservableCollection<ProjectViewModel>();


    private RelayCommand m_revisionSelected = null;

    #endregion

    
    #region properties



    public ObservableCollection<ReleaseViewModel> Revisions
    { 
      get { return m_revisions; }
      set { m_revisions = value; }
    }


    public RelayCommand RevisionSelected
    {
      get
      {
        if (m_revisionSelected == null)
        {
          m_revisionSelected = new RelayCommand( () => 
          {
            
          } );
        }

        return m_revisionSelected;
      }
    }


    #endregion

    #region methods


    public void Refresh()
    {
      string revisions = DatabaseGateway.Get("revision");

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(revisions);

      foreach (XmlNode node in doc.DocumentElement.SelectNodes("revision"))
      {
        m_revisions.Add(ReleaseViewModel.Create(node));
      }

    }

    #endregion

  }
}
