// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: User.cs
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)



using System;
using System.IO;
using System.Text;
using System.Xml;
using System.Collections;
using System.Collections.Generic;
using Viatronix.Enterprise;
using Viatronix.Enterprise.Logging;


namespace Viatronix.Enterprise.Entities
{

  #region User class

  /// <summary>
  /// Represents a user within the system
  /// </summary>
  public class User : Entity
  {

    #region class Roles

    /// <summary>
    /// Defines the allowable roles for the user
    /// </summary>
    public class Roles
    {
      public const string Service         = "Service";
      public const string Administrator   = "Administrator";
      public const string Operator        = "Operator";
      public const string Physician       = "Physician";
      public const string PhyAdmin        = "Phy-Admin";
      public const string OpAdmin         = "Op-Admin";
      public const string Auditor         = "Auditor";
  
    };

    #endregion

    #region class Attributes

    /// <summary>
    /// user fields directly related to the user table in the database
    /// </summary>
    public static class Attributes
    {
      public const string Login               = "login";
      public const string Name                = "name";
      public const string State               = "state";
      public const string DateAdded           = "add";
      public const string ExpirationDate      = "exp";
      public const string Settings            = "set";
      public const string AuthenticationType  = "auth";
      public const string Domain              = "dom";
      public const string Permissions         = "perm";
      public const string Applications        = "apps";
      public const string Role                = "role";
      public const string ContextId           = "ctx";
      public const string Profiles            = "profiles";
      public const string PasswordTemplate    = "template";

      // temporary attribute for passing password
      public const string Password          = "pwd";

      // temporary column generated when querying for a logged-in user
      public const string LoggedIn          = "in";

    } // class Attributes

    #endregion


    #region fields

 
     /// <summary>
    /// Rights collection
    /// </summary>
    private List<Right> m_rights = new List<Right>();

    /// <summary>
    /// login name
    /// </summary>
    private string m_login = string.Empty;

 
    /// <summary>
    /// Password
    /// </summary>
    private string m_password = string.Empty;

    /// <summary>
    /// user name
    /// </summary>
    private string m_name = string.Empty;

    /// <summary>
    /// Pssword template
    /// </summary>
    private string m_template = string.Empty;


    /// <summary>
    /// current user state
    /// </summary>
    private UserStates m_state = UserStates.None;

    /// <summary>
    /// Date the user as wadded
    /// </summary>
    private DateTimeOffset? m_dateAdded = null;

    /// <summary>
    /// Password change date
    /// </summary>
    private DateTimeOffset? m_passwordChangeDate = null;

    /// <summary>
    /// Date the user account expires (30 years from now)
    /// </summary>
    private DateTimeOffset? m_expirationDate = DateTimeOffset.Now.AddYears(30).Date;

    /// <summary>
    /// Context ID associated with the user
    /// </summary>
    private string m_contextId = string.Empty;

    /// <summary>
    /// assigned user role
    /// </summary>
    private string m_role = string.Empty;

    /// <summary>
    /// Settings for the user
    /// </summary>
    private Settings m_settings = Settings.None;

    /// <summary>
    /// Users valid applications (VC|CAR)
    /// </summary>
    private string m_applications = string.Empty;

    /// <summary>
    /// Authentication Type
    /// </summary>
    private string m_authenticationType = string.Empty;

    /// <summary>
    /// Authentication Domain
    /// </summary>
    private string m_domain = string.Empty;

    /// <summary>
    /// Permission mask filled in by the role
    /// </summary>
    private int m_permissions = 0;

    /// <summary>
    /// collection of profiles
    /// </summary>
    private Dictionary<string, UserProfile> m_profiles = new Dictionary<string, UserProfile>();

 
    #endregion


    #region properties


    /// <summary>
    /// Passwrod template name
    /// </summary>
    public string PasswordTemplate
    {
      get { return m_template; }
      set { SetProperty<string>("PasswordTemplate", value, ref m_template); ; }
    }

    /// <summary>
    /// User profiles
    /// </summary>
    public Dictionary<string, UserProfile> Profiles
    { 
      get { return m_profiles;  }
      set { m_profiles = value;  }
    }

    /// <summary>
    /// Sets/Gets the Application
    /// </summary>
    public string Applications
    {
      get { return m_applications;  }
      set { SetProperty<string>("Applications", value, ref m_applications);  }
    }


