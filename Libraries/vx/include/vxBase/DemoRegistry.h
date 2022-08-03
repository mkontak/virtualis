//
// Copyright © 2006, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission Viatronix Inc.
//
// Owner: Mark Kontak (mailto:mkontak@viatronix.com)


#include "SoftwareRegistry.h"

#pragma once

namespace sys
{

#pragma region enum 

  typedef enum Applications
  {
    VC = 0,
    CAR,
    CS,
    XP,
    VAS,
    FN,
    PTX
  } Applications;

#pragma endregion



  /*
  *  Implements the System Setting registry subkey access interface
  */
  class VX_BASE_DLL DemoRegistry64 : public SoftwareRegistry64
  {

#pragma region class Attributes

  private:

    class Attributes
    {

    public:

      static const std::string SystemId;
      static const std::string InstallDate;
      static const std::string Key;
      static const std::string Days;
      static const std::string ExpirationDate;

    };


#pragma endregion


    // functions
  protected:

    /// Constructor
    DemoRegistry64() { }

  public:

    // constructor
    DemoRegistry64(const std::string & sApplicationCode);

    // constructor
    DemoRegistry64(Applications app);


  public:

    /// Registers the application
    void Activate();

    /// Generates an activation key
    static std::string GenerateActivationKey( const std::string & sApplication, const std::string & sSystemId, int days = 30 );

    static inline std::string GenerateActivationKey(Applications application, const std::string & sSystemId, int days = 30)
    {   return GenerateActivationKey(ToString(application), sSystemId, days);  }

    /// Scrambles the text
    static std::string ScrambleApplication(const std::string & sApplication);

    /// Unscrambles the code
    static std::string  UnScrambleApplication(const std::string & sScrambledCode);

    /// Gets the application list
    static std::vector<std::string> GetApplicationList();

    /// Formats the application code to a string
    static std::string ToString(Applications app);

  private:

    /// Initializes the object
    void Initialize();



#pragma region properties

  public:

    // Returns true of the demo system has expired
    virtual bool IsExpired();

    // Gets the system id
    virtual const std::string & GetSystemId() { return m_sSystemId; }

    // Gets the Key
    virtual const std::string & GetActivationKey() { return m_sKey; }

    // Sets the Key
    virtual void SetActivationKey(const std::string & sKey);


    // Get the number of days the demo is to active
    virtual const DWORD GetActivationDays() { return m_days; }


    // Gets the installation date
    virtual const struct tm GetInstallationDate() { return m_installation;  }
    

    // Gets the expiration date
    virtual const struct tm GetExpirationDate() { return m_expiration;  }

#pragma endregion

    /// Key returned from Viatronix
    std::string m_sKey;

    /// Installation date
    struct tm  m_installation;

    /// Expiration date
    struct tm  m_expiration;

    /// System Id
    std::string  m_sSystemId;

    /// Activation days
    DWORD m_days;

    /// Application list
    std::vector<std::string> m_applications;

    /// Application 
    std::string m_sApplication;

  };  // class V3DSystemSettings


}