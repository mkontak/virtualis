using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Viatronix.Enterprise;
using Viatronix.Enterprise.Entities;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Utilities.Service.ServiceManager
{
  /// <summary>
  /// Helper class.
  /// </summary>
  public static class Global
  {

    #region fields

    /// <summary>
    /// The data provider.
    /// </summary>
    private static IDataProvider m_dataProvider = null;

    /// <summary>
    /// The service provider.
    /// </summary>
    private static Viatronix.Enterprise.SDK.Providers.IServiceProvider m_serviceProvider = null;

    #endregion

    #region methods

    /// <summary>
    /// Performs initialization.
    /// </summary>
    public static void Initialize()
    {
      List<IProvider> providers = ProviderFactory.Create();

      IProvider provider = null;
      if (providers.TryFind(ProviderType.Data, ref provider))
      {
        m_dataProvider = (IDataProvider)provider;
      }
      else
      {
        m_dataProvider = null;
      }

      if (providers.TryFind(ProviderType.Service, ref provider))
      {
        m_serviceProvider = (Viatronix.Enterprise.SDK.Providers.IServiceProvider)provider;
      }
      else
      {
        m_serviceProvider = null;
      }
    } // Initialize()


    /// <summary>
    /// Gets the hosts.
    /// </summary>
    /// <param name="query">The query.</param>
    /// <returns>The list of hosts.</returns>
    public static List<DicomHost> GetDicomHosts(XmlBuilder query = null)
    {
      if(m_dataProvider != null)
      {
        return m_dataProvider.GetDicomHosts(query);
      }
      else
      {
        throw new ViatronixException("The data provider could not be initialized.", ErrorCodes.None);
      }
    } // GetDicomHosts( query )


    /// <summary>
    /// Inserts the provided host.
    /// </summary>
    /// <param name="dicomHost">The dicom host.</param>
    public static void InsertDicomHost(DicomHost dicomHost)
    {
      if (m_dataProvider != null)
      {
        m_dataProvider.InsertDicomHost(dicomHost);
      }
      else
      {
        throw new ViatronixException("The data provider could not be initialized.", ErrorCodes.None);
      }
    } // InsertDicomHost( dicomHost )


    /// <summary>
    /// Updates the provided host.
    /// </summary>
    /// <param name="dicomHost">The dicom host.</param>
    public static void UpdateDicomHost(DicomHost dicomHost)
    {
      if (m_dataProvider != null)
      {
        m_dataProvider.UpdateDicomHost(dicomHost);
      }
      else
      {
        throw new ViatronixException("The data provider could not be initialized.", ErrorCodes.None);
      }
    } // UpdateDicomHost( dicomHost )


    /// <summary>
    /// Deletes the provided host.
    /// </summary>
    /// <param name="dicomHost">The dicom host.</param>
    public static void DeleteDicomHost(DicomHost dicomHost)
    {
      if (m_dataProvider != null)
      {
        m_dataProvider.DeleteDicomHost(dicomHost);
      }
      else
      {
        throw new ViatronixException("The data provider could not be initialized.", ErrorCodes.None);
      }
    } // DeleteDicomHost( dicomHost )


    /// <summary>
    /// Gets the system settings.
    /// </summary>
    /// <param name="query">The query.</param>
    /// <returns>The list of system settings.</returns>
    public static List<SystemSetting> GetSystemSettings(XmlBuilder query = null)
    {
      if (m_dataProvider != null)
      {
        return m_dataProvider.GetSystemSettings(query);
      }
      else
      {
        throw new ViatronixException("The data provider could not be initialized.", ErrorCodes.None);
      }
    } // GetSystemSettings( query )

    #endregion

  } // class Global
} // namespace Viatronix.Utilities.Service.ServiceManager
