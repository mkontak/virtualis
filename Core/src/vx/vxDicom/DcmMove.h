// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmMove.h
//
// Owner: Mark Kontak  mkontak@viatronix.net

#pragma once

// includes
#include "DcmClient.h"
#include "DcmThread.h"

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)


namespace dcm
{

// forward declaration
class DCM_DLL DcmImage;
class DCM_DLL DcmQueryRetrieveParameters;
class DCM_DLL DcmMoveParameters;
class DCM_DLL DcmHost;

/**
 * Implements a class that wraps the STORE
 */
class DCM_DLL DcmMove : public DcmClient, public DcmThread
{
protected:

  // constructor
  DcmMove(const std::string & sAEtitle = "");


public:

  /// Destructor
  virtual ~DcmMove();

  /// Aborts the store command
  virtual void Abort();

   /// Creates the find client object
  static std::shared_ptr<DcmMove> Create(const std::string & sAETitle = "");

  /// Creates the find client object 
  static std::shared_ptr<DcmMove> Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pServer);


  /// Adds the parameters needed to perform the move
  void AddParameters(std::shared_ptr<DcmMoveParameters> & parameters);

  /// Clears the parameters
  inline void ClearParameters()
  { m_parameters.clear(); }

  /// Thread process
  virtual void RunSynchronously();

private:

  /// Relays the callback from the client
  virtual void CallbackRelay(void * pResponse, DcmDataset * pResponseIds);


protected:

  /// Move parameters
  std::vector<std::shared_ptr<DcmMoveParameters>> m_parameters;

 
};  // class DcmMove
  
};  // namespace dcm

#pragma warning(pop)