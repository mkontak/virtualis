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
using System.Xml;

namespace Viatronix.Enterprise.Messaging
{
  public abstract class Parameters
  {

    #region properties

    /// <summary>
    /// Returns version number
    /// </summary>
    public abstract string Version { get; }

    #endregion

    #region methods

    /// <summary>
    /// Outputs the parameters as xml
    /// </summary>
    /// <returns>xml string</returns>
    public abstract string ToXml();
    
    /// <summary>
    /// Loads parameters from xml
    /// </summary>
    /// <param name="node">XmlNode</param>
    public abstract void LoadXml( XmlNode node );
    
    /// <summary>
    /// Checks parameters for equality
    /// </summary>
    /// <param name="obj">other</param>
    /// <returns>true if equal; otherwise false</returns>
    public abstract bool Equals( Parameters obj );

    /// <summary>
    /// Checks for equality
    /// </summary>
    /// <param name="first">Parameters 1</param>
    /// <param name="second">Parameters 2</param>
    /// <returns>true if equal; otherwise false</returns>
    public static bool Equals( Parameters first, Parameters second )
    {
      if( first != null )
        return first.Equals( second );

      return ( second == null );
    } // Equals( first, second )

    #endregion

  } // interface IParameters
} // namespace Viatronix.Enterprise.Messaging
