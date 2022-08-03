using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using Viatronix.Dicom;
using Viatronix.UI;


namespace Viatronix.v3D.Dicom.UI
{


  /// <summary>
  /// Summary description for DicomHeaderControl.
  /// </summary>
  public class DicomHeaderControl : System.Windows.Forms.UserControl
  {

    #region variables 

    private ArrayList m_expandedItems = new ArrayList();

    /// <summary>
    /// Colors for the group
    /// </summary>
    private Dictionary<string, Group> m_groups = new Dictionary<string, Group>();
    

    /// <summary>
    /// Last group searched
    /// </summary>
    private string m_lastGroup = string.Empty;

    /// <summary>
    /// Last search performed
    /// </summary>
    private string m_lastSearch = string.Empty;

    /// <summary>
    /// Collection of elements 
    /// </summary>
    private Viatronix.Dicom.ElementCollection m_elements;


    /// <summary>
    /// Dataset
    /// </summary>
    private Viatronix.Dicom.Image m_image ;

 
    /// <summary>
    /// Enable the context menu
    /// </summary>
    //bool m_enableContextMenu = false;

    /// <summary>
    /// Element list filter
    /// </summary>
    private string m_filter = String.Empty;

    /// <summary>
    /// Data grids title
    /// </summary>
    private System.Windows.Forms.ToolTip m_toolTip;
    private System.Windows.Forms.ContextMenu m_contextMenu;
    private System.Windows.Forms.MenuItem m_menuItemSaveElementAs;
    private System.Windows.Forms.MenuItem m_menuItemSaveElementAsTxt;
    private System.Windows.Forms.MenuItem m_menuItemSaveElementAsXml;
    private System.Windows.Forms.ImageList m_imageList;
    private Viatronix.UI.TreeListView m_treeListView;
    private System.Windows.Forms.ColumnHeader m_columnHeaderElement;
    private System.Windows.Forms.ColumnHeader m_columnHeaderValue;
    private System.Windows.Forms.ColumnHeader m_columnHeaderType;
    private ColumnHeader m_columnHeaderDescription;
    private MenuItem m_menuItemShowPrivateGroups;
    private MenuItem m_menuItemSeperator;
    private System.ComponentModel.IContainer components;

    #endregion

    #region constructor and destructor

    public DicomHeaderControl()
    {
      // This call is required by the Windows.Forms Form Designer.
      InitializeComponent();

      m_groups = (Dictionary<string, Group>)System.Configuration.ConfigurationManager.GetSection("groupSettings");
      
    }

    /// <summary> 
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose( bool disposing )
    {
      if( disposing )
      {
        if(components != null)
        {
          components.Dispose();
        }
      }
      base.Dispose( disposing );
    }

    #endregion

