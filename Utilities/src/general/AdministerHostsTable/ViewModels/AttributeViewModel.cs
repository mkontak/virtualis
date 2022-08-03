// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: AttributeViewModel.cpp
//
// Description: 
//
// Owner: Kyle R Chalupa (mailto: kchalupa@viatronix.com)

using System;
using System.Xml;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;

using Viatronix.Dicom;

namespace Viatronix.Utilities.General.AdministerHostsTable.ViewModels
{
  /// <summary>
  /// Represents a attribute that a host may have.
  /// </summary>
  public class AttributeViewModel : ViewModelBase, ICloneable
  {

    #region fields

    /// <summary>
    /// The model.
    /// </summary>
    private HostAttribute m_model = null;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    private AttributeViewModel()
    {
      m_model = new HostAttribute();
    } // AttributeViewModel()

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the name of the attribute.
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
    /// Gets or sets the value.
    /// </summary>
    public string Value
    {
      get { return m_model.Count > 0 ? m_model.get_Value(0) : string.Empty; }
      set
      {
        if (m_model.Count > 0)
        {
          m_model.set_Value(0, value);
        }
        else
        {
          m_model.AddValue(value);
        }

        RaisePropertyChanged("Value");
      }
    } // Value

    #endregion

    #region methods

    /// <summary>
    /// Creates a new attribute view model.
    /// </summary>
    public static AttributeViewModel Create()
    {
      return new AttributeViewModel();
    } // Create()


    /// <summary>
    /// Creates a attribute view mdoel from the host attribute.
    /// </summary>
    /// <param name="attribute">The data to create from.</param>
    public static AttributeViewModel Create(HostAttribute attribute)
    {
      AttributeViewModel viewModel = new AttributeViewModel();

      viewModel.m_model = attribute;

      return viewModel;
    } // Create( attribute )


    /// <summary>
    /// Gets the internal object.
    /// </summary>
    /// <returns>The internal object.</returns>
    internal HostAttribute GetInternalObject()
    {
      return m_model;
    } // GetInternalObject()


    /// <summary>
    /// Returns an instance of this object.
    /// </summary>
    /// <returns>An instance of this object.</returns>
    public object Clone()
    {
      return AttributeViewModel.Create(HostAttribute.Create((string)Name.Clone(), (string)Value.Clone()));
    } // Clone()

    #endregion

  } // class AttributeViewModel
} // namespace Viatronix.Utilities.General.AdministerHostsTable.ViewModels