     /// <summary>
    /// Gets the permission mask
    /// </summary>
    public int Permissions
    {
      get { return m_permissions; }
      set { SetProperty<int>("Permissions", value, ref m_permissions); }
    }


    /// <summary>
    /// Gets/Sets All rights for the user
    /// </summary>
    public List<Right> AllRights
    {
      get { return m_rights; }
      set { m_rights = value; }
    } // AllRights


    /// <summary>
    /// Gets/Sets the system the user is logged in on
    /// </summary>
    public Settings Settings
    {
      get { return m_settings; }
      set { SetProperty<Settings>("Setings", value, ref m_settings); }
    } // SystemAccount

    /// <summary>
    /// Gets/Sets the Password Change Date
    /// </summary>
    public DateTimeOffset PasswordChangeDate
    {
      get { return m_passwordChangeDate.Value; }
      set { SetProperty<DateTimeOffset?>("PasswordChangeDate", value, ref m_passwordChangeDate); }
    } // PasswordChangeDate

    /// <summary>
    /// Gets/Sets the Login Server
    /// </summary>
    public bool IsLoggedIn
    {
      get { return (m_contextId.Length > 0); }
    } // LoginServer


    /// <summary>
    /// Gets/Sets the login name
    /// </summary>
    public string Login
    {
      get { return m_login; }
      set { SetProperty<string>("Login", value, ref m_login); }
    } // Login

    /// <summary>
    /// Gets/Sets the user password
    /// </summary>
    public string Password
    {
      get { return m_password; }
      set { SetProperty("Password", value, ref m_password); }
    } // Password

    /// <summary>
    /// Gets/Sets the user name
    /// </summary>
    public string Name
    {
      get { return m_name; }
      set { SetProperty("Name", value, ref m_name );  }
    } // Name


    /// <summary>
    /// Gets/Sets the current user state
    /// </summary>
    public UserStates State
    {
      get { return m_state; }
      set { SetProperty<UserStates>( "State", value, ref m_state );  }
    } // State

    /// <summary>
    /// Gets the date the user was added.
    /// </summary>
    public DateTimeOffset DateAdded
    {
      get { return m_dateAdded.Value; }
      private set { SetProperty<DateTimeOffset?>("DateAdded", value, ref m_dateAdded);  }
    } // DateAdded

    /// <summary>
    /// Sets/Gets the expiration date and time for this account
    /// </summary>
    public DateTimeOffset ExpirationDate
    {
      get { return ( m_expirationDate.HasValue ? m_expirationDate.Value : DateTimeOffset.MaxValue ); }
      set { SetProperty<DateTimeOffset?>("ExpirationDate", value, ref m_expirationDate );  }
    } // ExpirationDate

    /// <summary>
    /// Gets/Sets the users role
    /// </summary>
    public string Role
    {
      get { return m_role; }
      set { SetProperty<string>("Role", value, ref m_role); }
    } // Role

    /// <summary>
    /// Gets the context Id
    /// </summary>
    public string ContextId
    {
      get { return m_contextId; }
      private set { SetProperty<string>("ContextId", value, ref m_contextId);  }
    }

    /// <summary>
    /// Gets the Service user flag
    /// </summary>
    public bool IsService
    {
      get { return m_role == Roles.Service; }
    } // IsService

    /// <summary>
    /// Gets the Administrator flag
    /// </summary>
    public bool IsAdministrator
    {
      get
      {
        return m_role == Roles.Administrator || m_role == Roles.OpAdmin || m_role == Roles.PhyAdmin;
      }
    } // IsAdministratorUser

    /// <summary>
    /// Gets the Operator flag
    /// </summary>
    public bool IsOperator
    {
      get { return m_role == Roles.Operator || m_role == Roles.OpAdmin; }
    } // IsOperatorUser

    /// <summary>
    /// Gets the Physician flag
    /// </summary>
    public bool IsPhysician
    {
      get { return m_role == Roles.Physician || m_role == Roles.PhyAdmin; }
    } // IsPhysician



    /// <summary>
    /// Empty means the login is blank
    /// </summary>
    public bool IsEmpty
    {
      get { return string.IsNullOrEmpty(m_login); }
    }

