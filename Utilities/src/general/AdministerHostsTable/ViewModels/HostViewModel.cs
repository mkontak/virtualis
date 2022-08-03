// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: HostViewModel.cpp
//
// Description: 
//
// Owner: Kyle R Chalupa (mailto: kchalupa@viatronix.com)

using System;
using System.Xml;
using System.Windows;
using System.Collections.Specialized;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using GalaSoft.MvvmLight;

using Viatronix.Dicom;

namespace Viatronix.Utilities.General.AdministerHostsTable.ViewModels
{
  /// <summary>
  /// Represents a host in the host table.
  /// </summary>
  public class HostViewModel : ViewModelBase, ICloneable
  {

    #region fields

    /// <summary>
    /// The model.
    /// </summary>
    private Host m_model = new Host();

    /// <summary>
    /// The list of attributes.
    /// </summary>
    private ObservableCollection<AttributeViewModel> m_attributes = new ObservableCollection<AttributeViewModel>();

    /// <summary>
    /// Determines if the host has attributes.
    /// </summary>
    private bool m_hasAttributes = false;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    private HostViewModel()
    {
      m_model = new Host();
      m_attributes.CollectionChanged += new NotifyCollectionChangedEventHandler(OnAttributesChangedHandler);
    } // HostViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the name of the host.
    /// </summary>
    public string Name
    {
      get { return m_model.Name; }
      set
      {
        m_model.Name = value;
        RaisePropertyChanged("Name");
      }
    } // Name


    /// <summary>
    /// Gets or sets the server name.
    /// </summary>
    public string Server
    {
      get { return m_model.Server; }
      set
      {
        m_model.Server = value;
        RaisePropertyChanged("Server");
      }
    } // Server


    /// <summary>
    /// Gets or sets the port.
    /// </summary>
    public uint Port
    {
      get { return m_model.Port; }
      set 
      {
        m_model.Port = value;
        RaisePropertyChanged("Port");
      }
    } // Port


    /// <summary>
    /// Gets or sets the AETitle.
    /// </summary>
    public string AETitle
    {
      get { return m_model.AETitle; }
      set
      {
        m_model.AETitle = value;
        RaisePropertyChanged("AETitle");
      }
    } // AETitle


    /// <summary>
    /// Gets or sets if the host can query.
    /// </summary>
    public bool CanQuery
    {
      get { return m_model.Supports(ServiceTypes.QUERY); }
      set
      {
        if (value)
        {
          m_model.AddSupport(ServiceTypes.QUERY);
        }
        else
        {
          m_model.RemoveSupport(ServiceTypes.QUERY);
        }

        RaisePropertyChanged("CanQuery");
      }
    } // CanQuery


    /// <summary>
    /// Gets or sets if the host can retrieve.
    /// </summary>
    public bool CanRetrieve
    {
      get { return m_model.Supports(ServiceTypes.RETRIEVE); }
      set
      {
        if (value)
        {
          m_model.AddSupport(ServiceTypes.RETRIEVE);
        }
        else
        {
          m_model.RemoveSupport(ServiceTypes.RETRIEVE);
        }

        RaisePropertyChanged("CanRetrieve");
      }
    } // CanRetrieve


    /// <summary>
    /// Gets or sets if the host can store.
    /// </summary>
    public bool CanStore
    {
      get { return m_model.Supports(ServiceTypes.STORE); }
      set
      {
        if (value)
        {
          m_model.AddSupport(ServiceTypes.STORE);
        }
        else
        {
          m_model.RemoveSupport(ServiceTypes.STORE);
        }
        RaisePropertyChanged("CanStore");
      }
    } // CanStore


    /// <summary>
    /// Gets or sets the attributes.
    /// </summary>
    public ObservableCollection<AttributeViewModel> Attributes
    {
      get { return m_attributes; }
      set
      {
        m_attributes = value;
        RaisePropertyChanged("Attributes");
      }
    } // Attributes


    /// <summary>
    /// Gets or sets if the host has attributes or not.
    /// </summary>
    public bool HasAttributes
    {
      get { return m_hasAttributes; }
      set
      {
        m_hasAttributes = value;
        RaisePropertyChanged("HasAttributes");
      }
    } // HasAttributes

