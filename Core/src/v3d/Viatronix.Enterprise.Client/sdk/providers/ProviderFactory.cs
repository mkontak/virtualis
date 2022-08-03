using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Viatronix.Enterprise.SDK.Providers;

namespace Viatronix.Enterprise.SDK.Providers
{
  public static class ProviderFactory
  {


 
    #region fields

    /// <summary>
    /// All providers
    /// </summary>
    private static List<IProvider> m_providers = null;

    #endregion

    /// <summary>
    /// Creates the specified provider
    /// </summary>
    /// <typeparam name="T">Type of provider</typeparam>
    /// <param name="type">Name/Type</param>
    /// <returns></returns>
    public static List<IProvider> Create()
    {

      if (m_providers == null)
      {

        m_providers = new List<IProvider>();

        try
        {


          // Providers
          Viatronix.Enterprise.Configuration.ProviderElementCollection providers = Viatronix.Enterprise.Configuration.ProviderSection.GetSection().Providers;


          // Make sure there are providers
          if (providers.Count == 0)
            throw new InvalidConfigurationException("No providers were defined in the configuration file");


          ISecurityProvider securityProvider = null;

          // =========================================
          // First look for all the security provider
          // =========================================
          foreach( Viatronix.Enterprise.Configuration.ProviderElement element in Viatronix.Enterprise.Configuration.ProviderSection.GetSection().Providers )
          {

            try
            {
              IProvider prov = (IProvider)System.Activator.CreateInstance(element.Type, new object[] { element.ToXml() });
              if (prov.IsType(ProviderType.Security))
              {
                if (securityProvider == null)
                  securityProvider = (ISecurityProvider)prov;
                m_providers.Add(securityProvider);
              }
            }
            catch (MissingMethodException)
            {
              // Ignore
            }
            catch (Exception )
            {
              throw;
            }
  
           }

          // Make sure the security provider was defined
          if (securityProvider == null)
            throw new InvalidConfigurationException("No security provider defined in the configuration file");

          // ====================================
          // Add the remainder of the providers
          // ====================================
          IProvider provider = null;
          foreach( Viatronix.Enterprise.Configuration.ProviderElement element in Viatronix.Enterprise.Configuration.ProviderSection.GetSection().Providers )
          {

            provider = null;

            // ===================================================================================================
            // If the provider does not already exist in the list then it is a non-security and we need to add it
            // ====================================================================================================
            if ( ! m_providers.TryFindByName(element.Name, ref provider) ) 
            {


              // ==================================================================================================================
              // If the security provider name is not specified then used the default which is the first one found in the list
              // ==================================================================================================================
              if (string.IsNullOrEmpty(element.SecurityProviderName))
              {
                provider = (IProvider)System.Activator.CreateInstance(element.Type, new object[] { element.ToXml(), securityProvider });
              }

              // ===================================================================================================================
              // Otherwise if the security provider name was supplied then look for the provider in the list to linke to it. If it
              // is not found then throuw an exception indicating that the configuration is invalid.
              // ===================================================================================================================
              else
              {
                IProvider secprov = null;
                if (m_providers.TryFindByName(element.SecurityProviderName, ref secprov))
                  provider = (IProvider)System.Activator.CreateInstance(element.Type, new object[] { element.ToXml(), (ISecurityProvider)secprov });
                else
                  throw new InvalidConfigurationException(string.Format("Invalid security provider name in {0} provider of the providers section", element.Name));

              }

              // Add the provider
              m_providers.Add(provider);

            } // END ... If the provider is not already in the list

          } // END ... FOr each provider in the section
              

        } 
        catch (Exception ex)
        {
          m_providers = null;
          Logging.Log.Error("Failed to create providers : " + ex.Message, "ProviderFactory", "Create");
          throw;
        }

      }


      return m_providers;

    } // Create<T>()

 
    /// <summary>
    /// Creates the specified provider
    /// </summary>
    /// <typeparam name="T">Type of provider</typeparam>
    /// <param name="name">Name/Type</param>
    /// <returns></returns>
    public static T Create<T>(string name)
    {

      Create();
  
      try
      {

        IProvider provider = null;

        if (!m_providers.TryFindByName(name, ref provider))
          throw new NotFoundException(string.Format("{0} - provider was not defined in the configuration file", name));

        return (T)provider;

      }
      catch (Exception ex)
      {
        Logging.Log.Error("Failed to create provider : " + ex.Message, "ProviderFactory", "Create");
        throw;
      }

    } // Create<T>()

    /// <summary>
    /// Creates the specified provider
    /// </summary>
    /// <typeparam name="T">Type of provider</typeparam>
    /// <param name="type">Name/Type</param>
    /// <returns></returns>
    public static T Create<T>(ProviderType type)
    {

      Create();

      try
      {

        IProvider provider = null;

        if (!m_providers.TryFind(type, ref provider))
          throw new NotFoundException(string.Format("{0} provider was not defined in the configuration file", type));

        return (T)provider;

      }
      catch (Exception ex)
      {
        Logging.Log.Error("Failed to create provider : " + ex.Message, "ProviderFactory", "Create");
        throw;
      }

    } // Create<T>()


  }
}
