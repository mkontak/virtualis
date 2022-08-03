// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Mark Kontak ( mailto:mkontak@viatronix.com )

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Data;
using System.Text.RegularExpressions;
using System.Text;
using System.Xml;
using System.Xml.Linq;
using System.Windows;
using System.Reflection;

using Viatronix.Enterprise;
using Viatronix.Enterprise.Data;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;


namespace Viatronix
{
  public static class ExtensionMethods
  {



    #region List<IProvider> Extension Methods


    /// <summary>
    /// Gets the provider specified by the type if not found then an exception is thrown
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns></returns>
    public static IProvider GetProvider(this List<IProvider> list, ProviderType type)
    {
      foreach (IProvider p in list)
      {
        if (p.IsType(type))
        {
          return p;
        }
      }

      throw new NotFoundException(string.Format("No {0} provider found in list", type));
    } // Find(this List<Series> list, string uid, ref Series series)

    /// <summary>
    /// Gets the provier type specified from the list if found
    /// </summary>
    /// <param name="list"></param>
    /// <param name="type"></param>
    /// <returns>True if found, false otherwise</returns>
    public static bool TryFind(this List<IProvider> list, ProviderType type, ref IProvider provider)
    {
      foreach (IProvider p in list)
      {
        if (p.IsType(type))
        {
          provider = p;
          return true;
        }
      }

      return false;
    } // TryFind(this List<Series> list, string uid, ref Series series)

    /// <summary>
    /// Gets the provier by the name specified from the list if found
    /// </summary>
    /// <param name="list"></param>
    /// <param name="name"></param>
    /// <returns>True if found, false otherwise</returns>
    public static bool TryFindByName(this List<IProvider> list, string name, ref IProvider provider)
    {
      foreach (IProvider p in list)
      {
        if (p.IsName(name))
        {
          provider = p;
          return true;
        }
      }

      return false;
    } // TryFind(this List<Series> list, string uid, ref Series series)

    #endregion

  }

 } // namespace Viatronix.Enterprise
