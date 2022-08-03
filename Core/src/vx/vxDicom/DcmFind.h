// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmFind.h
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



class DCM_DLL DcmFindParameters;
class DCM_DLL DcmHost;


/**
 * Implements a class that wraps the C-FIND
 */
class DCM_DLL DcmFind : public DcmClient, public DcmThread
{
protected:

  /// Constructor
  DcmFind(const std::string & sAETitle = "");


public:

  /// Destructor
  virtual ~DcmFind();

  /// Aborts the store command
  virtual void Abort();


  /// Creates the find client object
  static std::shared_ptr<DcmFind> Create(const std::string & sAETitle = "");

  /// Creates the find client object 
  static std::shared_ptr<DcmFind> Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pServer);

  /// Adds the dicom information used for lookup
  void AddParameters( std::shared_ptr<DcmFindParameters> & parameters );

  /// Clears the parameters
  inline void ClearParameters()
  { m_parameters.clear(); }

  /// Thread process
  virtual void RunSynchronously();

private:

  /// Relays the callback from the client
  virtual void CallbackRelay(void * pResponse, DcmDataset * pResponseIds);


public:



private:

  /// Find parameters
  std::vector<std::shared_ptr<DcmFindParameters>> m_parameters;

};  // class DcmFind
  
};  // namespace dcm

#pragma warning(pop)