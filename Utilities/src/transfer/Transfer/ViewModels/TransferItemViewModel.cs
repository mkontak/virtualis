using System;
using System.IO;
using System.Xml;
using System.Windows;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using GalaSoft.MvvmLight;

using Viatronix.Enterprise.Entities;

namespace Viatronix.Utilities.Transfer.ViewModels
{
  /// <summary>
  /// The items to be transferred.
  /// </summary>
  public class TransferItemViewModel : ViewModelBase, ICloneable
  {

    #region events

    /// <summary>
    /// The anonymization state has changed.
    /// </summary>
    public event RoutedEventHandler AnonymizeChanged;

    #endregion

    #region fields

    /// <summary>
    /// The name of the transfer item.
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// The application this item applies to.  * means any.
    /// </summary>
    private string m_application = "*";

    /// <summary>
    /// The series type this item applies to.
    /// </summary>
    private string m_seriesType = string.Empty;

    /// <summary>
    /// The folder inside the series to search for applicible files.
    /// </summary>
    private string m_searchInFolder = string.Empty;

    /// <summary>
    /// A comma seperated list of file types this item applies to.  
    /// </summary>
    private string m_formats = string.Empty;

    /// <summary>
    /// Is this item selected for transfer?
    /// </summary>
    private bool m_isSelected = true;

    /// <summary>
    /// Is this transfer item available if anonymization is enabled.
    /// </summary>
    private bool m_isDisabledAnonyimization = false;

    /// <summary>
    /// Is this item required for transfer?
    /// </summary>
    private bool m_isRequired = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public TransferItemViewModel()
    {
    } // TransferItemViewModel()
    
    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the name of the transfer item.
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set
      {
        m_name = value;
        RaisePropertyChanged("Name");
      }
    } // Name


    /// <summary>
    /// Gets the directory in the series to search.
    /// </summary>
    public string SearchInFolder
    {
      get { return m_searchInFolder; }
    } // SearchInFolder


    /// <summary>
    /// Gets or sets if this item is selected for transfer.
    /// </summary>
    public bool IsSelected
    {
      get { return m_isSelected; }
      set
      {
        m_isSelected = value;
        RaisePropertyChanged("IsSelectedForTransfer");
      }
    } // IsSelected


    /// <summary>
    /// Gets or sets if this 
    /// </summary>
    public bool IsDisabledAnonymization
    {
      get { return m_isDisabledAnonyimization; }
      set
      {
        m_isDisabledAnonyimization = value;
        RaisePropertyChanged("IsDisabledAnonymization");
      }
    } // IsDisabledAnonymization


    /// <summary>
    /// Gets or sets if this item is required for transfer.
    /// </summary>
    public bool IsRequired
    {
      get { return m_isRequired; }
      set
      {
        m_isRequired = value;
        RaisePropertyChanged("IsRequired");
      }
    } // IsRequired

    #endregion

    #region methods

    /// <summary>
    /// Is this item a complete match for the series.
    /// </summary>
    /// <param name="series">The series to check.</param>
    /// <returns>True if the item is the best match for the provided series, false otherwise.</returns>
    public bool IsCompleteMatch(Series series)
    {
      if (IsTypeMatch(series.Type) && m_application.ToLower() == series.Application.ToLower())
      {
        string url = series.Url;
        if (m_searchInFolder != string.Empty)
        {
          url = Path.Combine(series.Url, m_searchInFolder);
        }

        if (Directory.Exists(url))
        {
          foreach (string format in m_formats.Split(','))
          {
            if (Directory.GetFiles(url, format).Length > 0)
            {
              return true;
            }
          }
        }
      }

      return false;
    } // IsCompleteMatch( series )


    /// <summary>
    /// Checks to see if this item applies to the provided series.
    /// </summary>
    /// <param name="series">The series to check.</param>
    /// <returns>True if the item applies to the provided series, false otherwise.</returns>
    public bool IsMatch(Series series)
    {
      if (IsTypeMatch(series.Type) && (m_application.ToLower() == series.Application.ToLower() || m_application == "*"))
      {
        string url = series.Url;
        if(m_searchInFolder != string.Empty)
        {
          url = Path.Combine(series.Url, m_searchInFolder);
        }

        if(Directory.Exists(url))
        {
          foreach(string format in m_formats.Split(','))
          {
            if(Directory.GetFiles(url, format).Length > 0)
            {
              return true;
            }
          }
        }
      }

      return false;
    } // IsMatch( series )


    /// <summary>
    /// Returns a clone of this object.  Necessary as selecting different series checkbox states shouldn't be shared.
    /// </summary>
    /// <returns>A clone of this object.</returns>
    public object Clone()
    {
      TransferItemViewModel cloneItem = new TransferItemViewModel();

