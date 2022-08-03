// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// File: DcmDevice.cpp
//
// Owner: Mark Kontak  mkontak@viatronix.net

#include "Stdafx.h"
#include "DcmLog.h"
#include "DcmDevice.h"
#include "DcmUtility.h"



using namespace dcm;

/**
 * Constructor
 */
DcmDevice::DcmDevice() :
m_sAETitle(""),
m_sLogSource(vxLogFactory::GetDefaultLogger()),
m_iMaxPDU(ASC_DEFAULTMAXPDU),
m_transferArray(__nullptr),
m_abstractArray(__nullptr),
m_iNetworkTimeout(DCM_DefaultNetworkTimeout)
{

  Initialize();

} // DcmDevice()


/**
 * Constructor
 *
 * @param sAETitle      AE Title
 */
DcmDevice::DcmDevice(const std::string & sAETitle) :
m_sAETitle(sAETitle),
m_sLogSource(vxLogFactory::GetDefaultLogger()),
m_iMaxPDU(ASC_DEFAULTMAXPDU),
m_transferArray(__nullptr),
m_abstractArray(__nullptr),
m_iNetworkTimeout(DCM_DefaultNetworkTimeout)
{
  Initialize();

} // DcmDevice(const std::string & sAETitle)


/**
 * Constructor
 *
 * @param sAETitle      AE Title
 */
DcmDevice::DcmDevice(const std::string & sAETitle, int iMaxPDU, int iNetworkTimeout) :
m_sAETitle(sAETitle),
m_sLogSource(vxLogFactory::GetDefaultLogger()),
m_iMaxPDU(iMaxPDU),
m_transferArray(__nullptr),
m_abstractArray(__nullptr),
m_iNetworkTimeout(iNetworkTimeout)
{
  Initialize();

} // DcmDevice(const std::string & sAETitle)


/**
 * Destructor cleans up the network
 */
DcmDevice::~DcmDevice()
{

  if ( m_transferArray != __nullptr )
    delete [] m_transferArray;
 
  if ( m_abstractArray != __nullptr ) 
    delete [] m_abstractArray;

  WSACleanup(); 
} // ~DcmCore()


/**
 * Initialize the objects state
 */
void DcmDevice::Initialize()
{
  InitializeNetwork();
} // Initialize()


/** 
 * Initialize the winsock network
 */
void DcmDevice::InitializeNetwork()
{
  // Socket data for initialization
  WSAData winSockData;

  // Need at least version 1.1 
  WORD winSockVersionNeeded = MAKEWORD( 1, 1 );

  // Start the winsock
  WSAStartup(winSockVersionNeeded, &winSockData);

} // InitializeNetwork()


/**
 * Sets the transfer syntaxes for the server
 *
 * @param transferSyntax    Defines a group of UIDS to use for the syntax
 */
void DcmDevice::SetTransferSyntax(E_TransferSyntax transferSyntax)
{

  // Clear the old setting
  m_transferSyntaxes.clear();

  //====================================================================
  // The transfer syntax setting is based off a group of settings
  //===================================================================
  switch (transferSyntax)
  {
    case EXS_LittleEndianImplicit:
      /* we only support Little Endian Implicit */
      m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
    case EXS_LittleEndianExplicit:
      /* we prefer Little Endian Explicit */
      m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
    case EXS_BigEndianExplicit:
      /* we prefer Big Endian Explicit */
      m_transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
    case EXS_JPEGProcess14SV1TransferSyntax:
      /* we prefer JPEGLossless:Hierarchical-1stOrderPrediction (default lossless) */
      m_transferSyntaxes.push_back(UID_JPEGProcess14SV1TransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
       break;
    case EXS_JPEGProcess1TransferSyntax:
      /* we prefer JPEGBaseline (default lossy for 8 bit images) */
      m_transferSyntaxes.push_back(UID_JPEGProcess1TransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back( UID_BigEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
    case EXS_JPEGProcess2_4TransferSyntax:
      /* we prefer JPEGExtended (default lossy for 12 bit images) */
      m_transferSyntaxes.push_back(UID_JPEGProcess2_4TransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
       break;
    case EXS_JPEG2000:
      /* we prefer JPEG2000 Lossy */
      m_transferSyntaxes.push_back(UID_JPEG2000TransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
    case EXS_JPEG2000LosslessOnly:
      /* we prefer JPEG2000 Lossless */
      m_transferSyntaxes.push_back(UID_JPEG2000LosslessOnlyTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
    case EXS_RLELossless:
      /* we prefer RLE Lossless */
      m_transferSyntaxes.push_back(UID_RLELosslessTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
     case EXS_DeflatedLittleEndianExplicit:
      /* we prefer Deflated Explicit VR Little Endian */
      m_transferSyntaxes.push_back(UID_DeflatedExplicitVRLittleEndianTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
      break;
    default:
      // =====================================================================
      // We prefer explicit transfer syntaxes.
      // If we are running on a Little Endian machine we prefer
      // LittleEndianExplicitTransferSyntax to BigEndianTransferSyntax.
      // =====================================================================
      if (gLocalByteOrder == EBO_LittleEndian)  /* defined in dcxfer.h */
      {
        m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
        m_transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
      }
      else
      {
        m_transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
        m_transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
      }

      m_transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);

      break;

  } // END ... switch

} // SetTransferSyntaxes(E_TransferSyntax transferSyntax)


#pragma region transfer syntax methods


/**
 * Determines if the specified transfer syntax is supported
 *
 * @param sTransferSyntax       Trans syntax
 *
 * @return True if in list, false otherwise
 */
bool DcmDevice::SupportsTransferSyntax(const std::string & sTransferSyntax)
{

  bool bFound(false);
  for ( STRINGLIST::const_iterator it = m_transferSyntaxes.cbegin(); 
        it != m_transferSyntaxes.cend() && !bFound; 
        bFound = ((*it) == sTransferSyntax), it++ );
  return bFound;
} // SupportsTransferSyntax(const std::string & sTransferSyntax)


/**
 * Adds the transfer syntax to the list of not already present
 *
 * @param sTransferSyntax     Syntax to add
 */
void DcmDevice::AddTransferSyntax(const std::string & sTransferSyntax)
{
  if ( ! SupportsTransferSyntax(sTransferSyntax) )
    m_transferSyntaxes.push_back(sTransferSyntax);
} // AddTransferSyntax(const std::string & sTransferSyntax)


#pragma endregion


#pragma region abstract syntax methods

/**
 * Determines if the specified abstract syntax is supported
 *
 * @param sAbstractSyntax       abstract syntax
 *
 * @return True if in list, false otherwise
 */
bool DcmDevice::SupportsAbstractSyntax(const std::string & sAbstractSyntax)
{

  bool bFound(false);
  for ( STRINGLIST::const_iterator it = m_abstractSyntaxes.cbegin(); 
        it != m_abstractSyntaxes.cend() && !bFound; 
        bFound = ((*it) == sAbstractSyntax), it++ );
  return bFound;
} // SupportsAbstractSyntax(const std::string & sAbstractSyntax)


/**
 * Adds the abstract syntax to the list of not already present
 *
 * @param sAbstractSyntax     Syntax to add
 */
void DcmDevice::AddAbstractSyntax(const std::string & sAbstractSyntax)
{
  if ( ! SupportsAbstractSyntax(sAbstractSyntax) )
    m_abstractSyntaxes.push_back(sAbstractSyntax);
} // AddAbstractSyntax(const std::string & sAbstractSyntax)


#pragma endregion