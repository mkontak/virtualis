// Copyright© 2012, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form,
// in whole or in part, without the prior written permission of Viatronix Inc. 
//
// File: DcmStore.h
//
// Description: 
//
// Owner: Mark Kontak  (mailto: mkontak@viatronix.com)


#pragma once

// includes
#include "DcmClient.h"
#include "DcmThread.h"

// pragmas
#pragma warning(push)
#pragma warning(disable:4251)

// namespace
namespace dcm
{


// forward declarations
class DCM_DLL DcmImage;




/**
 * Implements a class that wraps the STORE
 */
class DCM_DLL DcmStore : public DcmClient, public DcmThread
{

protected:

  /// Default constructor
  DcmStore(const std::string & sAETitle = "");

public:

  /// Destructor
  virtual ~DcmStore();

#pragma region Create Methods

public:

   /// Creates the store client object
  static std::shared_ptr<DcmStore> Create(const std::string & sAETitleOrXml = "");

  /// Creates the store client object 
  static std::shared_ptr<DcmStore> Create(const std::string & sAETitle, std::shared_ptr<DcmHost> & pServer);



#pragma endregion

public:


  /// Aborts the store command
  virtual void Abort();

  /// Sends the first image in the list and removes it
  void Send();

  /// Adds file to list
  void AddFile(const std::string & sFile);

  /// Adds directory
  void AddDirectory(const std::string & sDirectory, const std::string & sPattern = "*.dcm", bool recursive = false);

  /// Clears all file from the list
  void ClearFiles();

  /// Adds image to list
  void AddImage(std::shared_ptr<DcmImage> pDcmImage);

  /// Clears all images from the list
  void ClearImages();

  /// Thread process
  virtual void RunSynchronously();


private:


  /// Returns true if there are images to store
  inline bool IsThereImagesToStore()
  { return ( GetImageCount() > 0 ); }


  /// Relays the callback from the client
  //virtual void CallbackRelay(void * pResponse) { }



#pragma region properties

public:

    /// Gets the total number of images and files that are to be stores
  inline int GetImageCount() 
  { return (int)(m_files.size() + m_images.size()); }


#pragma endregion

#pragma region fields

private:

  /// Items completed
  int m_iCompleted;

  /// Items remaining
  int m_iRemaining;

  /// File list
  STRINGLIST m_files;

  /// Image list
  std::vector<std::shared_ptr<DcmImage>> m_images;

#pragma endregion

};  // class DcmStore
  
};  // namespace dm

#pragma warning(pop)