    /// <summary>
    /// Gets the path to the user preferences.
    /// </summary>
    public string PreferencesDirectory
    {
      get
      {
        string path = System.IO.Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData), @"Viatronix\Users\" + this.Login);
        if (!System.IO.Directory.Exists(path))
          System.IO.Directory.CreateDirectory(path);

        return path;
      }
    } // PreferencesDirectory

    /// <summary>
    /// Gets/Sets the Authentication type
    /// </summary>
    public string AuthenticationType
    {
      get { return m_authenticationType; }
      set { SetProperty<string>("Authentication", value, ref m_authenticationType); }
    } // Authentication

    /// <summary>
    /// Gets/Sets the Authentication domain
    /// </summary>
    public string Domain
    {
      get { return m_domain; }
      set { SetProperty<string>("Domain", value, ref m_domain ); }
    } // Domain

    /// <summary>
    /// Gets a flag indicating we are using Viatronix authentication
    /// </summary>
    public bool  ViatronixAuthentication
    {
      get { return string.Compare("Viatronix", m_authenticationType, true) == 0 ; }
    } // ViatronixAuthentication


    #endregion

    #region methods


    /// <summary>
    /// Determines if the type specified matches the authentication type of the object. This
    /// is not case sensitive.
    /// </summary>
    /// <param name="type"></param>
    /// <returns></returns>
    public bool IsAuthenticationType(string type)
    {
      return string.Compare(m_authenticationType, type, true) == 0;
    } // IsAuthenticationType(string type)

    #region Create methods

 
    /// <summary>
    /// Creates the User object
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static User Create(XmlNode node)
    {
 
      // Create the new user
      User user = new User();

      // Make sure allocation was successful
      if (user == null)
        throw new OutOfMemoryException("Failed to allocate User object");

      user.Load(node);

      return user;

    } // Create(XmlNode node)


    /// <summary>
    /// Creates the User object
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public static User Create(string xml)
    {
      return Create(xml.ToXmlNode());
    } // Create(string xml)

    #endregion

 
    /// <summary>
    /// Hash code for this object
    /// </summary>
    /// <returns></returns>
    public override int GetHashCode()
    {
      return m_login.GetHashCode();
    }




    /// <summary>
    /// Check for the User Right Permission
    /// </summary>
    /// <param name="rightName">name of right to check</param>
    /// <returns>true if the user has the right set</returns>
    public bool IsAllowed(string rightName)
    {
      try
      {
        bool allowed = this.IsService;

        if ( ! allowed )
        {
          if (m_rights != null)
          {
            Right right = m_rights.Find(delegate(Right r) { return (r.Name == rightName); });


            if (right != null)
            {
              if (right.BitMask == (right.BitMask & m_permissions))
              {
                allowed = true;
              }
            }
          }
          else
          {
            Log.Error(string.Format("Rights have not been set cannot check permission [RIGHT={0}]", rightName), "User", "IsAllowed");
          }
        }

        return allowed;

      }
      catch (Exception ex)
      {
        Log.Error(string.Format("Failed checking user rights [LOGIN={0}] [RIGHT={1}] : {2}", m_login, rightName, ex.Message), "User", "IsAllowed");
        throw;
      }

    } // IsAllowed(string right)

    #endregion

    #region IEntity

    /// <summary>
    /// Creates the User object
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public override void Load(XmlNode node)
    {

      base.Load(node);

      // node.ValidateSchema(Schema.Settings);

      // Initialize
      m_login = node.Attributes[Attributes.Login].Value;
      m_name = node.Attributes[Attributes.Name].Value;

      // Use the TryGet because if there is no expiration then it is forever
      node.TryGetAttribute<DateTimeOffset?>(Attributes.ExpirationDate, ref m_expirationDate);
     
      m_dateAdded = DateTimeOffset.Parse(node.Attributes[Attributes.DateAdded].Value);
      m_domain = node.Attributes[Attributes.Domain].Value;
      m_authenticationType = node.Attributes[Attributes.AuthenticationType].Value;
      m_state = (UserStates)Int32.Parse(node.Attributes[Attributes.State].Value);
      m_settings = (Settings)Int32.Parse(node.Attributes[Attributes.Settings].Value);
      m_permissions = Int32.Parse(node.Attributes[Attributes.Permissions].Value);
      m_role = node.Attributes[Attributes.Role].Value;
      m_applications = node.Attributes[Attributes.Applications].Value;

      node.TryGetAttributeValue<string>(Attributes.Password, ref m_password);
      node.TryGetAttributeValue<string>(Attributes.ContextId, ref m_contextId);


      // Get any imbedded profiles
      XmlNode profiles = node.SelectSingleNode(Attributes.Profiles);
      if (profiles != null)
      {
        foreach (XmlNode pnode in profiles.ChildNodes)
        {
          UserProfile profile = UserProfile.Create(pnode);
          m_profiles.Add(profile.Name, profile);
        }
      } // END ... If the profiles are blank


    } // Load(XmlNode node)

