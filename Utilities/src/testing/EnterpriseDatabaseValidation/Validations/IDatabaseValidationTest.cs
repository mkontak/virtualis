// Copyright © 2010, Viatronix Inc., All Rights Reserved.
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

using Microsoft.Practices.EnterpriseLibrary.Data.Sql;

namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql.Validations
{
  /// <summary>
  /// Defines a interface for checking that the database is in good order.
  /// </summary>
  public interface IDatabaseValidationTest
  {

    /// <summary>
    /// Executes the test returning the result.
    /// </summary>
    /// <param name="database">The database to validate.</param>
    /// <returns>True if the test was succesful, false otherwise.</returns>
    bool Execute(SqlDatabase database);

  } // interface IDatabaseValidationTest
} // namespace Viatronix.Enterprise.Tests.DatabaseValidation.Sql.Validations
