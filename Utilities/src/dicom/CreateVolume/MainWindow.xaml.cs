using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Viatronix.Utilities.UI;


namespace Viatronix.Utilities.Dicom.CreateVolume
{
  
  /// <summary>
  /// Interaction logic for MainWindow.xaml
  /// </summary>
  public partial class MainWindow : Window
  {

    private ViewModel.CreateVolumeViewModel m_vm = new ViewModel.CreateVolumeViewModel();


    public MainWindow()
    {

      InitializeComponent();


      DataContext = m_vm;
    }


    private void OnPathButtonClick(object sender, RoutedEventArgs e)
    {
      FolderBrowserDialogEx folderBrowser = new FolderBrowserDialogEx();
      folderBrowser.SelectedPath = textBoxPath.Text;
      folderBrowser.Description = "Dicom Folder";
      folderBrowser.ShowBothFilesAndFolders = true;
      folderBrowser.ShowEditBox = true;
      folderBrowser.ShowNewFolderButton = false;
      folderBrowser.DontIncludeNetworkFoldersBelowDomainLevel = true;

      folderBrowser.ShowDialog();

      ((ViewModel.CreateVolumeViewModel)this.DataContext).Path = folderBrowser.SelectedPath;

    }

    private void OnVolumeFileButtonClick(object sender, RoutedEventArgs e)
    {
      FolderBrowserDialogEx folderBrowser = new FolderBrowserDialogEx();
      folderBrowser.Description = "Volume Folder";
      folderBrowser.SelectedPath = textBoxVolumeFile.Text;
      folderBrowser.ShowBothFilesAndFolders = false;
      folderBrowser.ShowEditBox = true;
      folderBrowser.ShowNewFolderButton = true;
      folderBrowser.DontIncludeNetworkFoldersBelowDomainLevel = true;

      folderBrowser.ShowDialog();

      ((ViewModel.CreateVolumeViewModel)this.DataContext).VolumeFile = folderBrowser.SelectedPath;

    }

    private void OnClosing(object sender, System.ComponentModel.CancelEventArgs e)
    {
      m_vm.CancelCommand.Execute(this);
    }

  }

}
