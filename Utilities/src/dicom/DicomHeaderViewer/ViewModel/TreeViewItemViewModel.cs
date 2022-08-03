using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using Viatronix.Enterprise.Dicom;


namespace Viatronix.Utilities.Dicom.Viewer.ViewModel
{

  /// <summary>
  /// Base class for all ViewModel classes displayed by TreeViewItems.  
  /// This acts as an adapter between a raw data object and a TreeViewItem.
  /// </summary>
  public class TreeViewItemViewModel : ViewModelBase
  {
    #region fields

    private ObservableCollection<TreeViewItemViewModel> m_children = new ObservableCollection<TreeViewItemViewModel>();

    private TreeViewItemViewModel m_parent;
    
     bool m_isSelected;

    protected string m_header;

    #endregion 

    #region Constructors

    protected TreeViewItemViewModel(TreeViewItemViewModel parent, string header)
    {
      m_header = header;
      m_parent = parent;

      m_children = new ObservableCollection<TreeViewItemViewModel>();

    }

    // This is used to create the DummyChild instance.
    private TreeViewItemViewModel()
    {
    }

    #endregion // Constructors


    #region properties

    /// <summary>
    /// Returns the logical child items of this object.
    /// </summary>
    public ObservableCollection<TreeViewItemViewModel> Children
    {
      get { return m_children; }
    }

 
    /// <summary>
    /// Gets/sets whether the TreeViewItem 
    /// associated with this object is selected.
    /// </summary>
    public bool IsSelected
    {
      get { return m_isSelected; }
      set
      {
        if (value != m_isSelected)
        {
          m_isSelected = value;
          this.OnPropertyChanged("IsSelected");
        }
      }
    }

    public string Header
    {
      get { return m_header;  }
      set { m_header = value; }

    }

    public TreeViewItemViewModel Parent
    {
      get { return m_parent; }
    }

    #endregion // IsSelected

    #region methods

    /// <summary>
    /// Invoked when the child items need to be loaded on demand.
    /// Subclasses can override this to populate the Children collection.
    /// </summary>
    public virtual TreeViewItemViewModel AddChild(string header)
    {

      foreach ( TreeViewItemViewModel child in m_children )
      {
        if ( child.Header == header  )
        {
          return child;
       }
      }

      TreeViewItemViewModel chld = new TreeViewItemViewModel(this, header);

      m_children.Add(chld);

      return chld;
    }

    /// <summary>
    /// Invoked when the child items need to be loaded on demand.
    /// Subclasses can override this to populate the Children collection.
    /// </summary>
    public virtual TreeViewItemViewModel AddChild(TreeViewItemViewModel item)
    {
      foreach (TreeViewItemViewModel child in m_children)
      {
        if (item.Header == child.Header)
        {
          return child;
        }
      }

      m_children.Add(item);

      return item;
    }




    #endregion // LoadChildren



   }
}
