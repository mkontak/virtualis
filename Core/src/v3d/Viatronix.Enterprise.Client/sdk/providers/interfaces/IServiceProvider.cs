using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Drawing;
using System.ComponentModel;
using Viatronix.Enterprise.Entities;

namespace Viatronix.Enterprise.SDK.Providers
{
  /// <summary>
  /// The service provider.
  /// </summary>
  public interface IServiceProvider : IProvider
  {

    #region methods

    /// <summary>
    /// Gets all Viatronix databases
    /// </summary>
    /// <returns></returns>
    List<Database> GetDatabases();

    /// <summary>
    /// Gets the list of all entities for the given table
    /// </summary>
    /// <param name="table">Table to get the data for</param>
    /// <returns></returns>
    List<IEntity> GetTable(Table table);

    #region AgentJob methods

    /// <summary>
    /// Gets the list of agent joibs
    /// </summary>
    /// <returns></returns>
    List<AgentJob> GetAgentJobs();


    /// <summary>
    /// Inserts a new agent job into the system
    /// </summary>
    /// <param name="job">Job to be inserted</param>
    void Insert(AgentJob job);

    /// <summary>
    /// Delete the specified agent job
    /// </summary>
    /// <param name="job">Job to be deleted</param>
    void Delete(AgentJob job);

    #endregion

    #endregion

  } // interface IServiceProvider
} // namespace Viatronix.Enterprise.SDK.Providers
