// Copyright © 2010, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Author: Greg Davidson ( mailto:gdavidson@viatronix.com )


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.UI
{
  public class ServiceContainer
  {

    #region fields

    /// <summary>
    /// singleton
    /// </summary>
    public static readonly ServiceContainer Instance = new ServiceContainer();

    /// <summary>
    /// service mappings
    /// </summary>
    private readonly Dictionary<Type, object> m_serviceMap = null;

    /// <summary>
    /// synchronization lock
    /// </summary>
    private readonly object m_serviceMapLock = null;

    #endregion

    #region construction

    /// <summary>
    /// private constructor
    /// </summary>
    private ServiceContainer()
    {
      m_serviceMap = new Dictionary<Type, object>();
      m_serviceMapLock = new object();
    } // ServiceContainer()

    #endregion

    #region methods

    /// <summary>
    /// Adds a service to the container
    /// </summary>
    /// <typeparam name="TServiceContract">generic service contract</typeparam>
    /// <param name="implementation">specific service implementation</param>
    public void AddService< TServiceContract >( TServiceContract implementation ) where TServiceContract : class
    {
      lock( m_serviceMapLock )
      {
        m_serviceMap[typeof( TServiceContract )] = implementation;
      }
    } // AddService< TServiceContract >( implementation ) 


    /// <summary>
    /// Retrieves the service from the container
    /// </summary>
    /// <typeparam name="TServiceContract">service contract</typeparam>
    /// <returns>service</returns>
    public TServiceContract GetService< TServiceContract >() where TServiceContract : class
    {
      object service;
      lock( m_serviceMapLock )
      {
        m_serviceMap.TryGetValue( typeof( TServiceContract ), out service );
      }
      return service as TServiceContract;
    } // GetService< TServiceContract >()

    #endregion

  } // class ServiceContainer
} // namespace Viatronix.UI