    #endregion

    #region methods

    /// <summary>
    /// Creates a new host view model.
    /// </summary>
    public static HostViewModel Create()
    {
      return new HostViewModel();
    } // Create()


    /// <summary>
    /// Creates a new host from the node.
    /// </summary>
    /// <param name="host">The host to initialize from.</param>
    /// <returns>A new object based on the host.</returns>
    public static HostViewModel Create(Host host)
    {
      HostViewModel viewModel = new HostViewModel();

      viewModel.m_model = host;
      foreach (HostAttribute attribute in viewModel.m_model.GetAttributes())
      {
        viewModel.m_attributes.Add(AttributeViewModel.Create(attribute));
      }

      return viewModel;
    } // Create( host )


    /// <summary>
    /// Gets the internal object.
    /// </summary>
    /// <returns>The internal object.</returns>
    internal Host GetInternalObject()
    {
      return m_model;
    } // GetInternalObject()


    /// <summary>
    /// Adds the attribute to the internal object.
    /// </summary>
    /// <param name="viewModel">The attribute to add.</param>
    public void AddAttribute(AttributeViewModel viewModel)
    {
      m_model.AddAttribute(viewModel.GetInternalObject());
      Attributes.Add(viewModel);
    } // AddAttribute( viewModel )


    /// <summary>
    /// Updates the current instance to the modified value.
    /// </summary>
    /// <param name="modified">The modified attribute.  Descended from current.</param>
    /// <param name="currentInstance">The attribute's name that is currently in the list.</param>
    public void ModifyAttribute(AttributeViewModel modified, string currentInstance)
    {
      List<HostAttribute> attributes = m_model.GetAttributes();
      for (int i = attributes.Count - 1; i > -1; --i)
      {
        if (attributes[i].Name == currentInstance)
        {
          attributes.RemoveAt(i);
          attributes.Add(modified.GetInternalObject());
        }
      }

      int count = attributes.Count;
      for (int i = count - 1; i > -1; --i)
      {
        m_model.RemoveAttribute(m_model.GetAttributes()[i].Name);

        // Adds to the end of the list.  The loop will end at the front of the new elements.
        m_model.AddAttribute(attributes[i]);
      }

      for (int i = m_attributes.Count - 1; i > -1; --i)
      {
        if (m_attributes[i].Name == currentInstance)
        {
          m_attributes.RemoveAt(i);
          m_attributes.Insert(i, modified);
        }
      }
    } // ModifyAttribute( modified, currentInstance )


    /// <summary>
    /// Removes the selected attribute.
    /// </summary>
    /// <param name="currentInstance">The attribute to remove.</param>
    public void RemoveAttribute(AttributeViewModel currentInstance)
    {
      m_model.RemoveAttribute(currentInstance.Name);
      m_attributes.Remove(currentInstance);
    } // RemoveAttribute( currentInstance )


    /// <summary>
    /// Returns a cloned instance of this object.
    /// </summary>
    /// <returns>A cloned instance of this object.</returns>
    public object Clone()
    {
      HostViewModel model = HostViewModel.Create();
      model.Name = (string)Name.Clone();
      model.Server = (string)Server.Clone();
      model.AETitle = (string)AETitle.Clone();
      model.Port = Port;
      model.CanQuery = CanQuery;
      model.CanStore = CanStore;
      model.CanRetrieve = CanRetrieve;

      foreach (AttributeViewModel attribute in Attributes)
      {
        model.AddAttribute((AttributeViewModel)attribute.Clone());
      }

      return model;
    } // Clone()

    #endregion

    #region event handlers

    /// <summary>
    /// Handles changes to the attributes collection.
    /// </summary>
    /// <param name="sender">The origin of the event.</param>
    /// <param name="args">Arguments to take into account.</param>
    private void OnAttributesChangedHandler(object sender, NotifyCollectionChangedEventArgs args)
    {
      HasAttributes = Attributes.Count > 0;
    } // OnAttributesChangedHandler( sender, args )

    #endregion

  } // class HostViewModel
} // namespace Viatronix.Utilities.General.AdministerHostsTable.ViewModels
