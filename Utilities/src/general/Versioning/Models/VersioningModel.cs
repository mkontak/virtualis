using System;
using System.Xml;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Versioning.ViewModels;


namespace Versioning.Models
{
  public class VersioningModel
  {

    #region fields

    

    #endregion

    #region properties




    #endregion


    #region methods

    public List<ReleaseViewModel> GetRevisions()
    {
      List<ReleaseViewModel> revisions = new List<ReleaseViewModel>();


      string results = DatabaseGateway.Get("revision");

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(results);

      foreach (XmlNode node in doc.DocumentElement.SelectNodes("revision"))
      {
        revisions.Add(ReleaseViewModel.Create(node));
      }


      return revisions;
    }

    public List<ProjectViewModel> GetProjects(ReleaseViewModel revision)
    {
      List<ProjectViewModel> projects = new List<ProjectViewModel>();


      string results = DatabaseGateway.Get("project", string.Format("<project revision=\"{0}\" />", revision.Id));

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(results);

      foreach (XmlNode node in doc.DocumentElement.SelectNodes("revision"))
      {
        projects.Add(ProjectViewModel.Create(node));
      }


      return projects;
    }



    #endregion


  }
}
