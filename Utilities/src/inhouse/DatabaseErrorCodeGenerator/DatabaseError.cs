// Copyright © 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// Owner: Kyle R Chalupa(mailto:kchalupa@viatronix.com)

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Viatronix.Utilities.InHouse.DatabaseErrorCodeGenerator
{
  /// <summary>
  /// Definition for a database error code.
  /// </summary>
  public class DatabaseError
  {

    #region fields

    /// <summary>
    /// The error's code.
    /// </summary>
    private string m_code = string.Empty;

    /// <summary>
    /// The name of the error.
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// The area of the system the error would occur.
    /// </summary>
    private string m_type = string.Empty;

    /// <summary>
    /// The description of the error.
    /// </summary>
    private string m_description = string.Empty;

    #endregion

    #region construction

    /// <summary>
    /// Constructor.
    /// </summary>
    public DatabaseError()
    {
    } // DatabaseError()


    /// <summary>
    /// Verbose constructor.
    /// </summary>
    /// <param name="name">Name of error.</param>
    /// <param name="code">Error code.</param>
    /// <param name="type">Occurs in what area.</param>
    /// <param name="description">A brief description.</param>
    public DatabaseError(string name, string code, string type, string description)
    {
      Name = name;
      Code = code;
      Type = type;
      Description = description;
    } // DatabaseError( name, code, type, description )

    #endregion

    #region properties

    /// <summary>
    /// Gets or sets the error code.
    /// </summary>
    public string Code
    {
      get { return m_code; }
      set { m_code = value; }
    } // Code


    /// <summary>
    /// The name of the error.
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { m_name = value; }
    } // Name


    /// <summary>
    /// Gets or sets the type.
    /// </summary>
    public string Type
    {
      get { return m_type; }
      set { m_type = value; }
    } // Type


    /// <summary>
    /// Gets or sets the description.
    /// </summary>
    public string Description
    {
      get { return m_description; }
      set { m_description = value; }
    } // Description

    #endregion

  } // class DatabaseError
} // namespace Viatronix.Utilities.InHouse.DatabaseErrorCodeGenerator
