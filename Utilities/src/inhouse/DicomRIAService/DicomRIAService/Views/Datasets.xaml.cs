using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;
using System.Windows.Navigation;

using DicomRIAService.Web.Models;

namespace DicomRIAService.Views
{
  public partial class Datasets : Page
  {
    public Datasets()
    {
      InitializeComponent();
    }

    // Executes when the user navigates to this page.
    protected override void OnNavigatedTo( NavigationEventArgs e )
    {
    }

    private void OnSendHandler( object sender, RoutedEventArgs e )
    {
      //SendWindow send = new SendWindow();
      //send.Dataset = DatasetsGrid.SelectedItem as Dataset;
      //send.Closed += new EventHandler( OnSendWindowClosedHandler );
      //send.Show();

      
    }


    private void OnSendWindowClosedHandler( object sender, EventArgs args )
    {
      //SendWindow window = sender as SendWindow;
      //if( window.Dataset == null )
      //  return;

      //try
      //{
      //  Progress progress = new Progress();
      //  progress.uid = window.Dataset.uid;
      //  progress.aetitle = window.AeTitle.Text;
      //  progress.server =  window.Server.Text;
      //  progress.port = int.Parse( window.Port.Text );
      //  progress.message = "1";

      //  DicomRIAService.Web.Services.DatasetsDomainContext ctx = (DicomRIAService.Web.Services.DatasetsDomainContext) ProgressSource.DomainContext;
      //  ctx.Progresses.Add( progress );
      //  ctx.SubmitChanges();

      //  ctx.Progresses.Remove( progress );
      //}
      //catch( Exception )
      //{
      //}

    }

  }
}