    #region Component Designer generated code

    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DicomHeaderControl));
      Viatronix.UI.TreeListViewItemCollection.TreeListViewItemCollectionComparer treeListViewItemCollectionComparer1 = new Viatronix.UI.TreeListViewItemCollection.TreeListViewItemCollectionComparer();
      this.m_contextMenu = new System.Windows.Forms.ContextMenu();
      this.m_menuItemShowPrivateGroups = new System.Windows.Forms.MenuItem();
      this.m_menuItemSeperator = new System.Windows.Forms.MenuItem();
      this.m_menuItemSaveElementAs = new System.Windows.Forms.MenuItem();
      this.m_menuItemSaveElementAsTxt = new System.Windows.Forms.MenuItem();
      this.m_menuItemSaveElementAsXml = new System.Windows.Forms.MenuItem();
      this.m_toolTip = new System.Windows.Forms.ToolTip(this.components);
      this.m_treeListView = new Viatronix.UI.TreeListView();
      this.m_columnHeaderElement = new System.Windows.Forms.ColumnHeader();
      this.m_columnHeaderDescription = new System.Windows.Forms.ColumnHeader();
      this.m_columnHeaderType = new System.Windows.Forms.ColumnHeader();
      this.m_columnHeaderValue = new System.Windows.Forms.ColumnHeader();
      this.m_imageList = new System.Windows.Forms.ImageList(this.components);
      this.SuspendLayout();
      // 
      // m_contextMenu
      // 
      this.m_contextMenu.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.m_menuItemShowPrivateGroups,
            this.m_menuItemSeperator,
            this.m_menuItemSaveElementAs});
      // 
      // m_menuItemShowPrivateGroups
      // 
      this.m_menuItemShowPrivateGroups.Index = 0;
      resources.ApplyResources(this.m_menuItemShowPrivateGroups, "m_menuItemShowPrivateGroups");
      this.m_menuItemShowPrivateGroups.Click += new System.EventHandler(this.OnShowPrivateGroupsClick);
      // 
      // m_menuItemSeperator
      // 
      this.m_menuItemSeperator.Index = 1;
      resources.ApplyResources(this.m_menuItemSeperator, "m_menuItemSeperator");
      // 
      // m_menuItemSaveElementAs
      // 
      this.m_menuItemSaveElementAs.Index = 2;
      this.m_menuItemSaveElementAs.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
            this.m_menuItemSaveElementAsTxt,
            this.m_menuItemSaveElementAsXml});
      resources.ApplyResources(this.m_menuItemSaveElementAs, "m_menuItemSaveElementAs");
      // 
      // m_menuItemSaveElementAsTxt
      // 
      this.m_menuItemSaveElementAsTxt.Index = 0;
      resources.ApplyResources(this.m_menuItemSaveElementAsTxt, "m_menuItemSaveElementAsTxt");
      // 
      // m_menuItemSaveElementAsXml
      // 
      this.m_menuItemSaveElementAsXml.Index = 1;
      resources.ApplyResources(this.m_menuItemSaveElementAsXml, "m_menuItemSaveElementAsXml");
      // 
      // m_treeListView
      // 
      this.m_treeListView.BackColor = System.Drawing.Color.White;
      this.m_treeListView.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.m_columnHeaderElement,
            this.m_columnHeaderDescription,
            this.m_columnHeaderType,
            this.m_columnHeaderValue});
      treeListViewItemCollectionComparer1.Column = 0;
      treeListViewItemCollectionComparer1.SortOrder = System.Windows.Forms.SortOrder.Ascending;
      this.m_treeListView.Comparer = treeListViewItemCollectionComparer1;
      resources.ApplyResources(this.m_treeListView, "m_treeListView");
      this.m_treeListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
      this.m_treeListView.LabelEdit = true;
      this.m_treeListView.MultiSelect = false;
      this.m_treeListView.Name = "m_treeListView";
      this.m_treeListView.SmallImageList = this.m_imageList;
      this.m_toolTip.SetToolTip(this.m_treeListView, resources.GetString("m_treeListView.ToolTip"));
      this.m_treeListView.UseCompatibleStateImageBehavior = false;
      this.m_treeListView.AfterCollapse += new Viatronix.UI.TreeListViewEventHandler(this.OnAfterCollapse);
      this.m_treeListView.AfterExpand += new Viatronix.UI.TreeListViewEventHandler(this.OnAfterExpand);
      // 
      // m_columnHeaderElement
      // 
      resources.ApplyResources(this.m_columnHeaderElement, "m_columnHeaderElement");
      // 
      // m_columnHeaderDescription
      // 
      resources.ApplyResources(this.m_columnHeaderDescription, "m_columnHeaderDescription");
      // 
      // m_columnHeaderType
      // 
      resources.ApplyResources(this.m_columnHeaderType, "m_columnHeaderType");
      // 
      // m_columnHeaderValue
      // 
      resources.ApplyResources(this.m_columnHeaderValue, "m_columnHeaderValue");
      // 
      // m_imageList
      // 
      this.m_imageList.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
      resources.ApplyResources(this.m_imageList, "m_imageList");
      this.m_imageList.TransparentColor = System.Drawing.Color.Transparent;
      // 
      // DicomHeaderControl
      // 
      this.AllowDrop = true;
      this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(143)))), ((int)(((byte)(180)))));
      this.ContextMenu = this.m_contextMenu;
      this.Controls.Add(this.m_treeListView);
      resources.ApplyResources(this, "$this");
      this.ForeColor = System.Drawing.Color.Navy;
      this.Name = "DicomHeaderControl";
      this.FontChanged += new System.EventHandler(this.OnFontChanged);
      this.Resize += new System.EventHandler(this.OnResize);
      this.BackColorChanged += new System.EventHandler(this.OnBackColorChange);
      this.ResumeLayout(false);

    }
    #endregion

    #region properties

    //public string Title
    //{
     // set { m_groupBoxDicomInformation.Text = value; }
     // get { return m_groupBoxDicomInformation.Text; }
    //}

    public bool EnableContextMenu
    {
      get { return (this.ContextMenu != null); }
      set { this.ContextMenu = (value == true ? m_contextMenu : null); }
    }

    #endregion

    #region event handlers


    private void OnFontChanged(object sender, System.EventArgs e)
    {
      this.m_treeListView.Font = this.Font;
   }

    private void OnBackColorChange(object sender, System.EventArgs e)
    {
      this.m_treeListView.BackColor = this.BackColor;
    }

    private void OnResize(object sender, System.EventArgs e )
    {

 
      

      int totalWidth = this.m_treeListView.Width - 5;

      //this.m_columnHeaderElement.Width = (int)( 0.4951f * (float)totalWidth );

      int typeWidth = (int)( 0.0873f * (float)totalWidth);
      this.m_columnHeaderType.Width = ( typeWidth < 45 ? typeWidth : 45);

      this.m_columnHeaderValue.Width = totalWidth - ( this.m_columnHeaderElement.Width + this.m_columnHeaderType.Width );

    }


    //private void OnSaveElementsAsXmlClick(object sender, System.EventArgs e)
    //{

    //  ElementSaveAs saveAs = new ElementSaveAs(".xml");
    //  saveAs.Text = "Save Elements As Xml";
    //  saveAs.Filename = System.IO.Path.ChangeExtension(m_elementList.DicomFileName,vxIO.FileExtensions.Xml);

    //  if ( saveAs.ShowDialog() == DialogResult.OK )
    //  {
    //    string fileName = System.IO.Path.ChangeExtension(saveAs.Filename,vxIO.FileExtensions.Xml);

    //    // TODO
    //    //m_elementList.Dataset.WriteXml(fileName, System.Data.XmlWriteMode.IgnoreSchema);
    //  }
    //}

    //private void OnSaveElementsAsTextClick(object sender, System.EventArgs e)
    //{

    //  ElementSaveAs saveAs = new ElementSaveAs(".txt");
    //  saveAs.Text = "Save Elements As Text";
    //  saveAs.Filename = System.IO.Path.ChangeExtension(m_elementList.DicomFileName,vxIO.FileExtensions.Text);

    //  if ( saveAs.ShowDialog() == DialogResult.OK )
    //  {
    //    string fileName = System.IO.Path.ChangeExtension(saveAs.Filename,vxIO.FileExtensions.Text);

    //    m_elementList.WriteElements(fileName);
    //  }
 
    //}



    #endregion

    #region methods


    /// <summary>
    /// Performs the search on the dicom header
    /// </summary>
    /// <param name="filter">Filter to be used on the dataset</param>
    private void PerformSearch(string filter)
    {

      if ( filter != "" )
      {
        //DataView dataView = (DataView)m_treeListView.DataSource;

        //if ( dataView != null )
        //{
        //  dataView.RowFilter = filter;
        //}
      }

    } // PerformSearch()


    /// <summary>
    /// Set the element list using a dicom file
    /// </summary>
    /// <param name="dicomFile">Dicom file tobe read</param>
    public void SetElements(string dicomFile)
    {
      SetElements(dicomFile, false);
    }




    /// <summary>
    /// Set the element list using a dicom file
    /// </summary>
    /// <param name="dicomFile">Dicom file tobe read</param>
    public void SetElements(string dicomFile, bool savePreviousSettings)
    {

      try
      {
        SetElements((Viatronix.Dicom.Image)Viatronix.Dicom.ImageFactory.Create(dicomFile, ParameterType.FILE), savePreviousSettings);
      }
      catch ( Exception ex)
      {
        Viatronix.UI.MessageBox.ShowError(this.Parent, ex);        
      }
      

    }

    /// <summary>
    /// Sets the element list using the supplied element list
    /// </summary>
    /// <param name="image">The dicom image.</param>
    public void SetElements(Viatronix.Dicom.Image image)
    {
      SetElements(image, false);
    }


    /// <summary>
    /// Sets the element list using the supplied element list
    /// </summary>
    /// <param name="elementList">Element list</param>
    public void SetElements(Viatronix.Dicom.Image image, bool savePreviousSettings )
    {

      m_image = image;
      m_elements = image.GetElements();
      
      LoadTreeListView();

    }
  
 
     

    /// <summary>
    /// Loads the tree list view with the dicom header information
    /// </summary>
    /// <param name="search"></param>
    private void LoadTreeListView()
    {
           
      /// Clear 
      m_treeListView.Items.Clear();

      LoadElements(m_treeListView.Items, m_elements);


    } // LoadTreeListView(string search)


    /// <summary>
    /// Loads the elements into the tree list view. This is called recursivly
    /// </summary>
    /// <param name="elements"></param>
    /// <param name="search"></param>
    private void LoadElements(TreeListViewItemCollection col, Viatronix.Dicom.ElementCollection elements)
    {

      /// Clear 
      //Hashtable groupList = new Hashtable();


      //foreach (IElement element in elements.Elements.Values)
      //{
      //  string description = element.Name.ToLower();

      //  string groupId    = String.Format("{0:X4}",element.GroupId).ToLower();
      //  //if ( ! this.m_menuItemShowPrivateG

      //  string groupDesc = groupId;
      //  Color groupColor = System.Drawing.Color.Black;

      //  if (m_groups.ContainsKey(groupId))
      //  {
      //    groupDesc = m_groups[groupId].Description;
      //    groupColor = m_groups[groupId].Color;
      //  }
  
      //  TreeListViewItem tlvGroup = (TreeListViewItem)groupList[groupId];

      //  if (tlvGroup == null)
      //  {
      //    tlvGroup = new TreeListViewItem(groupId);
      //    tlvGroup.ForeColor = groupColor;
   

      //    if (m_expandedItems.Contains(groupId)) 
      //      tlvGroup.Expand();

      //    tlvGroup.SubItems.Add(groupDesc);

      //    foreach (ListViewItem.ListViewSubItem sub in tlvGroup.SubItems) 
      //    {
      //      sub.ForeColor = groupColor; 
      //    }

      //    groupList.Add(groupId, tlvGroup);
      //    col.Add(tlvGroup);
      //  }


      //  TreeListViewItem item = new TreeListViewItem(String.Format("{0:X4}", element.ElementId).ToLower()); 
      //  item.SubItems.Add(element.Name);
      //  item.SubItems.Add(System.Convert.ToString(element.VR));
      //  item.SubItems.Add(element.Value);

      //  foreach (ListViewItem.ListViewSubItem sub in item.SubItems)
      //  {
      //    sub.ForeColor = groupColor;
      //  }

      //  tlvGroup.Items.Add(item);

      //  // Kontak told me to do this.  
      //  //Dictionary<int, IElement> elms = element.Elements;

      //  //if (elms.Count > 0)
      //  //  LoadElements(item.Items, elms);

      //}

    }







    #endregion

    private void OnShowPrivateGroupsClick(object sender, EventArgs e)
    {
      m_menuItemShowPrivateGroups.Checked = !m_menuItemShowPrivateGroups.Checked;

      LoadTreeListView();
    }

    private void OnSearchClick(object sender, EventArgs e)
    {
      DicomHeaderSearchDialog search = new DicomHeaderSearchDialog(this.m_image);

      search.ShowDialog();

    }

    /// <summary>
    /// After an expand adds the state to the list
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnAfterExpand(object sender, Viatronix.UI.TreeListViewEventArgs e)
    {

      TreeListViewItem item = e.Item;

      string key = item.Text;

      while (item.Parent != null)
      {
        key = key + "," + item.Parent.Text;
        item = item.Parent;
      }


      if (!m_expandedItems.Contains(key))
      {
        m_expandedItems.Add(key);
      }
    }// OnAfterExpand(object sender, Viatronix.UI.TreeListViewEventArgs e)

    /// <summary>
    /// After a collapse remove the state from the list
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    private void OnAfterCollapse(object sender, Viatronix.UI.TreeListViewEventArgs e)
    {
      TreeListViewItem item = e.Item;

      string key = item.Text;

      while (item.Parent != null)
      {
        key = key + "," + item.Parent.Text;
        item = item.Parent;
      }


      if (m_expandedItems.Contains(key))
      {
        m_expandedItems.Remove(key);
      }

    } // OnAfterCOllapse(object sender, Viatronix.UI.TreeListViewEventArgs e)



    
  }


  public class ListViewGroupSorter : IComparer
  {
    private SortOrder m_order;

    // Stores the sort order.
    public ListViewGroupSorter(SortOrder theOrder)
    {
      m_order = theOrder;
    }

    // Compares the groups by header value, using the saved sort
    // order to return the correct value.
    public int Compare(object x, object y)
    {
      int result = String.Compare(((System.Windows.Forms.ListViewGroup)x).Header, ((System.Windows.Forms.ListViewGroup)y).Header, true);

      if (m_order == SortOrder.Ascending)
      {
        return result;
      }
      else
      {
        return -result;
      }
    }
  }

}