      cloneItem.Name = (string)Name.Clone();
      cloneItem.m_application = (string)m_application.Clone();
      cloneItem.m_seriesType = (string)m_seriesType.Clone();
      cloneItem.m_searchInFolder = (string)m_searchInFolder.Clone();
      cloneItem.m_formats = (string)m_formats.Clone();
      cloneItem.m_searchInFolder = (string)m_searchInFolder.Clone();
      cloneItem.IsSelected = IsSelected;
      cloneItem.IsRequired = IsRequired;
      cloneItem.IsDisabledAnonymization = IsDisabledAnonymization;

      return cloneItem;
    } // Clone()


    /// <summary>
    /// Creates an transfer item from the xml.
    /// </summary>
    /// <param name="itemNode"></param>
    /// <returns></returns>
    public static TransferItemViewModel Create(XmlNode itemNode)
    {
      if (itemNode.Name == "item")
      {
        TransferItemViewModel item = new TransferItemViewModel();

        item.Name = itemNode.Attributes["name"].Value;
        item.m_application = itemNode.Attributes["app"].Value;
        item.m_seriesType = itemNode.Attributes["series"].Value;
        item.m_searchInFolder = itemNode.Attributes["searchInFolder"].Value;
        item.m_formats = itemNode.Attributes["formats"].Value;
        item.IsSelected = System.Convert.ToBoolean(itemNode.Attributes["isChecked"].Value);
        item.IsRequired = System.Convert.ToBoolean(itemNode.Attributes["isRequired"].Value);

        if (itemNode.Attributes["isDisabledAnonymization"] != null)
        {
          item.IsDisabledAnonymization = System.Convert.ToBoolean(itemNode.Attributes["isDisabledAnonymization"].Value);
        }
        else
        {
          item.IsDisabledAnonymization = false;
        }


        return item;
      }
      else
      {
        throw new ArgumentException("Supplied node does not meet expectations.");
      }
    } // Create( itemNode )

    
    /// <summary>
    /// Retrieves all files that apply to the item.
    /// </summary>
    /// <param name="seriesDirectory">The series base directory.</param>
    /// <returns>The list of files that apply to this item.</returns>
    public string[] RetrieveFiles(string seriesDirectory)
    {
      string searchDirectory = seriesDirectory;
      if(m_searchInFolder.Trim() != string.Empty && Directory.Exists(Path.Combine(seriesDirectory, m_searchInFolder)))
      {
        searchDirectory = Path.Combine(seriesDirectory, m_searchInFolder);
      }

      List<string> files = new List<string>();
      foreach(string format in m_formats.Split(','))
      {
        foreach(string file in Directory.GetFiles(searchDirectory, format))
        {
          files.Add(file);
        }
      }

      return files.ToArray();
    } // RetrieveFiles( seriesDirectory )


    /// <summary>
    /// Is the target type a match for the series type.
    /// </summary>
    /// <param name="seriesType">The series type.</param>
    /// <returns>True if the target type is a match for the series type.</returns>
    private bool IsTypeMatch(SeriesTypes seriesType)
    {
      if(seriesType == SeriesTypes.Dicom)
      {
        return m_seriesType.ToLower() == "dicom";
      }
      else if(seriesType == SeriesTypes.Preprocessed)
      {
        return m_seriesType.ToLower() == "preprocessed";
      }
      else if(seriesType == SeriesTypes.Cad)
      {
        return m_seriesType.ToLower() == "cad";
      }
      else if (seriesType == SeriesTypes.Session)
      {
        return m_seriesType.ToLower() == "session";
      }

      return false;
    } // IsTypeMatch( seriesType )

    #endregion

  } // class TransferItemViewModel

  #region extension methods

  /// <summary>
  /// Extensions for a List<TransferItemExtensions>.
  /// </summary>
  public static class TransferItemExtensions
  {

    #region methods

    /// <summary>
    /// Checks if a list contains a named item.
    /// </summary>
    /// <param name="items">The list of items.</param>
    /// <param name="name">The name of the item.</param>
    /// <returns>True if the list contains name, false otherwise.</returns>
    public static bool Contains(this IEnumerable<TransferItemViewModel> items, string name)
    {
      foreach (TransferItemViewModel item in items)
      {
        if (item.Name.ToLower() == name.ToLower())
        {
          return true;
        }
      }

      return false;
    } // Contains(this List<TransferItemViewModel>, name )


    /// <summary>
    /// Finds the item by name in the list.
    /// </summary>
    /// <param name="items">The list of items.</param>
    /// <param name="name">The name of the item.</param>
    /// <returns>The item with provided name, otherwise null.</returns>
    public static TransferItemViewModel Find(this IEnumerable<TransferItemViewModel> items, string name)
    {
      foreach (TransferItemViewModel item in items)
      {
        if (item.Name.ToLower() == name.ToLower())
        {
          return item;
        }
      }

      return null;
    } // Find( items, name )

    #endregion

  } // class TransferItemExtensions

  #endregion

} // namespace Viatronix.Utilities.Transfer.ViewModels
