
namespace DicomRIAService.Web.Services
{
  using System;
  using System.Configuration;
  using System.Collections.Generic;
  using System.ComponentModel;
  using System.ComponentModel.DataAnnotations;
  using System.Data;
  using System.Diagnostics;
  using System.Linq;
  using System.ServiceModel.DomainServices.EntityFramework;
  using System.ServiceModel.DomainServices.Hosting;
  using System.ServiceModel.DomainServices.Server;
  using DicomRIAService.Web.Models;


  // Implements application logic using the DatasetsEntities context.
  // TODO: Add your application logic to these methods or in additional methods.
  // TODO: Wire up authentication (Windows/ASP.NET Forms) and uncomment the following to disable anonymous access
  // Also consider adding roles to restrict access as appropriate.
  // [RequiresAuthentication]
  [EnableClientAccess()]
  public class DatasetsDomainService : LinqToEntitiesDomainService<DatasetsEntities>
  {

    // TODO:
    // Consider constraining the results of your query method.  If you need additional input you can
    // add parameters to this method or create additional query methods with different names.
    // To support paging you will need to add ordering to the 'Datasets' query.
    public IQueryable<Dataset> GetDatasets()
    {
      this.ObjectContext.Datasets.MergeOption = System.Data.Objects.MergeOption.OverwriteChanges;
      return this.ObjectContext.Datasets.OrderBy( d => d.name );
    }

    public void InsertDataset( Dataset dataset )
    {
      if( ( dataset.EntityState != EntityState.Detached ) )
      {
        this.ObjectContext.ObjectStateManager.ChangeObjectState( dataset, EntityState.Added );
      }
      else
      {
        this.ObjectContext.Datasets.AddObject( dataset );
      }
    }

    public void UpdateDataset( Dataset currentDataset )
    {
      this.ObjectContext.Datasets.AttachAsModified( currentDataset, this.ChangeSet.GetOriginal( currentDataset ) );
    }

    public void DeleteDataset( Dataset dataset )
    {
      if( ( dataset.EntityState == EntityState.Detached ) )
      {
        this.ObjectContext.Datasets.Attach( dataset );
      }
      this.ObjectContext.Datasets.DeleteObject( dataset );
    }

    // TODO:
    // Consider constraining the results of your query method.  If you need additional input you can
    // add parameters to this method or create additional query methods with different names.
    // To support paging you will need to add ordering to the 'Progresses' query.
    public IQueryable<Progress> GetProgresses()
    {
      this.ObjectContext.Progresses.MergeOption = System.Data.Objects.MergeOption.OverwriteChanges;
      return this.ObjectContext.Progresses.OrderBy( p => p.date );
    }


    public void InsertProgress( Progress progress )
    {
      string app = ConfigurationManager.AppSettings[ "externalApp" ];

      ProcessStartInfo info = new ProcessStartInfo( app, string.Format( "-c {0} -s {1} -p {2} -u {3}", progress.aetitle, progress.server, progress.port, progress.uid ));
      Process.Start( info );
    }


    public void DeleteProgress( Progress progress )
    {
      if( ( progress.EntityState == EntityState.Detached ) )
      {
        this.ObjectContext.Progresses.Attach( progress );
      }
      this.ObjectContext.Progresses.DeleteObject( progress );
    }

  }
}