    /// <summary>
    /// Creates the User object
    /// </summary>
    /// <param name="node"></param>
    /// <returns></returns>
    public override object Clone()
    {

      User user = new User();
      CopyTo(user);

      // node.ValidateSchema(Schema.Settings);

      // Initialize
      user.m_login = m_login;
      user.m_name = m_name;
      user.m_expirationDate = m_expirationDate;
      user.m_dateAdded = m_dateAdded;
      user.m_domain = m_domain;
      user.m_authenticationType = m_authenticationType;
      user.m_state = m_state;

      user.m_settings = m_settings;
      user.m_permissions = m_permissions;
      user.m_role = m_role;
      user.m_applications = m_applications;
      user.m_passwordChangeDate = m_passwordChangeDate;
      user.m_contextId = m_contextId;
      user.m_password = m_password;
      user.m_template = m_template;

      foreach (KeyValuePair<string, UserProfile> pair in m_profiles)
      { user.m_profiles.Add(pair.Key, (UserProfile)pair.Value.Clone()); }

      return user;
    } // Load(XmlNode node)


    /// <summary>
    /// Creates an xml representation of a User object.
    /// </summary>
    /// <returns>Xml string representing the User object.</returns>
    public override string ToXml()
    {

      StringBuilder builder = new StringBuilder();
      using (XmlWriter writer = XmlWriter.Create(builder, new XmlWriterSettings() { OmitXmlDeclaration = true, Indent = false }))
      {


        writer.WriteStartElement(this.Tag);
        {



          writer.WriteAttributeString(Attributes.Login, m_login);
          writer.WriteAttributeString(Attributes.Name, m_name);
          writer.WriteAttributeString(Attributes.State, ((int)m_state).ToString());
          writer.WriteAttributeString(Attributes.AuthenticationType, m_authenticationType);
          writer.WriteAttributeString(Attributes.Domain, m_domain);
          writer.WriteAttributeString(Attributes.Password, ( string.IsNullOrEmpty(m_password) ? m_password : Viatronix.Encryption.Crypto.Encrypt(m_password)));
          writer.WriteAttributeString(Attributes.Permissions, m_permissions.ToString());
          writer.WriteAttributeString(Attributes.Applications, m_applications);
          writer.WriteAttributeString(Attributes.Settings, ((int)m_settings).ToString() );

          if (m_expirationDate.HasValue)
            writer.WriteAttributeString(Attributes.ExpirationDate, m_expirationDate.Value.ToString("O"));

          if (m_dateAdded.HasValue)
            writer.WriteAttributeString(Attributes.DateAdded, m_dateAdded.Value.ToString("O"));

          if (!string.IsNullOrEmpty(m_contextId))
            writer.WriteAttributeString(Attributes.ContextId, m_contextId);

          writer.WriteAttributeString(Attributes.Role, m_role);

          WriteAttributeStrings(writer);
          WriteElements(writer);


          if (m_profiles.Count > 0)
          {
            // output profile xml
            writer.WriteStartElement(Attributes.Profiles);
            {
              foreach (KeyValuePair<string, UserProfile> pair in m_profiles)
                writer.WriteRaw(pair.Value.ToXml());
            }
            writer.WriteEndElement();
          }

        }

        writer.WriteEndElement();

      }

      return builder.ToString();

    } // ToXml()


    /// <summary>
    /// Checks for value equality of task objects
    /// </summary>
    /// <param name="obj">A job to compare with this instance or a null reference</param>
    /// <returns>true if they are equal values; otherwise false</returns>
    public override bool Equals(IEntity obj)
    {
      User other = obj as User;

      return other != null &&
             base.Equals(other) &&
              string.Equals(m_login, other.m_login);

    } // Equals( obj )

    #endregion


  } // class User

  #endregion User class


} // namespace Viatronix.Enterprise.Entities

