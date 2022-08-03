using System;
using System.Xml;
using System.Data;
using System.Data.Sql;
using System.Data.Common;
using System.Data.SqlClient;
using System.Data.SqlTypes;

using Microsoft.Practices.EnterpriseLibrary;
using Microsoft.Practices.EnterpriseLibrary.Data;
using Microsoft.Practices.EnterpriseLibrary.Data.Sql;
using Microsoft.Practices.EnterpriseLibrary.Common;
using Microsoft.VisualStudio.TestTools.UnitTesting;

using Viatronix.UnitTests.DatabaseAccessHelper;

namespace Viatronix.UnitTests.SecuritySubSystem.Tests
{
  /// <summary>
  /// Validates getAuthenticationType stored proc.
  /// </summary>
  [TestClass]
  public class GetAuthenticationTypeTests
  {

    #region fields

    /// <summary>
    /// The service context id.
    /// </summary>
    private Guid m_serviceContextId = Guid.Empty;

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization for each test.
    /// </summary>
    [TestInitialize]
    public void TestInitialize()
    {
      SecurityGateway.ValidateDatabase();

      m_serviceContextId = SecurityGateway.AcquireContext(SecurityGateway.SRVCUSERNAME, SecurityGateway.SRVCPASSWORD);
    } // TestInitialize()


    /// <summary>
    /// Performs cleanup for each test.
    /// </summary>
    [TestCleanup]
    public void TestCleanup()
    {
      SecurityGateway.ReleaseContext(m_serviceContextId, SecurityGateway.SRVCUSERNAME);

      SecurityGateway.ValidateDatabase();
    } // TestCleanup()


    /// <summary>
    /// Tests getting by the id.
    /// </summary>
    [TestMethod]
    public void GtAuthTp_TestGetById()
    {
      string results = SecurityGateway.GetAuthenticationType(m_serviceContextId, "<authenticationtype id=\"Viatronix\" />");

      XmlDocument doc = new XmlDocument();
      doc.LoadXml(results);

      Assert.AreEqual("collection", doc.DocumentElement.Name, "The returned node did not match expectations.");
      Assert.AreEqual("authenticationtype", doc.DocumentElement.Attributes["id"].Value, "The returned collection id did not match expectations.");
      Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"authenticationtype\"]/authenticationtype[@id=\"Viatronix\"]").Count,
        "The number of returned authenticationTypes did not match expectations.");
    } // GtAuth_TestGetById()


    /// <summary>
    /// Tests getting the default authentication type.
    /// </summary>
    [TestMethod]
    public void GtAuthTp_TestGetByDefault()
    {
      string defaultAuthenticationType = string.Empty;

      try
      {
        Database database = SecurityGateway.CreateDatabase();
        using (DbConnection conn = database.CreateConnection())
        {
          //
          // Retrieve the current default authentication type.
          //

          DbCommand cmd = database.GetSqlStringCommand("SELECT * FROM [v3d].[AuthenticationTypes] WHERE [default]=\'True\'");
          DataSet ds = database.ExecuteDataSet(cmd);

          Assert.AreEqual(1, ds.Tables.Count, "The number of returned tables did not match exceptions.");
          Assert.AreEqual(1, ds.Tables[0].Rows.Count, "The number of returned rows did not match exceptions.");

          defaultAuthenticationType = ds.Tables[0].Rows[0]["id"].ToString();

          //
          // Clear all other types of default state.
          //

          cmd = database.GetSqlStringCommand("UPDATE [v3d].[AuthenticationTypes] SET [default] = \'False\'");
          database.ExecuteNonQuery(cmd);

          //
          // Insert a new authentication type and mark as default.
          //

          cmd = database.GetSqlStringCommand("INSERT INTO [v3d].[AuthenticationTypes] ([id], [desc], [default])" +
            " VALUES ('Picture-Password', 'Enter a picture password as Windows 8 Supports.', 'True')");

          database.ExecuteNonQuery(cmd);

          //
          // Get by the default type and not default and verify both.
          //

          // Get the default type.
          string results = SecurityGateway.GetAuthenticationType(m_serviceContextId, "<authenticationtype default=\"1\" />");

          XmlDocument doc = new XmlDocument();
          doc.LoadXml(results);

          Assert.AreEqual("collection", doc.DocumentElement.Name, "The results document node name did not match expectations.");
          Assert.AreEqual("authenticationtype", doc.DocumentElement.Attributes["id"].Value,
            "The collection id returned did not match expectations.");

          Assert.AreEqual(1, doc.DocumentElement.SelectNodes("/collection[@id=\"authenticationtype\"]/authenticationtype").Count,
            "The nummber of returned authentication types did not match expectations.");

          Assert.AreEqual(1, doc.DocumentElement.SelectNodes(string.Format(
            "/collection[@id=\"authenticationtype\"]/authenticationtype[@id=\"{0}\" and @default=\"1\"]", "Picture-Password")).Count,
            "The nummber of returned authentication types did not match expectations.");

          // Get the types that are not marked default.
          results = SecurityGateway.GetAuthenticationType(m_serviceContextId, "<authenticationtype default=\"0\" />");

          doc = new XmlDocument();
          doc.LoadXml(results);

          Assert.AreEqual("collection", doc.DocumentElement.Name, "The results document node name did not match expectations.");
          Assert.AreEqual("authenticationtype", doc.DocumentElement.Attributes["id"].Value,
            "The collection id returned did not match expectations.");

          Assert.AreEqual(2, doc.DocumentElement.SelectNodes("/collection[@id=\"authenticationtype\"]/authenticationtype").Count,
            "The number of returned nodes did not match expectations.");
        }
      }
      finally
      {
        Database database = SecurityGateway.CreateDatabase();
        using(DbConnection conn = database.CreateConnection())
        {
          // Delete the added authentication type.
          DbCommand cmd = database.GetSqlStringCommand("DELETE FROM [v3d].[AuthenticationTypes] WHERE [id]=\'Picture-Password\'");
          database.ExecuteNonQuery(cmd);

          // Reset the default state to the original holder.
          cmd = database.GetSqlStringCommand(string.Format("UPDATE [v3d].[AuthenticationTypes] SET [default]=\'True\' WHERE [id]=\'{0}\'", defaultAuthenticationType));
          database.ExecuteNonQuery(cmd);
        }
      }
    } // GtAuth_TestGetByDefault()

    #endregion

  } // class GetAuthenticationTypeTests
} // namespace Viatronix.UnitTests.SecuritySubSystem.Tests
