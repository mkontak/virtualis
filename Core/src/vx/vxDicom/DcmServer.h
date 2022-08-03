// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmServer.h
//
// Owner: Mark Kontak  mkontak@viatronix.net


#pragma once

#include "DcmDevice.h"
#include "DcmEnums.h"
#include "DcmThread.h"
#include "DcmCallback.h"


#pragma warning(push)
#pragma warning(disable:4251)

namespace dcm
{


// Forward declarations
class DcmAssociation;

#define ASSOC std::pair<std::string, std::shared_ptr<DcmAssociation>>
#define ASSOCMAP std::map<std::string, std::shared_ptr<DcmAssociation>> 

//=======================
// Dicom Server Class
//======================
class DCM_DLL DcmServer : public DcmDevice, public DcmThread
{


#pragma region constructor

private:

  /// Constructor
  DcmServer();

public:

  
  /// Constructor 
  DcmServer(const std::string & sAETitle, int iPort);

  
  /// Destructor
  virtual ~DcmServer();
   

#pragma endregion

#pragma region methods

public:

  /// Starts the server. 
  virtual HANDLE Start(LPVOID callbackData, std::function< void( PDCM_CALLBACK_PARAMETERS, LPVOID) > callbackFunction );

  /// Creates a Dicom Server from the xml std::string
  static std::shared_ptr<DcmServer> Create(const std::string & sXml);

  /// Creates a Dicom Server from the xml std::string
  static std::shared_ptr<DcmServer> Create(const std::string & sAETitle, int iPort);

  /// Stops the server
  virtual void Abort();

private:


  /// Run
  void RunSynchronously();

  /// Cleans up either all the associations that are finished or all the associations
  void CleanupAssociations();

  /// Unblocks connection (used for abort)
  static DWORD WINAPI UnblockConnection(LPVOID lpParam);

  /// Validates the called AE Title
  void ValidateCalledAETitle(T_ASC_Association * pAssociation, const std::string & sCalledAETitle);

  /// Validates the calling AE Title
  void ValidateCallingAETitle(T_ASC_Association * pAssociation, const std::string & sCallingAETitle);

  /// Attempts to accept the association
  void AcceptAssociation();

  /// Association callback handler
  static void AssociationCallback(PDCM_CALLBACK_PARAMETERS pCallbackParameters, LPVOID lpCallbackData);

  /// Drops the network
  void DropNetwork();
 
#pragma endregion



#pragma region properties

public:

  /// Gets the list of accepted AE Titles
  const std::vector<std::string> GetAcceptedAETitles()
  { return m_acceptedAETitles; }
 
  /// Get the port number
  const int GetPort() const
  { return m_iPort; }
 
  /// Gets the state
  const DcmStates GetState() const
  { return m_eState; }

#pragma endregion

#pragma region fields


private:

  /// Server name (gethostname())
  std::string m_sName;

  /// Network
  T_ASC_Network * m_pNetwork;

 
  /// Accepted AE Titles
  std::vector<std::string> m_acceptedAETitles;

  /// Port to listen on
  int m_iPort;

  /// Associations List
  ASSOCMAP m_associations;

  /// Flag to indicate if the association has the right called AE Title.
  bool m_bValidateCalledAETitle;

  /// State of the server
  DcmStates m_eState;

  /// Aborting flag
  bool m_bAborting;

#pragma endregion


};  // class Server


}; // namespace dcm

#pragma warning(pop)